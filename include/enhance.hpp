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

#ifndef _H_CVL_ENHANCE_H_
#define _H_CVL_ENHANCE_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdlib>
#include <cstdio>

#include <vector>

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
    // Compute contrast stretch parameters based on histogram clipping.
    // https://answers.opencv.org/question/75510/how-to-make-auto-adjustmentsbrightness-and-contrast-for-image-android-opencv-image-correction
    inline void auto_alpha_beta(const cv::Mat & image, float clipHistPercent, float & alpha, float & beta) {
      
      double minimo, massimo;
      
      // Get image max value
      int histSize = -1;
      if(image.depth() == CV_8U)  histSize = 256;
      if(image.depth() == CV_16U) histSize = 65536;
      
      if(histSize == -1) {
        cvl::log::error("cvl::enhance::auto_alpha_beta() error: image depth not supported\n");
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

        // locate left cut (clamped: su istogrammi degeneri il while usciva dall'array)
        int lo = 0;
        while(lo < histSize - 1 && accumulator[lo] < clipHistPercent)
          lo++;
        minimo = lo;

        // locate right cut
        int hi = histSize - 1;
        while(hi > lo && accumulator[hi] >= (maximum - clipHistPercent))
          hi--;
        massimo = hi;

      }

      // current range (almeno 1 per evitare la divisione per zero su immagini piatte)
      float inputRange = massimo - minimo;
      if(inputRange <= 0) inputRange = 1;
      
      alpha = (histSize - 1) / inputRange; // alpha expands current range to histsize range
      beta = -minimo * alpha;              // beta shifts current range so that minGray will go to 0
      
    }
    
  } /* utils */

  //****************************************************************************
  // histogramEqualization
  //****************************************************************************
  // Apply histogram equalization to a single image.
  inline void histogramEqualization(cv::Mat & src) {
    cv::equalizeHist(src, src);
  }

  //****************************************************************************
  // histogramEqualization
  //****************************************************************************
  // Apply histogram equalization to a list of images.
  inline void histogramEqualization(std::vector<cv::Mat> & images) {
    
    for(size_t i=0; i<images.size(); ++i)
      histogramEqualization(images[i]);
    
  }

  //****************************************************************************
  // contrast
  //****************************************************************************
  // I(x,y) = alpha * I(x,y) + beta
  // Apply a linear contrast adjustment.
  inline void contrast(cv::Mat & image, float alpha, float beta) {
    
    if(image.channels() != 1) {
      cvl::log::error("cvl::enhance::contrast() error: works only with single channel images\n");
    }
    cv::convertScaleAbs(image, image, alpha, beta);
    
  }

  //****************************************************************************
  // contrast
  //****************************************************************************
  // Apply a linear contrast adjustment to multiple images.
  inline void contrast(std::vector<cv::Mat> & images, float alpha, float beta) {
    
    for(size_t i=0; i<images.size(); ++i)
      contrast(images[i], alpha, beta);
    
  }

  //****************************************************************************
  // autoContrast
  //****************************************************************************
  // I(x,y) = alpha * I(x,y) + beta
  // Apply automatic contrast stretching with histogram clipping.
  inline void autoContrast(cv::Mat & image, float clipHistPercent) {

    if(image.channels() != 1) {
      cvl::log::error("cvl::enhance::autoContrast() error: works only with single channel images\n");
    }

    // convertScaleAbs produce sempre 8 bit: su immagini a 16 bit lo stretch
    // calcolato sul range 65535 saturerebbe quasi tutti i pixel a 255
    if(image.depth() != CV_8U) {
      cvl::log::error("cvl::enhance::autoContrast() error: works only with CV_8U images\n");
    }

    float alpha, beta;
                    
    utils::auto_alpha_beta(image, clipHistPercent, alpha, beta);
    
    cv::convertScaleAbs(image, image, alpha, beta);
                    
  }
  
  //****************************************************************************
  // autoContrast
  //****************************************************************************
  // Apply automatic contrast stretching to multiple images.
  inline void autoContrast(std::vector<cv::Mat> & images, float clipHistPercent) {
    
    for(size_t i=0; i<images.size(); ++i)
      autoContrast(images[i], clipHistPercent);
    
  }
  
  //****************************************************************************
  // unsharpMask
  //****************************************************************************
  // Sharpen an image using unsharp masking.
  inline void unsharpMask(cv::Mat & src, int size = 3, double sigma = 1, double threshold = 5, double amount = 3) {

    // GaussianBlur richiede un kernel di lato dispari
    size += (1 - (size % 2));

    cv::Mat srcF32;

    src.convertTo(srcF32, CV_32FC1);

    cv::Mat blurred;

    cv::GaussianBlur(srcF32, blurred, cv::Size(size,size), sigma, sigma);

    cv::Mat lowConstrastMask = cv::abs(srcF32 - blurred) < threshold;

    cv::Mat sharpened = srcF32*(1+amount) + blurred*(-amount);

    srcF32.copyTo(sharpened, lowConstrastMask);

    sharpened.convertTo(src , CV_8UC1);

  }
  
  //****************************************************************************
  // unsharpMask
  //****************************************************************************
  // Apply unsharp masking to multiple images.
  inline void unsharpMask(std::vector<cv::Mat> & images, double size = 3, double sigma = 1, double threshold = 5, double amount = 1) {
    
    for(unsigned int i=0; i<images.size(); ++i)
      unsharpMask(images[i], size, sigma, threshold, amount);
    
  }

  //****************************************************************************
  // laplacianUnsharpMask
  //****************************************************************************
  // Sharpen an image using a Laplacian filter.
  inline void laplacianUnsharpMask(cv::Mat & src, double amount = 1.1) {
    
    cv::Mat filtered;
    cv::medianBlur(src, filtered, 1);
    
    cv::Mat laplace;
    cv::Laplacian(filtered, laplace, CV_8U);
    
    src = filtered - (amount*laplace);
    
  }

  //****************************************************************************
  // laplacianUnsharpMask
  //****************************************************************************
  // Apply Laplacian sharpening to multiple images.
  inline void laplacianUnsharpMask(std::vector<cv::Mat> & images, double amount) {
    
    for(unsigned int i=0; i<images.size(); ++i)
      laplacianUnsharpMask(images[i], amount);
    
  }

  //****************************************************************************
  // clahe
  //****************************************************************************
  // https://en.wikipedia.org/wiki/Adaptive_histogram_equalization#Contrast_Limited_AHE
  // Apply CLAHE (Contrast Limited Adaptive Histogram Equalization).
  inline void clahe(cv::Mat & src, float clipLimit, const cv::Size & tilesGridSize) {
    
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    
    clahe->setClipLimit(clipLimit);
    clahe->setTilesGridSize(tilesGridSize);

    cv::Mat dst;
    clahe->apply(src, dst);
    
    dst.copyTo(src);

  }
  
  // Apply CLAHE to multiple images.
  inline void clahe(std::vector<cv::Mat> & images, float clipLimit, const cv::Size & tilesGridSize) {
    
    for(unsigned int i=0; i<images.size(); ++i)
      clahe(images[i], clipLimit, tilesGridSize);
    
  }
  
  //****************************************************************************
  // get
  //****************************************************************************
  // Dispatch enhancement by name for a single image.
  inline void get(cv::Mat & src, const std::string & algo, double param1, double param2 = 1,  double param3 = 5, double param4 = 1) {
    
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
      return clahe(src, param1, cv::Size(param2, param3));
    }
    
    cvl::log::error("cvl::enhance::get() error: unknown enhance type '%s'\n"
                    "You can choose between:\n"
                    "      LAPLACIAN, UNSHARPMASK, CONTRAST, AUTO_CONTRAST, CLAHE or EQUALIZE_HISTOGRAM\n", algo.c_str());

  }

  //****************************************************************************
  // get
  //****************************************************************************
  // Dispatch enhancement by name for multiple images.
  inline void get(std::vector<cv::Mat> & src, const std::string & algo, double param1, double param2 = 1,  double param3 = 5, double param4 = 1) {
    
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
      return clahe(src, param1, cv::Size(param2, param3));
    }
    
    cvl::log::error("cvl::enhance::get() error: unknown enhance type '%s'\n"
                    "You can choose between:\n"
                    "      LAPLACIAN, UNSHARPMASK, CONTRAST, AUTO_CONTRAST, CLAHE or EQUALIZE_HISTOGRAM\n", algo.c_str());
    
  }
  
} /* namespace */

#endif /* _H_CVL_ENHANCE_H_ */
