/*
 * GNU GENERAL PUBLIC LICENSE
 *
 * Copyright (C) 2019
 * Created by Leonardo Parisi (leonardo.parisi[at]gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _H_CVL_TARGET_MASS_H_
#define _H_CVL_TARGET_MASS_H_

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
  void fixed(std::vector<blob_t> & blobs, cv::Mat & dst, int radius) {
  
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
   void erode(std::vector<blob_t> & blobs, const cv::Mat & src, size_t targetMass, size_t minMass = 1, int labelingNeighboursType = cvl::labeling::NEXT_NEAREST_NEIGHBOURS) {
  
     std::vector<blob_t> tmpBlobs;
     
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
         std::vector<blob_t> tmpBlobs;
         
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
     void try2split(blob_t & blob, const cv::Mat & src, std::vector<blob_t> & blobsFound, size_t targetMass, bool toDark, size_t minMass = 1, int labelingNeighboursType = cvl::labeling::NEXT_NEAREST_NEIGHBOURS) {
                  
       // vettore che contiene i vari blob che vanno divisi
       std::vector<blob_t> blobsToSplit;
       
       blobsToSplit.push_back(blob);
       
       // vettore con i blob temporanei trovati a ogni passo
       std::vector<blob_t> tmpBlobs;
       
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
         
         blob_t checkPoint = blobsToSplit[i];
         
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
  void split(std::vector<blob_t> & blobs, const cv::Mat & src, size_t targetMass, bool toDark, size_t minMass = 1, int labelingNeighboursType = cvl::labeling::NEXT_NEAREST_NEIGHBOURS) {
        
    // vettore di appoggio con il risultato finale
    std::vector<blob_t> foundedBlobs;
    
    // provo a dividere ogni blob
    for(uint32_t i=0; i<blobs.size(); ++i) {
      
      // se e' piu' grande della massa di entrata lo provo a dividere
      if(blobs[i].pixels.size() > targetMass){
        
        std::vector<blob_t> tmpBlobs;

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


#endif /* _H_CVL_TARGET_MASS_H_ */
