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

#ifndef _H_CVL_CONVERT_H_
#define _H_CVL_CONVERT_H_

#include <cstdlib>
#include <cstdio>

#include <opencv2/opencv.hpp>

//*****************************************************************************/
// namespace cvl::convert
//*****************************************************************************/
namespace cvl::convert {
  
  //****************************************************************************/
  // from16to8()
  //****************************************************************************/
  void from16to8(const cv::Mat & src, cv::Mat & dst) {
    
    static bool firstImage = true;
    
    static double MinPix = INT_MAX;
    static double MaxPix = 0;

    if(firstImage) {
      
      int nHistSize = 65536;
      float fRange[] = { 0.0, 65536};
      const float * fHistRange = { fRange };

      cv::Mat matHist;
      cv::calcHist(&src, 1, 0, cv::Mat(), matHist, 1, &nHistSize, &fHistRange);
      
      for(int i=0; i<matHist.rows; ++i) {
        MinPix = i;
        if(matHist.at<float>(i) != 0) break;
      }
      
      MinPix -= MinPix * 0.2;
      
      int skip = 0;
      
      for(int i=matHist.rows-1; i>=0; --i) {
        MaxPix = i;
        if(matHist.at<float>(i) != 0) skip++;
        if(skip == 2) break;
      }
      
      firstImage = false;
          
    }
   
    double alpha = 255 / (MaxPix-MinPix);
    double beta = -255*MinPix/(MaxPix-MinPix);
    
    src.convertTo(dst, CV_8UC1, alpha, beta);
      
  }

} /* namespace cvl::convert */

#endif /* _H_CVL_CONVERT_H_ */
