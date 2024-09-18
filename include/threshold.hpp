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

#ifndef _H_CVL_THRESHOLD_H_
#define _H_CVL_THRESHOLD_H_

#include <cstdlib>
#include <cstdio>

#include <string>

#include <opencv2/opencv.hpp>

//****************************************************************************/
// namespace
//****************************************************************************/
namespace cvl::threshold {
  
  //****************************************************************************/
  // namespace
  //****************************************************************************/
  namespace utils {
  
    //****************************************************************************/
    // otsu() - ChatGPT
    // Funzione per calcolare la soglia di Otsu escludendo un valore specifico
    //****************************************************************************/
    int otsu(const cv::Mat & src, int excludeValue) {
        
      // Controlla che l'immagine sia in scala di grigi
      if(src.channels() != 1) {
        fprintf(stderr, "cvl::threshold::otsu() error: the image must be a CV_8UC1\n");
        abort();
      }

      // per restire che non voglio escudere nulla (messo da me)
      if(excludeValue < 0 || excludeValue > 255)
        excludeValue = 256;
      
      int totalPixels = (int)src.total();
      std::vector<int> histogram(257, 0);

      // Calcola l'istogramma dell'immagine
      for(int y=0; y<src.rows; y++)
        for(int x=0; x<src.cols; x++)
          ++histogram[src.at<uchar>(y,x)];
  
      // Calcola il numero totale di pixel da considerare
      int pixelsToConsider = totalPixels - histogram[excludeValue];

      // Somma totale dei pixel
      double sum = 0;

      for(int i=0; i<256; i++) {
        if(i!=excludeValue) {
          sum += i * histogram[i];
        }
      }

      // Variabili per il calcolo della soglia di Otsu
      double sumB = 0;
      int wB = 0;
      int wF = 0;
      double varMax = 0;
      
      // valore di soglia
      int threshold = 0;
      
      // Calcolo della soglia di Otsu
      for(int i=0; i<256; i++) {
        
        if(i != excludeValue) {
          
          // Peso della classe background
          wB += histogram[i];
          
          if(wB == 0) continue;
          
          // Peso della classe foreground
          wF = pixelsToConsider - wB;
          
          if(wF == 0) break;

          sumB += static_cast<double>(i * histogram[i]);

          // Media della classe background
          double mB = sumB / wB;
          
          // Media della classe foreground
          double mF = (sum - sumB) / wF;

          // Calcola la varianza tra le classi
          double varBetween = static_cast<double>(wB) * static_cast<double>(wF) * (mB - mF) * (mB - mF);

          // Aggiorna la soglia se la varianza è massima
          if(varBetween > varMax) {
            varMax = varBetween;
            threshold = i;
          }
          
        }
        
      }

      return threshold;
      
    }

  
    //****************************************************************************/
    // moda
    //****************************************************************************/
    int moda(const cv::Mat & src, cv::Mat mask = cv::Mat(), int valueToSkip = -1) {
      
      cv::Mat histo;
      cvl::histogram(src, histo, mask, valueToSkip);
      
      float maxFreq = 0;

      int moda;
      
      for(int color=0; color<256; ++color) {
        
        if(histo.at<float>(color) > maxFreq) {
          maxFreq = histo.at<float>(color);
          moda = color;
        }
                
      }
     
      return moda;
      
    }
  
    //****************************************************************************/
    // median
    //****************************************************************************/
    int median(const cv::Mat & src, cv::Mat mask = cv::Mat(), int valueToSkip = -1) {
      
      int countNonZero = src.rows * src.cols;
      
      // se ci sta definita la mashera o il valore da ignorare
      if(!mask.empty() || valueToSkip != -1) {
        
        // se la maschera è vuota la definisco
        if(!mask.empty()) mask = cv::Mat::zeros(src.size(), CV_8UC1);
        
        countNonZero = 0;
        
        for(int y=0; y<src.rows; ++y) {
          for(int x=0; x<src.cols; ++x) {
            
            // se il valore lo devo ignorare modifico la maschera
            if(src.at<uchar>(y,x) == src.at<uchar>(y,x)) mask.at<uchar>(y,x) = 0;
            
            if(mask.at<uchar>(y,x) != 0) ++countNonZero;
              
          }
        }
        
      }
            
      cv::Mat histo;
      cvl::histogram(src, histo, mask);
      
      int medianPixel = std::round(countNonZero * 0.5);
      
      int comulativePixel = 0;

      for(int color=0; color<256; ++color) {
        
        comulativePixel += histo.at<float>(color);
        
        if(comulativePixel >= medianPixel) return color;
                
      }
      
      fprintf(stderr, "cvl::median() error: I'm not supposed to be here\n");
      
      abort();
      
    }
    
    //****************************************************************************/
    // percentile
    //****************************************************************************/
    int percentile(const cv::Mat & src, float _percentile, cv::Mat mask = cv::Mat(), int valueToSkip = -1) {
      
      int countNonZero = src.rows * src.cols;
      
      // se ci sta definita la mashera o il valore da ignorare
      if(!mask.empty() || valueToSkip != -1) {
        
        // se la maschera è vuota la definisco
        if(!mask.empty()) mask = cv::Mat::zeros(src.size(), CV_8UC1);
        
        countNonZero = 0;
        
        for(int y=0; y<src.rows; ++y) {
          for(int x=0; x<src.cols; ++x) {
            
            // se il valore lo devo ignorare modifico la maschera
            if(src.at<uchar>(y,x) == src.at<uchar>(y,x)) mask.at<uchar>(y,x) = 0;
            
            if(mask.at<uchar>(y,x) != 0) ++countNonZero;
              
          }
        }
        
      }
            
      _percentile = std::round((countNonZero / 100.0) * _percentile);
      
      cv::Mat histo;
      cvl::histogram(src, histo, mask);
      
      int comulativePixel = 0;
      
      for(int color=0; color<256; ++color) {
        
        comulativePixel += histo.at<float>(color);
        
        if(comulativePixel >= _percentile) return color;
                
      }
      
      fprintf(stderr, "cvl::median() error: I'm not supposed to be here\n");
      
      abort();
      
    }
  
  
  } /* namespace */


  //****************************************************************************/
  // otsu
  //****************************************************************************/
  double otsu(cv::Mat & src, cv::Mat & dst, const cv::Mat & mask = cv::Mat()) {
    
    // TODO: qua dovrei controlalre che è grande uguale e dello stesso tipo
    if(dst.empty()) {
      dst = cv::Mat(src.size(), CV_8UC1);
    }
    
    double value = cv::threshold(src, dst, 128, 255, cv::THRESH_OTSU | cv::THRESH_BINARY) + 1;
    
    if(!mask.empty())
      cv::bitwise_and(dst, mask, dst);
    
    return value;
    
  }
  
  //****************************************************************************/
  // adaptive
  //****************************************************************************/
  double adaptive(cv::Mat & src, cv::Mat & dst, int adaptiveMethod, int thresholdType, int blockSize, double C, const cv::Mat & mask = cv::Mat()) {

    // TODO: qua dovrei controlalre che è grande uguale e dello stesso tipo
    if(dst.empty()) {
      dst = cv::Mat(src.size(), CV_8UC1);
    }
    
    cv::adaptiveThreshold(src, dst, 255, adaptiveMethod, thresholdType, blockSize, C);
  
    if(!mask.empty())
      cv::bitwise_and(dst, mask, dst);
    
    return -1;
    
  }
  
  //****************************************************************************/
  // global
  //****************************************************************************/
  // openCV global threshold doesn't use threshold value in a strict way. this means why there is -1 here!!!
  double global(cv::Mat & src, cv::Mat & dst, double threshold, const cv::Mat & mask = cv::Mat()) {
    
    if(src.type() > CV_8UC1){
      fprintf(stderr, "cvl::global() error: the input image must be a CV_8UC1 image.\n");
      exit(EXIT_FAILURE);
    }
    
    // TODO: qua dovrei controlalre che è grande uguale e dello stesso tipo
    if(dst.empty()) {
      dst = cv::Mat(src.size(), CV_8UC1);
    }
    
    double value = cv::threshold(src, dst, (threshold - 1), 255, cv::THRESH_BINARY) + 1;
    
    if(!mask.empty())
      cv::bitwise_and(dst, mask, dst);
    
    return value;
    
  }
  
  //*****************************************************************************/
  // median
  //*****************************************************************************/
  double median(cv::Mat & src, cv::Mat & dst, double factor, int valueToSkip, const cv::Mat & mask = cv::Mat()) {
    
    double threshold = utils::median(src, mask, valueToSkip);
    
    if(threshold == 0) {
      threshold = factor;
    } else {
      threshold *= factor;
    }
    
    // TODO: qua dovrei controlalre che è grande uguale e dello stesso tipo
    if(dst.empty()) {
      dst = cv::Mat(src.size(), CV_8UC1);
    }
    
    double value = cv::threshold(src, dst, (threshold - 1), 255, cv::THRESH_BINARY) + 1;
    
    if(!mask.empty())
      cv::bitwise_and(dst, mask, dst);
    
    return value;
    
  }
  
  //*****************************************************************************/
  // moda
  //*****************************************************************************/
  double moda(cv::Mat & src, cv::Mat & dst, double factor, int valueToSkip, const cv::Mat & mask = cv::Mat()) {
    
    double threshold = utils::moda(src, mask, valueToSkip);
    
    if(threshold == 0) {
      threshold = factor;
    } else {
      threshold *= factor;
    }
    
    // TODO: qua dovrei controlalre che è grande uguale e dello stesso tipo
    if(dst.empty()) {
      dst = cv::Mat(src.size(), CV_8UC1);
    }
    
    double value = cv::threshold(src, dst, (threshold - 1), 255, cv::THRESH_BINARY) + 1;
    
    if(!mask.empty())
      cv::bitwise_and(dst, mask, dst);
    
    return value;
    
  }

  //*****************************************************************************/
  // percentile
  //*****************************************************************************/
  double percentile(cv::Mat & src, cv::Mat & dst, double percentile, int valueToSkip, const cv::Mat & mask = cv::Mat()) {
    
    double threshold = utils::percentile(src, percentile, mask, valueToSkip);
   
    // TODO: qua dovrei controlalre che è grande uguale e dello stesso tipo
    if(dst.empty()) {
      dst = cv::Mat(src.size(), CV_8UC1);
    }
    
    double value = cv::threshold(src, dst, (threshold - 1), 255, cv::THRESH_BINARY) + 1;
    
    if(!mask.empty())
      cv::bitwise_and(dst, mask, dst);
    
    return value;
    
  }

  //*****************************************************************************/
  // get
  //*****************************************************************************/
  double get(cv::Mat & src, cv::Mat & dst, const std::string & algo, double param1, double param2, int param3, const cv::Mat & mask = cv::Mat()) {
    
    if(algo.compare("GLOBAL") == 0) {
      return global(src, dst, param1, mask);
    }
    
    if(algo.compare("MODA") == 0) {
      return moda(src, dst, param1, param3, mask);
    }
    
    if(algo.compare("MEDIAN") == 0) {
      return median(src, dst, param1, param3, mask);
    }
    
    if(algo.compare("PERCENTILE") == 0) {
      return percentile(src, dst, param1, param3, mask);
    }
    
    if(algo.compare("OTSU") == 0) {
      return otsu(src, dst, mask);
    }
    
    if(algo.compare("ADAPTIVE") == 0) {
      return adaptive(src, dst, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, param1, param2, mask);
    }
    
    fprintf(stderr, "error unknown type of threshold evaluation criteria.\n");
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "      GLOBAL, MEDIAN, MODA, PERCENTILE, OTSU or ADAPTIVE\n");
    
    exit(EXIT_FAILURE);
    
  }

  
} /* namespace */

#endif /* _H_CVL_THRESHOLD_H_ */

