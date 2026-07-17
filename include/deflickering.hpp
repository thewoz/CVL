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

#ifndef _H_CVL_DEFLICKERING_H_
#define _H_CVL_DEFLICKERING_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

//***************************************************************************************************//
// asDeflickering
//***************************************************************************************************//
// Temporal deflickering utility operating on a sliding window of frames.
class asDeflickering {

private:

  std::vector<cv::Mat> imageList;

  cv::Mat mask;

  bool inited;
  bool invert;

  int deflickerMatrixSize;

  int halfDeflickerMatrixSize;

  int deflickerTime;
  
  int halfDeflickerTime;

  int deflickerWindows;

public:

  //***************************************************************************************************//
  // asDeflickering
  //***************************************************************************************************//
  // Construct an uninitialized deflickering helper.
  asDeflickering() { inited = false; }
  

  //***************************************************************************************************//
  // init
  //***************************************************************************************************//
  // Initialize deflickering parameters, mask, and input image list.
  void init(std::vector<cv::Mat> & images, int _deflickerWindows, int _deflickerTime, int _deflickerMatrixSize, std::string maskPath, bool _invert) {

    invert = _invert;

    deflickerWindows = _deflickerWindows;

    deflickerMatrixSize = _deflickerMatrixSize;

    if(deflickerMatrixSize % 2 == 0){
      cvl::log::error("asDeflickering::init() error: the deflicker matrix size must be odd\n");
    }

    halfDeflickerMatrixSize = (int)(deflickerMatrixSize * 0.5);

    deflickerTime = _deflickerTime;

    if(deflickerTime % 2 == 0 || deflickerTime > deflickerWindows){
      cvl::log::error("asDeflickering::init() error: the deflicker time must be odd and smaller than the deflicker window\n");
    }

    halfDeflickerTime = (int)(deflickerTime * 0.5);

    if((int)images.size() < deflickerWindows){
      cvl::log::error("asDeflickering::init() error: %zu images provided but the deflicker window is %d\n", images.size(), deflickerWindows);
    }

    // copy the input images into the working list
    imageList.resize(deflickerWindows);

    for(int i=0; i<deflickerWindows; ++i){

      if(images[i].empty() || images[i].type() != CV_8UC1 || images[i].size() != images[0].size()){
        cvl::log::error("asDeflickering::init() error: the input images must be non-empty CV_8UC1 images of the same size\n");
      }

      imageList[i] = images[i].clone();

    }

    // load the mask from file, or use an all-white mask when no path is given
    if(!maskPath.empty()){

      cvl::io::expandPath(maskPath);

      mask = cvl::open(maskPath, cv::IMREAD_GRAYSCALE);

      // deflicker() indicizza mask e immagini con le stesse coordinate
      if(mask.type() != CV_8UC1 || mask.size() != imageList[0].size()){
        cvl::log::error("asDeflickering::init() error: the mask must be a CV_8UC1 image of the same size as the input images\n");
      }

    } else { mask = cv::Mat(imageList[0].size(), CV_8UC1, cv::Scalar(255)); }

    inited = true;

  }

  //***************************************************************************************************//
  // deflicker
  //***************************************************************************************************//
  // Apply deflickering to the provided images using the configured window.
  void deflicker(std::vector<cv::Mat> & images){

    if(!inited){
      cvl::log::error("asDeflickering::deflicker() error: the class must be initialized first\n");
    }

    // le finestre temporali coprono [0, deflickerWindows-1]: con meno immagini
    // l'accesso a images[i] sarebbe out-of-bounds, con di più resterebbero
    // frame mai elaborati
    if((int)images.size() != deflickerWindows){
      cvl::log::error("asDeflickering::deflicker() error: %zu images provided but the deflicker window is %d\n", images.size(), deflickerWindows);
    }

    for(size_t i=0; i<images.size(); ++i){
      if(images[i].empty() || images[i].type() != CV_8UC1 || images[i].size() != mask.size()){
        cvl::log::error("asDeflickering::deflicker() error: the images must be non-empty CV_8UC1 images of the same size as the mask\n");
      }
    }

    int rS, rE, cS, cE;

    for(int r=0; r<mask.rows; ++r) {

	    for(int c=0; c<mask.cols; ++c) {

	      if(mask.at<uchar>(r,c) != 0) {

          getCenteredROI(r, c, rS, rE, cS, cE);

          int index = 0;

          do {

            int from;
            int to;

            getFromTo(index, from, to);

            double avg = 0;

            for(int i=from; i<=to; ++i)
              avg += getAvg(rS, rE, cS, cE, imageList[i]);

            avg /= (double) deflickerTime;

            for(int i=from; i<=to; ++i){

              double currentAvg = getAvg(rS, rE, cS, cE, imageList[i]);

              int target = round(imageList[i].at<uchar>(r,c) + (avg - currentAvg));

              if(target > 255) target = 255;
              if(target < 0) target = 0;

             if(invert)
               images[i].at<uchar>(r,c) = 255 - target;
             else
               images[i].at<uchar>(r,c) = target;

            }

            ++index;

          } while(index < (int)images.size());

        }

      }

    }

  }


private:


  //***************************************************************************************************//
  // getFromTo
  //***************************************************************************************************//
  // Compute temporal window bounds for the given index.
  void getFromTo(int i, int & from, int & to) {

    int last = deflickerWindows - 1;

    int f = i - halfDeflickerTime;
    int t = i + halfDeflickerTime;

    // keep the window size constant by shifting it inside [0, last]
    if(f < 0)    { t += -f;       f = 0; }
    if(t > last) { f -= t - last; t = last; }
    if(f < 0)      f = 0;

    from = f;
    to   = t;

  }



  //***************************************************************************************************//
  // getCenteredROI
  //***************************************************************************************************//
  // Compute a window of size deflickerMatrixSize centered at pixel (row, col),
  // shifted to stay inside the image bounds.
  void getCenteredROI(int row, int col, int & rS, int & rE, int & cS, int & cE) {

    int half = halfDeflickerMatrixSize;

    int lastRow = mask.rows - 1;
    int lastCol = mask.cols - 1;

    int r0 = row - half;  int r1 = row + half;
    int c0 = col - half;  int c1 = col + half;

    if(r0 < 0)       { r1 += -r0;          r0 = 0; }
    if(r1 > lastRow) { r0 -= r1 - lastRow; r1 = lastRow; }
    if(r0 < 0)         r0 = 0;

    if(c0 < 0)       { c1 += -c0;          c0 = 0; }
    if(c1 > lastCol) { c0 -= c1 - lastCol; c1 = lastCol; }
    if(c0 < 0)         c0 = 0;

    rS = r0;  rE = r1;
    cS = c0;  cE = c1;

  }



  //***************************************************************************************************//
  // getAvg
  //***************************************************************************************************//
  // Compute average intensity within the provided ROI.
  double getAvg(int rS, int rE, int cS, int cE, const cv::Mat & image) const {

    double avg = 0;

    for(int r=rS; r<=rE; ++r)
      for(int c=cS; c<=cE; ++c)
        avg += image.at<uchar>(r,c);

    return (avg / (double)(deflickerMatrixSize * deflickerMatrixSize));

  }



};

#endif
