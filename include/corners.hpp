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

#ifndef _H_CVL_CORNERS_H_
#define _H_CVL_CORNERS_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdio>
#include <cstdlib>

#include <vector>
#include <string>

//*****************************************************************************
// namespace cvl
//*****************************************************************************
namespace cvl {

  //*****************************************************************************
  // class corners_t
  //*****************************************************************************
  // Container for loading and accessing 2D corner tracks from files.
  class corners_t {

  public:

    // un indice di frame oltre questa soglia indica un file corrotto: meglio
    // fermarsi subito che allocare coords[frame] per milioni di frame vuoti
    static constexpr int MAX_FRAME_INDEX = 10000;

    std::vector<std::vector<cv::Point2d>> coords;

    //*****************************************************************************
    // load
    //*****************************************************************************
    // Load corner tracks from a text file.
    void load(const std::string & filepath) {

      cvl::loader_t loader;
      loader(filepath, "2 4 5", this, &corners_t::filler);

    }

    //*****************************************************************************
    // size
    //*****************************************************************************
    // Return number of frames loaded.
    size_t size() const { return coords.size(); }

    //*****************************************************************************
    // operator []
    //*****************************************************************************
    // Access per-frame corner coordinates.
    const std::vector<cv::Point2d> & operator [] (size_t index) const { return coords[index]; }
          std::vector<cv::Point2d> & operator [] (size_t index)       { return coords[index]; }

  private:

    //*****************************************************************************
    // filler
    //*****************************************************************************
    // Loader callback for pushing coordinates into frames.
    void filler(const cvl::loader_t::data_t & arguments) {

      int frame = arguments.get<int>(0);

      // un frame negativo manderebbe coords[frame] out-of-bounds, uno abnorme
      // farebbe esplodere la resize: entrambi indicano un file corrotto
      if(frame < 0 || frame > MAX_FRAME_INDEX)
        cvl::log::error("cvl::corners_t::load() error: bad frame index %d in the input file (valid range [0, %d])\n", frame, MAX_FRAME_INDEX);

      cv::Point2d coord;

      coord.x = arguments.get<double>(1);
      coord.y = arguments.get<double>(2);

      if((int)coords.size()<=frame) coords.resize(frame+1);

      coords[frame].push_back(coord);

    }

  }; /* class corners_t */

} /* namespace cvl */

#endif /* _H_CVL_CORNERS_H_ */
