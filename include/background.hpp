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

#ifndef _H_CVL_BACKGROUND_H_
#define _H_CVL_BACKGROUND_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

#include "utils.hpp"

//*****************************************************************************/
// namespace
//*****************************************************************************/
namespace cvl::background {

  //*****************************************************************************/
  // lightest
  //*****************************************************************************/
  // Build a background image taking the maximum value for each pixel.
  inline void lightest(const std::vector<cv::Mat> & src, cv::Mat & dst) {
    
    // Metto tutti i pixel a 0
    dst.setTo(cv::Scalar(0,0,0));
    
    for(size_t i=0; i<src.size(); ++i)
      cv::max(src[i], dst, dst);
    
  }

  //*****************************************************************************/
  // darkest
  //*****************************************************************************/
  // Build a background image taking the minimum value for each pixel.
  inline void darkest(const std::vector<cv::Mat> & src, cv::Mat & dst) {

    // Metto tutti i pixel a 255
    dst.setTo(cv::Scalar(255,255,255));
    
    for(size_t i=0; i<src.size(); ++i)
      cv::min(src[i], dst, dst);
    
  }

  //*****************************************************************************/
  // median
  //*****************************************************************************/
  // Build a background image taking the per-pixel median.
  inline void median(const std::vector<cv::Mat> & src, cv::Mat & dst) {
    
    int histo[256];

    int median_index = (int) ((src.size() + 1) / 2);

    // row pointers for every input image (reused row by row)
    std::vector<const uchar *> srcRow(src.size());

    // assumo che tutte le img sono grandi uguali
    for(int y=0; y<src[0].rows; ++y){

      for(size_t index=0; index<src.size(); ++index)
        srcRow[index] = src[index].ptr<uchar>(y);

      uchar * dstRow = dst.ptr<uchar>(y);

      for(int x=0; x<src[0].cols; ++x){

        memset(histo, 0, sizeof(int) * 256);

        for(size_t index=0; index<src.size(); ++index)
          histo[srcRow[index][x]]++;

        int comulative = 0;

        for(int i=0; i<256; ++i){

          comulative += histo[i];

          if(comulative >= median_index) {

            dstRow[x] = i;

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
  inline void moda(const std::vector<cv::Mat> & src, cv::Mat & dst) {
    
    int histo[256];

    // row pointers for every input image (reused row by row)
    std::vector<const uchar *> srcRow(src.size());

    // assumo che tutte le img sono grandi uguali
    for(int y=0; y<src[0].rows; ++y){

      for(size_t index=0; index<src.size(); ++index)
        srcRow[index] = src[index].ptr<uchar>(y);

      uchar * dstRow = dst.ptr<uchar>(y);

      for(int x=0; x<src[0].cols; ++x){

        memset(histo, 0, sizeof(int) * 256);

        for(size_t index=0; index<src.size(); ++index)
          histo[srcRow[index][x]]++;

        int max = 0;
        int moda_index = 0;

        for(int i=0; i<256; ++i){

          if(max < histo[i]) {

            max = histo[i];

            moda_index = i;

          }

        } // for i

        dstRow[x] = moda_index;

      }

    }

  }

  //*****************************************************************************/
  // avg
  //*****************************************************************************/
  // Build a background image taking the per-pixel average.
  inline void avg(const std::vector<cv::Mat> & src, cv::Mat & dst) {
    
    cv::Mat avgTemp = cv::Mat::zeros(src[0].size(), CV_32F);

    // cv::accumulate accepts an 8-bit source into a 32F accumulator,
    // so no explicit per-frame conversion is needed
    for(size_t i=0; i<src.size(); ++i)
      cv::accumulate(src[i], avgTemp);

    cv::convertScaleAbs(avgTemp, dst, 1.0 / (double) src.size());
    
  }

  //*****************************************************************************/
  // bucket
  //*****************************************************************************/
  // Build a background image by masking pixels close to the median (bucket mode).
  inline void bucket(const std::vector<cv::Mat> & src, cv::Mat & dst){
    
    int delta = 10;
    
    median(src, dst);
    
    for(int y=0; y<src[0].rows; ++y){

      const uchar * srcRow = src[0].ptr<uchar>(y);
      uchar * dstRow = dst.ptr<uchar>(y);

      for(int x=0; x<src[0].cols; ++x){

        int pixelOri = srcRow[x];
        int pixelBgr = dstRow[x];

        if((pixelOri <= (pixelBgr + delta)) && (pixelOri >= (pixelBgr - delta))){
          dstRow[x] = 0;
        } else {
          dstRow[x] = pixelOri;
        }

      }

    }
    
  }
  
  //*****************************************************************************/
  // fixed
  //*****************************************************************************/
  // Copy pixels above a threshold from the source to the destination.
  inline void fixed(const cv::Mat & src, cv::Mat & dst, uchar value) {
     
    for(int y=0; y<src.rows; ++y) {
      const uchar * srcRow = src.ptr<uchar>(y);
      uchar * dstRow = dst.ptr<uchar>(y);
      for(int x=0; x<src.cols; ++x) {
        if(srcRow[x] >= value) dstRow[x] = srcRow[x];
      }
    }

  }
  
  //*****************************************************************************/
  // get
  //*****************************************************************************/
  // Dispatch background evaluation by name (LIGHTEST, DARKEST, MEDIAN, MODA, AVG, FIXED, NONE).
  inline void get(const std::vector<cv::Mat> & src, cv::Mat & dst, const std::string & algo, uchar value = 0) {
    
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
      
    cvl::log::error("cvl::background::get() error: unknown background type '%s'\n"
                    "You can choose between:\n"
                    "    LIGHTEST, DARKEST, MEDIAN, MODA, AVG, FIXED or NONE\n", algo.c_str());
    
  }
  
} /* namespace */

#endif /* _H_CVL_BACKGROUND_H_ */
