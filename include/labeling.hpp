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

#ifndef _H_CVL_LABELING_H_
#define _H_CVL_LABELING_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <map>
#include <string>

#include <opencv2/opencv.hpp>

#include "blob.hpp"

#define MERGE_MASS_LABEL(a,b) { labelMass[b] += labelMass[a]; labelMass[a] = -b; }

//*****************************************************************************/
// namespace cvl::labeling
//*****************************************************************************/
namespace cvl::labeling {

  //*****************************************************************************/
  // namespace utils
  //*****************************************************************************/
  namespace utils {

    //*****************************************************************************/
    // freeMatrix
    //*****************************************************************************/
    void freeMatrix(int ** matrixLabel, const cv::Size & size){
      
      if(matrixLabel != NULL) {
        
        for(size_t i=0; i<size.height; ++i)
          free(matrixLabel[i]);
        
        free(matrixLabel);
        
      }
      
    }
    
    //*****************************************************************************/
    // allocMatrix
    //*****************************************************************************/
    void allocMatrix(int ** & matrixLabel, const cv::Size & size) {
      
      if(matrixLabel != NULL) freeMatrix(matrixLabel, size);
      
      matrixLabel = (int **) malloc(size.height * sizeof(int *));
      
      if(matrixLabel == NULL){
        fprintf(stderr, "error in matrix malloc\n");
        exit(EXIT_FAILURE);
      }
      
      for(size_t i=0; i<size.height; ++i){
        
        matrixLabel[i] = (int *) malloc(size.width * sizeof(int));
        
        if(matrixLabel[i] == NULL){
          fprintf(stderr, "error in matrix malloc\n");
          exit(EXIT_FAILURE);
        }
        
        memset(matrixLabel[i], 0, size.width * sizeof(int));
        
      }
      
    }

    //*****************************************************************************/
    // init
    //*****************************************************************************/
    void init(const cv::Size & imgSize, cv::Size & workImageSize, int **  & matrixLabel) {

      if(matrixLabel == NULL){
        
        workImageSize.width  = imgSize.width  + 2;
        workImageSize.height = imgSize.height + 2;
        
        allocMatrix(matrixLabel, workImageSize);
        
      } else {
        
        if(workImageSize.height != (imgSize.height + 2) || workImageSize.width != (imgSize.width + 2)){
          
          freeMatrix(matrixLabel, workImageSize);
          
          workImageSize.height = imgSize.height + 2;
          workImageSize.width  = imgSize.width  + 2;
          
          allocMatrix(matrixLabel, workImageSize);
          
        } else {
          
          for(int i=0; i<workImageSize.height; i++)
            memset(matrixLabel[i], 0, workImageSize.width * sizeof(int));
          
        }
        
      }
      
    }
    
    //*****************************************************************************/
    // findRoot
    //*****************************************************************************/
    inline int findRoot(int startLabel, const std::vector<int> & labelMass) {
      
      while(labelMass[startLabel] < 0)
        startLabel = -labelMass[startLabel];
      
      return startLabel;
      
    }
    
    
  } /* namespace utils */
  
  enum { NEAREST_NEIGHBOURS, NEXT_NEAREST_NEIGHBOURS };
  
  //*****************************************************************************/
  // get
  //*****************************************************************************/
  void get(cv::Mat & src, int neighboursType, size_t minClusterMass, cvl::blobs_t & blobs, const cv::Mat & original, const cv::Point & offset = cv::Point(0,0)) {
    
    bool eight_connectivity = true;
    
    if(neighboursType == NEAREST_NEIGHBOURS) {
      eight_connectivity = false;
    }
    
    if(neighboursType != NEAREST_NEIGHBOURS && neighboursType != NEXT_NEAREST_NEIGHBOURS) {
    
      fprintf(stderr, "error unknown type of cluster connectivity criteria.\n");
      fprintf(stderr, "You can choose between:\n");
      fprintf(stderr, "      NEAREST_NEIGHBOURS, NEXT_NEAREST_NEIGHBOURS\n");
      
      exit(EXIT_FAILURE);
    
    }
    
    std::vector<int> labelMass(1,0);

    int label = 0;
    
    cv::Size workImageSize;
    
    int ** matrixLabel = NULL;
    
    utils::init(src.size(), workImageSize, matrixLabel);
    
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
            for(size_t i=0; i<neighbors_found; ++i)
              if(min_label > neighbors[i]) min_label = neighbors[i];
            
            matrixLabel[y][x] = min_label;
            
            labelMass[min_label] += 1;
            
            for(int i=0; i<neighbors_found; ++i)
              for(int j=i-1; j>=0; --j)
                if(neighbors[i] == neighbors[j])
                  neighbors[i] = -1;
            
            for(size_t i=0; i<neighbors_found; ++i)
              if(neighbors[i] != min_label && neighbors[i] != -1)
                MERGE_MASS_LABEL(neighbors[i], min_label);
            
          }
          
        }
        
      }
      
    }
    
    std::map<size_t,std::vector<cv::Point>> foundBlobs;
    std::map<size_t,std::vector<cv::Point>>::iterator blobsIter;
    
    for(int y=1; y<workImageSize.height-1; ++y) {
      
      for(int x=1; x<workImageSize.width-1; ++x) {
        
        if(matrixLabel[y][x] != 0) {
          
          int found_label = utils::findRoot(matrixLabel[y][x], labelMass);
          
          if((blobsIter = foundBlobs.find(found_label)) == foundBlobs.end()) {
            
            foundBlobs[found_label].clear();
            
            foundBlobs[found_label].push_back(cv::Point(x-1,y-1));
            
          } else {
            
            blobsIter->second.push_back(cv::Point(x-1,y-1));
            
          }
          
        }
        
      }
      
    }
    
    blobs.clear();
    
    for(blobsIter=foundBlobs.begin(); blobsIter!=foundBlobs.end(); blobsIter++){
      
      if(blobsIter->second.size() < minClusterMass) continue;
      
      cvl::blob_t blob;
      
      for(size_t i=0; i<blobsIter->second.size(); ++i)
        blob.add(blobsIter->second[i].x+offset.x, blobsIter->second[i].y+offset.y, original.at<uchar>(blobsIter->second[i].y+offset.y, blobsIter->second[i].x+offset.x));
      
      blobs.add(blob);
      
    }

    blobs.finalize();
    
    utils::freeMatrix(matrixLabel, workImageSize);
    
  }
  
#if(0)

  //*****************************************************************************/
  // get
  //*****************************************************************************/
  void get(cv::Mat & src, int neighboursType, size_t minClusterMass, std::vector<std::vector<cv::Point>> & blobs) {
    
    bool eight_connectivity = true;
    
    if(neighboursType == NEAREST_NEIGHBOURS) {
      eight_connectivity = false;
    }
    
    if(neighboursType != NEAREST_NEIGHBOURS && neighboursType != NEXT_NEAREST_NEIGHBOURS) {
    
      fprintf(stderr, "error unknown type of cluster connectivity criteria.\n");
      fprintf(stderr, "You can choose between:\n");
      fprintf(stderr, "      NEAREST_NEIGHBOURS, NEXT_NEAREST_NEIGHBOURS\n");
      
      exit(EXIT_FAILURE);
    
    }
    
    std::vector<int> labelMass(1,0);

    int label = 0;

    blobs.clear();
    
    cv::Size workImageSize;
    
    int ** matrixLabel = NULL;
    
    utils::init(src.size(), workImageSize, matrixLabel);
    
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
            for(size_t i=0; i<neighbors_found; ++i)
              if(min_label > neighbors[i]) min_label = neighbors[i];
            
            matrixLabel[y][x] = min_label;
            
            labelMass[min_label] += 1;
            
            for(int i=0; i<neighbors_found; ++i)
              for(int j=i-1; j>=0; --j)
                if(neighbors[i] == neighbors[j])
                  neighbors[i] = -1;
            
            for(size_t i=0; i<neighbors_found; ++i)
              if(neighbors[i] != min_label && neighbors[i] != -1)
                MERGE_MASS_LABEL(neighbors[i], min_label);
            
          }
          
        }
        
      }
      
    }
    
    std::map<size_t,std::vector<cv::Point>> foundBlobs;
    std::map<size_t,std::vector<cv::Point>>::iterator blobsIter;
    
    for(int y=1; y<workImageSize.height-1; ++y) {
      
      for(int x=1; x<workImageSize.width-1; ++x) {
        
        if(matrixLabel[y][x] != 0) {
          
          int found_label = utils::findRoot(matrixLabel[y][x], labelMass);
          
          if((blobsIter = foundBlobs.find(found_label)) == foundBlobs.end()) {
            
            foundBlobs[found_label].clear();
            
            foundBlobs[found_label].push_back(cv::Point(x-1,y-1));
            
          } else {
            
            blobsIter->second.push_back(cv::Point(x-1,y-1));
            
          }
          
        }
        
      }
      
    }
    
    for(blobsIter=foundBlobs.begin(); blobsIter!=foundBlobs.end(); blobsIter++){
      
      if(blobsIter->second.size() < minClusterMass) continue;
      
      std::vector<cv::Point> blob;
      
      for(size_t i=0; i<blobsIter->second.size(); ++i)
        blob.push_back(cv::Point(blobsIter->second[i].x, blobsIter->second[i].y));
      
      blobs.push_back(blob);
      
    }
        
    utils::freeMatrix(matrixLabel, workImageSize);
    
  }

#endif

} /* namespace cvl::labeling */


#endif /* _H_CVL_LABELING_H_ */


