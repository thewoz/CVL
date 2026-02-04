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

#ifndef _H_CVL_BARICENTER_H_
#define _H_CVL_BARICENTER_H_

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

//****************************************************************************
// namespace
//****************************************************************************
namespace cvl::baricenter {

  //****************************************************************************
	// geometric
  //****************************************************************************
  /// Compute the geometric barycenter of a set of pixel coordinates.
  template <class T>
	cv::Point2d geometric(const std::vector<T> & pixels) {

		cv::Point2d baricenter(0.0, 0.0);

		for(size_t i=0; i<pixels.size(); ++i) {
		      
		  baricenter.x += pixels[i].x;
		  baricenter.y += pixels[i].y;
		  
		}
		
		baricenter.x /= (double) pixels.size();
		baricenter.y /= (double) pixels.size();
		
		baricenter.x += 0.5;
		baricenter.y += 0.5;

		return baricenter;

	}

	//****************************************************************************
	// maxIntensity
	//****************************************************************************
  /// Return the coordinates of the pixel with the maximum intensity.
	cv::Point2d maxIntensity(const std::vector<cv::Point3i> & pixels) {
		    
    size_t maxPixelIntensity = 0;
		
		for(size_t i=0; i<pixels.size(); ++i) {

		  if(pixels[i].z > pixels[maxPixelIntensity].z )
				maxPixelIntensity = i;
			
		}
		
		cv::Point2d baricenter(0.0, 0.0);
		  
		baricenter.x = pixels[maxPixelIntensity].x + 0.5;
		baricenter.y = pixels[maxPixelIntensity].y + 0.5;
	 
		return baricenter;
			
	}

	//****************************************************************************
	// lowIntensity
	//****************************************************************************
  /// Return the coordinates of the pixel with the minimum intensity.
	cv::Point2d lowIntensity(const std::vector<cv::Point3i> & pixels) {
		
		size_t lowPixelIntensity = pixels.size() - 1;
		
		for(size_t i=0; i<pixels.size(); ++i) {

		  if(pixels[i].z < pixels[lowPixelIntensity].z )
				lowPixelIntensity = i;
			
		}
		
		cv::Point2d baricenter(0.0, 0.0);
		  
		baricenter.x = pixels[lowPixelIntensity].x + 0.5;
		baricenter.y = pixels[lowPixelIntensity].y + 0.5;
	 
		return baricenter;
			
	}

	//****************************************************************************
	// maxIntensityWeighted
	//****************************************************************************
  /// Compute a weighted barycenter using intensity as weight.
	cv::Point2d maxIntensityWeighted(const std::vector<cv::Point3i> & pixels) {
		
    size_t totalPixelsIntensity = 0;
		
		cv::Point2d baricenter(0.0, 0.0);
		
		for(size_t i=0; i<pixels.size(); ++i) {
		  
		  totalPixelsIntensity += pixels[i].z;
		  
		  baricenter.x += (pixels[i].z * pixels[i].x);
		  baricenter.y += (pixels[i].z * pixels[i].y);
		  
		}
		
		/* if is a complet back blob in order to avoid the division */
		/* by zero we compute the no-weighted baricentery  */
		if(totalPixelsIntensity == 0)
		  return geometric(pixels);

		baricenter.x /= (double)totalPixelsIntensity;
		baricenter.y /= (double)totalPixelsIntensity;
		  
		baricenter.x += 0.5;
		baricenter.y += 0.5;

		return baricenter;

	}

	//*****************************************************************************
	// lowIntensityWeighted
	//*****************************************************************************
  /// Compute a weighted barycenter using inverted intensity (255 - value) as weight.
	cv::Point2d lowIntensityWeighted(const std::vector<cv::Point3i> & pixels) {

    size_t totalPixelsIntensity = 0;
		
		cv::Point2d baricenter(0.0, 0.0);
		
		for(size_t i=0; i<pixels.size(); ++i) {
		  
		  totalPixelsIntensity += 255 - pixels[i].z;
		  
		  baricenter.x += ((255 - pixels[i].z) * pixels[i].x);
		  baricenter.y += ((255 - pixels[i].z) * pixels[i].y);
		  
		}
		
		/* if is a complet back blob in order to avoid the division */
		/* by zero we compute the no-weighted baricentery  */
		if(totalPixelsIntensity == 0)
		  return geometric(pixels);

		baricenter.x /= (double)totalPixelsIntensity;
		baricenter.y /= (double)totalPixelsIntensity;
		  
		baricenter.x += 0.5;
		baricenter.y += 0.5;

		return baricenter;
	
	}

	//*****************************************************************************
	// get
	//*****************************************************************************
  /// Dispatch barycenter evaluation by name.
  cv::Point2d get(const std::vector<cv::Point3i> & pixels, const std::string & algo) {

		if(algo.compare("MAX_INTENSITY") == 0)
		  return maxIntensity(pixels);
		  
		if(algo.compare("MAX_INTENSITY_WEIGHTED") == 0)
		  return maxIntensityWeighted(pixels);
		  
		if(algo.compare("LOW_INTENSITY") == 0)
		 return lowIntensity(pixels);
		  
		if(algo.compare("LOW_INTENSITY_WEIGHTED") == 0)
		  return lowIntensityWeighted(pixels);
		  
	  if(algo.compare("GEOMETRIC") == 0)
		  return geometric(pixels);	  
		
    fprintf(stderr, "error unknown type of baricenter evaluation criteria.\n");
    fprintf(stderr, "You can choose between:\n");
    fprintf(stderr, "      MAX_INTENSITY, MAX_INTENSITY_WEIGHTED, LOW_INTENSITY, LOW_INTENSITY_WEIGHTED or GEOMETRIC\n");
		 
		exit(EXIT_FAILURE);

		return cv::Point2d(-1,-1);
		
	}

} /* namespace */

#endif /* _H_CVL_BARICENTER_H_ */

