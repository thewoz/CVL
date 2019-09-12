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

#ifndef _H_CVL_ENHANCE_H_
#define _H_CVL_ENHANCE_H_

#include <cstdlib>
#include <cstdio>

#include <vector>

#include <opencv2/opencv.hpp>

/*****************************************************************************/
// namespace
/*****************************************************************************/
namespace cvl::enhance {

  /*****************************************************************************/
  // histogramEqualization
  /*****************************************************************************/
  void histogramEqualization(cv::Mat & src) {
    cv::equalizeHist(src, src);
  }

  /*****************************************************************************/
  // histogramEqualization
  /*****************************************************************************/
  void histogramEqualization(std::vector<cv::Mat> & images) {
    
    for(uint32_t i=0; i<images.size(); ++i)
      histogramEqualization(images[i]);
    
  }

  /*****************************************************************************/
  // unsharpMask
  /*****************************************************************************/
  void unsharpMask(cv::Mat & src, uint32_t size = 3, double sigma = 1, double threshold = 5, double amount = 3) {
    
    #if(1)
    
    cv::Mat srcF32;
    
    src.convertTo(srcF32, CV_32FC1);

    cv::Mat blurred;
    
    cv::GaussianBlur(srcF32, blurred, cv::Size(size,size), sigma, sigma);
      
    cv::Mat lowConstrastMask = cv::abs(srcF32 - blurred) < threshold;
    
    cv::Mat sharpened = srcF32*(1+amount) + blurred*(-amount);
    
    srcF32.copyTo(sharpened, lowConstrastMask);
    
    sharpened.convertTo(src , CV_8UC1); 
    
		#else

    size += (1 - (size % 2));
    
    cv::Mat srcF32;
    
    src.convertTo(srcF32, CV_32FC1);
    
    cv::Mat blurred;
    
    cv::GaussianBlur(srcF32, blurred, cv::Size(size,size), sigma, sigma);
    
    cv::Mat hp = srcF32 - blurred;
    
    cv::Mat hpabs = cv::abs(hp);
    
    cv::Mat hpthr;
    
    cv::threshold(hpabs, hpthr, threshold, 1.0, cv::THRESH_BINARY);
    
    cv::Mat ret1 = srcF32 + amount * hp.mul(hpthr);
    
    ret1.convertTo(src, CV_8UC1);
		
		#endif
		
  }
  
  /*****************************************************************************/
  // unsharpMask
  /*****************************************************************************/
  void unsharpMask(std::vector<cv::Mat> & images, uint32_t size = 3, double sigma = 1, double threshold = 5, double amount = 1) {
    
    for(unsigned int i=0; i<images.size(); ++i)
      unsharpMask(images[i], size, sigma, threshold, amount);
    
  }
  
  //TODO: fare generale

} /* namespace */

#endif /* _H_CVL_ENHANCE_H_ */
