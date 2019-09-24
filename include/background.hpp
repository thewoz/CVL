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

#ifndef _H_CVL_BACKGROUND_H_
#define _H_CVL_BACKGROUND_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

/*****************************************************************************/
// namespace
/*****************************************************************************/
namespace cvl::background {

  /*****************************************************************************/
  // lightest
  /*****************************************************************************/
  void lightest(const std::vector<cv::Mat> & src, cv::Mat & dst) {
    
    // Metto tutti i pixel a 0
    dst.setTo(cv::Scalar(0,0,0));
    
    for(size_t i=0; i<src.size(); ++i)
      cv::max(src[i], dst, dst);
    
  }


  /*****************************************************************************/
  // darkest
  /*****************************************************************************/
  void darkest(const std::vector<cv::Mat> & src, cv::Mat & dst) {

    // Metto tutti i pixel a 255
    dst.setTo(cv::Scalar(255,255,255));
    
    for(size_t i=0; i<src.size(); ++i)
      cv::min(src[i], dst, dst);
    
  }


  /*****************************************************************************/
  // median
  /*****************************************************************************/
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


  /*****************************************************************************/
  // avg
  /*****************************************************************************/
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


  /*****************************************************************************/
  // bucket
  /*****************************************************************************/
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
  
  
  /*****************************************************************************/
  // get
  /*****************************************************************************/
  void get(const std::vector<cv::Mat> & src, cv::Mat & dst, const std::string & algo) {
    
    if(algo.compare("LIGHTEST") == 0)
      return lightest(src, dst);
      
    if(algo.compare("DARKEST") == 0)
      return darkest(src, dst);
      
    if(algo.compare("MEDIAN") == 0)
      return median(src, dst);
      
    if(algo.compare("AVG") == 0)
      return avg(src, dst);
      
    if(algo.compare("NONE") == 0)
      return (void) dst.setTo(cv::Scalar(0,0,0));
      
    fprintf(stderr, "error unknown type of background evaluation criteria '%s'.\n", algo.c_str());
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "    LIGHTEST, DARKEST, MEDIAN, AVG or NONE\n");
      
    exit(EXIT_FAILURE);
    
  }

  
} /* namespace */

#endif /* _H_CVL_BACKGROUND_H_ */
