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

#ifndef _H_CVL_BLOB_H_
#define _H_CVL_BLOB_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>
#include <climits>

//*****************************************************************************
// namespace
//*****************************************************************************
namespace cvl {

  //*****************************************************************************
  // blob_t
  //*****************************************************************************
  // Container for a connected component with pixel list and derived statistics.
  class blob_t {

  public:

    std::vector<cv::Point3i> pixels;

  public:

    static std::string baricenterType;

    //enum { EXTERNAL_CONTOUR, INTERNAL_CONTOUR };

    int frame = -1;

    int label = -1;

    // inizializzati da finalize(); il valore di default evita letture
    // indeterminate (print(), untilSplit) su blob mai finalizzati
    int iMin = 0; int iMax = 0;

    cv::Point2d baricenter;

    cv::Rect roi;

    cv::Point2d offset;

    double avgIntensity = 0;
    
    //cv::RotatedRect rotatedRect;
    
    //*****************************************************************************
    // blob_t
    //*****************************************************************************
    // Create an empty blob with label -1.
    blob_t() { pixels.clear(); label = -1; }
    
    //*****************************************************************************
    // add
    //*****************************************************************************
    // Append a pixel (x, y, intensity) to the blob.
    void add(int x, int y, int i) { pixels.push_back(cv::Point3i(x,y,i)); }
    
    //*****************************************************************************
    // size
    //*****************************************************************************
    // Return the number of pixels in the blob.
    size_t size() const { return pixels.size(); }
    
    //*****************************************************************************
    // getImageSize
    //*****************************************************************************
    // Return the width/height of the blob bounding box.
    cv::Size getImageSize() const { return cv::Size(roi.width, roi.height); }
    
    //*****************************************************************************
    // clear
    //*****************************************************************************
    // Remove all pixels from the blob.
    void clear() { pixels.clear(); }
    
    //*****************************************************************************
    // finalize
    //*****************************************************************************
    // Compute min/max, average intensity, ROI, and barycenter for the blob.
    void finalize() {
      
      if(pixels.empty()) return;
      
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
    // Print blob information to the provided stream.
    void print(FILE * out = stdout) {
      
      fprintf(out, "label: %d\n", label);
      fprintf(out, "roi: %d %d - %d %d\n", roi.x, roi.y, roi.width, roi.height);
      fprintf(out, "offset: %f %f\n", offset.x, offset.y);
      fprintf(out, "baricenter: %f %f\n", baricenter.x, baricenter.y);
      fprintf(out, "iMin: %d iMax: %d\n", iMin, iMax);
      fprintf(out, "size: %zu\n", pixels.size());
      fprintf(out, "pixels:\n");
      for(int i=0; i<(int)pixels.size(); ++i) fprintf(out, "\t%d) %d %d - %d\n", i, pixels[i].x, pixels[i].y, pixels[i].z);
      fprintf(out, "\n");
      
    }

    //*****************************************************************************
    // draw
    //*****************************************************************************
    // NOTA (vale per tutti i draw*): i pixel vengono scritti con at<>() senza
    // bounds check; dst, tenuto conto di offset, deve essere abbastanza grande
    // da contenere il ROI del blob, altrimenti la scrittura è out-of-bounds.
    //*****************************************************************************
    // Draw the blob onto dst using the provided color (8-bit 1 or 3 channel).
    void draw(cv::Mat & dst, const cv::Vec3b & color, const cv::Point & _offset = cv::Point(0,0)) const {
      
      bool status = false;

      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++){
          dst.at<uchar>(pixels[i].y-_offset.y, pixels[i].x-_offset.x) = color[0];
        }
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<cv::Vec3b>(pixels[i].y-_offset.y, pixels[i].x-_offset.x) = color;
        status = true;
      }
       
      if(!status) {
        cvl::log::error("cvl::blob_t::draw() error: image type not recognized\n");
      }

    }
    
    //*****************************************************************************
    // draw
    //*****************************************************************************
    // Draw the blob onto an int label image.
    void draw(cv::Mat & dst, int color, const cv::Point & _offset = cv::Point(0,0)) const {
      
      bool status = false;
      
      if(dst.type() == CV_32SC1) {
        for(size_t i=0; i<pixels.size(); i++){
          dst.at<int>(pixels[i].y-_offset.y, pixels[i].x-_offset.x) = color;
        }
        status = true;
      }
       
      if(!status) {
        cvl::log::error("cvl::blob_t::draw() error: image type not recognized\n");
      }

    }
    
    //*****************************************************************************
    // draw
    //*****************************************************************************
    // Draw the blob using stored intensity values.
    void draw(cv::Mat & dst, const cv::Point & _offset = cv::Point(0,0)) const {
      
      bool status = false;
      
      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<uchar>(pixels[i].y-_offset.y, pixels[i].x-_offset.x) = pixels[i].z;
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<cv::Vec3b>(pixels[i].y-_offset.y, pixels[i].x-_offset.x) = cv::Vec3b(pixels[i].z, pixels[i].z, pixels[i].z);
        status = true;
      }
      
      if(!status) {
        cvl::log::error("cvl::blob_t::draw() error: image type not recognized\n");
      }
      
    }
    
    //*****************************************************************************
    // draw
    //*****************************************************************************
    // Draw the blob by sampling pixel values from the original image.
    void draw(cv::Mat & dst, const cv::Mat & ori, const cv::Point & _offset = cv::Point(0,0)) const {
      
      bool status = false;
      
      if(dst.type() != ori.type()) {
        cvl::log::error("cvl::blob_t::draw() error: images must have the same type\n");
      }
      
      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++) {
          int x = pixels[i].x - _offset.x;
          int y = pixels[i].y - _offset.y;
          dst.at<uchar>(y,x) = ori.at<uchar>(pixels[i].y, pixels[i].x);
        }
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++){
          int x = pixels[i].x - _offset.x;
          int y = pixels[i].y - _offset.y;
          dst.at<cv::Vec3b>(y,x) = ori.at<cv::Vec3b>(pixels[i].y, pixels[i].x);
        }
        status = true;
      }
       
      if(!status) {
        cvl::log::error("cvl::blob_t::draw() error: image type not recognized\n");
      }
      
    }
    
    //*****************************************************************************
    // drawInv
    //*****************************************************************************
    // Draw the blob with inverted intensity values.
    void drawInv(cv::Mat & dst, const cv::Point & _offset = cv::Point(0,0)) const {
      
      bool status = false;

      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<uchar>(pixels[i].y-_offset.y, pixels[i].x-_offset.x) = 255 - pixels[i].z;
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++)
          dst.at<cv::Vec3b>(pixels[i].y-_offset.y, pixels[i].x-_offset.x) = cv::Vec3b(255 - pixels[i].z, 255 - pixels[i].z, 255 - pixels[i].z);
        status = true;
      }
      
      if(!status) {
        cvl::log::error("cvl::blob_t::draw() error: image type not recognized\n");
      }
      
    }
    
    //*****************************************************************************
    // drawInv
    //*****************************************************************************
    // Draw the blob with inverted intensity values from the original image.
    void drawInv(cv::Mat & dst, const cv::Mat & ori, const cv::Point & _offset = cv::Point(0,0)) const {
      
      bool status = false;
      
      if(dst.type() == CV_8UC1) {
        for(size_t i=0; i<pixels.size(); i++) {
          int x = pixels[i].x - _offset.x;
          int y = pixels[i].y - _offset.y;
          dst.at<uchar>(y,x) = 255 - ori.at<uchar>(pixels[i].y, pixels[i].x);
        }
        status = true;
      }
      
      if(dst.type() == CV_8UC3) {
        for(size_t i=0; i<pixels.size(); i++){
          int x = pixels[i].x - _offset.x;
          int y = pixels[i].y - _offset.y;
          const cv::Vec3b & value = ori.at<cv::Vec3b>(pixels[i].y, pixels[i].x);
          dst.at<cv::Vec3b>(y,x) = cv::Vec3b(255 - value[0], 255 - value[1], 255 - value[2]);
        }
        status = true;
      }
      
      if(!status) {
        cvl::log::error("cvl::blob_t::draw() error: image type not recognized\n");
      }
      
    }
    
    //*****************************************************************************
    // drawMask
    //*****************************************************************************
    // Draw a white mask of the blob.
    void drawMask(cv::Mat & dst, const cv::Point & _offset = cv::Point(0,0)) const { draw(dst, cv::Vec3b(255,255,255), _offset); }
    
    //*****************************************************************************
    // drawLabel
    //*****************************************************************************
    // Draw the blob label at its barycenter.
    void drawLabel(cv::Mat & dst, const cv::Vec3b & color = cvl::yellow, int scaleFactor = 1) const {
      
      cvl::drawNumber(dst, label, cv::Point2i((baricenter.x*scaleFactor) + roi.width * 0.5, (baricenter.y*scaleFactor) + roi.height * 0.5), color);
      
    }
    
    //*****************************************************************************
    // saveContours
    //*****************************************************************************
    // Save blob contour points to a stream.
    void saveContours(FILE * output, int camera, int _frame) const {
      
      fprintf(output, "%d %d %d %zu ", camera, _frame, label, pixels.size());
      
      for(size_t i=0; i<pixels.size(); ++i)
        fprintf(output, "%d %d ", pixels[i].x, pixels[i].y);
      
      fprintf(output, "\n\n\n");
      
    }
    
    //*****************************************************************************
    // println
    //*****************************************************************************
    // Print blob pixel coordinates in a simple text format.
    void println(FILE * output = stdout) const {
      
      fprintf(output, "%d %d %zu\n", frame, label, pixels.size());
      
      for(size_t i=0; i<pixels.size(); ++i)
        fprintf(output, "\t %d %d\n", pixels[i].x, pixels[i].y);
      
      fprintf(output, "\n");

    }

    //*****************************************************************************
    // operator <
    //*****************************************************************************
    // Sort blobs by barycenter position (row-major).
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
    // Access a pixel by index.
    const cv::Point3i & operator [] (size_t index) const { return pixels[index]; }
          cv::Point3i & operator [] (size_t index)       { return pixels[index]; }

  };

  //*****************************************************************************
  // operator -
  //*****************************************************************************
  // Return Euclidean distance between two blob barycenters.
  inline double operator - (const cvl::blob_t & A, const cvl::blob_t & B) {
    
    return cv::sqrt(((A.baricenter.x-B.baricenter.x)*(A.baricenter.x-B.baricenter.x)) + ((A.baricenter.y-B.baricenter.y)*(A.baricenter.y-B.baricenter.y)));
    
  }

  inline std::string cvl::blob_t::baricenterType = "GEOMETRIC";

} /* namespace */


#endif /* _H_CVL_BLOB_H_ */

