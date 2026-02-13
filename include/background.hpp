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

#ifndef _H_CVL_BACKGROUND_H_
#define _H_CVL_BACKGROUND_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

//*****************************************************************************/
// namespace
//*****************************************************************************/
namespace cvl::background {

  //*****************************************************************************/
  // lightest
  //*****************************************************************************/
  // Build a background image taking the maximum value for each pixel.
  void lightest(const std::vector<cv::Mat> & src, cv::Mat & dst) {
    
    // Metto tutti i pixel a 0
    dst.setTo(cv::Scalar(0,0,0));
    
    for(size_t i=0; i<src.size(); ++i)
      cv::max(src[i], dst, dst);
    
  }

  //*****************************************************************************/
  // darkest
  //*****************************************************************************/
  // Build a background image taking the minimum value for each pixel.
  void darkest(const std::vector<cv::Mat> & src, cv::Mat & dst) {

    // Metto tutti i pixel a 255
    dst.setTo(cv::Scalar(255,255,255));
    
    for(size_t i=0; i<src.size(); ++i)
      cv::min(src[i], dst, dst);
    
  }

  //*****************************************************************************/
  // median
  //*****************************************************************************/
  // Build a background image taking the per-pixel median.
  void median(const std::vector<cv::Mat> & src, cv::Mat & dst) {
    
    int histo[256];
    
    int median_index = (int) (src.size() * 0.5);
    
    int comulative = 0;
    
    // assumo che tutte le img sono grandi uguali
    for(int y=0; y<src[0].rows; ++y){
      
      for(int x=0; x<src[0].cols; ++x){
        
        memset(histo, 0, sizeof(int) * 256);
        
        for(size_t index=0; index<src.size(); ++index)
          histo[src[index].at<uchar>(y,x)]++;
        
        comulative = 0;
        
        for(int i=0; i<256; ++i){
          
          comulative += histo[i];
          
          if(comulative >= median_index) {
            
            dst.at<uchar>(y,x) = i;
            
            break;
            
          }
          
        }
      }
    }
    
  }

  //*****************************************************************************/
  // moda
  //*****************************************************************************/
  // Build a background image taking the per-pixel mode (most frequent value).
  void moda(const std::vector<cv::Mat> & src, cv::Mat & dst) {
    
    int histo[256];
    
    int moda_index = 0;
    
    int max = 0;
    
    // assumo che tutte le img sono grandi uguali
    for(int y=0; y<src[0].rows; ++y){
      
      for(int x=0; x<src[0].cols; ++x){
        
        memset(histo, 0, sizeof(int) * 256);
        
        for(size_t index=0; index<src.size(); ++index)
          histo[src[index].at<uchar>(y,x)]++;
        
        max = 0;
        
        for(int i=0; i<256; ++i){
          
          if(max < histo[i]) {
            
            max = histo[i];
            
            moda_index = i;
                        
          }
          
        } // for i
        
        dst.at<uchar>(y,x) = moda_index;
        
      }
      
    }
    
  }

  //*****************************************************************************/
  // avg
  //*****************************************************************************/
  // Build a background image taking the per-pixel average.
  void avg(const std::vector<cv::Mat> & src, cv::Mat & dst) {
    
    cv::Mat avgTemp = cv::Mat::zeros(src[0].size(), CV_32F);
    cv::Mat rawImg32F = cv::Mat::zeros(src[0].size(), CV_32F);
    
    for(size_t i=0; i<src.size(); ++i) {
      
      src[i].convertTo(rawImg32F, CV_32F);
      
      cv::accumulate(rawImg32F, avgTemp);
      
    }
    
    avgTemp.mul((double)(1.0/(src.size()*1.0)));
    
    cv::convertScaleAbs(avgTemp, dst);
    
  }

  //*****************************************************************************/
  // bucket
  //*****************************************************************************/
  // Build a background image by masking pixels close to the median (bucket mode).
  void bucket(const std::vector<cv::Mat> & src, cv::Mat & dst){
    
    int delta = 10;
    
    median(src, dst);
    
    for(int y=0; y<src[0].rows; ++y){
      
      for(int x=0; x<src[0].cols; ++x){
        
        int pixelOri = src[0].at<uchar>(y,x);
        int pixelBgr = dst.at<uchar>(y,x);
        
        if((pixelOri <= (pixelBgr + delta)) && (pixelOri >= (pixelBgr - delta))){
          dst.at<uchar>(y,x) = 0;
        } else {
          dst.at<uchar>(y,x) = pixelOri;
        }
        
      }
      
    }
    
  }
  
  //*****************************************************************************/
  // fixed
  //*****************************************************************************/
  // Copy pixels above a threshold from the source to the destination.
  void fixed(const cv::Mat & src, cv::Mat & dst, uchar value) {
     
    for(int y=0; y<src.rows; ++y) {
      for(int x=0; x<src.cols; ++x) {
        if(src.at<uchar>(y,x) >= value) dst.at<uchar>(y,x) = src.at<uchar>(y,x);
      }
    }

  }
  
  //*****************************************************************************/
  // get
  //*****************************************************************************/
  // Dispatch background evaluation by name (LIGHTEST, DARKEST, MEDIAN, MODA, AVG, FIXED, NONE).
  void get(const std::vector<cv::Mat> & src, cv::Mat & dst, const std::string & algo, uchar value = 0) {
    
    int index = src.size() * 0.5;
    
    if(algo.compare("LIGHTEST") == 0)
      return lightest(src, dst);
      
    if(algo.compare("DARKEST") == 0)
      return darkest(src, dst);
      
    if(algo.compare("MEDIAN") == 0)
      return median(src, dst);
      
    if(algo.compare("MODA") == 0)
      return moda(src, dst);
    
    if(algo.compare("AVG") == 0)
      return avg(src, dst);
      
    if(algo.compare("NONE") == 0)
      return (void) src[index].copyTo(dst);
    
    if(algo.compare("FIXED") == 0)
      return fixed(src[index], dst, value);
      
    fprintf(stderr, "error unknown type of background evaluation criteria '%s'.\n", algo.c_str());
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "    LIGHTEST, DARKEST, MEDIAN, MODA, AVG, FIXED or NONE\n");
      
    exit(EXIT_FAILURE);
    
  }
  
} /* namespace */

#endif /* _H_CVL_BACKGROUND_H_ */
