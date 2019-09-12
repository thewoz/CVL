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
// asTargetMass
/*****************************************************************************/
class asTargetMass {
  
private:
  
  char labelingNeighboursType[PATH_MAX];
  
  uint32_t targetMass;
  
  uint32_t minMass;
  
  bool toDark;
  
  std::vector<asBlob> blobsFound;
  
  
public:

  /*****************************************************************************/
  // fixed
  /*****************************************************************************/
  void fixed(std::vector<asBlob> & blobs, cv::Mat & dst, int value) {
  
    dst.setTo(cv::Scalar(0,0,0));
    
    cv::Scalar color = cv::Scalar(255, 255, 255);
    
    // per ogni blocb disegno un cerchio intorno al baricentro
    for(uint32_t i=0; i<blobs.size(); ++i) {
    
      blobs[i].computeBaricenter("GEOMETRIC");
      
      //if(blobs[i].pixels.size() < 36)
        cv::circle(dst, cv::Point(blobs[i].baricenter.x, blobs[i].baricenter.y), value, color, -1);
      //else
        //blobs[i].drawMask(dst);
      
    }
    
  }
  
  /*****************************************************************************/
  // erode
  /*****************************************************************************/
  void erode(std::vector<asBlob> & blobs, const cv::Mat & src){
    
    //cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO | CV_GUI_EXPANDED);
    
    //FIX
    //init(dictionary);

    //cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3), cv::Point(1,1));
 
    // provo a dividere ogni blob
    for(uint32_t i=0; i<blobs.size(); ++i) {
      
      uint32_t mass = (uint32_t)blobs[i].pixels.size();
      
      // se e' piu' grande della massa di entrata lo provo a dividere
      if(mass > targetMass){
        
        // creo un immagine di appoggio
        cv::Mat thrsImg = cv::Mat::zeros(blobs[i].size, CV_8UC1);
      
        // ci disegno il blob sopra
        blobs[i].drawMask(thrsImg, blobs[i].offset);
  
        //cv::imshow("Display window", thrsImg); cv::waitKey(0);
        
        //printf("start %d\n", mass);
        
        // ciclo
        for(;mass>targetMass;){
        
          std::vector<cv::Point> contours;
            
          cvl::contours::get(thrsImg, cvl::contours::INTERNAL, contours);
          
          for(uint32_t j=0; j<contours.size(); ++j)
            thrsImg.at<uchar>(contours[j]) = 0;
          
          //printf("elimino %lu\n", contours.size());

          mass -= contours.size();
          
         // printf("mass %d\n", mass);
          
          //cv::imshow("Display window", thrsImg); cv::waitKey(200);
        
        }
        
        // vettore di blobs in appoggio
        std::vector<asBlob> tmpBlobs;
        
        // labbello
        cvl::labeling::get(thrsImg, labelingNeighboursType, minMass, tmpBlobs, src, blobs[i].offset);

        blobs[i].pixels.clear();
        
        for(uint32_t j=0; j<tmpBlobs.size(); ++j)
          blobs[i].pixels.insert(blobs[i].pixels.end(), tmpBlobs[j].pixels.begin(), tmpBlobs[j].pixels.end());
        
        blobs[i] = tmpBlobs[0];
        
        printf("%d\n",i);

      }  // if mass
      
      
    }  // for i

  //printf("AAA\n");

    
  }
  
  
  /*****************************************************************************/
  // split
  /*****************************************************************************/
  void split(std::vector<asBlob> & blobs, const cv::Mat & src, const char * dictionary){
    
    init(dictionary);
    
    // vettore di appoggio con il risultato finale
    std::vector<asBlob> foundedBlobs;
    
    // provo a dividere ogni blob
    for(uint32_t i=0; i<blobs.size(); ++i) {
      
      // se e' piu' grande della massa di entrata lo provo a dividere
      if(blobs[i].pixels.size() > targetMass){
        
        // provo a splittarlo
        try2split(blobs[i], src);
        
        // ciclo su tutti i blob trovati
        for(uint32_t j=0; j<blobsFound.size(); ++j) {
          
          blobsFound[j].label = (int32_t) foundedBlobs.size();
          
          foundedBlobs.push_back(blobsFound[j]);
          
        }
        
      } else {
        
        blobs[i].label = (int32_t) foundedBlobs.size();
        
        foundedBlobs.push_back(blobs[i]);
        
      }
      
    }
    
    blobs.swap(foundedBlobs);
    
    // pulisco la memoria
    blobsFound = std::vector<asBlob>();
    
  }
  
  
private:
  
  /*****************************************************************************/
  // try2split
  /*****************************************************************************/
  void try2split(asBlob & blob, const cv::Mat & src) {
		  
    blobsFound.clear();
		  
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
  
  /*****************************************************************************/
  // init
  /*****************************************************************************/
  void init(const char * dictionary){
    
    targetMass = mpl::configure::getParam<uint32_t>("TARGET_MASS", dictionary);
    
    minMass = mpl::configure::getParam<uint32_t>("TARGET_MASS_MIN_MASS", dictionary);
    
    strcpy(labelingNeighboursType, mpl::configure::getParam<const char *>("TARGET_MASS_NEIGHBOURS_TYPE", dictionary));
    
    if(strcmp("TO_DARK",  mpl::configure::getParam<const char *>("TARGET_MASS_MODE", dictionary)) == 0) toDark = true;
    else if(strcmp("TO_WHITE", mpl::configure::getParam<const char *>("TARGET_MASS_MODE", dictionary)) == 0) toDark = false;
    else { fprintf(stderr, "error unknown type of TARGET_MASS_MODE. You can choose between: TO_DARK or TO_WHITE\n");
      exit(EXIT_FAILURE);
    }
    
  }
	 
  
};



#endif /* asTargetMass_h */
