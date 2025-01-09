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
  void split(const cvl::blob_t & blob, const cv::Mat & splitOn, const cv::Mat & original, cvl::blobs_t & blobsFound, bool toDark, int enterMass, int exitMass, int minMass, bool recursiveSplitting, int labelingNeighboursType) {
    
    if(exitMass < minMass) {
      fprintf(stderr, "error in blobSplitting::split(): exitMass must be equal or grate than minMass\n");
      abort();
    }
    
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    //cv::namedWindow("blobs", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
  #endif
    
    // vettore che contiene i blob che saranno trovati
    blobsFound.clear();
    
    // vettore che contiene i vari blob che vanno divisi
    cvl::blobs_t blobsToSplit;
    
    blobsToSplit.add(blob);
    
    // vettore con i blob temporanei trovati a ogni passo
    cvl::blobs_t tmpBlobs;
    
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
        
        //cv::imshow("blobs", thrsImg); cv::waitKey(0);
        
        // trovo le componeti connesse
        cvl::labeling::get(thrsImg, labelingNeighboursType, minMass, tmpBlobs, original, blob.offset);
        
        // se l'ho diviso o è rimasto un troppo piccolo o non è rimansto nulla esco
        if(tmpBlobs.size() > 1 || tmpBlobs.size() == 0 || (tmpBlobs.size() == 1 && tmpBlobs[0].size() <= exitMass)) break;
        
        if(toDark) { --threshold; /*if(threshold > tmpBlobs[0].iMax) threshold = tmpBlobs[0].iMax;*/ }
        else       { ++threshold; /*if(threshold < tmpBlobs[0].iMin) threshold = tmpBlobs[0].iMin;*/ }
        
      } // ciclo sulla soglia
      
      // se alla fine rimasto un solo cluster o nessuno aggiungo l'originale al risulato
      if(tmpBlobs.size() <= 1) {
        
        blobsFound.add(blobsToSplit[i]);
        
        // se il blob invece è stato diviso
      } else {
        
        // ciclo su i blob trovati
        for(size_t j=0; j<tmpBlobs.size(); ++j) {
          
          // se il blob trovato e' ancora abbastanza grande e lo splitig e' ricorsivo lo aggiungo ai blob da dividere
          if(tmpBlobs[j].size() > enterMass && recursiveSplitting) {
            blobsToSplit.add(tmpBlobs[j]);
            // altrimenti lo aggiungo al risultato
          } else {
            blobsFound.add(tmpBlobs[j]);
          }
          
        } // for j
        
      } // else
      
    } // for i
    
  }

  }  /* namespace */

  //#define _DEBUG_CVL_BLOBSPLITTING_

  //****************************************************************************
  // get
  //****************************************************************************
  void get(cvl::blobs_t & blobs, const cv::Mat & splitOn, const cv::Mat & original, const std::string & mode, int enterMass, int exitMass, int minMass, bool recursiveSplitting, int labelingNeighboursType){
    
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    static int debugFrame = 22; // va modificato il valore di questo
    static int runFrame = 0;
  #endif
    
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
    cvl::blobs_t newBlobs;
    
    // provo a dividere ogni blob
    for(size_t i=0; i<blobs.size(); ++i) {
      
      // se e' piu' grande della massa di entrata lo provo a dividere
      if(blobs[i].size() > enterMass) {
        
        cvl::blobs_t blobsSplited;
        
        // provo a splittarlo
        utils::split(blobs[i], splitOn, original, blobsSplited, toDark, enterMass, exitMass, minMass, recursiveSplitting, labelingNeighboursType);
        
        // ciclo su tutti i blob trovati
        for(size_t j=0; j<blobsSplited.size(); ++j)
          newBlobs.add(blobsSplited[j]);
        
      } else { newBlobs.add(blobs[i]); }
      
    }
    
    // creo un immagine come se fosse tutto cielo (sfondo)
    cv::Mat markers(original.size(), CV_32SC1, cv::Scalar::all(newBlobs.size()+1));
    
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    cv::namedWindow("markers", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO | cv::WINDOW_GUI_EXPANDED);
    cv::Mat markersColor(original.size(), CV_8UC3, cv::Scalar::all(0));
    // creo la tabella di colori
    std::vector<cv::Vec3b> colorTab;
  #endif
    
    // ciclo sui blob di partenza (quelli non splittati)
    for(size_t i=0; i<blobs.size(); ++i) {
      
      // mi calcolo un raggio doppio che lo racchiude
      double r = sqrt(blobs[i].size() / M_PI) * 2.5;
      
      // disegno un cerchio nero per dire che quell'aria non so cosa sia
      cv::circle(markers, blobs[i].baricenter, r, cv::Scalar(0), cv::FILLED);
      
      //TODO: forse meglio questo da valutare allargando anche i punti
      //cv::rectangle(markers, blobs[i].rotatedRect.boundingRect(), cv::Scalar(0), cv::FILLED);
      
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
      cv::circle(markersColor, blobs[i].baricenter, r, cvl::White, cv::FILLED);
      //cv::rectangle(markers, blobs[i].rotatedRect.boundingRect(), cv::Scalar(0), cv::FILLED);
  #endif
      
    }
    
    // ciclo sui blob dopo lo split e metto i seed
    for(int i=0; i<newBlobs.size(); ++i) {
      
      newBlobs[i].draw(markers, i+1);
      
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
      colorTab.push_back(cvl::getRandomColor(30));
      newBlobs[i].draw(markersColor, colorTab[i]);
  #endif
      
    }
    
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    cv::imshow("markers", markersColor); if(runFrame==debugFrame||debugFrame==-1) cv::waitKey(0);
  #endif
    
    cv::Mat originalColor; cvtColor(splitOn, originalColor, cv::COLOR_GRAY2BGR);
    
    cv::watershed(originalColor, markers);
    
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    cv::Mat wshed(markers.size(), CV_8UC3);
  #endif
    
    // pulisco i pixel che facevano parte dei blob
    for(int i=0; i<newBlobs.size(); ++i) newBlobs[i].clear();
    
    // ciclo sull'immagine
    for(int i=0; i<markers.rows; ++i) {
      
      for(int j=0; j<markers.cols; ++j) {
        
        // mi prendo l'index
        int index = markers.at<int>(i,j);
        
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
        
        if(index == -1)
          wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
        else if(index == 0 || index > newBlobs.size())
          wshed.at<cv::Vec3b>(i,j) = cv::Vec3b(0,0,0);
        else
          wshed.at<cv::Vec3b>(i,j) = colorTab[index-1];
        
  #endif
        
        if(index > 0 && index <= newBlobs.size()) {
          newBlobs[index-1].add(j, i, original.at<uchar>(i,j));
        }
        
      }
      
    }
    
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    wshed = wshed + markersColor * 0.5;
    cv::imshow("markers", wshed); if(runFrame==debugFrame||debugFrame==-1) cv::waitKey(0);
  #endif
    
    // se per qualche motivo il watershed mi ha ucciso un blob rimetto quello originale
    // questo caso è successo sicuramente una volta in 20241206_ACQ4 CAM3 frame 1092
    // quando un blob, che era solo una riga (4x1), accanto al bordo, y=0, veniva ucciso
    for(int i=0; i<newBlobs.size(); ++i)
      if(newBlobs[i].pixels.size() == 0)
        newBlobs[i].pixels = blobs[i].pixels;

    blobs = newBlobs;
    
    blobs.finalize();
    
  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    ++runFrame;
  #endif
    
  }

} /* namespace */


#endif /* _H_CVL_BLOBSPLITTING_H_ */


