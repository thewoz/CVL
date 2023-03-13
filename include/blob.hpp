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

#ifndef _H_CVL_BLOBS_H_
#define _H_CVL_BLOBS_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

#include <mpl/log.hpp>
#include <mpl/opencv.hpp>

#include "contours.hpp"

#include "baricenter.hpp"

//*****************************************************************************
// namespace
//*****************************************************************************
namespace cvl {

//*****************************************************************************
// blob_t
//*****************************************************************************
 class blob_t {
  
public:
   
  static std::string baricenterType;
  
  enum { EXTERNAL_CONTOUR, INTERNAL_CONTOUR };
  
  int32_t label;
  
  uint32_t iMin; uint32_t iMax;
  
  std::vector<cv::Point3i> pixels;
  
  cv::Point2d baricenter;
  
  cv::Rect roi;
  
  cv::Size size;
  
  cv::Point offset;
  
  //*****************************************************************************
  // blob_t
  //*****************************************************************************
  blob_t() { pixels.clear(); label = -1; }
  
  //*****************************************************************************
  // add
  //*****************************************************************************
  void add(uint32_t x, uint32_t y, uint32_t i) { pixels.push_back(cv::Point3i(x,y,i)); }
  
  //*****************************************************************************
  // mass
  //*****************************************************************************
  size_t mass() const { return pixels.size(); }
   
  //*****************************************************************************
  // clear
  //*****************************************************************************
   void clear() { pixels.clear(); }
   
  //*****************************************************************************
  // finalize
  //*****************************************************************************
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
    
    baricenter = cvl::baricenter::get(pixels, baricenterType);
    
  }
  
  //*****************************************************************************
  // draw
  //*****************************************************************************
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
  
   void draw(cv::Mat & dst, int value) {
     
     for(uint32_t i=0; i<pixels.size(); i++)
       dst.at<int>(pixels[i].y, pixels[i].x) = value;

   }
   
   
   
   //*****************************************************************************
  // draw
   //*****************************************************************************
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
  
  //*****************************************************************************
  // draw
  //*****************************************************************************
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
  
  //*****************************************************************************
  // draw
  //*****************************************************************************
  void drawLabel(cv::Mat & dst, const cv::Vec3b & color = cv::Yellow, int scaleFactor = 1) const {
    
    cv::drawNumber(dst, label, cv::Point2i((baricenter.x*scaleFactor) + size.width * 0.5, (baricenter.y*scaleFactor) + size.height * 0.5), color);
    
  }

  //*****************************************************************************
  // drawMask
  //*****************************************************************************
  void drawMask(cv::Mat & dst, const cv::Point & offset = cv::Point(0,0)) const {
    
    draw(dst, cv::Vec3b(255,255,255), offset);
    
  }
   
  
  //*****************************************************************************
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
  
  //*****************************************************************************
  // drawBaricenter
  //*****************************************************************************
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
  
  //*****************************************************************************
  // shift
  //*****************************************************************************
//  void shift(const cv::Point & offset) {
//
//    for(uint32_t i=0; i<pixels.size(); i++) {
//      pixels[i].x += offset.x;
//      pixels[i].y += offset.y;
//    }
//
//  }
  

  
  //*****************************************************************************
  // setPixels
  //*****************************************************************************
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
  
  //*****************************************************************************
  // operator <
  //*****************************************************************************
  inline bool operator < (const blob_t & blob) const {

         if(baricenter.y < blob.baricenter.y) return true;
    else if(baricenter.y > blob.baricenter.y) return false;
    else if(baricenter.x < blob.baricenter.x) return true;
    else return false;

    return false;

  }
  
  //*****************************************************************************
  // getContour
  //*****************************************************************************
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

//*****************************************************************************
// operator -
//*****************************************************************************
double operator - (const blob_t & A, const blob_t & B) {

  return cv::sqrt(((A.baricenter.x-B.baricenter.x)*(A.baricenter.x-B.baricenter.x)) + ((A.baricenter.y-B.baricenter.y)*(A.baricenter.y-B.baricenter.y)));

}




//*****************************************************************************
// blobs_t
//*****************************************************************************
class blobs_t {
  
public:
  
  std::vector<blob_t> blobs;
  
  cv::Point2d baricenter;
  
  //*****************************************************************************
  //  operator []
  //*****************************************************************************
  blob_t & operator [] (size_t index) { return blobs[index]; }
  
  //*****************************************************************************
  // size
  //*****************************************************************************
  size_t size() const { return blobs.size(); }
  
  //*****************************************************************************
  // add
  //*****************************************************************************
  void add(const blob_t & blob) { blobs.push_back(blob); }
  
  //*****************************************************************************
  // finalize
  //*****************************************************************************
  void finalize() {

    // ordino i punti
    std::sort(blobs.begin(), blobs.end());

    baricenter = cv::Point2d(0,0);

    // mi calcolo il baricentro e rinomino i label dei punti
    for(uint32_t i=0; i<blobs.size(); ++i) {

      blobs[i].finalize();
      
      baricenter += blobs[i].baricenter;

      blobs[i].label = i;

    }

    baricenter /= (double) blobs.size();

  }
  
  
  //*****************************************************************************
  // draw
  //*****************************************************************************
  void draw(cv::Mat & dst, const cv::Vec3b & color, bool mode = true) const {
    
   if(dst.channels() != 1 && dst.channels() !=3)
        mpl::log::error("The destination image is not a one or three channel image.\n");
    
    if(mode) dst.setTo(0);
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      blobs[i].draw(dst, color);
    
  }
  
  //*****************************************************************************
  // draw
  //*****************************************************************************
  void draw(cv::Mat & dst, bool mode = true) const {

    if(dst.channels() != 1 && dst.channels() !=3)
         mpl::log::error("The destination image is not a one or three channel image.\n");
    
    if(mode) dst.setTo(0);
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      blobs[i].draw(dst);
    
  }
  
  //*****************************************************************************
  // drawMask
  //*****************************************************************************
  void drawMask(cv::Mat & dst, bool mode = true) const {
    
    if(dst.channels() != 1)
         mpl::log::error("The destination image is not a one channel image.\n");
       
    if(mode) dst.setTo(0);
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      blobs[i].drawMask(dst);
    
  }
  
  //*****************************************************************************
  // drawBaricenters
  //*****************************************************************************
  void drawBaricenters(cv::Mat & dst, const cv::Vec3b & color = cv::Red, int scaleFactor = 1) {
    
    if(dst.channels() !=3)
         mpl::log::error("The destination image is not a three channel image.\n");
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      dst.at<cv::Vec3b>(cv::Point2i(blobs[i].baricenter.x*scaleFactor,blobs[i].baricenter.y*scaleFactor)) = color;
                        
    
  }
  
  //*****************************************************************************
  // drawLabels
  //*****************************************************************************
  void drawLabels(cv::Mat & dst, const cv::Vec3b & color = cv::Yellow, int scaleFactor = 1) {
    
    if(dst.channels() !=3)
         mpl::log::error("The destination image is not a three channel image.\n");
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      blobs[i].drawLabel(dst, color, scaleFactor);
    
  }
  
  //*****************************************************************************
  // drawContours
  //*****************************************************************************
  void drawContours(cv::Mat & dst, const cv::Vec3b & color = cv::Blue) {
    
    if(dst.channels() !=3)
      mpl::log::error("The destination image is not a three channel image.\n");
    
    cv::Mat mask = cv::Mat::zeros(dst.size(), CV_8UC1);
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      blobs[i].drawMask(mask);
        
    // Vettore con i pixels del contorno
    std::vector<cv::Point> contours;
    
    // Mi trovo il contorni
    cvl::contours::get(mask, cvl::contours::EXTERNAL, contours);
    
    for(uint32_t k=0; k<contours.size(); ++k)
      dst.at<cv::Vec3b>(contours[k]) = color;
    
  }
  
  //*****************************************************************************
  // enclosingCircle
  //*****************************************************************************
  void enclosingCircle(cv::Point2f & center, float & radius) {
  
    std::vector<cv::Point2f> points(blobs.size());
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      points[i] = blobs[i].baricenter;
    
    cv::minEnclosingCircle(points, center, radius);
  
  }
  
  //*****************************************************************************
  // getConvexHull
  //*****************************************************************************
  void getConvexHull(std::vector<cv::Point> & hull) {
  
    std::vector<cv::Point> points(blobs.size());
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      points[i] = blobs[i].baricenter;
    
    cv::convexHull(points, hull, false, true);
  
  }
  
  //*****************************************************************************
  // boundingRect
  //*****************************************************************************
  void boundingRect(cv::Rect & roi) {
    
    std::vector<cv::Point2f> points(blobs.size());
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      points[i] = blobs[i].baricenter;
    
    roi = cv::boundingRect(points);
  
  }
  
  //*****************************************************************************
  // getPoints
  //*****************************************************************************
  void getPoints(std::vector<cv::Point2f> & points) {
    
    points.resize(blobs.size());
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      points[i] = blobs[i].baricenter;
    
  }
  
  //*****************************************************************************
  // applyRTS
  //*****************************************************************************
  void applyRTS(const cv::Point2d & p0, cv::Mat & R, cv::Mat & T, double S = 1.0) {

    cv::Point2d tmp;
      
    for(std::size_t i=0; i<blobs.size(); ++i) {

      for(std::size_t j=0; j<blobs[i].pixels.size(); ++j) {

        tmp.x = (((((double*)R.data)[0]*(blobs[i].pixels[j].x-p0.x)) + (((double*)R.data)[1]*(blobs[i].pixels[j].y-p0.y))) * S) + ((double*)T.data)[0] + p0.x;
        tmp.y = (((((double*)R.data)[2]*(blobs[i].pixels[j].x-p0.x)) + (((double*)R.data)[3]*(blobs[i].pixels[j].y-p0.y))) * S) + ((double*)T.data)[1] + p0.y;

        blobs[i].pixels[j].x = std::round(tmp.x);
        blobs[i].pixels[j].y = std::round(tmp.y);

      }

      blobs[i].finalize();
      
    }
    
  }
  
  //*****************************************************************************
  // save
  //*****************************************************************************
  void save(FILE * outputPoints, int32_t camera, int32_t frame, FILE * outputFrame = NULL) {
    
    for(uint32_t i=0; i<blobs.size(); ++i)
      fprintf(outputPoints, "%d %04d %04d %.4f %.4f %03lu\n", camera, frame, blobs[i].label, blobs[i].baricenter.x, blobs[i].baricenter.y, blobs[i].mass());

    if(outputFrame != NULL) {
      fprintf(outputFrame, "%d %04d %04lu\n", camera, frame, blobs.size());
      fflush(outputFrame);
    }    

    fflush(outputPoints);

  }
  
};

std::string blob_t::baricenterType = "GEOMETRIC";

} /* namespace */
  

#endif /* _H_CVL_BLOBS_H_ */


