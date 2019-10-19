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

#ifndef _H_CVL_CLUSTERING_H_
#define _H_CVL_CLUSTERING_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <algorithm>

#include <opencv2/opencv.hpp>

#include <mpl/utils.hpp>
#include <mpl/clustering.hpp>

#include "blob.hpp"

/*****************************************************************************/
// namespace
/*****************************************************************************/
namespace cvl::clustering {
   
  /*****************************************************************************/
  // get
  /*****************************************************************************/
  void get(asBlobs & cluster, double factor, std::vector<asBlobs> & clusters) {
    
    std::vector<std::vector<size_t>> clustersIndex = mpl::clustering::byNNDistance(cluster.blobs, factor);

    clusters.clear();
    
    clusters.resize(clustersIndex.size());
    
    for(size_t i=0; i<clustersIndex.size(); ++i) {
      for(size_t j=0; j<clustersIndex[i].size(); ++j)
        clusters[i].add(cluster.blobs[clustersIndex[i][j]]);
    }
    
    for(size_t i=0; i<clusters.size(); ++i) {
      clusters[i].finalize();

    }
    
  }

  /*****************************************************************************/
   // follow
   /*****************************************************************************/
  cv::Point2d follow(asBlobs & cluster, double factor, const cv::Point2d & coords, std::vector<asBlobs> & clusters) {
    
    // Clusterizzo
    cvl::clustering::get(cluster, factor, clusters);
    
    size_t nearIndex = 0;
    double nearDist  = DBL_MAX;
    
    for(size_t i=0; i<clusters.size(); ++i) {
      
      double dist = cv::norm(coords - clusters[i].baricenter());
      
      if(dist < nearDist) {
        nearDist = dist;
        nearIndex = i;
      }
      
    }
    
    cluster = clusters[nearIndex];
    
    return clusters[nearIndex].baricenter();
    
  }
  
  
} /* namespace */


#endif /* _H_CVL_CLUSTERING_H_ */


 
//  /*****************************************************************************/
//  // namespace
//  /*****************************************************************************/
//  namespace utils {
//
//    /*****************************************************************************/
//    //  asNN
//    /*****************************************************************************/
//    class asNN {
//
//    private:
//
//      std::vector<double> data;
//
//      double max;
//
//      bool squaredValue;
//      bool skipZero;
//
//      asNN(){ };
//
//    public:
//
//      enum { SQUARED_VALUES, NO_SQUARED_VALUES };
//      enum { SKIP_ZERO, NO_SKIP_ZERO };
//
//      /*****************************************************************************/
//      //  asNN
//      /*****************************************************************************/
//      asNN(uint32_t size, bool _squaredValue = NO_SQUARED_VALUES, bool _skipZero = NO_SKIP_ZERO) : squaredValue(_squaredValue), skipZero(_skipZero) { max = std::numeric_limits<double>::max(); data.resize(size, max); }
//
//      /*****************************************************************************/
//      //  add
//      /*****************************************************************************/
//      inline void add(uint32_t index, double value){
//
//        if(((skipZero && value !=0) || !skipZero) && value < data[index]) data[index] = value;
//
//      }
//
//      /*****************************************************************************/
//      //  getMedian
//      /*****************************************************************************/
//      double getMedian() {
//
//        uint32_t haveLink = 0;
//
//        for(uint32_t i=0; i<data.size(); ++i) if(data[i]!=max) ++haveLink;
//
//        if(haveLink == 0) return -1;
//
//        uint32_t halfSize = haveLink * 0.5;
//
//        //std::nth_element(data.begin(), data.begin()+halfSize+2, data.end());
//
//        std::sort(data.begin(), data.end());
//
//        //for(uint32_t i=0; i<data.size(); ++i) printf("AAAA %e\n", data[i]);
//
//        double median = 0;
//
//        if((haveLink % 2) != 0) median = data[halfSize];
//        else if(squaredValue == NO_SQUARED_VALUES) median = (data[halfSize-1] + data[halfSize]) * 0.5;
//        else median = pow((sqrt(data[halfSize-1]) + sqrt(data[halfSize])) * 0.5, 2);
//
//        //if(haveLink != data.size()){ for(uint32_t i=0; i<data.size(); ++i) printf("%e\n", data[i]); }
//        //for(uint32_t i=0; i<data.size(); ++i) printf("%e\n", data[i]);
//
//        return median;
//
//      }
//
//    };
//
//  }
