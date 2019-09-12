/*
 * MIT License
 *
 * Copyright © 2019
 * Created by Leonardo Parisi (leonardo.parisi[at]gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _H_CVL_BLOBSPLITTING_H_
#define _H_CVL_BLOBSPLITTING_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

#include "blob.hpp"
#include "labeling.hpp"

/*****************************************************************************/
// namespace
/*****************************************************************************/
namespace cvl::blobsplitting {
  
  /*****************************************************************************/
  // namespace
  /*****************************************************************************/
  namespace utils {
    
    /*****************************************************************************/
    // split
    // se devo andare con la soglia verso l'alto o il basso
    //    bool toDark;
    //
    //    // massa per cui smetto di splittare
    //    uint32_t exitMass;
    //
    //    // massa per cui inizio a splittare
    //    uint32_t enterMass;
    //
    //    // massa minimi per cui mi tengo il blob
    //    uint32_t minMass;
    //
    //    // tipologia di connettivita
    //    char labelingNeighboursType[PATH_MAX];
    //
    //    // se devo continuare a dividire una volta diviso
    //    bool recursiveSplitting;
    /*****************************************************************************/
    void split(asBlob & blob, const cv::Mat & src, std::vector<asBlob> & blobsFound, bool toDark, uint32_t enterMass, uint32_t exitMass, uint32_t minMass, bool recursiveSplitting, const std::string & labelingNeighboursType) {
      
      // vettore che contiene i blob che saranno trovati
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
        
        // ciclo sulla soglia fino a che non splitto o raggiungo il minimo
        for(;;) {
          
          // applico la nuova soglia
          if(toDark){
            if(threshold < targetThreshold) break;
            cv::threshold(blobImg, thrsImg, (threshold - 1), 255, cv::THRESH_BINARY_INV);
          } else {
            if(threshold > targetThreshold) break;
            cv::threshold(blobImg, thrsImg, (threshold - 1), 255, cv::THRESH_BINARY);
          }
          //
          
          // trovo le componeti connesse
          cvl::labeling::get(thrsImg, labelingNeighboursType, minMass, tmpBlobs, src, blob.offset);
          
          // se non ho ancora un solo blob con massa ancora grande continuo
          if(!(tmpBlobs.size() == 1 && tmpBlobs[0].pixels.size() > exitMass)) break;
          
          if(toDark) { --threshold; /*if(threshold > tmpBlobs[0].iMax) threshold = tmpBlobs[0].iMax;*/ }
          else       { ++threshold; /*if(threshold < tmpBlobs[0].iMin) threshold = tmpBlobs[0].iMin;*/ }
          
        } // ciclo sulla soglia
        
        // se e' alla fine rimasto un solo cluster l'aggiungo al risulato
        if(tmpBlobs.size() <= 1) {
          
          blobsFound.push_back(blobsToSplit[i]);
          
          // se il blob e' stato diviso
        } else {
          
          // ciclo su i blob trovati
          for(uint32_t j=0; j<tmpBlobs.size(); ++j){
            
            // se il blob trovato e' ancora abbastanza grande e lo splitig e' ricorsivo lo aggiungo ai blob da dividere
            if(tmpBlobs[j].pixels.size() > enterMass && recursiveSplitting){
              
              blobsToSplit.push_back(tmpBlobs[j]);
              
              // oppure lo aggiungo al risultato
            } else{
              
              //TODO: qua dovrei espandere
              blobsFound.push_back(tmpBlobs[j]);
              
            }
            
          } // for j
          
        } // else
        
      } // for i
      
    }
    
  }  /* namespace */
  
  
  
  /*****************************************************************************/
  // get
  /*****************************************************************************/
  void get(std::vector<asBlob> & blobs, const cv::Mat & src, const std::string & mode, uint32_t enterMass, uint32_t exitMass, uint32_t minMass, bool recursiveSplitting, const std::string & labelingNeighboursType){

    bool toDark = true;
    
    if(mode.compare("TO_WHITE") == 0) {
      toDark = false;
    }
    
    if(mode.compare("TO_DARK") != 0 &&  mode.compare("TO_WHITE") != 0) {
      
      fprintf(stderr, "error unknown type of BLOB_SPLIT_MODE.\n");
      fprintf(stderr, "You can choose between:\n");
      fprintf(stderr, "      TO_DARK, TO_WHITE\n");
      
      exit(EXIT_FAILURE);
      
    }
    
    // vettore di appoggio con il risultato finale
    std::vector<asBlob> foundedBlobs;
    
    // provo a dividere ogni blob
    for(uint32_t i=0; i<blobs.size(); ++i) {
      
      // se e' piu' grande della massa di entrata lo provo a dividere
      if(blobs[i].pixels.size() > enterMass){
        
        std::vector<asBlob> blobsFound;
        
        // provo a splittarlo
        utils::split(blobs[i], src, blobsFound, toDark, enterMass, exitMass, minMass, recursiveSplitting, labelingNeighboursType);
        
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
    
  }
  
  
} /* namespace */


#endif /* _H_CVL_BLOBSPLITTING_H_ */


