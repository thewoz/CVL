//
//  asTargetMass.hpp
//  THESEUS
//
//  Created by Leonardo Parisi on 03/03/17.
//  Copyright © 2017 COBBS. All rights reserved.
//

#ifndef asTargetMass_h
#define asTargetMass_h

#include <cstdlib>
#include <cstdio>

#include <vector>

#include <opencv2/opencv.hpp>

#include <mpl/configure.hpp>


#include "blob.hpp"
#include "labeling.hpp"
#include "contours.hpp"

/*****************************************************************************/
// namespace cvl::targetmass
/*****************************************************************************/
namespace cvl::targetmass {

  /*****************************************************************************/
  // fixed
  /*****************************************************************************/
  void fixed(std::vector<asBlob> & blobs, cv::Mat & dst, int radius) {
  
    dst.setTo(cv::Scalar(0,0,0));
    
    cv::Scalar color = cv::Scalar(255, 255, 255);
    
    // per ogni blocb disegno un cerchio intorno al baricentro
    for(uint32_t i=0; i<blobs.size(); ++i) {
        
        cv::circle(dst, cv::Point(blobs[i].baricenter.x, blobs[i].baricenter.y), radius, color, -1);
      
    }
    
  }

  /*****************************************************************************/
   // erode
   /*****************************************************************************/
   void erode(std::vector<asBlob> & blobs, const cv::Mat & src, size_t targetMass, size_t minMass = 1, const std::string & labelingNeighboursType = "NEXT_NEAREST_NEIGHBOURS") {
  
     std::vector<asBlob> tmpBlobs;
     
     // provo a dividere ogni blob
     for(uint32_t i=0; i<blobs.size(); ++i) {
       
       int mass = (uint32_t)blobs[i].pixels.size();
       
       // se e' piu' grande della massa di entrata lo provo a dividere
       if(mass > targetMass) {
         
         // creo un immagine di appoggio
         cv::Mat thrsImg = cv::Mat::zeros(blobs[i].size, CV_8UC1);
       
         // ci disegno il blob sopra
         blobs[i].drawMask(thrsImg, blobs[i].offset);
         
         // ciclo
         for(;mass>targetMass;){
         
           std::vector<cv::Point> contours;
             
           cvl::contours::get(thrsImg, cvl::contours::INTERNAL, contours);
                      
           if(mass - contours.size() >= minMass) {
           
             for(uint32_t j=0; j<contours.size(); ++j)
               thrsImg.at<uchar>(contours[j]) = 0;
           
             mass -= contours.size();
             
           } else { break; }
                    
         }
                  
         // vettore di blobs in appoggio
         std::vector<asBlob> tmpBlobs;
         
         // labbello
         cvl::labeling::get(thrsImg, labelingNeighboursType, minMass, tmpBlobs, src, blobs[i].offset);

         tmpBlobs.insert(tmpBlobs.end(), tmpBlobs.begin(), tmpBlobs.end());
                  
       } else {
           
         tmpBlobs.push_back(blobs[i]);
           
       }
       
     }  // for i
     
   }
  
  /*****************************************************************************/
  // namespace utils
  /*****************************************************************************/
  namespace utils {
    
    /*****************************************************************************/
     // try2split
     /*****************************************************************************/
     void try2split(asBlob & blob, const cv::Mat & src, std::vector<asBlob> & blobsFound, size_t targetMass, bool toDark, size_t minMass = 1, const std::string & labelingNeighboursType = "NEXT_NEAREST_NEIGHBOURS") {
                  
       // vettore che contiene i vari blob che vanno divisi
       std::vector<asBlob> blobsToSplit;
       
       blobsToSplit.push_back(blob);
       
       // vettore con i blob temporanei trovati a ogni passo
       std::vector<asBlob> tmpBlobs;
       
       // immagini di appoggio
       cv::Mat blobImg = cv::Mat(blob.size, CV_8UC1);
       cv::Mat thrsImg = cv::Mat(blob.size, CV_8UC1);
       
       uint32_t threshold;
         
       uint32_t targetThreshold;
       
       // ciclo su tutti i blob
       for(uint32_t i=0; i<blobsToSplit.size(); ++i) {
         
         // in base se devo andare verso il nero o meno
         if(toDark){
           blobImg.setTo(cv::Scalar(255,255,255));
           targetThreshold = blobsToSplit[i].iMin;
           threshold       = blobsToSplit[i].iMax;
         } else {
           blobImg.setTo(cv::Scalar(0,0,0));
           targetThreshold = blobsToSplit[i].iMax;
           threshold       = blobsToSplit[i].iMin;
         }
         
         // dimenzione imagine disegno il blob
         blobsToSplit[i].draw(blobImg, blob.offset);
         
         asBlob checkPoint = blobsToSplit[i];
         
         bool toAdd = false;
         
         // ciclo sulla soglia fino a che non splitto o raggiungo il minimo
         for(;;) {
           
           // applico la nuova soglia
           if(toDark){
             if(threshold < targetThreshold) { toAdd = true; break; }
             cv::threshold(blobImg, thrsImg, (threshold - 1), 255, cv::THRESH_BINARY_INV);
           } else {
             if(threshold > targetThreshold) { toAdd = true; break; }
             cv::threshold(blobImg, thrsImg, (threshold - 1), 255, cv::THRESH_BINARY);
           }
           
           // trovo le componeti connesse
           cvl::labeling::get(thrsImg, labelingNeighboursType, minMass, tmpBlobs, src, blob.offset);
           
           // se viene diviso i due
           if(tmpBlobs.size() > 2) break;
           
           // se e' uno e ha raggiunto una massa giusta
           if(tmpBlobs.size() == 1 && tmpBlobs[0].pixels.size() <= targetMass) break;
           
           if(tmpBlobs.size() == 0) { tmpBlobs.push_back(checkPoint); toAdd = true; break; }
           
           if(tmpBlobs.size() == 1) checkPoint = tmpBlobs[0];
           
           if(toDark) { --threshold; /*if(threshold > tmpBlobs[0].iMax) threshold = tmpBlobs[0].iMax;*/ }
           else       { ++threshold; /*if(threshold < tmpBlobs[0].iMin) threshold = tmpBlobs[0].iMin;*/ }
           
         } // ciclo sulla soglia
         
         // ciclo su tutti i blob trovati
         for(uint32_t j=0; j<tmpBlobs.size(); ++j){
           
           // se ha ragiunto la massa giusta
           if(tmpBlobs[j].pixels.size() <= targetMass || toAdd){
             blobsFound.push_back(tmpBlobs[j]);
           } else {
             blobsToSplit.push_back(tmpBlobs[j]);
           }
           
         }
         
       } // for i
       
       //if(mode) cv::destroyWindow("Display window");
       
     }
    
  } /* namespace utils */
  

  /*****************************************************************************/
  // split
  /*****************************************************************************/
  void split(std::vector<asBlob> & blobs, const cv::Mat & src, size_t targetMass, bool toDark, size_t minMass = 1, const std::string & labelingNeighboursType = "NEXT_NEAREST_NEIGHBOURS") {
        
    // vettore di appoggio con il risultato finale
    std::vector<asBlob> foundedBlobs;
    
    // provo a dividere ogni blob
    for(uint32_t i=0; i<blobs.size(); ++i) {
      
      // se e' piu' grande della massa di entrata lo provo a dividere
      if(blobs[i].pixels.size() > targetMass){
        
        std::vector<asBlob> tmpBlobs;

        // provo a splittarlo
        utils::try2split(blobs[i], src, tmpBlobs, targetMass, toDark, minMass, labelingNeighboursType);
        
        // ciclo su tutti i blob trovati
        for(uint32_t j=0; j<tmpBlobs.size(); ++j) {
          
          tmpBlobs[j].label = (int32_t) foundedBlobs.size();
          
          foundedBlobs.push_back(tmpBlobs[j]);
          
        }
        
      } else {
        
        blobs[i].label = (int32_t) foundedBlobs.size();
        
        foundedBlobs.push_back(blobs[i]);
        
      }
      
    }
    
    blobs.swap(foundedBlobs);
    
  }

} /* namespace cvl::targetmass */


#endif /* asTargetMass_h */
