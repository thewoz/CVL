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

/**
 * CVL example.
 *
 * Build:  make example            (needs the headers installed: make install)
 * Run:    ~/bin/cvl [output.png]  (default: cvl_example.png)
 *
 * The program needs no input data: it draws a synthetic frame of bright spots
 * on a noisy, unevenly lit background, then runs a typical CVL pipeline on it
 * (denoise, threshold, label, split the touching spots, measure) and saves the
 * frame next to the annotated result.
 */

#include <cstdio>
#include <cstdlib>

#include <string>
#include <vector>

#include <cvl/cvl.hpp>

//*****************************************************************************
// makeFrame
//*****************************************************************************
// Draw the synthetic frame: Gaussian spots over a horizontal light gradient,
// plus sensor noise. Two pairs of spots overlap on purpose.
static cv::Mat makeFrame() {

  const cv::Size size(640, 400);

  // centro (x, y), sigma e intensità di picco di ogni spot
  const std::vector<cv::Vec4f> spots = {
    { 90,  80,  9, 170}, {230,  60,  7, 150}, {400,  90, 10, 180}, {560,  70,  8, 160},
    {150, 210,  7, 160}, {173, 218,  7, 165},                       // coppia sovrapposta
    {330, 200, 11, 185}, {500, 220,  7, 150},
    { 80, 330,  7, 155}, {250, 320,  9, 170},
    {418, 332,  7, 160}, {441, 320,  7, 170},                       // coppia sovrapposta
    {570, 340, 10, 175}
  };

  cv::Mat frame(size, CV_32FC1);

  for(int y=0; y<size.height; ++y) {

    float * row = frame.ptr<float>(y);

    for(int x=0; x<size.width; ++x) {

      // illuminazione non uniforme: da 20 a sinistra a 45 a destra
      float value = 20.0f + 25.0f * (float)x / (float)size.width;

      for(const cv::Vec4f & spot : spots) {
        float dx = (float)x - spot[0];
        float dy = (float)y - spot[1];
        value += spot[3] * std::exp(-(dx*dx + dy*dy) / (2.0f * spot[2] * spot[2]));
      }

      row[x] = value;

    }

  }

  // rumore del sensore, con seme fisso per avere sempre la stessa immagine
  cv::RNG rng(12345);
  cv::Mat noise(size, CV_32FC1);
  rng.fill(noise, cv::RNG::NORMAL, 0, 6);

  frame += noise;

  cv::Mat frame8;
  frame.convertTo(frame8, CV_8UC1);

  return frame8;

}

//*****************************************************************************
// main
//*****************************************************************************
int main(int argc, char * const argv[]) {

  std::string outputPath = (argc > 1) ? argv[1] : "cvl_example.png";

  printf("%s\n", CVL_LIB_VERSION);

  cv::Mat frame = makeFrame();

  // --- Denoise ---

  // The filters work in place: keep the original frame for the measurements.
  cv::Mat work = frame.clone();
  cvl::denoising::get(work, "GAUSSIAN", 5);

  // --- Threshold ---

  // Otsu picks the threshold by itself; the return value is the one it used.
  cv::Mat binary;
  double threshold = cvl::threshold::get(work, binary, "OTSU", 0, 0, 0);

  // --- Label ---

  // Connected components (8-connectivity) of at least 20 pixels become blobs.
  // The intensities stored in each blob are read from the original frame.
  cvl::blobs_t blobs;
  cvl::labeling::get(binary, cvl::labeling::NEXT_NEAREST_NEIGHBOURS, 20, blobs, frame);

  size_t found = blobs.size();

  // --- Split ---

  // A blob larger than enterMass is thresholded higher and higher ("TO_WHITE")
  // until it falls apart; the watershed then gives the pieces their full area back.
  int enterMass = 500, exitMass = 40, minMass = 10;
  cvl::blobsplitting::get(blobs, work, frame, "TO_WHITE", enterMass, exitMass, minMass, /*recursiveSplitting=*/false, /*useWatershed=*/true, /*radius=*/0, cvl::labeling::NEXT_NEAREST_NEIGHBOURS);

  printf("Otsu threshold %.0f: %zu blobs found, %zu after splitting\n\n", threshold, found, blobs.size());

  // --- Measure ---

  printf("label       x       y   mass   max\n");

  for(size_t i=0; i<blobs.size(); ++i)
    printf("%5d %7.2f %7.2f %6zu %5d\n", blobs[i].label, blobs[i].baricenter.x, blobs[i].baricenter.y, blobs[i].size(), blobs[i].iMax);

  // --- Draw ---

  cv::Mat result;
  cv::cvtColor(frame, result, cv::COLOR_GRAY2BGR);

  // The palette is BGR; colors can also be looked up by name.
  blobs.drawContours(result, cvl::colors::get("lime"));

  for(size_t i=0; i<blobs.size(); ++i) {
    cvl::draw::cross(result, blobs[i].baricenter, 3, cv::Scalar(cvl::red));
    cvl::drawNumber(result, blobs[i].label, cv::Point(blobs[i].roi.x + blobs[i].roi.width + 2, blobs[i].roi.y + 6), cvl::yellow);
  }

  // --- Save ---

  // The frame on the left, the annotated result on the right.
  cv::Mat left, canvas;
  cv::cvtColor(frame, left, cv::COLOR_GRAY2BGR);
  cv::hconcat(left, result, canvas);

  cvl::save(canvas, outputPath);

  printf("\nSaved %s\n", outputPath.c_str());

  return 0;

}
