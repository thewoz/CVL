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

//*****************************************************************************/
// namespace cvl::clustering
//*****************************************************************************/
namespace cvl::clustering {
   
  constexpr auto distanceOp = [](const blob_t & a, const blob_t & b) { return cv::norm(a.baricenter - b.baricenter); };

  //*****************************************************************************/
  // NN
  //*****************************************************************************/
  void NN(const cvl::blobs_t & blobs, std::vector<cvl::blobs_t> & clusters, double factor = 1) {
    
    std::vector<std::vector<size_t>> clustersIndex = mpl::clustering::byMedianFirstNNDistance(blobs, factor, distanceOp);
    
    clusters.clear();
    
    clusters.resize(clustersIndex.size());
    
    // per ogni cluster trovato
    for(size_t i=0; i<clustersIndex.size(); ++i) {
      
      // aggiungo al cluster il blob
      for(size_t j=0; j<clustersIndex[i].size(); ++j)
        clusters[i].add(blobs[clustersIndex[i][j]]);
      
      // Mi calcolo in baricentro del cluster e rinomino i balob dentro il cluster
      clusters[i].finalize();

    }
    
  }

  //*****************************************************************************/
  // follow
  //*****************************************************************************/
  cv::Point2d follow(cvl::blobs_t & blobs, const cv::Point2d & coords, std::vector<cvl::blobs_t> & clusters, double factor = 1) {
    
    // Clusterizzo
    cvl::clustering::NN(blobs, clusters, factor);
    
    size_t nearIndex = 0;
    double nearDist  = DBL_MAX;
    
    // ciclo su tutti i cluster trovati
    for(size_t i=0; i<clusters.size(); ++i) {
      
      // mi calcolo la distanza tra il cluster vecchio e quelli nuovi
      double dist = cv::norm(coords - clusters[i].baricenter);
      
      // mi prendo il piu vicino a quelo vecchio
      if(dist < nearDist) {
        nearDist  = dist;
        nearIndex = i;
      }
      
    }
    
    blobs = clusters[nearIndex];
    
    blobs.finalize();
        
    return clusters[nearIndex].baricenter;
    
  }
  
} /* namespace cvl::clustering */


#endif /* _H_CVL_CLUSTERING_H_ */
