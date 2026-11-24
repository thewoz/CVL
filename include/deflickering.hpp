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

#ifndef _H_CVL_DEFLICKERING_H_
#define _H_CVL_DEFLICKERING_H_

#ifndef _H_CVL_H_
  #error "Do not include this header directly; include <cvl/cvl.hpp> instead."
#endif

#include <cstdlib>
#include <cstdio>

#include <vector>
#include <string>

//*****************************************************************************
// namespace cvl
//*****************************************************************************
namespace cvl {

  //*****************************************************************************
  // class deflickering_t
  //*****************************************************************************
  // Temporal deflickering on a window of frames.
  //
  // Each frame j is corrected pixel by pixel as
  //
  //   ADDITIVE:       out_j = in_j + (ref_j - L_j)   (flicker as an intensity offset)
  //   MULTIPLICATIVE: out_j = in_j * ref_j / L_j     (flicker as a gain, e.g. lights or exposure)
  //
  // where L_j is the local mean of frame j over a deflickerMatrixSize x deflickerMatrixSize
  // box and ref_j is the average of L over deflickerTime frames centered on j (the
  // temporal window is shifted to stay inside the provided frames). Slow illumination
  // changes are kept, frame-to-frame brightness oscillations are removed.
  //
  // The local means are computed once per frame with getLocalMean() and passed back to
  // deflicker(), so a sliding window only pays for the frame that enters it.
  class deflickering_t {

  public:

    //*****************************************************************************
    // init
    //*****************************************************************************
    // Set the temporal window (frames) and the spatial box (pixels), both odd,
    // and the correction mode: "ADDITIVE" or "MULTIPLICATIVE".
    void init(int _deflickerTime, int _deflickerMatrixSize, const std::string & mode = "ADDITIVE") {

      deflickerTime = _deflickerTime;

      if(deflickerTime < 1 || deflickerTime % 2 == 0){
        cvl::log::error("cvl::deflickering_t::init() error: the deflicker time must be odd and positive\n");
      }

      halfDeflickerTime = deflickerTime / 2;

      deflickerMatrixSize = _deflickerMatrixSize;

      if(deflickerMatrixSize < 1 || deflickerMatrixSize % 2 == 0){
        cvl::log::error("cvl::deflickering_t::init() error: the deflicker matrix size must be odd and positive\n");
      }

      if(mode != "ADDITIVE" && mode != "MULTIPLICATIVE"){
        cvl::log::error("cvl::deflickering_t::init() error: the mode must be ADDITIVE or MULTIPLICATIVE\n");
      }

      multiplicative = (mode == "MULTIPLICATIVE");

      inited = true;

    }

    //*****************************************************************************
    // getTime
    //*****************************************************************************
    // Return the temporal window size (frames).
    int getTime() const { return deflickerTime; }

    //*****************************************************************************
    // getLocalMean
    //*****************************************************************************
    // Compute the local mean (CV_32F) of a CV_8UC1 frame over the spatial box.
    cv::Mat getLocalMean(const cv::Mat & image) const {

      if(!inited){
        cvl::log::error("cvl::deflickering_t::getLocalMean() error: the class must be initialized first\n");
      }

      if(image.empty() || image.type() != CV_8UC1){
        cvl::log::error("cvl::deflickering_t::getLocalMean() error: the image must be a non-empty CV_8UC1 image\n");
      }

      // in float: blur su CV_8U arrotonderebbe la media all'intero
      cv::Mat imageF; image.convertTo(imageF, CV_32F);

      cv::Mat localMean;

      cv::blur(imageF, localMean, cv::Size(deflickerMatrixSize, deflickerMatrixSize), cv::Point(-1,-1), cv::BORDER_REFLECT);

      return localMean;

    }

    //*****************************************************************************
    // deflicker
    //*****************************************************************************
    // Deflicker every frame of the window. localMeans[j] must be getLocalMean(images[j]).
    void deflicker(const std::vector<cv::Mat> & images, const std::vector<cv::Mat> & localMeans, std::vector<cv::Mat> & output) const {

      if(!inited){
        cvl::log::error("cvl::deflickering_t::deflicker() error: the class must be initialized first\n");
      }

      int n = (int)images.size();

      if(n < deflickerTime){
        cvl::log::error("cvl::deflickering_t::deflicker() error: %d images provided but the deflicker time is %d\n", n, deflickerTime);
      }

      if((int)localMeans.size() != n){
        cvl::log::error("cvl::deflickering_t::deflicker() error: %zu local means provided for %d images\n", localMeans.size(), n);
      }

      for(int i=0; i<n; ++i){
        if(images[i].empty() || images[i].type() != CV_8UC1 || images[i].size() != images[0].size() ||
           localMeans[i].type() != CV_32FC1 || localMeans[i].size() != images[0].size()){
          cvl::log::error("cvl::deflickering_t::deflicker() error: the images must be non-empty CV_8UC1 images of the same size with matching CV_32F local means\n");
        }
      }

      // somme cumulate delle medie locali: la media su [from, to] costa due sole operazioni
      std::vector<cv::Mat> prefix(n + 1);

      prefix[0] = cv::Mat::zeros(images[0].size(), CV_32F);

      for(int i=0; i<n; ++i)
        prefix[i+1] = prefix[i] + localMeans[i];

      output.resize(n);

      cv::Mat reference, corrected;

      for(int j=0; j<n; ++j){

        int from, to;

        getFromTo(j, n, from, to);

        // riferimento = media temporale delle medie locali
        reference = (prefix[to+1] - prefix[from]) * (1.0 / (double)deflickerTime);

        cv::Mat imageF; images[j].convertTo(imageF, CV_32F);

        if(multiplicative) {

          // il max evita la divisione per zero nelle zone completamente nere
          cv::divide(reference, cv::max(localMeans[j], 1.0), corrected);

          corrected = imageF.mul(corrected);

        } else { corrected = imageF + (reference - localMeans[j]); }

        // convertTo arrotonda e satura in [0, 255]
        corrected.convertTo(output[j], CV_8U);

      }

    }

  private:

    bool inited = false;

    int deflickerTime = 0;

    int halfDeflickerTime = 0;

    int deflickerMatrixSize = 0;

    bool multiplicative = false;

    //*****************************************************************************
    // getFromTo
    //*****************************************************************************
    // Compute the temporal window of deflickerTime frames centered on i, shifted inside [0, n-1].
    void getFromTo(int i, int n, int & from, int & to) const {

      int last = n - 1;

      int f = i - halfDeflickerTime;
      int t = i + halfDeflickerTime;

      if(f < 0)    { t += -f;       f = 0; }
      if(t > last) { f -= t - last; t = last; }

      from = f;
      to   = t;

    }

  }; /* class deflickering_t */

} /* namespace cvl */

#endif /* _H_CVL_DEFLICKERING_H_ */
