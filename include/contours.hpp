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

#ifndef _H_CVL_COUNTOURS_H_
#define _H_CVL_COUNTOURS_H_

#include <cstdlib>
#include <cstdio>

#include <vector>

#include <opencv2/opencv.hpp>


/*****************************************************************************/
// namespace
/*****************************************************************************/
namespace cvl::contours {
  
  enum { EXTERNAL, INTERNAL };
  
 	/*****************************************************************************/
  // get
  /*****************************************************************************/
  void get(const cv::Mat & image, bool mode, std::vector<cv::Point> & contours, const cv::Point & offset = cv::Point(0,0)) {
    
    if(image.channels() != 1)
      mpl::log::error("The destination image must have one channel image.\n");
    //  cv::Point offset_i;
    
    //  offset_i.x = offset.x - 1;
    //  offset_i.y = offset.y - 1;
    
   // cv::Mat workImg = cv::Mat::zeros(image.rows + 2, image.cols + 2, CV_8UC1);
    
   // image.copyTo(workImg(cv::Rect(1, 1, image.cols, image.rows)));
    
    // drawMask(image, offset_i);
    
    contours.clear();
    
    if(mode == EXTERNAL){
      
      for(int32_t y=0; y<image.rows; ++y) {
        
        for(int32_t x=0; x<image.cols; ++x) {
          
          // con !0 faccio l'esterno con ==0 l'interno
          if(image.at<uchar>(y,x) != 0) continue;
          
          if((y-1) >= 0 && image.at<uchar>(y-1,x) != 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }
          
          if((y+1) < image.rows && image.at<uchar>(y+1,x) != 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }
          
          if((x+1) < image.cols && image.at<uchar>(y,x+1) != 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }
          
          if((x-1) >= 0 && image.at<uchar>(y,x-1) != 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }
          
        }
        
      }
      
    }
    
    if(mode == INTERNAL){
      
      for(int32_t y=0; y<image.rows; ++y) {
        
        for(int32_t x=0; x<image.cols; ++x) {
          
          // con !0 faccio l'esterno con ==0 l'interno
          if(image.at<uchar>(y,x) == 0) continue;
          
          if((y-1) >= 0 && image.at<uchar>(y-1,x) == 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }
          
          if((y+1) < image.rows && image.at<uchar>(y+1,x) == 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }
          
          if((x+1) < image.cols && image.at<uchar>(y,x+1) == 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }
          
          if((x-1) >= 0 && image.at<uchar>(y,x-1) == 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }
          
        }
        
      }
      
    }
    
//    for(uint32_t i=0; i<contours.size(); ++i){
//      contours[i].x -= 1;
//      contours[i].y -= 1;
//    }
    
  }
  
} /* namespace */

#endif /* _H_CVL_COUNTOURS_H_ */


