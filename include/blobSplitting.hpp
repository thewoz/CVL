/*
 * GNU GENERAL PUBLIC LICENSE
 *
 * Copyright (C) 2021-2026
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

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>
#include <random>
#include <utility>

//#define _DEBUG_CVL_BLOBSPLITTING_

//*****************************************************************************
// namespace cvl::blobsplitting
//*****************************************************************************
namespace cvl::blobsplitting {

  //*****************************************************************************
  // fixed
  //*****************************************************************************
  // Draw fixed-radius target circles for each blob barycenter.
  inline void fixed(cvl::blobs_t & blobs, cv::Mat & dst, int radius) {

    dst.setTo(cv::Scalar(0,0,0));

    cv::Scalar color = cv::Scalar(255, 255, 255);

    // per ogni blob disegno un cerchio intorno al baricentro
    for(size_t i=0; i<blobs.size(); ++i)
      cv::circle(dst, cv::Point(blobs[i].baricenter.x, blobs[i].baricenter.y), radius, color, -1);

  }

  //*****************************************************************************
  // namespace utils
  //*****************************************************************************
  namespace utils {

    //*****************************************************************************
    // untilSplit
    //*****************************************************************************
    // Iteratively threshold a single blob (toward dark or light) until it
    // splits into more than one connected component or reaches exitMass.
    inline void untilSplit(const cvl::blob_t & blob, const cv::Mat & splitOn, const cv::Mat & original, bool toDark, int exitMass, int minMass, int labelingNeighboursType, cvl::blobs_t & tmpBlobs) {

      tmpBlobs.clear();

      // immagini di appoggio
      cv::Mat blobImg = cv::Mat(blob.getImageSize(), CV_8UC1);
      cv::Mat thrsImg = cv::Mat(blob.getImageSize(), CV_8UC1);

      int threshold;

      int targetThreshold;

      // in base se devo andare verso il nero o meno
      if(toDark){
        blobImg.setTo(cv::Scalar(255,255,255));
        targetThreshold = blob.iMin;
        threshold       = blob.iMax;
      } else {
        blobImg.setTo(cv::Scalar(0,0,0));
        targetThreshold = blob.iMax;
        threshold       = blob.iMin;
      }

      // disegno il blob nell'immagine di appoggio
      blob.draw(blobImg, splitOn, blob.offset);

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

        // trovo le componenti connesse
        cvl::labeling::get(thrsImg, labelingNeighboursType, minMass, tmpBlobs, original, blob.offset);

        // se l'ho diviso o è rimasto un troppo piccolo o non è rimasto nulla esco
        if(tmpBlobs.size() > 1 || tmpBlobs.size() == 0 || (tmpBlobs.size() == 1 && (int)tmpBlobs[0].size() <= exitMass)) break;

        if(toDark) { --threshold; }
        else       { ++threshold; }

      } // ciclo sulla soglia

    }

    //*****************************************************************************
    // untilErode
    //*****************************************************************************
    // Erode a single blob (peeling internal contours) until its mass drops to
    // or below exitMass, then label the remaining mask.
    inline void untilErode(const cvl::blob_t & blob, const cv::Mat & original, int exitMass, int minMass, int labelingNeighboursType, cvl::blobs_t & tmpBlobs) {

      tmpBlobs.clear();

      int mass = (int)blob.size();

      // creo un immagine di appoggio
      cv::Mat thrsImg = cv::Mat::zeros(blob.getImageSize(), CV_8UC1);

      // ci disegno il blob sopra
      blob.drawMask(thrsImg, blob.offset);

      // ciclo
      for(; mass > exitMass; ) {

        std::vector<cv::Point> contours;

        cvl::contours::get(thrsImg, cvl::contours::INTERNAL, contours);

        if(mass - (int)contours.size() >= minMass) {

          for(size_t j=0; j<contours.size(); ++j)
            thrsImg.at<uchar>(contours[j]) = 0;

          mass -= (int)contours.size();

        } else { break; }

      }

      // rietichetto
      cvl::labeling::get(thrsImg, labelingNeighboursType, minMass, tmpBlobs, original, blob.offset);

    }

    //*****************************************************************************
    // split
    //*****************************************************************************
    // Split a single blob into smaller ones (via erosion or iterative thresholding).
    // NOTA: il blob deve essere "finalized" (blob_t::finalize() già chiamata):
    // untilSplit/untilErode usano iMin/iMax/roi/offset, che altrimenti non sono significativi.
    inline void split(const cvl::blob_t & blob, const cv::Mat & splitOn, const cv::Mat & original, cvl::blobs_t & blobsFound, const std::string & mode, int enterMass, int exitMass, int minMass, bool recursiveSplitting, int labelingNeighboursType) {

      if(exitMass < minMass) {
        cvl::log::error("cvl::blobsplitting::split() error: exitMass must be greater than or equal to minMass\n");
      }

      if(blob.size() == 0 || blob.roi.width <= 0 || blob.roi.height <= 0) {
        cvl::log::error("cvl::blobsplitting::split() error: the blob is empty or not finalized (call blob_t::finalize() first)\n");
      }

      // vettore che contiene i blob che saranno trovati
      blobsFound.clear();

      // vettore che contiene i vari blob che vanno divisi
      cvl::blobs_t blobsToSplit;

      blobsToSplit.add(blob);

      // vettore con i blob temporanei trovati a ogni passo
      cvl::blobs_t tmpBlobs;

      // ciclo su tutti i blob
      for(size_t i=0; i<blobsToSplit.size(); ++i) {

        if(mode.compare("ERODE") == 0) {
          untilErode(blobsToSplit[i], original, exitMass, minMass, labelingNeighboursType, tmpBlobs);
        } else {
          bool toDark = (mode.compare("TO_WHITE") != 0);
          untilSplit(blobsToSplit[i], splitOn, original, toDark, exitMass, minMass, labelingNeighboursType, tmpBlobs);
        }

        // se alla fine rimasto un solo cluster o nessuno aggiungo l'originale al risulato
        if(tmpBlobs.size() <= 1) {

          blobsFound.add(std::move(blobsToSplit[i]));

          // se il blob invece è stato diviso
        } else {

          // ciclo su i blob trovati
          for(size_t j=0; j<tmpBlobs.size(); ++j) {

            // se il blob trovato e' ancora abbastanza grande e lo splitting e' ricorsivo lo aggiungo ai blob da dividere
            if((int)tmpBlobs[j].size() > enterMass && recursiveSplitting) {
              blobsToSplit.add(std::move(tmpBlobs[j]));
              // altrimenti lo aggiungo al risultato
            } else {
              blobsFound.add(std::move(tmpBlobs[j]));
            }

          } // for j

        } // else

      } // for i

    }

  } /* namespace utils */

  //*****************************************************************************
  // get
  //*****************************************************************************
  // Split blobs using the chosen mode, optionally refining with watershed segmentation.
  inline void get(cvl::blobs_t & blobs, const cv::Mat & splitOn, const cv::Mat & original, const std::string & mode, int enterMass, int exitMass, int minMass, bool recursiveSplitting, bool useWatershed, int radius, int labelingNeighboursType){

  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    static int debugFrame = 22; // va modificato il valore di questo
    static int runFrame = 0;
  #endif

    if(mode.compare("ERODE") != 0 && mode.compare("TO_DARK") != 0 &&  mode.compare("TO_WHITE") != 0 && mode.compare("FIXED") != 0) {

      cvl::log::error("cvl::blobsplitting::get() error: unknown split mode '%s'\n"
                      "You can choose between:\n"
                      "      ERODE, TO_DARK, TO_WHITE, FIXED\n", mode.c_str());

    }

    // FIXED non passa dallo split iterativo: disegna cerchi a raggio fisso
    // sui baricentri e rietichetta direttamente sull'immagine originale
    if(mode.compare("FIXED") == 0) {

      cv::Mat mask(original.size(), CV_8UC1);

      fixed(blobs, mask, radius);

      cvl::labeling::get(mask, labelingNeighboursType, minMass, blobs, original);

      return;

    }

    // vettore di appoggio con il risultato finale
    cvl::blobs_t newBlobs;

    // provo a dividere ogni blob
    for(size_t i=0; i<blobs.size(); ++i) {

      // se e' piu' grande della massa di entrata lo provo a dividere
      if((int)blobs[i].size() > enterMass) {

        cvl::blobs_t blobsSplited;

        // provo a splittarlo
        utils::split(blobs[i], splitOn, original, blobsSplited, mode, enterMass, exitMass, minMass, recursiveSplitting, labelingNeighboursType);

        // ciclo su tutti i blob trovati
        for(size_t j=0; j<blobsSplited.size(); ++j)
          newBlobs.add(std::move(blobsSplited[j]));

      } else { newBlobs.add(blobs[i]); }

    }

    // se non devo raffinare con il watershed mi fermo qui
    if(!useWatershed) {

      blobs = std::move(newBlobs);

      blobs.finalize();

      return;

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

      // mi calcolo un raggio che lo racchiude (2.5 volte quello del cerchio di pari area)
      double r = sqrt(blobs[i].size() / M_PI) * 2.5;

      // disegno un cerchio nero per dire che quell'area non so cosa sia
      cv::circle(markers, blobs[i].baricenter, r, cv::Scalar(0), cv::FILLED);

      //TODO: forse meglio questo da valutare allargando anche i punti
      //cv::rectangle(markers, blobs[i].rotatedRect.boundingRect(), cv::Scalar(0), cv::FILLED);

  #ifdef _DEBUG_CVL_BLOBSPLITTING_
      cv::circle(markersColor, blobs[i].baricenter, r, cvl::White, cv::FILLED);
      //cv::rectangle(markers, blobs[i].rotatedRect.boundingRect(), cv::Scalar(0), cv::FILLED);
  #endif

    }

    // ciclo sui blob dopo lo split e metto i seed
    for(int i=0; i<(int)newBlobs.size(); ++i) {

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

    // vettore dove metto i nuovi i pixels dei vari blob
    std::vector<std::vector<cv::Point3i>> tmpPixels(newBlobs.size());

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

        if(index > 0 && index <= (int)newBlobs.size()) {
          //newBlobs[index-1].add(j, i, original.at<uchar>(i,j));
          tmpPixels[index-1].push_back(cv::Point3i(j, i, original.at<uchar>(i,j)));
        }

      }

    }

  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    wshed = wshed + markersColor * 0.5;
    cv::imshow("markers", wshed); if(runFrame==debugFrame||debugFrame==-1) cv::waitKey(0);
  #endif

    // se il watershed mi ha ucciso un blob rimetto quello originale
    // questo caso è successo sicuramente una volta in 20241206_ACQ4 CAM3 frame 1092
    // quando un blob, che era solo una riga (4x1), accanto al bordo, y=0, veniva ucciso
    for(int i=0; i<(int)newBlobs.size(); ++i) {

      // solo se ci stanno i pixel nuovi
      if(tmpPixels[i].size() != 0) {

        newBlobs[i].clear();

        newBlobs[i].pixels = tmpPixels[i];

      }

    }

    blobs = std::move(newBlobs);

    blobs.finalize();

  #ifdef _DEBUG_CVL_BLOBSPLITTING_
    ++runFrame;
  #endif

  }

} /* namespace cvl::blobsplitting */


#endif /* _H_CVL_BLOBSPLITTING_H_ */
