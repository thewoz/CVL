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


#ifndef _H_CVL_SUBPIXEL_H_
#define _H_CVL_SUBPIXEL_H_


#include <cstdio>
#include <cstdlib>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

//****************************************************************************/
// cvl::subpixel
//****************************************************************************/
namespace cvl::subpixel {
  
  /*****************************************************************************/
   // get() -
   /*****************************************************************************/
   void get(const cv::Mat image, std::vector<cv::Point2f> & corners, int roiSize = 15, const std::string & algorithm = "old") {
     
     if(corners.size() > 0) {
     
       // Criteri di stop
       cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.0001);
       
       // Cerco i subpixel con l'algoritmo vecchio
       if(!algorithm.compare("old"))
         cv::cornerSubPix(image, corners, cv::Size(roiSize,roiSize), cv::Size(-1,-1), criteria);
       
       // Cerco i subpixel con l'algoritmo nuovo
       if(!algorithm.compare("new"))
         cv::find4QuadCornerSubpix(image, corners, cv::Size(roiSize,roiSize));
     
     }
       
   }

} /* namespace cvl::subpixels */


#endif /* _H_CVL_SUBPIXEL_H_ */
