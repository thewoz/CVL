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

#ifndef _H_CVL_DENOISING_H_
#define _H_CVL_DENOISING_H_

#include <cstdlib>
#include <cstdio>

#include <string>

#include <opencv2/opencv.hpp>

/*****************************************************************************/
// namespace
/*****************************************************************************/
namespace cvl::denoising {
  
  /*****************************************************************************/
  // namespace
  /*****************************************************************************/
  namespace utils {
    
    /*****************************************************************************/
    // median
    /*****************************************************************************/
    int median(cv::Mat & src) {
      
      if(src.channels() > 1){
        fprintf(stderr, "The destination image must have one channel.\n");
        exit(EXIT_FAILURE);
      }
      
      int histSize = 256;
      float range[] = { 0, 256 };
      const float* histRange = { range };
      
      cv::Mat hist;
      
      cv::calcHist(&src, 1, 0, src, hist, 1, &histSize, &histRange);
      
      int median_index = (int) (src.rows * src.cols * 0.5);
      
      int comulative_pixel = 0;;
      
      for(int median=0; median<256; ++median){
        
        comulative_pixel += hist.at<float>(median) * median;
        
        if(comulative_pixel >= median_index) return median;
        
      }
      
      return 255;
      
    }
    
  } /* namescape */
  
  
  /*****************************************************************************/
  // morphology
  /*****************************************************************************/
  void morphology(cv::Mat & image, uint32_t size = 5) {
    
    cv::morphologyEx(image, image, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(size,size)));
    
  }
  
  
  /*****************************************************************************/
  // gaussian
  /*****************************************************************************/
  void gaussian(cv::Mat & src, uint32_t size = 3) {
    
    cv::GaussianBlur(src, src, cv::Size(size,size), 0);
    
  }
  
  
  /*****************************************************************************/
  // median
  /*****************************************************************************/
  void median(cv::Mat & src, uint32_t size = 3) {
    
    cv::medianBlur(src, src, size);
    
  }
  
  
  /*****************************************************************************/
  // lowlightFilter
  /*****************************************************************************/
  void lowlightFilter(cv::Mat & src, double factor = 2.0) {
    
    int median = utils::median(src) * factor;
    
    for(int y = 0; y < src.rows; ++y)
      for(int x = 0; x < src.cols; ++x)
        if(src.at<uchar>(y,x) <= median)
          src.at<uchar>(y,x) = 0;
    
    gaussian(src);
    
  }
  
  
  /*****************************************************************************/
  // starflag
  /*****************************************************************************/
  void starflag(cv::Mat & src, cv::Mat & workImage, int kernel_size, uchar * denoiseMatrix) {
    
    int alfa = (int) ((kernel_size - 1) * 0.5);
    
    for (int y = 0; y < src.rows; ++y) {
      for (int x = 0; x < src.cols; ++x) {
        
        int hDeMat = y - alfa;
        if (hDeMat < 0) hDeMat = 0;
        
        int wDeMat = x - alfa;
        if (wDeMat < 0) wDeMat = 0;
        int tmp_wDeMat = wDeMat;
        
        int hDeMatTo = y + alfa + 1;
        if (hDeMat > workImage.rows) hDeMat = workImage.rows;
        
        int wDeMatTo = x + alfa + 1;
        if (wDeMatTo > workImage.cols) wDeMat = workImage.cols;
        
        unsigned int iDeMatGood = 0;
        unsigned int iDeMat = 0;
        
        memset(denoiseMatrix, 0, kernel_size * kernel_size * sizeof (uchar));
        
        for(; hDeMat < hDeMatTo; ++hDeMat) {
          for(wDeMat = tmp_wDeMat; wDeMat < wDeMatTo; ++wDeMat) {
            denoiseMatrix[iDeMat] = src.at<uchar>(hDeMat, wDeMat);
            iDeMat++;
          }
        }
        
        iDeMatGood = iDeMat;
        
        double avgDeMat = 0;
        
        for(iDeMat = 0; iDeMat < iDeMatGood; ++iDeMat)
          avgDeMat += denoiseMatrix[iDeMat];
        
        avgDeMat /= (double) iDeMatGood;
        
        double minDelta = DBL_MAX;
        unsigned int minDeltaPivot = 0;
        
        for(iDeMat = 0; iDeMat < iDeMatGood; ++iDeMat) {
          
          double distance = fabs((double) denoiseMatrix[iDeMat] - avgDeMat);
          
          if(distance < minDelta) {
            minDeltaPivot = iDeMat;
            minDelta = distance;
          }
          
        }
        
        workImage.at<uchar>(y, x) = denoiseMatrix[minDeltaPivot];
        
      }
      
    }
    
    src = workImage;
    
  }


  /*****************************************************************************/
  // get
  /*****************************************************************************/
  void get(cv::Mat & src, const std::string & algo, double param = 3, uint32_t time = 1) {
    
    uint32_t size = param;
      
    if(size % 2 == 0) size++;
    
    if(algo.compare("GAUSSIAN") == 0) {
      for(size_t i=0; i<time; ++i)
        gaussian(src, size);
    }
    
    if(algo.compare("MEDIAN") == 0) {
      for(size_t i=0; i<time; ++i)
        cvl::denoising::median(src, size);
    }
      
    if(algo.compare("MORPHOLOGY") == 0) {
      for(size_t i=0; i<time; ++i)
        morphology(src, size);
    }
      
    if(algo.compare("LOWLIGHTFILTER") == 0) {
      for(size_t i=0; i<time; i++)
        lowlightFilter(src,param);
    }
      
    if(algo.compare("STARFLAG") == 0) {
      
      uchar * denoiseMatrix = (uchar *) malloc(size * size * sizeof (uchar));
      //TODO to check the return value
      
      cv::Mat workImage = cv::Mat(src.size(), CV_8UC1);

      for(size_t i=0; i<time; i++)
        starflag(src, workImage, size, denoiseMatrix);
      
      free(denoiseMatrix);
      
    }
    
    fprintf(stderr, "error unknown type of denoising evaluation criteria.\n");
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "      GAUSSIAN, MEDIAN, MORPHOLOGY, LOWLIGHTFILTER or STARFLAG\n");
    
    exit(EXIT_FAILURE);
      
  }

} /* namespace */

#endif /* _H_CVL_DENOISING_H_ */
