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
  // namespace
  /*****************************************************************************/
  namespace utils {
    
    /*****************************************************************************/
    // auto_alpha_beta
    /*****************************************************************************/
    void auto_alpha_beta(const cv::Mat & image, float clipHistPercent, float & alpha, float & beta) {
      
      cv::Mat hist;
      
      // Get image max value
      int histSize = -1;
      if(image.depth() == CV_8U)  histSize = 256;
      if(image.depth() == CV_16U) histSize = 65536;

      if(histSize == -1) {
        fprintf(stderr, "error auto_alpha_beta() image depth not supported\n");
        abort();
      }
      
      float range[2]; range[0] = 0; range[1] = histSize;
      const float * histRange = { range };
            
      double minimo, massimo;
      
      CV_Assert(clipHistPercent>=0);
      
      if(clipHistPercent == 0) {
        
        // keep full available range
        cv::minMaxLoc(image, &minimo, &massimo);
        
      } else {
        
        cv::calcHist(&image, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);
        
        // calculate cumulative distribution from the histogram
        std::vector<float> accumulator(histSize);
        accumulator[0] = hist.at<float>(0);
        
        for(int i = 1; i < histSize; i++)
          accumulator[i] = accumulator[i - 1] + hist.at<float>(i);
        
        // locate points that cuts at required value
        float maximum = accumulator.back();
        clipHistPercent *= (maximum / 100.0); //make percent as absolute
        clipHistPercent /= 2.0; // left and right wings
                                // locate left cut
        minimo = 0;
        while(accumulator[minimo] < clipHistPercent)
          minimo++;
        
        // locate right cut
        massimo = histSize - 1;
        while (accumulator[massimo] >= (maximum - clipHistPercent))
          massimo--;
        
      }
      
      // current range
      float inputRange = massimo - minimo;
      
      alpha = (histSize - 1) / inputRange;   // alpha expands current range to histsize range
      beta = -minimo * alpha;             // beta shifts current range so that minGray will go to 0
      
    }
    
  }

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
  // autoContrast
  /*****************************************************************************/
  void autoContrast(cv::Mat & image, float clipHistPercent) {
    
    if(image.channels() != 1) {
      fprintf(stderr, "error autoContrast() works only with one channel image\n");
      abort();
    }

    float alpha, beta;
                    
    utils::auto_alpha_beta(image, clipHistPercent, alpha, beta);
    
    if(image.depth() == CV_8U) {
    
      for(int y = 0; y < image.rows; ++y) {
        for(int x = 0; x < image.cols; ++x) {
          image.at<uchar>(y,x) = cv::saturate_cast<uchar>(alpha*(image.at<uchar>(y,x)) + beta);
          }
        }

    }
    
    if(image.depth() == CV_16U) {
      
      for(int y = 0; y < image.rows; ++y) {
        for(int x = 0; x < image.cols; ++x) {
          image.at<ushort>(y,x) = cv::saturate_cast<ushort>(alpha*(image.at<ushort>(y,x)) + beta);
        }
      }
      
    }
                    
  }
  
  /*****************************************************************************/
  // autoContrast
  /*****************************************************************************/
  void autoContrast(std::vector<cv::Mat> & images, float clipHistPercent) {
    
    for(uint32_t i=0; i<images.size(); ++i)
      autoContrast(images[i], clipHistPercent);
    
  }
  
  /*****************************************************************************/
  // unsharpMask
  /*****************************************************************************/
  void unsharpMask(cv::Mat & src, double size = 3, double sigma = 1, double threshold = 5, double amount = 3) {
    
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
  void unsharpMask(std::vector<cv::Mat> & images, double size = 3, double sigma = 1, double threshold = 5, double amount = 1) {
    
    for(unsigned int i=0; i<images.size(); ++i)
      unsharpMask(images[i], size, sigma, threshold, amount);
    
  }
  
  /*****************************************************************************/
  // get
  /*****************************************************************************/
  void get(cv::Mat & src, const std::string & algo, double param1, double param2,  double param3, double param4) {
    
    if(algo.compare("UNSHARPMASK") == 0) {
      return unsharpMask(src, param1, param2, param3, param4);
    }
    
    if(algo.compare("AUTO_CONTRAST") == 0) {
      return autoContrast(src, param1);
    }
    
    if(algo.compare("EQUALIZE_HISTOGRAM") == 0) {
      return histogramEqualization(src);
    }
    
    fprintf(stderr, "error unknown type of enhance evaluation criteria.\n");
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "      UNSHARPMASK, AUTO_CONTRAST or EQUALIZE_HISTOGRAM\n");
    
    exit(EXIT_FAILURE);
    
  }
  
  /*****************************************************************************/
  // get
  /*****************************************************************************/
  void get(std::vector<cv::Mat> & src, const std::string & algo, double param1, double param2,  double param3, double param4) {
    
    if(algo.compare("UNSHARPMASK") == 0) {
      return unsharpMask(src, param1, param2, param3, param4);
    }
    
    if(algo.compare("AUTO_CONTRAST") == 0) {
      return autoContrast(src, param1);
    }
    
    if(algo.compare("EQUALIZE_HISTOGRAM") == 0) {
      return histogramEqualization(src);
    }
    
    fprintf(stderr, "error unknown type of enhance evaluation criteria.\n");
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "      UNSHARPMASK, AUTO_CONTRAST or EQUALIZE_HISTOGRAM\n");
    
    exit(EXIT_FAILURE);
    
  }
  
} /* namespace */

#endif /* _H_CVL_ENHANCE_H_ */
