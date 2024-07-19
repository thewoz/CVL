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

#ifndef _H_CVL_COUNTOURS_H_
#define _H_CVL_COUNTOURS_H_

#include <cstdlib>
#include <cstdio>

#include <vector>

#include <opencv2/opencv.hpp>


//*****************************************************************************/
// namespace
//*****************************************************************************/
namespace cvl::contours {
  
  enum { EXTERNAL, INTERNAL };
  
 	//*****************************************************************************/
  // get
  //*****************************************************************************/
  void get(const cv::Mat & image, bool mode, std::vector<cv::Point> & contours) {
    
    if(image.channels() != 1)
      mpl::log::error("The destination image must have one channel image.\n");
    
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


