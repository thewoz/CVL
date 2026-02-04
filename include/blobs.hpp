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

#include "blob.hpp"
#include "contours.hpp"
#include "baricenter.hpp"

//*****************************************************************************
// namespace
//*****************************************************************************
namespace cvl {

  //*****************************************************************************
  // class blobs_t
  //*****************************************************************************
  /// Collection of blobs with utilities for drawing and geometry.
  class blobs_t {
  
  private:
    
    std::vector<cvl::blob_t> blobs;    

  public:
    
    cv::Point2d baricenter;
    
    //*****************************************************************************
    // blobs_t
    //*****************************************************************************
    /// Create an empty blob collection.
    blobs_t() { clear(); }
    
    //*****************************************************************************
    //  operator []
    //*****************************************************************************
    /// Access a blob by index.
    const cvl::blob_t & operator [] (size_t index) const { return blobs[index]; }
          cvl::blob_t & operator [] (size_t index)       { return blobs[index]; }

    //*****************************************************************************
    // size
    //*****************************************************************************
    /// Return number of blobs.
    size_t size() const { return blobs.size(); }
    
    //*****************************************************************************
    // clear
    //*****************************************************************************
    /// Remove all blobs.
    void clear() { return blobs.clear(); }
    
    //*****************************************************************************
    // erase
    //*****************************************************************************
    /// Remove blob at the given index.
    void erase(size_t index) { blobs.erase(blobs.begin() + index); }
    
    //*****************************************************************************
    // add
    //*****************************************************************************
    /// Append a blob.
    void add(const cvl::blob_t & blob) { blobs.push_back(blob); }
    
    //*****************************************************************************
    // add
    //*****************************************************************************
    /// Append all blobs from another collection.
    void add(const cvl::blobs_t & _blobs) { blobs.insert(blobs.end(), _blobs.begin(), _blobs.end()); }
    
    //*****************************************************************************
    // insert
    //*****************************************************************************
    /// Insert a blob at the given index.
    void insert(size_t index, const cvl::blob_t & blob) { blobs.insert(blobs.begin() + index, blob); }
    
    //*****************************************************************************
    // begin
    //*****************************************************************************
    /// Iterator to the first blob.
    std::vector<cvl::blob_t>::const_iterator begin() const { return blobs.begin(); }
    std::vector<cvl::blob_t>::iterator       begin()       { return blobs.begin(); }
    
    //*****************************************************************************
    // end
    //*****************************************************************************
    /// Iterator to the end of the collection.
    std::vector<cvl::blob_t>::const_iterator end() const { return blobs.end(); }
    std::vector<cvl::blob_t>::iterator       end()       { return blobs.end(); }

    //*****************************************************************************
    // finalize
    //*****************************************************************************
    /// Sort blobs, recompute stats, labels, and global barycenter.
    void finalize() {
      
      // ordino i punti
      std::sort(blobs.begin(), blobs.end());
      
      baricenter = cv::Point2d(0,0);
      
      // mi calcolo il baricentro e rinomino i label dei punti
      for(size_t i=0; i<blobs.size(); ++i) {
        
        blobs[i].finalize();
        
        if(!std::isnormal(blobs[i].baricenter.x) || !std::isnormal(blobs[i].baricenter.y)) {
          fprintf(stderr, "error: bad baricenter\n");
          abort();
        }
        
        baricenter += blobs[i].baricenter;
        
        blobs[i].label = (int)i;
        
      }
      
      baricenter /= (double) blobs.size();
      
    }
  
    //*****************************************************************************
    // draw
    //*****************************************************************************
    /// Draw all blobs using a single color.
    void draw(cv::Mat & dst, const cv::Vec3b & color) const {
      
      if(dst.channels() != 1 && dst.channels() !=3)
        mpl::log::error("The destination image is not a one or three channel image.\n");
            
      for(size_t i=0; i<blobs.size(); ++i)
        blobs[i].draw(dst, color);
      
    }
    
    //*****************************************************************************
    // draw
    //*****************************************************************************
    /// Draw blobs with their stored intensities.
    void draw(cv::Mat & dst, bool mode = true) const {
      
      if(dst.channels() != 1 && dst.channels() !=3)
        mpl::log::error("The destination image is not a one or three channel image.\n");
      
      if(mode) dst.setTo(0);
      
      for(size_t i=0; i<blobs.size(); ++i)
        blobs[i].draw(dst);
      
    }
    
    //*****************************************************************************
    // drawMask
    //*****************************************************************************
    /// Draw a white mask for all blobs.
    void drawMask(cv::Mat & dst, bool mode = true) const {
      
      if(dst.channels() != 1)
        mpl::log::error("The destination image is not a one channel image.\n");
      
      if(mode) dst.setTo(0);
      
      for(size_t i=0; i<blobs.size(); ++i)
        blobs[i].drawMask(dst);
      
    }
    
    //*****************************************************************************
    // drawBaricenters
    //*****************************************************************************
    /// Draw barycenters for each blob.
    void drawBaricenters(cv::Mat & dst, const cv::Vec3b & color = cvl::Red, int scaleFactor = 1) const {
      
      if(dst.channels() !=3)
        mpl::log::error("The destination image is not a three channel image.\n");
      
      for(size_t i=0; i<blobs.size(); ++i) {
        cv::Point2i coord = cv::Point2i(blobs[i].baricenter.x * scaleFactor, blobs[i].baricenter.y * scaleFactor);
        if(coord.x < 0 || coord.x >= dst.cols) continue;
        if(coord.y < 0 || coord.y >= dst.rows) continue;
        dst.at<cv::Vec3b>(coord.y, coord.x) = color;
      }
      
    }
    
    //*****************************************************************************
    // drawLabels
    //*****************************************************************************
    /// Draw labels for each blob.
    void drawLabels(cv::Mat & dst, const cv::Vec3b & color = cvl::Yellow, int scaleFactor = 1) const {
      
      if(dst.channels() !=3)
        mpl::log::error("The destination image is not a three channel image.\n");
      
      for(size_t i=0; i<blobs.size(); ++i)
        blobs[i].drawLabel(dst, color, scaleFactor);
      
    }
    
    //*****************************************************************************
    // drawContours
    //*****************************************************************************
    /// Draw external contours of the blob mask.
    void drawContours(cv::Mat & dst, const cv::Vec3b & color = cvl::Blue) const {
      
      if(dst.channels() !=3)
        mpl::log::error("The destination image is not a three channel image.\n");
      
      cv::Mat mask = cv::Mat::zeros(dst.size(), CV_8UC1);
      
      for(size_t i=0; i<blobs.size(); ++i)
        blobs[i].drawMask(mask);
      
      // Vettore con i pixels del contorno
      std::vector<cv::Point> contours;
      
      // Mi trovo il contorni
      cvl::contours::get(mask, cvl::contours::EXTERNAL, contours);
      
      for(size_t k=0; k<contours.size(); ++k)
        dst.at<cv::Vec3b>(contours[k]) = color;
      
    }
    
    //*****************************************************************************
    // enclosingCircle
    //*****************************************************************************
    /// Compute a minimum enclosing circle of blob barycenters.
    void enclosingCircle(cv::Point2f & center, float & radius) const {
      
      std::vector<cv::Point2f> points(blobs.size());
      
      for(size_t i=0; i<blobs.size(); ++i)
        points[i] = blobs[i].baricenter;
      
      cv::minEnclosingCircle(points, center, radius);
      
    }
    
    //*****************************************************************************
    // getConvexHull
    //*****************************************************************************
    /// Compute the convex hull of blob barycenters.
    void getConvexHull(std::vector<cv::Point2f> & hull) const {
      
      std::vector<cv::Point2f> points(blobs.size());
      
      for(size_t i=0; i<blobs.size(); ++i)
        points[i] = blobs[i].baricenter;
      
      cv::convexHull(points, hull, false, true);
      
    }
    
    //*****************************************************************************
    // boundingRect
    //*****************************************************************************
    /// Compute a bounding box around blob barycenters.
    void boundingRect(cv::Rect & roi) const{
      
      std::vector<cv::Point2f> points(blobs.size());
      
      for(size_t i=0; i<blobs.size(); ++i)
        points[i] = blobs[i].baricenter;
      
      roi = cv::boundingRect(points);
      
    }
    
    //*****************************************************************************
    // getBaricenter
    //*****************************************************************************
    //cv::Point2d getBaricenter() const { return baricenter; }
    
    //*****************************************************************************
    // getBarycenters
    //*****************************************************************************
    /// Fill a vector with all blob barycenters.
    void getBarycenters(std::vector<cv::Point2f> & barycenters) const {
      
      barycenters.resize(blobs.size());
      
      for(size_t i=0; i<blobs.size(); ++i)
        barycenters[i] = blobs[i].baricenter;
      
    }
    
    //*****************************************************************************
    // applyRTS
    //*****************************************************************************
    //  void applyRTS(const cv::Point2d & p0, cv::Mat & R, cv::Mat & T, double S = 1.0) {
    //
    //    cv::Point2d tmp;
    //
    //    for(std::size_t i=0; i<blobs.size(); ++i) {
    //
    //      for(std::size_t j=0; j<blobs[i].size(); ++j) {
    //
    //        tmp.x = (((((double*)R.data)[0]*(blobs[i].pixels[j].x-p0.x)) + (((double*)R.data)[1]*(blobs[i].pixels[j].y-p0.y))) * S) + ((double*)T.data)[0] + p0.x;
    //        tmp.y = (((((double*)R.data)[2]*(blobs[i].pixels[j].x-p0.x)) + (((double*)R.data)[3]*(blobs[i].pixels[j].y-p0.y))) * S) + ((double*)T.data)[1] + p0.y;
    //
    //        blobs[i].pixels[j].x = std::round(tmp.x);
    //        blobs[i].pixels[j].y = std::round(tmp.y);
    //
    //      }
    //
    //      blobs[i].finalize();
    //
    //    }
    //
    //  }
    
    //*****************************************************************************
    // save
    //*****************************************************************************
    /// Save blob barycenters and optional frame summary to streams.
    void save(FILE * outputPoints, int camera, int frame, FILE * outputFrame = NULL) const {
      
      for(size_t i=0; i<blobs.size(); ++i)
        fprintf(outputPoints, "%d %04d %04d %.4f %.4f %.1f\n", camera, frame, blobs[i].label, blobs[i].baricenter.x, blobs[i].baricenter.y, blobs[i].avgIntensity);
      
      if(outputFrame != NULL) {
        fprintf(outputFrame, "%04d %04lu\n", frame, blobs.size());
        fflush(outputFrame);
      }
      
      fflush(outputPoints);
      
    }
    
    //*****************************************************************************
    // saveContours
    //*****************************************************************************
    /// Save contours for all blobs.
    void saveContours(FILE * output, int camera, int frame) const {
      
      for(size_t i=0; i<blobs.size(); ++i)
        blobs[i].saveContours(output, camera, frame);
      
      fflush(output);
      
    }
    

  }; /* class */

} /* namespace */

#endif /* _H_CVL_BLOBS_H_ */

