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

#ifndef _H_CVL_ENHANCE_H_
#define _H_CVL_ENHANCE_H_

#include <cstdlib>
#include <cstdio>

#include <vector>

#include <opencv2/opencv.hpp>

//****************************************************************************
// namespace
//****************************************************************************
namespace cvl::enhance {
  
  //****************************************************************************
  // namespace
  //****************************************************************************
  namespace utils {
    
    //****************************************************************************
    // auto_alpha_beta
    //****************************************************************************
    // https://answers.opencv.org/question/75510/how-to-make-auto-adjustmentsbrightness-and-contrast-for-image-android-opencv-image-correction
    void auto_alpha_beta(const cv::Mat & image, float clipHistPercent, float & alpha, float & beta) {
      
      double minimo, massimo;
      
      // Get image max value
      int histSize = -1;
      if(image.depth() == CV_8U)  histSize = 256;
      if(image.depth() == CV_16U) histSize = 65536;
      
      if(histSize == -1) {
        fprintf(stderr, "error auto_alpha_beta() image depth not supported\n");
        abort();
      }
      
      if(clipHistPercent == 0) {
        
        // keep full available range
        cv::minMaxLoc(image, &minimo, &massimo);
        
      } else {
        
        cv::Mat hist;
        
        float range[2]; range[0] = 0; range[1] = histSize;
        const float * histRange = { range };
        
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
      
      alpha = (histSize - 1) / inputRange; // alpha expands current range to histsize range
      beta = -minimo * alpha;              // beta shifts current range so that minGray will go to 0
      
    }
    
  } /* utils */

  //****************************************************************************
  // histogramEqualization
  //****************************************************************************
  void histogramEqualization(cv::Mat & src) {
    cv::equalizeHist(src, src);
  }

  //****************************************************************************
  // histogramEqualization
  //****************************************************************************
  void histogramEqualization(std::vector<cv::Mat> & images) {
    
    for(size_t i=0; i<images.size(); ++i)
      histogramEqualization(images[i]);
    
  }

  //****************************************************************************
  // contrast
  //****************************************************************************
  // I(x,y) = alpha * I(x,y) + beta
  void contrast(cv::Mat & image, float alpha, float beta) {
    
    if(image.channels() != 1) {
      fprintf(stderr, "error autoContrast() works only with one channel image\n");
      abort();
    }
    cv::convertScaleAbs(image, image, alpha, beta);
    
  }

  //****************************************************************************
  // autoContrast
  //****************************************************************************
  // I(x,y) = alpha * I(x,y) + beta
  void autoContrast(cv::Mat & image, float clipHistPercent) {
    
    if(image.channels() != 1) {
      fprintf(stderr, "error autoContrast() works only with one channel image\n");
      abort();
    }

    float alpha, beta;
                    
    utils::auto_alpha_beta(image, clipHistPercent, alpha, beta);
    
    cv::convertScaleAbs(image, image, alpha, beta);
                    
  }
  
  //****************************************************************************
  // autoContrast
  //****************************************************************************
  void autoContrast(std::vector<cv::Mat> & images, float clipHistPercent) {
    
    for(size_t i=0; i<images.size(); ++i)
      autoContrast(images[i], clipHistPercent);
    
  }
  
  //****************************************************************************
  // unsharpMask
  //****************************************************************************
  void unsharpMask(cv::Mat & src, int size = 3, double sigma = 1, double threshold = 5, double amount = 3) {
    
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
  
  //****************************************************************************
  // unsharpMask
  //****************************************************************************
  void unsharpMask(std::vector<cv::Mat> & images, double size = 3, double sigma = 1, double threshold = 5, double amount = 1) {
    
    for(unsigned int i=0; i<images.size(); ++i)
      unsharpMask(images[i], size, sigma, threshold, amount);
    
  }

  //****************************************************************************
  // laplacianUnsharpMask
  //****************************************************************************
  void laplacianUnsharpMask(cv::Mat & src, double amount = 1.1) {
    
    cv::Mat filtered;
    cv::medianBlur(src, filtered, 1);
    
    cv::Mat laplace;
    cv::Laplacian(filtered, laplace, CV_8U);
    
    src = filtered - (amount*laplace);
    
  }

  //****************************************************************************
  // laplacianUnsharpMask
  //****************************************************************************
  void laplacianUnsharpMask(std::vector<cv::Mat> & images, double amount) {
    
    for(unsigned int i=0; i<images.size(); ++i)
      laplacianUnsharpMask(images[i], amount);
    
  }

  //****************************************************************************
  // clahe
  //****************************************************************************
  // https://en.wikipedia.org/wiki/Adaptive_histogram_equalization#Contrast_Limited_AHE
  void clahe(cv::Mat & src, float clipLimit, const cv::Size & tilesGridSize) {
    
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    
    clahe->setClipLimit(clipLimit);
    clahe->setTilesGridSize(tilesGridSize);

    cv::Mat dst;
    clahe->apply(src, dst);
    
    dst.copyTo(src);

  }
  
  void clahe(std::vector<cv::Mat> & images, float clipLimit, const cv::Size & tilesGridSize) {
    
    for(unsigned int i=0; i<images.size(); ++i)
      clahe(images[i], clipLimit, tilesGridSize);
    
  }
  
  //****************************************************************************
  // get
  //****************************************************************************
  void get(cv::Mat & src, const std::string & algo, double param1, double param2 = 1,  double param3 = 5, double param4 = 1) {
    
    if(algo.compare("LAPLACIAN") == 0) {
      return laplacianUnsharpMask(src, param1);
    }
    
    if(algo.compare("UNSHARPMASK") == 0) {
      return unsharpMask(src, param1, param2, param3, param4);
    }
    
    if(algo.compare("CONTRAST") == 0) {
      return contrast(src, param1, param2);
    }
    
    if(algo.compare("AUTO_CONTRAST") == 0) {
      return autoContrast(src, param1);
    }
    
    if(algo.compare("EQUALIZE_HISTOGRAM") == 0) {
      return histogramEqualization(src);
    }
    
    if(algo.compare("CLAHE") == 0) {
      return clahe(src, param1, cv::Size(param2, param2));
    }
    
    fprintf(stderr, "error unknown type of enhance evaluation criteria.\n");
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "      UNSHARPMASK, AUTO_CONTRAST, CLAHE or EQUALIZE_HISTOGRAM\n");
    
    exit(EXIT_FAILURE);
    
  }
  
  //****************************************************************************
  // get
  //****************************************************************************
  void get(std::vector<cv::Mat> & src, const std::string & algo, double param1, double param2 = 1,  double param3 = 5, double param4 = 1) {
    
    if(algo.compare("LAPLACIAN") == 0) {
      return laplacianUnsharpMask(src, param1);
    }
    
    if(algo.compare("UNSHARPMASK") == 0) {
      return unsharpMask(src, param1, param2, param3, param4);
    }
    
    if(algo.compare("AUTO_CONTRAST") == 0) {
      return autoContrast(src, param1);
    }
    
    if(algo.compare("EQUALIZE_HISTOGRAM") == 0) {
      return histogramEqualization(src);
    }
    
    if(algo.compare("CLAHE") == 0) {
      return clahe(src, param1, cv::Size(param2, param3));
    }
    
    fprintf(stderr, "error unknown type of enhance evaluation criteria.\n");
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "      UNSHARPMASK, AUTO_CONTRAST, CLAHE or EQUALIZE_HISTOGRAM\n");

    exit(EXIT_FAILURE);
    
  }
  
} /* namespace */

#endif /* _H_CVL_ENHANCE_H_ */
