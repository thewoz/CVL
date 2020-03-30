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
// namespace cvl::clustering
/*****************************************************************************/
namespace cvl::clustering {
   
  /*****************************************************************************/
  // NN
  /*****************************************************************************/
  void NN(const asBlobs & cluster, std::vector<asBlobs> & clusters, double factor = 1) {
    
    std::vector<std::vector<size_t>> clustersIndex = mpl::clustering::byNNDistance(cluster.blobs, factor);
    
    clusters.clear();
    
    clusters.resize(clustersIndex.size());
    
    for(size_t i=0; i<clustersIndex.size(); ++i) {
      
      for(size_t j=0; j<clustersIndex[i].size(); ++j) {
        clusters[i].add(cluster.blobs[clustersIndex[i][j]]);
        clusters[i][j].label = (int)j;
        clusters[i][j].finalize(); //NOTE: non sono sicuro che serva
      }
      
      // Mi calcolo in baricentro del cluster
      clusters[i].finalize();

    }
    
  }

  /*****************************************************************************/
   // follow
   /*****************************************************************************/
  cv::Point2d follow(asBlobs & cluster, const cv::Point2d & coords, std::vector<asBlobs> & clusters, double factor = 1) {
    
    // Clusterizzo
    cvl::clustering::NN(cluster, clusters, factor);
    
    size_t nearIndex = 0;
    double nearDist  = DBL_MAX;
    
    for(size_t i=0; i<clusters.size(); ++i) {
      
      double dist = cv::norm(coords - clusters[i].baricenter);
      
      if(dist < nearDist) {
        nearDist = dist;
        nearIndex = i;
      }
      
    }
    
    cluster = clusters[nearIndex];
    
    return clusters[nearIndex].baricenter;
    
  }
  
  
} /* namespace cvl::clustering */


#endif /* _H_CVL_CLUSTERING_H_ */
