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

#ifndef _H_CVL_BLOBSPLITTING_H_
#define _H_CVL_BLOBSPLITTING_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>
#include <random>

#include <opencv2/opencv.hpp>

#include "blob.hpp"
#include "labeling.hpp"

//#define _DEBUG_CVL_BLOBSPLITTING_


//*****************************************************************************
// namespace
//*****************************************************************************
namespace cvl::blobsplitting {

  //*****************************************************************************
  // namespace
  //*****************************************************************************
  namespace utils {

    //*****************************************************************************
    // split
    // se devo andare con la soglia verso l'alto o il basso
    //    bool toDark;
    //
    //    // massa per cui smetto di splittare
    //    int exitMass;
    //
    //    // massa per cui inizio a splittare
    //    int enterMass;
    //
    //    // massa minimi per cui mi tengo il blob
    //    int minMass;
    //
    //    // tipologia di connettivita
    //    char labelingNeighboursType[PATH_MAX];
    //
    //    // se devo continuare a dividire una volta diviso
    //    bool recursiveSplitting;
    //*****************************************************************************
    void split(blob_t & blob, const cv::Mat & splitOn, const cv::Mat & original, std::vector<blob_t> & blobsFound, bool toDark, uint32_t enterMass, uint32_t exitMass, uint32_t minMass, bool recursiveSplitting, int labelingNeighboursType) {
      
    #ifdef _DEBUG_CVL_BLOBSPLITTING_
      //cv::namedWindow("blobs", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
    #endif
      
      // vettore che contiene i blob che saranno trovati
      blobsFound.clear();
      
      // vettore che contiene i vari blob che vanno divisi
      std::vector<blob_t> blobsToSplit;
      
      blobsToSplit.push_back(blob);
      
      // vettore con i blob temporanei trovati a ogni passo
      std::vector<blob_t> tmpBlobs;
      
      // immagini di appoggio
      cv::Mat blobImg = cv::Mat(blob.getImageSize(), CV_8UC1);
      cv::Mat thrsImg = cv::Mat(blob.getImageSize(), CV_8UC1);
      
      int threshold;
      
      int targetThreshold;
      
      // ciclo su tutti i blob
      for(size_t i=0; i<blobsToSplit.size(); ++i) {
        
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
        
        //printf("threshold min %d threshold max %d\n", blobsToSplit[i].iMin, blobsToSplit[i].iMax);
        
        //        if(targetThreshold == 0 && threshold ==0) {
        //          printf("i %d\n", i);
        //          printf("baricenter %f %f\n", blobsToSplit[i].baricenter.x, blobsToSplit[i].baricenter.y);
        //          printf("mass %lu\n", blobsToSplit[i].mass());
        //          fflush(stdout);
        //          cv::namedWindow("blobs", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
        //          blobsToSplit[i].draw(blobImg, blob.offset);
        //          cv::imshow("blobs", blobImg); cv::waitKey(0);
        //          abort();
        //        }
        
        // dimenzione imagine disegno il blob
        blobsToSplit[i].draw(blobImg, splitOn, blob.offset);
        
        //cv::imshow("blobs", blobImg); cv::waitKey(0);
        
        // ciclo sulla soglia fino a che non splitto o raggiungo il minimo
        for(;;) {
          
          //printf("threshold %d\n", threshold); fflush(stdout);
          
          // applico la nuova soglia
          if(toDark){
            if(threshold < targetThreshold) break;
            cv::threshold(blobImg, thrsImg, (threshold - 1), 255, cv::THRESH_BINARY_INV);
          } else {
            if(threshold > targetThreshold) break;
            cv::threshold(blobImg, thrsImg, (threshold - 1), 255, cv::THRESH_BINARY);
          }
          //
          
          //cv::imshow("blobs", thrsImg); cv::waitKey(0);
          
          // trovo le componeti connesse
          cvl::labeling::get(thrsImg, labelingNeighboursType, minMass, tmpBlobs, original, blob.offset);
          
          // se l'ho diviso o è rimasto un troppo piccolo esco
          if(tmpBlobs.size() > 1 || tmpBlobs.size() == 0 || tmpBlobs[0].size() <= exitMass) break;
          
          if(toDark) { --threshold; /*if(threshold > tmpBlobs[0].iMax) threshold = tmpBlobs[0].iMax;*/ }
          else       { ++threshold; /*if(threshold < tmpBlobs[0].iMin) threshold = tmpBlobs[0].iMin;*/ }
          
        } // ciclo sulla soglia
        
        // se e' alla fine rimasto un solo cluster l'aggiungo l'originale al risulato
        if(tmpBlobs.size() == 1) {
          
          blobsFound.push_back(blobsToSplit[i]);
          
          // se il blob e' stato diviso
        } else {
          
          // pulisco quelli troppo piccoli
          for(size_t j=0; j<tmpBlobs.size(); ++j) {
            if(tmpBlobs[j].size() < exitMass) {
              tmpBlobs.erase(tmpBlobs.begin() + j);
              --j;
            }
          }
          
          // se ne ho piu di uno
          if(tmpBlobs.size() > 1) {
            
            // ciclo su i blob trovati
            for(size_t j=0; j<tmpBlobs.size(); ++j){
              
              // se il blob trovato e' ancora abbastanza grande e lo splitig e' ricorsivo lo aggiungo ai blob da dividere
              if(tmpBlobs[j].size() > enterMass && recursiveSplitting){
                
                blobsToSplit.push_back(tmpBlobs[j]);
                
                // oppure lo aggiungo al risultato
              } else{
                
                blobsFound.push_back(tmpBlobs[j]);
                
              }
              
            } // for j
            
            // se era rimasta solo polvere
          } else {
            
            blobsFound.push_back(blobsToSplit[i]);
            
          }
          
        } // else
        
      } // for i
      
    }

  }  /* namespace */

  //****************************************************************************
  // get
  //****************************************************************************
  void get(std::vector<blob_t> & blobs, const cv::Mat & splitOn, const cv::Mat & original, const std::string & mode, uint32_t enterMass, uint32_t exitMass, uint32_t minMass, bool recursiveSplitting, int labelingNeighboursType){
    
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
    std::vector<blob_t> foundedBlobs;
    
    // provo a dividere ogni blob
    for(size_t i=0; i<blobs.size(); ++i) {
      
      // se e' piu' grande della massa di entrata lo provo a dividere
      if(blobs[i].size() > enterMass) {
        
        std::vector<blob_t> blobsFound;
        
        // provo a splittarlo
        utils::split(blobs[i], splitOn, original, blobsFound, toDark, enterMass, exitMass, minMass, recursiveSplitting, labelingNeighboursType);
        
        // ciclo su tutti i blob trovati
        for(size_t j=0; j<blobsFound.size(); ++j) {
          
          blobsFound[j].label = (int)foundedBlobs.size();
          
          foundedBlobs.push_back(blobsFound[j]);
          
        }
        
      } else {
        
        blobs[i].label = (int)foundedBlobs.size();
        
        foundedBlobs.push_back(blobs[i]);
        
      }
      
    }
        
    // creo un immagine come se fosse tutto cielo (sfondo)
    cv::Mat markers(original.size(), CV_32SC1, cv::Scalar::all(foundedBlobs.size()+1));
    
#ifdef _DEBUG_CVL_BLOBSPLITTING_
  cv::namedWindow("markers", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
  cv::Mat markersColor(original.size(), CV_8UC3, cv::Scalar::all(0));
  // creo la tabella di colori
  std::vector<cv::Vec3b> colorTab;
#endif

    // ciclo sui blob di partenza
    for(size_t i=0; i<blobs.size(); ++i) {
      
      // mi calcolo un raggio doppio che lo racchiude
      double r = sqrt(blobs[i].size() / M_PI) * 2.0;
      
      // disegno un cerchio nero per dire che quell'aria non so cosa sia
      cv::circle(markers, blobs[i].baricenter, r, cv::Scalar(0), cv::FILLED);
      
      //TODO: forse meglio questo da valutare allargando anche i punti
      //cv::RotatedRect rect = cv::minAreaRect(blobs[i].pixels);
      //cv::rectangle(markers, rect.boundingRect(), cv::Scalar(0), cv::FILLED);
      
#ifdef _DEBUG_CVL_BLOBSPLITTING_
      cv::circle(markersColor, blobs[i].baricenter, r, cv::White, cv::FILLED);
#endif

    }
    
    // ciclo sui blob dopo lo split e metto i seed
    for(int i=0; i<foundedBlobs.size(); ++i) {
      
      foundedBlobs[i].draw(markers, i+1);
      
#ifdef _DEBUG_CVL_BLOBSPLITTING_
      colorTab.push_back(cv::getRandomColor());
      foundedBlobs[i].draw(markersColor, colorTab[i]);
#endif
      
    }
    
#ifdef _DEBUG_CVL_BLOBSPLITTING_
  cv::imshow("markers", markersColor); cv::waitKey(0);
#endif
    
    cv::Mat originalColor; cvtColor(splitOn, originalColor, cv::COLOR_GRAY2BGR);
    
    cv::watershed(originalColor, markers);
    
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    cv::Mat wshed(markers.size(), CV_8UC3);
  #endif
    
    // pulisco i pixel che facevano parte dei blob
    for(int i=0; i<foundedBlobs.size(); ++i)
      foundedBlobs[i].clear();
    
    for(int i=0; i<markers.rows; ++i) {
      
      for(int j=0; j<markers.cols; ++j) {
        
        int index = markers.at<int>(i,j);
        
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
        if(index == -1)
          //wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255);
          wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
        else if(index <= 0 || index > foundedBlobs.size())
          wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
        else
          wshed.at<cv::Vec3b>(i,j) = colorTab[index-1];
  #endif
        
        if(index > 0 && index <= foundedBlobs.size()) {
          foundedBlobs[index-1].add(j, i, original.at<uchar>(i,j));
        }
        
      }
      
    }
    
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    wshed = wshed + markersColor * 0.5;
    cv::imshow("markers", wshed); cv::waitKey(0);
  #endif
    
    for(int i=0; i<foundedBlobs.size(); ++i) {
      if(foundedBlobs[i].size() <= 0) { foundedBlobs.erase(foundedBlobs.begin()+i); --i; continue; }
      foundedBlobs[i].finalize();
    }
    
    blobs.swap(foundedBlobs);
    
  }

} /* namespace */


#endif /* _H_CVL_BLOBSPLITTING_H_ */


