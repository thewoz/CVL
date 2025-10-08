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

#ifndef _H_CVL_BLOB_H_
#define _H_CVL_BLOB_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

#include <mpl/log.hpp>
#include <mpl/graph.hpp>

#include "contours.hpp"
#include "baricenter.hpp"

//*****************************************************************************
// namespace
//*****************************************************************************
namespace cvl {

  //*****************************************************************************
  // blob_t
  //*****************************************************************************
  class blob_t : public mpl::graph::node_t {
    
  public:
    
    std::vector<cv::Point3i> pixels;
    
  public:
    
    static std::string baricenterType;
    
    //enum { EXTERNAL_CONTOUR, INTERNAL_CONTOUR };
    
    int label;
    
    int iMin; int iMax;
    
    cv::Point2d baricenter;
    
    cv::Rect roi;
    
    cv::Point2d offset;
    
    double avgIntensity;
    
    //cv::RotatedRect rotatedRect;
    
    //*****************************************************************************
    // blob_t
    //*****************************************************************************
    blob_t() { pixels.clear(); label = -1; }
    
    //*****************************************************************************
    // add
    //*****************************************************************************
    void add(int x, int y, int i) { pixels.push_back(cv::Point3i(x,y,i)); }
    
    //*****************************************************************************
    // size
    //*****************************************************************************
    size_t size() const { return pixels.size(); }
    
    //*****************************************************************************
    // getImageSize
    //*****************************************************************************
    cv::Size getImageSize() const { return cv::Size(roi.width, roi.height); }
    
    //*****************************************************************************
    // clear
    //*****************************************************************************
    void clear() { pixels.clear(); }
    
    //*****************************************************************************
    // finalize
    //*****************************************************************************
    void finalize() {
      
      int xMin = INT_MAX; int xMax = 0;
      int yMin = INT_MAX; int yMax = 0;
      
      iMin = INT_MAX; iMax = 0;
      
      //std::vector<cv::Point2f> tmpPixels;
      
      avgIntensity = 0;
      
      for(size_t i=0; i<pixels.size(); i++) {
        
        if(pixels[i].x > xMax) xMax = pixels[i].x;
        if(pixels[i].x < xMin) xMin = pixels[i].x;
        if(pixels[i].y > yMax) yMax = pixels[i].y;
        if(pixels[i].y < yMin) yMin = pixels[i].y;
        
        if(pixels[i].z > iMax) iMax = pixels[i].z;
        if(pixels[i].z < iMin) iMin = pixels[i].z;
        
        avgIntensity += pixels[i].z;;
        
        //tmpPixels.push_back(cv::Point2d(pixels[i].x, pixels[i].y));
        
      }
      
      avgIntensity /= (double)pixels.size();
      
      offset = cv::Point(xMin, yMin);
      
      roi = cv::Rect(xMin, yMin, xMax - xMin + 1, yMax - yMin + 1);
      
      baricenter = cvl::baricenter::get(pixels, baricenterType);
      
      //rotatedRect = cv::minAreaRect(tmpPixels);
      
    }
    
    //*****************************************************************************
    // print()
    //*****************************************************************************
    void print(FILE * out = stdout) {
      
      fprintf(out, "label: %d\n", label);
      fprintf(out, "roi: %d %d - %d %d\n", roi.x, roi.y, roi.width, roi.height);
      fprintf(out, "offset: %f %f\n", offset.x, offset.y);
      fprintf(out, "baricenter: %f %f\n", baricenter.x, baricenter.y);
      fprintf(out, "iMin: %d iMax: %d\n", iMin, iMax);
      fprintf(out, "size: %lu\n", pixels.size());
      fprintf(out, "pixels:\n");
      for(int i=0; i<pixels.size(); ++i) fprintf(out, "\t%d) %d %d - %d\n", i, pixels[i].x, pixels[i].y, pixels[i].z);
      fprintf(out, "\n");
      
    }
    
//    //*****************************************************************************
//    // draw
//    //*****************************************************************************
//      void draw(cv::Mat & dst, const cv::Mat & src, const cv::Point & offset = cv::Point(0,0)) const {
//    
//        if(dst.channels() != 1 && dst.channels() !=3)
//          mpl::log::error("The destination image is not a one or three channel image.\n");
//    
//        if(dst.channels() == 1) {
//    
//          for(uint32_t i=0; i<pixels.size(); i++)
//            dst.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x) = src.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x);
//    
//        }
//    
//        if(dst.channels() == 3) {
//    
//          for(uint32_t i=0; i<pixels.size(); i++)
//            dst.at<cv::Vec3b>(pixels[i].y-offset.y, pixels[i].x-offset.x) = src.at<cv::Vec3b>(pixels[i].y-offset.y, pixels[i].x-offset.x);
//    
//        }
//    
//      }
    
//    //*****************************************************************************
//    // draw
//    //*****************************************************************************
//    void draw(cv::Mat & dst, int value) {
//      
//      for(uint32_t i=0; i<pixels.size(); i++)
//        dst.at<int>(pixels[i].y, pixels[i].x) = value;
//      
//    }
    
    //*****************************************************************************
    // draw
    //*****************************************************************************
    void draw(cv::Mat & dst, const cv::Vec3b & color, const cv::Point & offset = cv::Point(0,0)) const {
      
      bool status = false;

      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++){
          dst.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x) = color[0];
        }
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<cv::Vec3b>(pixels[i].y-offset.y, pixels[i].x-offset.x) = color;
        status = true;
      }
       
      if(!status) {
        fprintf(stderr, "blob_t::draw() error: image type not recognized\n");
        abort();
      }

    }
    
    //*****************************************************************************
    // draw
    //*****************************************************************************
    void draw(cv::Mat & dst, int color, const cv::Point & offset = cv::Point(0,0)) const {
      
      bool status = false;
      
      if(dst.type() == CV_32SC1) {
        for(size_t i=0; i<pixels.size(); i++){
          dst.at<int>(pixels[i].y-offset.y, pixels[i].x-offset.x) = color;
        }
        status = true;
      }
       
      if(!status) {
        fprintf(stderr, "blob_t::draw() error: image type not recognized\n");
        abort();
      }

    }
    
    //*****************************************************************************
    // draw
    //*****************************************************************************
    void draw(cv::Mat & dst, const cv::Point & offset = cv::Point(0,0)) const {
      
      bool status = false;
      
      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x) = pixels[i].z;
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<cv::Vec3b>(pixels[i].y-offset.y, pixels[i].x-offset.x) = cv::Vec3b(pixels[i].z, pixels[i].z, pixels[i].z);
        status = true;
      }
      
      if(!status) {
        fprintf(stderr, "blob_t::draw() error: image type not recognized\n");
        abort();
      }
      
    }
    
    //*****************************************************************************
    // draw
    //*****************************************************************************
    void draw(cv::Mat & dst, const cv::Mat & ori, const cv::Point & offset = cv::Point(0,0)) const {
      
      bool status = false;
      
      if(dst.type() != ori.type()) {
        fprintf(stderr, "blob_t::draw() error: image must have the same type\n");
        abort();
      }
      
      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++) {
          int x = pixels[i].x - offset.x;
          int y = pixels[i].y - offset.y;
          dst.at<uchar>(y,x) = ori.at<uchar>(pixels[i].y, pixels[i].x);
        }
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++){
          int x = pixels[i].x - offset.x;
          int y = pixels[i].y - offset.y;
          uchar value = ori.at<uchar>(pixels[i].y, pixels[i].x);
          dst.at<cv::Vec3b>(y,x) = cv::Vec3b(value, value, value);
        }
        status = true;
      }
       
      if(!status) {
        fprintf(stderr, "blob_t::draw() error: image type not recognized\n");
        abort();
      }
      
    }
    
    //*****************************************************************************
    // drawInv
    //*****************************************************************************
    void drawInv(cv::Mat & dst, const cv::Point & offset = cv::Point(0,0)) const {
      
      bool status = false;

      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<uchar>(pixels[i].y-offset.y, pixels[i].x-offset.x) = 255 - pixels[i].z;
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<cv::Vec3b>(pixels[i].y-offset.y, pixels[i].x-offset.x) = cv::Vec3b(255 - pixels[i].z, 255 - pixels[i].z, 255 - pixels[i].z);
        status = true;
      }
      
      if(!status) {
        fprintf(stderr, "blob_t::draw() error: image type not recognized\n");
        abort();
      }
      
    }
    
    //*****************************************************************************
    // drawInv
    //*****************************************************************************
    void drawInv(cv::Mat & dst, const cv::Mat & ori, const cv::Point & offset = cv::Point(0,0)) const {
      
      bool status = false;
      
      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++) {
          int x = pixels[i].x - offset.x;
          int y = pixels[i].y - offset.y;
          dst.at<uchar>(y,x) = 255 - ori.at<uchar>(pixels[i].y, pixels[i].x);
        }
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++){
          int x = pixels[i].x - offset.x;
          int y = pixels[i].y - offset.y;
          uchar value = 255 - ori.at<uchar>(pixels[i].y, pixels[i].x);
          dst.at<cv::Vec3b>(y,x) = cv::Vec3b(value,  value, value);
        }
        status = true;
      }
      
      if(!status) {
        fprintf(stderr, "blob_t::draw() error: image type not recognized\n");
        abort();
      }
      
    }
    
    //*****************************************************************************
    // drawMask
    //*****************************************************************************
    void drawMask(cv::Mat & dst, const cv::Point & offset = cv::Point(0,0)) const { draw(dst, cv::Vec3b(255,255,255), offset); }
    
    //*****************************************************************************
    // drawLabel
    //*****************************************************************************
    void drawLabel(cv::Mat & dst, const cv::Vec3b & color = cvl::Yellow, int scaleFactor = 1) const {
      
      cvl::drawNumber(dst, label, cv::Point2i((baricenter.x*scaleFactor) + roi.width * 0.5, (baricenter.y*scaleFactor) + roi.height * 0.5), color);
      
    }
    
    //*****************************************************************************
    // saveContours
    //*****************************************************************************
    void saveContours(FILE * output, int camera, int frame) const {
      
      fprintf(output, "%d %d %d %lu ", camera, frame, label, pixels.size());
      
      for(size_t i=0; i<pixels.size(); ++i)
        fprintf(output, "%d %d ", pixels[i].x, pixels[i].y);
      
      fprintf(output, "\n\n\n");
      
    }
    
    //*****************************************************************************
    // println
    //*****************************************************************************
    void println(FILE * output = stdout) const {
      
      fprintf(output, "%d %d %lu\n", frame, label, pixels.size());
      
      for(size_t i=0; i<pixels.size(); ++i)
        fprintf(output, "\t %d %d\n", pixels[i].x, pixels[i].y);
      
      fprintf(output, "\n");
      
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
    inline bool operator < (const cvl::blob_t & blob) const {
      
           if(baricenter.y < blob.baricenter.y) return true;
      else if(baricenter.y > blob.baricenter.y) return false;
      else if(baricenter.x < blob.baricenter.x) return true;
      else return false;
      
      return false;
      
    }
    
    //*****************************************************************************
    //  operator []
    //*****************************************************************************
    const cv::Point3i & operator [] (size_t index) const { return pixels[index]; }
          cv::Point3i & operator [] (size_t index)       { return pixels[index]; }
    
    //*****************************************************************************
    // getContour
    //*****************************************************************************
    //void getContour(bool mode, std::vector<cv::Point> & contours) const {
    //  void getContour(std::vector<cv::Point> & contours) const {
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
    //        //if(mode == EXTERNAL_CONTOUR && image.at<uchar>(y,x) != 0) continue;
    //        //if(mode == INTERNAL_CONTOUR && image.at<uchar>(y,x) == 0) continue;
    //        if(image.at<uchar>(y,x) != 0) continue;
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
  double operator - (const cvl::blob_t & A, const cvl::blob_t & B) {
    
    return cv::sqrt(((A.baricenter.x-B.baricenter.x)*(A.baricenter.x-B.baricenter.x)) + ((A.baricenter.y-B.baricenter.y)*(A.baricenter.y-B.baricenter.y)));
    
  }

  std::string cvl::blob_t::baricenterType = "GEOMETRIC";

} /* namespace */


#endif /* _H_CVL_BLOB_H_ */


