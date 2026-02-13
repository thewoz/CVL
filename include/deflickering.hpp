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

#ifndef _H_asDeflickering_H_
#define _H_asDeflickering_H_

#include <cstdlib>
#include <cstdio>

#include <vector>

#include <opencv2/opencv.hpp>

//***************************************************************************************************//
// asDeflickering
//***************************************************************************************************//
/// Temporal deflickering utility operating on a sliding window of frames.
class asDeflickering {

private:

  std::vector<cv::Mat> imageList;

  cv::Mat mask;

  bool inited;
  bool invert;

  uint32_t deflickerMatrixSize;

  uint32_t halfDeflickerMatrixSize;

  uint32_t deflickerTime;
  
  uint32_t halfDeflickerTime;

  uint32_t deflickerWindows;

public:

  //***************************************************************************************************//
  // asDeflickering
  //***************************************************************************************************//
  /// Construct an uninitialized deflickering helper.
  asDeflickering() { inited = false; }
  

  //***************************************************************************************************//
  // init
  //***************************************************************************************************//
  /// Initialize deflickering parameters, mask, and input image list.
  void init(std::vector<cv::Mat> & images, uint32_t _deflickerWindows, uint32_t _deflickerTime, uint32_t _deflickerMatrixSize, std::string maskPath, bool _invert) {

    for(uint32_t i=0; i<deflickerWindows; ++i)
      imageList[i] = images[i].clone();
        
    invert = _invert;
    
    if(maskPath.empty()){
      
      mpl::io::expandPath(maskPath);

      mask = cvl::open(maskPath, cv::IMREAD_GRAYSCALE);
      
    } else { mask = cv::Mat(imageList[0].size(), CV_8UC1, cv::Scalar(255,255,255)); }
    
    deflickerWindows = _deflickerWindows;

    imageList.resize(deflickerWindows);
  
    deflickerMatrixSize = _deflickerMatrixSize;

    if(deflickerMatrixSize % 2 == 0){
      fprintf(stderr, "error the size of the matrix in deflickeing should be odd\n");
      exit(EXIT_FAILURE);
    }

    halfDeflickerMatrixSize= (uint32_t)(deflickerMatrixSize * 0.5);;
  
    deflickerTime = _deflickerTime;

    if(deflickerTime % 2 == 0 || deflickerTime > deflickerWindows){
      fprintf(stderr, "the deflicker time should be odd and less of the deflicker window\n");
      exit(EXIT_FAILURE);
    }
  
    halfDeflickerTime = (uint32_t)(deflickerTime * 0.5);

    inited = true;

  }

  

  //***************************************************************************************************//
  // initImages
  //***************************************************************************************************//
//  void initImages(std::vector<cv::Mat> & images) {
//
//    if(!inited){
//      fprintf(stderr, "asDeflickering class must be initializaetd\n");
//      exit(EXIT_FAILURE);
//    }
//
//    for(uint32_t i=0; i<deflickerWindows; ++i)
//      imageList[i] = images[i].clone();
//
//    deflicker(images);
//
//  }



  //***************************************************************************************************//
  // addImage
  //***************************************************************************************************//
//  void addImage(cv::Mat & image) {
//
//    if(!inited){
//      fprintf(stderr, "asDeflickering class must be initializaetd\n");
//      exit(EXIT_FAILURE);
//    }
//
//    /* shift the others */
//    for(uint32_t j=1; j<deflickerWindows; ++j)
//      imageList[j-1] = imageList[j];
//
//    /* load the new one */
//    imageList[deflickerWindows-1] = image.clone();
//
//  }



  //***************************************************************************************************//
  // deflicker
  //***************************************************************************************************//
  /// Apply deflickering to the provided images using the configured window.
  void deflicker(std::vector<cv::Mat> & images){

    if(!inited){
      fprintf(stderr, "asDeflickering class must be initializaetd\n");
      exit(EXIT_FAILURE);
    }

    uint32_t xS, xE, yS, yE;

    for(uint32_t h=0; h<mask.rows; ++h) {

	    for(uint32_t w=0; w<mask.cols; ++w) {

	      if(mask.at<uchar>(w,h) != 0) {

          getCenteredROI(w, h, xS, xE, yS, yE);

          uint32_t index = 0;

          do {

            uint32_t from;
            uint32_t to;

            getFromTo(index, from, to);

            double avg = 0;

            for(uint32_t i=from; i<=to; ++i)
              avg += getAvg(xS, xE, yS, yE, imageList[i]);

            avg /= (double) deflickerTime;

            for(uint32_t i=from; i<=to; ++i){

              double currentAvg = getAvg(xS, xE, yS, yE, imageList[i]);

              int target = round(imageList[i].at<uchar>(w,h) + (avg - currentAvg));

              if(target > 255) target = 255;
              if(target < 0) target = 0;

             if(invert)
               images[i].at<uchar>(w,h) = 255 - target;
             else
               images[i].at<uchar>(w,h) = target;

            }

            ++index;

          } while(index<images.size());

        }

      }

    }

  }


private:


  //***************************************************************************************************//
  // getFromTo
  //***************************************************************************************************//
  /// Compute temporal window bounds for the given index.
  void getFromTo(uint32_t i, uint32_t & from, uint32_t & to) {

    from = i - halfDeflickerTime;
    to   = i + halfDeflickerTime;

    if(from < 0) { to += -from; from = 0; }
    if(to >= deflickerWindows-1) { from -= to - deflickerWindows + 1; to = deflickerWindows - 1; }

  }



  //***************************************************************************************************//
  // getCenteredROI
  //***************************************************************************************************//
  /// Compute a centered ROI around (x, y), clamped to image bounds.
  void getCenteredROI(uint32_t x, uint32_t y, uint32_t & xS, uint32_t & xE, uint32_t & yS, uint32_t & yE) {

    xS = x - halfDeflickerMatrixSize;
    xE = x + halfDeflickerMatrixSize;

    yS = y - halfDeflickerMatrixSize;
    yE = y + halfDeflickerMatrixSize;

    if(xS < 0) { xE += -xS; xS = 0; }
    if(xE >= mask.cols) { xS -= xE - mask.cols + 1; xE = mask.rows - 1; }

    if(yS < 0) { yE += -yS; yS = 0; }
    if(yE >= mask.rows) { yS -= yE - mask.cols + 1; yE = mask.rows - 1; }

  }



  //***************************************************************************************************//
  // getAvg
  //***************************************************************************************************//
  /// Compute average intensity within the provided ROI.
  double getAvg(uint32_t xS, uint32_t xE, uint32_t yS, uint32_t yE, const cv::Mat & image) const {

    double avg = 0;

    for(uint32_t x=xS; x<=xE; ++x)
      for(uint32_t y=yS; y<=yE; ++y)
        avg += image.at<uchar>(x,y);

    return (avg / (double)(deflickerMatrixSize * deflickerMatrixSize));

  }



};
























#endif
