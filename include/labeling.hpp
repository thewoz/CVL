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

#ifndef _H_CVL_LABELING_H_
#define _H_CVL_LABELING_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>
#include <utility>
#include <climits>

#define MERGE_MASS_LABEL(a,b) do { labelMass[(b)] += labelMass[(a)]; labelMass[(a)] = -(b); } while(0)

//*****************************************************************************
// namespace cvl::labeling
//*****************************************************************************
namespace cvl::labeling {

  //*****************************************************************************
  // namespace utils
  //*****************************************************************************
  namespace utils {

    //*****************************************************************************
    // findRoot
    //*****************************************************************************
    // Find the root label in the union-find structure.
    inline int findRoot(int startLabel, const std::vector<int> & labelMass) {

      while(labelMass[startLabel] < 0)
        startLabel = -labelMass[startLabel];

      return startLabel;

    }


  } /* namespace utils */

  enum { NEAREST_NEIGHBOURS, NEXT_NEAREST_NEIGHBOURS };

  //*****************************************************************************
  // get
  //*****************************************************************************
  // Label connected components and return them as blobs.
  inline void get(const cv::Mat & src, int neighboursType, size_t minClusterMass, cvl::blobs_t & blobs, const cv::Mat & original, const cv::Point & offset = cv::Point(0,0)) {

    if(src.empty() || src.type() != CV_8UC1) {
      cvl::log::error("cvl::labeling::get() error: the input image must be a non-empty CV_8UC1 image\n");
    }

    // le intensità dei blob vengono lette da original con at<uchar>
    if(original.empty() || original.type() != CV_8UC1) {
      cvl::log::error("cvl::labeling::get() error: the original image must be a non-empty CV_8UC1 image\n");
    }

    bool eight_connectivity = true;

    if(neighboursType == NEAREST_NEIGHBOURS) {
      eight_connectivity = false;
    }

    if(neighboursType != NEAREST_NEIGHBOURS && neighboursType != NEXT_NEAREST_NEIGHBOURS) {

      cvl::log::error("cvl::labeling::get() error: unknown connectivity type\n"
                      "You can choose between:\n"
                      "      NEAREST_NEIGHBOURS, NEXT_NEAREST_NEIGHBOURS\n");

    }

    std::vector<int> labelMass(1,0);

    int label = 1;

    // matrice dei label con bordo di 1 pixel, un solo buffer contiguo
    // (matrixLabel[y] punta alla riga y, così la sintassi [y][x] resta invariata)
    cv::Size workImageSize(src.cols + 2, src.rows + 2);

    std::vector<int> matrixData((size_t)workImageSize.height * workImageSize.width, 0);

    std::vector<int *> matrixLabel(workImageSize.height);

    for(int i=0; i<workImageSize.height; ++i)
      matrixLabel[i] = matrixData.data() + (size_t)i * workImageSize.width;

    int neighbors_found;

    int neighbors[4];

    for(int y=1; y<workImageSize.height-1; ++y) {

      for(int x=1; x<workImageSize.width-1; ++x) {

        /* if the pixel is not a background pixel */
        if(src.at<uchar>(y-1,x-1) != 0){

          neighbors_found = 0;

          if(matrixLabel[y][x-1] != 0){
            neighbors[neighbors_found] = utils::findRoot(matrixLabel[y][x-1], labelMass);
            neighbors_found++;
          }

          if(matrixLabel[y-1][x] != 0){
            neighbors[neighbors_found] = utils::findRoot(matrixLabel[y-1][x], labelMass);
            neighbors_found++;
          }

          if(eight_connectivity){

            if(matrixLabel[y-1][x-1] != 0){
              neighbors[neighbors_found] = utils::findRoot(matrixLabel[y-1][x-1], labelMass);
              neighbors_found++;
            }

            if(matrixLabel[y-1][x+1] != 0){
              neighbors[neighbors_found] = utils::findRoot(matrixLabel[y-1][x+1], labelMass);
              neighbors_found++;
            }

          }

          if(neighbors_found == 0){

            matrixLabel[y][x] = label;

            labelMass.push_back(1);

            label++;

          } else {

            int min_label = INT_MAX;

            /* find the neighbours with the minimum label */
            for(int i=0; i<neighbors_found; ++i)
              if(min_label > neighbors[i]) min_label = neighbors[i];

            matrixLabel[y][x] = min_label;

            labelMass[min_label] += 1;

            for(int i=0; i<neighbors_found; ++i)
              for(int j=i-1; j>=0; --j)
                if(neighbors[i] == neighbors[j])
                  neighbors[i] = -1;

            for(int i=0; i<neighbors_found; ++i)
              if(neighbors[i] != min_label && neighbors[i] != -1)
                MERGE_MASS_LABEL(neighbors[i], min_label);

          }

        }

      }

    }

    // the labels are dense integers in [1, labelMass.size()-1], so a vector
    // indexed by root label is enough to collect the pixels (O(1) per pixel)
    std::vector<std::vector<cv::Point>> foundBlobs(labelMass.size());

    for(int y=1; y<workImageSize.height-1; ++y) {

      for(int x=1; x<workImageSize.width-1; ++x) {

        if(matrixLabel[y][x] != 0) {

          int found_label = utils::findRoot(matrixLabel[y][x], labelMass);

          foundBlobs[found_label].push_back(cv::Point(x-1,y-1));

        }

      }

    }

    blobs.clear();

    for(size_t root=0; root<foundBlobs.size(); ++root){

      std::vector<cv::Point> & points = foundBlobs[root];

      if(points.size() < minClusterMass) continue;

      cvl::blob_t blob;

      blob.pixels.reserve(points.size());

      for(size_t i=0; i<points.size(); ++i)
        blob.add(points[i].x+offset.x, points[i].y+offset.y, original.at<uchar>(points[i].y+offset.y, points[i].x+offset.x));

      blobs.add(std::move(blob));

    }

    blobs.finalize();

  }

} /* namespace cvl::labeling */


#endif /* _H_CVL_LABELING_H_ */
