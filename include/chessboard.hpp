/*
 * MIT License
 *
 * Copyright © 2019 CoBBS
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


#ifndef _H_CVL_CORNERS_H_
#define _H_CVL_CORNERS_H_

#include <cstdio>
#include <cstdlib>

#include <vector>

#include <opencv2/opencv.hpp>

#include <mpl/stdio.hpp>
#include <mpl/opencv.hpp>

#include <cvl/subpixel.hpp>

//****************************************************************************/
// cvl::chessboard
//****************************************************************************/
namespace cvl::chessboard {
  
  //****************************************************************************/
  // get() -
  //****************************************************************************/
  bool get(const cv::Mat & image, const cv::Size & patternSize, std::vector<cv::Point2f> & corners, const std::string & mode, const std::string & algorithm = "old") {
        
    bool patternfound = false;
    
    // Se devo trovare la scacchiera
    if(!mode.compare("checkboard")) {
      
      // chessBoard flags
      int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;
      
      // Cerco i subpixel con l'algoritmo nuovo
      if(!algorithm.compare("new")){
        // cerco la scacchiera
        patternfound = cv::findChessboardCornersSB(image, patternSize, corners, cv::CALIB_CB_EXHAUSTIVE | cv::CALIB_CB_ACCURACY);
      
      }
      
      // Cerco i subpixel con l'algoritmo vecchio
      if(!algorithm.compare("old")){
        
        // cerco la scacchiera
        patternfound = cv::findChessboardCorners(image, patternSize, corners, chessBoardFlags | cv::CALIB_CB_FAST_CHECK);
             
        // Se non l'ho trovata ci riprovo senza la flag cv::CALIB_CB_FAST_CHECK
        if(!patternfound)
          patternfound = cv::findChessboardCorners(image, patternSize, corners, chessBoardFlags);
        
      }
      
      // Se l'ho trovata mi calcolo i subpixels
      if(patternfound) {
        cvl::subpixel::get(image, corners);
      }
      
      //FIXME: non controllo old new
      
      return patternfound;
      
    }
  
    // Se devo cercare i cerchi
    if(mode.compare("circles")) {
      
      cv::SimpleBlobDetector::Params params;
      
      params.thresholdStep = 10;
      params.minThreshold = 20;
      params.maxThreshold = 250;
      params.minRepeatability = 2;
      params.minDistBetweenBlobs = 10;
      
      params.filterByColor = false;
      params.blobColor = 0;
      
      params.filterByArea = true;
      params.minArea = 10000;
      params.maxArea = 100000; 
      
      params.filterByCircularity = false;
      params.minCircularity = 0.8f;
      params.maxCircularity = std::numeric_limits<float>::max();
      
      params.filterByInertia = false;
      //minInertiaRatio = 0.6;
      params.minInertiaRatio = 0.1f;
      params.maxInertiaRatio = std::numeric_limits<float>::max();
      
      params.filterByConvexity = true;
      //minConvexity = 0.8;
      params.minConvexity = 0.95f;
      params.maxConvexity = std::numeric_limits<float>::max();
      
      patternfound = cv::findCirclesGrid(image, patternSize, corners, cv::CALIB_CB_ASYMMETRIC_GRID | cv::CALIB_CB_CLUSTERING, cv::SimpleBlobDetector::create(params));
      
      return patternfound;
      
    }
    
    fprintf(stderr, "operational mode in cobbs::checkbort not found\n");
    
    abort();
        
  }
  
  //****************************************************************************/
  // get() -
  //****************************************************************************/
  bool get(const std::string & imagePath, const cv::Size & patternSize, std::vector<cv::Point2f> & corners, const std::string & mode, const std::string & algorithm = "old") {
    
    cv::Mat image = cv::open(imagePath, cv::IMREAD_GRAYSCALE);

    return get(image, patternSize, corners, mode, algorithm);
    
  }
  
  //****************************************************************************/
  // saveImage() -
  //****************************************************************************/
  void saveImage(cv::Mat image, const std::string & file, const std::vector<cv::Point2f> & corners, const cv::Size & boardSize, bool status) {
    
    if(image.type()==CV_8UC1) cvtColor(image, image, cv::COLOR_GRAY2RGB);
    
    // Disegno i coornes trovati
    cv::drawChessboardCorners(image, boardSize, cv::Mat(corners), status);
    
    // Salvo l'immagine
    cv::save(image, file);
    
  }
  
  //****************************************************************************/
  // saveCorners() -
  //****************************************************************************/
  void saveCorners(FILE * output, const std::string & file, const std::vector<cv::Point2f> & corners, const cv::Size & boardSize) {

    //size_t rows = boardSize.height;
    size_t cols = boardSize.width;

    // Salvo le coordinate dei corners
    for(size_t i=0; i<corners.size(); ++i) {

      size_t y = int(i/cols);
      size_t x = i - (y * cols);

      fprintf(output, "%s %lu %lu 0 %.8f %.8f\n", (mpl::io::subdirName(file) + "_" + mpl::io::basename(file)).c_str(), x, y, corners[i].x, corners[i].y);

    }
    
    fflush(output);
    
  }

} /* namespace cvl::chessboard */


#endif /* _H_CVL_CORNERS_H_ */
