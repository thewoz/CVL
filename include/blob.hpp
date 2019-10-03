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

#ifndef _H_CVL_BLOBS_H_
#define _H_CVL_BLOBS_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

#include <mpl/log.hpp>

#include "baricenter.hpp"



/*****************************************************************************/
// asBlob
/*****************************************************************************/
class asBlob {
  
public:
  
  enum { EXTERNAL_CONTOUR, INTERNAL_CONTOUR };
  
  int32_t label;
  
  uint32_t iMin; uint32_t iMax;
  
  std::vector<cv::Point3i> pixels;
  
  cv::Point2d baricenter;
  
  cv::Rect roi;
  
  cv::Size size;
  
  cv::Point offset;
  
  /*****************************************************************************/
  // asBlob
  /*****************************************************************************/
  asBlob() { pixels.clear(); label = -1; }
  
  /*****************************************************************************/
  // add
  /*****************************************************************************/
  void add(uint32_t x, uint32_t y, uint32_t i) { pixels.push_back(cv::Point3i(x,y,i)); }
  
  /*****************************************************************************/
  // finalize
  /*****************************************************************************/
  void finalize() {
    
    uint32_t xMin = UINT_MAX; uint32_t xMax = 0;
    uint32_t yMin = UINT_MAX; uint32_t yMax = 0;
    
    iMin = UINT_MAX; iMax = 0;
    
    for(uint32_t i=0; i<pixels.size(); i++) {
      
      if(pixels[i].x > xMax) xMax = pixels[i].x;
      if(pixels[i].x < xMin) xMin = pixels[i].x;
      if(pixels[i].y > yMax) yMax = pixels[i].y;
      if(pixels[i].y < yMin) yMin = pixels[i].y;
      
      if(pixels[i].z > iMax) iMax = pixels[i].z;
      if(pixels[i].z < iMin) iMin = pixels[i].z;
      
    }
    
    offset = cv::Point(xMin, yMin);
    
    roi    = cv::Rect(xMin, yMin, xMax - xMin + 1, yMax - yMin + 1);
    
    size   = cv::Size(xMax - xMin + 1, yMax - yMin + 1);
    
  }
  
  /*****************************************************************************/
  // computeBaricenter
  /*****************************************************************************/
  cv::Point2d computeBaricenter(const std::string & algo = "GEOMETRIC") {
    
    baricenter = cvl::baricenter::get(pixels, algo);
    
    return baricenter;
    
  }
  
  /*****************************************************************************/
  // draw
  /*****************************************************************************/
  void draw(cv::Mat & dst, const cv::Vec3b & color, const cv::Point & offset = cv::Point(0,0)) const {
    
    if(dst.channels() == 1) {
      
      for(uint32_t i=0; i<pixels.size(); i++)
        dst.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x) = color[0];
      
    }
    
    if(dst.channels() == 3) {
      
      for(uint32_t i=0; i<pixels.size(); i++)
        dst.at<cv::Vec3b>(pixels[i].y-offset.y, pixels[i].x-offset.x) = color;
      
    }
    
  }
  
  /*****************************************************************************/
  // draw
  /*****************************************************************************/
//  void draw(cv::Mat & dst, const cv::Mat & src, const cv::Point & offset = cv::Point(0,0)) const {
//
//    if(dst.channels() != 1 && dst.channels() !=3)
//      mpl::log::error("The destination image is not a one or three channel image.\n");
//
//    if(dst.channels() == 1) {
//
//      for(uint32_t i=0; i<pixels.size(); i++)
//        dst.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x) = src.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x);
//
//    }
//
//    if(dst.channels() == 3) {
//
//      for(uint32_t i=0; i<pixels.size(); i++)
//        dst.at<cv::Vec3b>(pixels[i].y-offset.y, pixels[i].x-offset.x) = src.at<cv::Vec3b>(pixels[i].y-offset.y, pixels[i].x-offset.x);
//
//    }
//
//  }
  
  /*****************************************************************************/
  // draw
  /*****************************************************************************/
  void draw(cv::Mat & dst, const cv::Point & offset = cv::Point(0,0)) const {
    
    if(dst.channels() == 1) {
      
      for(uint32_t i=0; i<pixels.size(); i++)
        dst.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x) = pixels[i].z;
      
    }
    
    if(dst.channels() == 3) {
      
      for(uint32_t i=0; i<pixels.size(); i++)
        dst.at<cv::Vec3b>(pixels[i].y-offset.y, pixels[i].x-offset.x) = cv::Vec3b(pixels[i].z, pixels[i].z, pixels[i].z);
      
    }
    
  }
  
  /*****************************************************************************/
  // draw
  /*****************************************************************************/
  void drawLabel(cv::Mat & dst, const cv::Vec3b & color = cv::Vec3b(255,255,0)) const {
    
    cv::drawNumber(dst, label, cv::Point2i(baricenter.x + size.width  * 0.5, baricenter.y + size.height * 0.5), color);
    
  }

  /*****************************************************************************/
  // drawMask
  /*****************************************************************************/
  void drawMask(cv::Mat & dst, const cv::Point & offset = cv::Point(0,0)) const {
    
    draw(dst, cv::Vec3b(255,255,255), offset);
    
  }
  
  /*****************************************************************************/
  // drawContours
//  /*****************************************************************************/
//  void drawContours(cv::Mat & dst, bool mode, const cv::Vec3b & color = cv::Vec3b(0,0,255)) const {
//    
//    if(dst.channels() != 3)
//      mpl::log::error("The destination image not have three channel.\n");
//    
//    std::vector<cv::Point> contours;
//    
//    getContour(mode, contours);
//    
//    for(uint32_t i=0; i<contours.size(); ++i)
//      dst.at<cv::Vec3b>(contours[i]) = color;
//    
//  }
  
  /*****************************************************************************/
  // drawBaricenter
  /*****************************************************************************/
//  void drawBaricenter(cv::Mat & dst, const char * algo, const cv::Vec3b & color = cv::Vec3b(255,0,0)) {
//
//    if(dst.channels() != 3)
//      mpl::log::error("The destination image not have three channel.\n");
//
//    computeBaricenter(algo);
//
//    int32_t x = (int32_t) baricenter.x;
//    int32_t y = (int32_t) baricenter.y;
//
//    if(y<0 || y>=dst.rows || x<0 || x>=dst.cols)
//      mpl::log::error("Baricenter position not in image range.\n");
//
//    dst.at<cv::Vec3b>(y,x) = color;
//
//  }
  
  /*****************************************************************************/
  // shift
  /*****************************************************************************/
//  void shift(const cv::Point & offset) {
//
//    for(uint32_t i=0; i<pixels.size(); i++) {
//      pixels[i].x += offset.x;
//      pixels[i].y += offset.y;
//    }
//
//  }
  

  
  /*****************************************************************************/
  // setPixels
  /*****************************************************************************/
//  void setPixels(const cv::Mat & src, const cv::Point & offset = cv::Point(0,0)) {
//
//    iMin = UINT_MAX; iMax = 0;
//
//    for(uint32_t i=0; i<pixels.size(); i++) {
//
//      pixels[i].z = src.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x);
//
//      if(pixels[i].z > iMax) iMax = pixels[i].z;
//      if(pixels[i].z < iMin) iMin = pixels[i].z;
//
//    }
//
//  }
  
  /*****************************************************************************/
  // operator <
  /*****************************************************************************/
//  inline bool operator < (const asBlob & blob) const {
//
//    if(baricenter.y < blob.baricenter.y) return true;
//    else if(baricenter.y > blob.baricenter.y) return false;
//    else if(baricenter.x < blob.baricenter.x) return true;
//    else return false;
//
//    return false;
//
//  }
  
  /*****************************************************************************/
  // getContour
  /*****************************************************************************/
//  void getContour(bool mode, std::vector<cv::Point> & contours) const {
//
//    cv::Size size_i;
//
//    size_i.width  = size.width  + 2;
//    size_i.height = size.height + 2;
//
//    cv::Point offset_i;
//
//    offset_i.x = offset.x - 1;
//    offset_i.y = offset.y - 1;
//
//    cv::Mat image = cv::Mat::zeros(size_i, CV_8UC1);
//
//    drawMask(image, offset_i);
//
//    contours.clear();
//
//    for(int32_t y=0; y<image.rows; ++y) {
//
//      for(int32_t x=0; x<image.cols; ++x) {
//
//        // con !0 faccio l'esterno con ==0 l'interno
//        if(mode == EXTERNAL_CONTOUR && image.at<uchar>(y,x) != 0) continue;
//        if(mode == INTERNAL_CONTOUR && image.at<uchar>(y,x) == 0) continue;
//
//        if((y-1) >= 0 && image.at<uchar>(y-1,x) != 0){
//          contours.push_back(cv::Point(x,y));
//          continue;
//        }
//
//        if((y+1) >= 0 && image.at<uchar>(y+1,x) != 0){
//          contours.push_back(cv::Point(x,y));
//          continue;
//        }
//
//        if((x+1) >= 0 && image.at<uchar>(y,x+1) != 0){
//          contours.push_back(cv::Point(x,y));
//          continue;
//        }
//
//        if((x-1) >= 0 && image.at<uchar>(y,x-1) != 0){
//          contours.push_back(cv::Point(x,y));
//          continue;
//        }
//
//      }
//
//    }
//
//    for(uint32_t i=0; i<contours.size(); ++i){
//      contours[i].x += offset_i.x;
//      contours[i].y += offset_i.y;
//    }
//
//  }
  
};

/*****************************************************************************/
// operator -
/*****************************************************************************/
double operator - (const asBlob & A, const asBlob & B) {

  return cv::sqrt(((A.baricenter.x-B.baricenter.x)*(A.baricenter.x-B.baricenter.x)) + ((A.baricenter.y-B.baricenter.y)*(A.baricenter.y-B.baricenter.y)));

}







/*****************************************************************************/
// asBlobs
/*****************************************************************************/
class asBlobs {
  
public:
  
  std::vector<asBlob> blobs;
  
  /*****************************************************************************/
  //  operator []
  /*****************************************************************************/
  asBlob & operator [] (size_t index) { return blobs[index]; }
  
  /*****************************************************************************/
  // size
  /*****************************************************************************/
  size_t size() const { return blobs.size(); }
  
  /*****************************************************************************/
  // add
  /*****************************************************************************/
  void add(asBlob & blob) { blobs.push_back(blob); }
  
  /*****************************************************************************/
  // baricenter
  /*****************************************************************************/
  cv::Point2d baricenter() {
    
    cv::Point2d coord(0,0);
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      coord += blobs[i].baricenter;
    
    coord /= blobs.size();
    
    return coord;
    
  }
  
  /*****************************************************************************/
  // draw
  /*****************************************************************************/
  void draw(cv::Mat & dst, const cv::Vec3b & color) const {
    
   if(dst.channels() != 1 && dst.channels() !=3)
        mpl::log::error("The destination image is not a one or three channel image.\n");
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      blobs[i].draw(dst, color);
    
  }
  
  /*****************************************************************************/
  // draw
  /*****************************************************************************/
  void draw(cv::Mat & dst) const {
    
    if(dst.channels() != 1 && dst.channels() !=3)
         mpl::log::error("The destination image is not a one or three channel image.\n");
       
    for(uint32_t i=0; i<blobs.size(); ++i)
      blobs[i].draw(dst);
    
  }
  
  /*****************************************************************************/
  // drawMask
  /*****************************************************************************/
  void drawMask(cv::Mat & dst) const {
    
    if(dst.channels() != 1)
         mpl::log::error("The destination image is not a one channel image.\n");
       
    for(uint32_t i=0; i<blobs.size(); ++i)
      blobs[i].drawMask(dst);
    
  }
  
  /*****************************************************************************/
  // drawBaricenters
  /*****************************************************************************/
  void drawBaricenters(cv::Mat & dst, const cv::Vec3b & color = cv::Vec3b(255,0,0)) {
    
    if(dst.channels() !=3)
         mpl::log::error("The destination image is not a three channel image.\n");
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      dst.at<cv::Vec3b>(blobs[i].baricenter) = color;
          
  }
  
  /*****************************************************************************/
  // drawLabels
  /*****************************************************************************/
  void drawLabels(cv::Mat & dst, const cv::Vec3b & color = cv::Vec3b(255,255,0)) {
    
    if(dst.channels() !=3)
         mpl::log::error("The destination image is not a three channel image.\n");
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      blobs[i].drawLabel(dst);
    
  }
  
//  void drawContorus(cv::Mat & dst, const cv::Vec3b & color = cv::Vec3b(0,0,255)) {
//    
//    // Vettore con i pixels del contorno
//    std::vector<cv::Point> contours;
//    
//    // Mi trovo il contorni
//    cvl::contours::get(resultsImage, cvl::contours::INTERNAL, contours);
//    
//    for(uint32_t k=0; k<contours.size(); ++k)
//      finalImg.at<cv::Vec3b>(contours[k]) = color;
//    
//  }
  
  /*****************************************************************************/
  // save
  /*****************************************************************************/
  void save(FILE * output, uint32_t frame) {
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      fprintf(output, "%04d %.4f %.4f %0d\n", frame, blobs[i].baricenter.x, blobs[i].baricenter.y, i);
    
  }
  
};

#endif /* _H_CVL_BLOBS_H_ */


