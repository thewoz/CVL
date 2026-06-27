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

#ifndef _H_CVL_COUNTOURS_H_
#define _H_CVL_COUNTOURS_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdlib>
#include <cstdio>

#include <vector>

//*****************************************************************************/
// namespace
//*****************************************************************************/
namespace cvl::contours {
  
  enum { EXTERNAL, INTERNAL };
  
  //*****************************************************************************/
  // scale
  //*****************************************************************************/
  // Scale a contour about its centroid by scaleFactor.
  inline void scale(std::vector<cv::Point> & contour, double scaleFactor) {
    
    std::vector<cv::Point> scaledContour;

    if(contour.empty()) return;

    cv::Moments m = cv::moments(contour);

    if(m.m00 == 0.0) return;

    cv::Point2d center((m.m10 / m.m00), (m.m01 / m.m00));

    for(cv::Point & p : contour) {
      p.x = cvRound(center.x + scaleFactor * (p.x - center.x));
      p.y = cvRound(center.y + scaleFactor * (p.y - center.y));
    }
    
  }

 	//*****************************************************************************/
  // get
  //*****************************************************************************/
  // Extract external or internal contour pixels from a binary image.
  inline void get(const cv::Mat & image, bool mode, std::vector<cv::Point> & contours) {
    
    if(image.channels() != 1)
      cvl::log::error("cvl::contours::get() error: the input image must be a one channel image\n");
    
    contours.clear();
    
    if(mode == EXTERNAL){

      for(int y=0; y<image.rows; ++y) {

        const uchar * cur = image.ptr<uchar>(y);
        const uchar * up  = (y-1) >= 0          ? image.ptr<uchar>(y-1) : nullptr;
        const uchar * dn  = (y+1) < image.rows  ? image.ptr<uchar>(y+1) : nullptr;

        for(int x=0; x<image.cols; ++x) {

          // con !0 faccio l'esterno con ==0 l'interno
          if(cur[x] != 0) continue;

          if(up && up[x] != 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }

          if(dn && dn[x] != 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }

          if((x+1) < image.cols && cur[x+1] != 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }

          if((x-1) >= 0 && cur[x-1] != 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }

        }

      }

    }

    if(mode == INTERNAL){

      for(int y=0; y<image.rows; ++y) {

        const uchar * cur = image.ptr<uchar>(y);
        const uchar * up  = (y-1) >= 0          ? image.ptr<uchar>(y-1) : nullptr;
        const uchar * dn  = (y+1) < image.rows  ? image.ptr<uchar>(y+1) : nullptr;

        for(int x=0; x<image.cols; ++x) {

          // con !0 faccio l'esterno con ==0 l'interno
          if(cur[x] == 0) continue;

          if(up && up[x] == 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }

          if(dn && dn[x] == 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }

          if((x+1) < image.cols && cur[x+1] == 0){
            contours.push_back(cv::Point(x,y));
            continue;
          }

          if((x-1) >= 0 && cur[x-1] == 0){
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

