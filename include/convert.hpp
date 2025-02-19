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

  enum { LINEAR, LINEAR_MINMAX, LINEAR_CUTWINGS, LINEAR_CUTEXTREMAL };

  //****************************************************************************/
  // from16to8()
  //****************************************************************************/
  void from16to8(const cv::Mat & src, cv::Mat & dst, int mode, double alpha, double beta) {

    src.convertTo(dst, CV_8UC1, alpha, beta);

  }

  //****************************************************************************/
  // from16to8()
  //****************************************************************************/
  std::pair<double,double> from16to8(const cv::Mat & src, cv::Mat & dst, int mode, double value = 0) {
  
    if(mode == LINEAR) {
      double alpha = 255 / 65535.0;
      double beta  = 0;
      src.convertTo(dst, CV_8UC1, alpha, beta);
      return std::pair<double,double>(alpha, beta);
    }
    
    
    if(mode == LINEAR_MINMAX) {

      double min, max;
      
      cv::Mat histogram;
      cvl::histogram16bit(src, histogram, min, max);
      
      //printf("LINEAR_MINMAX min %f max %f\n", min, max);
      
      double alpha = 255 / (max-min);
      double beta  = - (255*min) / (max-min);
      
      src.convertTo(dst, CV_8UC1, alpha, beta);
      
      return std::pair<double,double>(alpha, beta);

    }
    
    if(mode == LINEAR_CUTWINGS) {
      
      cv::Mat histogram;
      cvl::histogram16bit(src, histogram);
      
      double min = 0;
      double counterMin = 0;
      
      double max = 65535;
      double counterMax = 0;

      double target = (src.rows * src.cols) * value;
      
      for(int i=0; i<histogram.rows; ++i) {
        min = i;
        counterMin += histogram.at<float>(i);
        if(counterMin >= target) break;
      }

      for(int i=histogram.rows-1; i>=0; --i) {
        max = i;
        counterMax += histogram.at<float>(i);
        if(counterMax >= target) break;
      }
      
      //printf("LINEAR_CUTWINGS min %f max %f\n", min, max);
      
      double alpha = 255 / (max-min);
      double beta  = - (255*min) / (max-min);
      
      src.convertTo(dst, CV_8UC1, alpha, beta);
      
      return std::pair<double,double>(alpha, beta);

    }
  
    if(mode == LINEAR_CUTEXTREMAL) {
      
      cv::Mat histogram;
      cvl::histogram16bit(src, histogram);
      
      double min = 0;
      double max = 65535;
      
      int skip = 0;
      
      for(int i=0; i<histogram.rows; ++i) {
        min = i;
        if(histogram.at<float>(i) != 0) skip++;
        if(skip == 2) break;
      }
      
      skip = 0;
      
      for(int i=histogram.rows-1; i>=0; --i) {
        max = i;
        if(histogram.at<float>(i) != 0) skip++;
        if(skip == 2) break;
      }
      
      //printf("LINEAR_CUTEXTREMAL min %f max %f\n", min, max);
      
      double alpha = 255 / (max-min);
      double beta  = - (255*min) / (max-min);
      
      src.convertTo(dst, CV_8UC1, alpha, beta);
      
      return std::pair<double,double>(alpha, beta);

    }
    
    fprintf(stderr, "error in cvl::from16to8(): mode not recognized\n");
    abort();
    
  }

} /* namespace cvl::convert */

#endif /* _H_CVL_CONVERT_H_ */
