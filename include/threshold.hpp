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

#ifndef _H_CVL_THRESHOLD_H_
#define _H_CVL_THRESHOLD_H_

#include <cstdlib>
#include <cstdio>

#include <string>

#include <opencv2/opencv.hpp>

/*****************************************************************************/
// namespace
/*****************************************************************************/
namespace cvl::threshold {

  enum { WHIT_ZERO, SKIP_ZERO };
  
  /*****************************************************************************/
  // namespace
  /*****************************************************************************/
  namespace utils {
    
    /*****************************************************************************/
    // median
    /*****************************************************************************/
    int median(const cv::Mat & src, bool skipZero, const cv::Mat & mask) {
      
      if(src.channels() > 1){
        fprintf(stderr, "The destination image not have one channel.\n");
        exit(EXIT_FAILURE);
      }
      
      int median_index;
      
      if(!mask.empty()){
        
        if(skipZero==SKIP_ZERO){
          
          median_index = 0;
          
          for(int y=0; y<src.rows; ++y)
            for(int x=0; x<src.cols; ++x)
              if(mask.at<uchar>(y,x) != 0 && src.at<uchar>(y,x) != 0)
                median_index++;
          
        } else { median_index = (int)(cv::countNonZero(mask) * 0.5); }
        
      } else {
        
        (skipZero==SKIP_ZERO) ? median_index = (int)(cv::countNonZero(src) * 0.5) : median_index = (int)(src.cols * src.rows * 0.5);
        
      }
      
      int comulative_pixel = 0;
      
      int histSize = 256;
      float range[] = { 0, 256 };
      const float* histRange = { range };
      
      cv::Mat hist;
      
      cv::calcHist(&src, 1, 0, mask, hist, 1, &histSize, &histRange);
      
      int median;
      
      skipZero ? median = 1 : median = 0;
      
      for(; median<256; ++median){
        
        comulative_pixel += hist.at<float>(median);
        
        if(comulative_pixel >= median_index) return median;
        
      }
      
      return 255;
      
    }
    
  } /* namespace */
  
  /*****************************************************************************/
  // otsu
  /*****************************************************************************/
  double otsu(cv::Mat & src, const cv::Mat & mask) {
    
    double value = cv::threshold(src, src, 128, 255, cv::THRESH_OTSU | cv::THRESH_BINARY) + 1;
    
    if(!mask.empty())
      cv::bitwise_and(src, mask, src);
    
    return value;
    
  }
  
  
  /*****************************************************************************/
  // adaptive
  /*****************************************************************************/
  double adaptive(cv::Mat & src, const cv::Mat & mask, int adaptiveMethod = cv::ADAPTIVE_THRESH_MEAN_C, int blockSize = 11, double C = 4) {
    
    cv::adaptiveThreshold(src, src, 255, adaptiveMethod, cv::THRESH_BINARY, blockSize, C);
    
    if(!mask.empty())
      cv::bitwise_and(src, mask, src);
    
    return -1;
    
  }
  
  
  /*****************************************************************************/
  // global
  /*****************************************************************************/
  // openCV global threshold doesn't use threshold value in a strict way. this means why there is -1 here!!!
  double global(cv::Mat & src,  double threshold, const cv::Mat & mask) {
    
    double value = cv::threshold(src, src, (threshold - 1), 255, cv::THRESH_BINARY) + 1;
    
    if(!mask.empty())
      cv::bitwise_and(src, mask, src);
    
    return value;
    
  }
  
  
  /*****************************************************************************/
  // median
  /*****************************************************************************/
  double median(cv::Mat & src, double factor, bool skipZero, const cv::Mat & mask) {
    
    double threshold = utils::median(src, skipZero, mask);
    
    if(threshold == 0) {
      threshold = factor;
    } else {
      threshold *= factor;
    }
    
    double value = cv::threshold(src, src, (threshold - 1), 255, cv::THRESH_BINARY) + 1;
    
    if(!mask.empty())
      cv::bitwise_and(src, mask, src);
    
    return value;
    
  }


  /*****************************************************************************/
  // get
  /*****************************************************************************/
  double get(cv::Mat & src, const std::string & algo, double param1, double param2, const cv::Mat & mask) {
    
    if(algo.compare("GLOBAL") == 0) {
      return global(src, param1, mask);
    }
    
    if(algo.compare("MEDIAN") == 0) {
      return median(src, param1, WHIT_ZERO, mask);
    }
    
    if(algo.compare("MEDIAN_SKIP_ZERO") == 0) {
      return median(src, param1, SKIP_ZERO, mask);
    }
    
    if(algo.compare("OTSU") == 0) {
      return otsu(src, mask);
    }
    
    if(algo.compare("ADAPTIVE") == 0) {
      return adaptive(src, mask, cv::ADAPTIVE_THRESH_MEAN_C, param1, param2);
    }
    
    fprintf(stderr, "error unknown type of threshold evaluation criteria.\n");
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "      GLOBAL, MEDIAN, MEDIAN_SKIP_ZERO, OTSU or ADAPTIVE\n");
    
    exit(EXIT_FAILURE);
    
  }

  
} /* namespace */

#endif /* _H_CVL_THRESHOLD_H_ */

