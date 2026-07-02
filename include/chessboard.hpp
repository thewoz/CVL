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

#ifndef _H_CVL_CHESSBOARD_H_
#define _H_CVL_CHESSBOARD_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdio>
#include <cstdlib>

#include <vector>

//****************************************************************************/
// cvl::chessboard
//****************************************************************************/
namespace cvl::chessboard {
  
  //****************************************************************************/
  // get() -
  //****************************************************************************/
  // Detect a checkerboard or circles grid from an image.
  inline bool get(const cv::Mat & image, const cv::Size & patternSize, std::vector<cv::Point2f> & corners, const std::string & mode, const std::string & algorithm = "old") {
        
    bool patternfound = false;
    
    // Se devo trovare la scacchiera
    if(!mode.compare("checkerboard")) {
      
      // Cerco i subpixel con l'algoritmo nuovo
      if(!algorithm.compare("new")) {
        
        // cerco la scacchiera
        patternfound = cv::findChessboardCornersSB(image, patternSize, corners, cv::CALIB_CB_EXHAUSTIVE | cv::CALIB_CB_ACCURACY);
        
        return patternfound;
        
      }
      
      // Cerco i subpixel con l'algoritmo vecchio
      if(!algorithm.compare("old")) {
        
        // cerco la scacchiera
        patternfound = cv::findChessboardCorners(image, patternSize, corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK);
        
        return patternfound;
        
      }
      
      cvl::log::error("cvl::chessboard::get() error: algorithm not found\n");

      return false;
      
    }
    
    if(!mode.compare("circles")) {
      
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
    
    cvl::log::error("cvl::chessboard::get() error: operational mode '%s' not found\n", mode.c_str());

    return false;

  }
  
  //****************************************************************************/
  // get() -
  //****************************************************************************/
  // Load an image from disk and detect a checkerboard or circles grid.
  inline bool get(const std::string & imagePath, const cv::Size & patternSize, std::vector<cv::Point2f> & corners, const std::string & mode, const std::string & algorithm = "old") {
    
    cv::Mat image = cvl::open(imagePath, cv::IMREAD_GRAYSCALE);

    return get(image, patternSize, corners, mode, algorithm);
    
  }
  
  //****************************************************************************/
  // saveImage() -
  //****************************************************************************/
  // Save an image with detected corners drawn.
  inline void saveImage(cv::Mat image, const std::string & file, const std::vector<cv::Point2f> & corners, const cv::Size & boardSize, bool status) {
    
    if(image.type()==CV_8UC1) cvtColor(image, image, cv::COLOR_GRAY2RGB);
    
    // Disegno i coornes trovati
    cv::drawChessboardCorners(image, boardSize, cv::Mat(corners), status);
    
    // Salvo l'immagine
    cvl::save(image, file);
    
  }
  
  //****************************************************************************/
  // saveCorners() -
  //****************************************************************************/
  // Save detected corners to a stream in a simple text format.
  inline void saveCorners(FILE * output, const std::string & file, const std::vector<cv::Point2f> & corners, const cv::Size & boardSize) {

    //size_t rows = boardSize.height;
    size_t cols = boardSize.width;

    // Salvo le coordinate dei corners
    for(size_t i=0; i<corners.size(); ++i) {

      size_t y = int(i/cols);
      size_t x = i - (y * cols);

      fprintf(output, "%s %lu %lu 0 %.8f %.8f\n", (cvl::io::subdirName(file) + "_" + cvl::io::basename(file)).c_str(), x, y, corners[i].x, corners[i].y);

    }
    
    fflush(output);
    
  }

} /* namespace cvl::chessboard */


//****************************************************************************/
// namespace cvl
//****************************************************************************/
namespace cvl {

  //****************************************************************************/
  // chessboard_t
  //****************************************************************************/
  // Helper container for object/image point correspondences.
  class chessboard_t {
    
  public:
    
    std::vector<std::vector<cv::Point3d>> objectPoints;
    std::vector<std::vector<cv::Point2d>> imagePoints;
    
    //****************************************************************************/
    // scale
    //****************************************************************************/
    // Scale all object points by a constant factor.
    void scale(double factor) {
      
      for(int i=0; i<(int)objectPoints.size(); ++i) {

        for(int j=0; j<(int)objectPoints[i].size(); ++j) {
          
          objectPoints[i][j].x *= factor;
          objectPoints[i][j].y *= factor;
          objectPoints[i][j].z *= factor;
                        
        }
      
      }
      
    }
    
    //****************************************************************************/
    // load
    //****************************************************************************/
    // Load calibration points from a text file.
    void load(std::string filepath) {
      
      cvl::loader loader;
      loader(filepath, "1 2 3 4 5 6", this, &chessboard_t::filler);
      
    }
    
  private:

    // ultimo frame letto dal loader (stato per-istanza, non condiviso)
    std::string currentFrame;

    //****************************************************************************/
    // filler
    //****************************************************************************/
    // Loader callback for populating object/image points.
    void filler(const cvl::loader::data & arguments) {

      if(objectPoints.empty() || arguments.get<std::string>(0).compare(currentFrame)) {

        objectPoints.push_back(std::vector<cv::Point3d>());
        imagePoints.push_back(std::vector<cv::Point2d>());

        currentFrame = arguments.get<std::string>(0);

      }
      
      cv::Point3d objectPoint;
      
      objectPoint.x = arguments.get<double>(1);
      objectPoint.y = arguments.get<double>(2);
      objectPoint.z = arguments.get<double>(3);
      
      objectPoints.back().push_back(objectPoint);
      
      cv::Point2d imagePoint;
      
      imagePoint.x = arguments.get<double>(4);
      imagePoint.y = arguments.get<double>(5);
      
      imagePoints.back().push_back(imagePoint);
      
    }
    
  }; /* class chessboard_t */

} /* namespace */

#endif /* _H_CVL_CHESSBOARD_H_ */
