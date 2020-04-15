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
