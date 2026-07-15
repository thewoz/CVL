/*
 * CVL – smoke test suite
 *
 * Self-contained regression tests for the header-only CVL library.
 * Built and run via `make test` from the repository root.
 *
 * Each block exercises one module against known inputs and asserts on the
 * result. Failure prints the line and the failed condition and aborts with a
 * non-zero exit code, so `make test` fails loudly on a regression.
 *
 * NOTE: cvl::log::error() calls abort(), so the intentional-error
 * paths (bad arguments, empty inputs, ...) cannot be exercised in-process and
 * are not covered here.
 */

#include "cvl.hpp"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

static int checks = 0;

#define CHECK(cond) do { \
    if(!(cond)) { fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); exit(1); } \
    ++checks; \
  } while(0)

//****************************************************************************/
// helper: temp directory for the file-based loaders
//****************************************************************************/
static std::string g_tmpDir;

static std::string tmpPath(const char * name) {
  return g_tmpDir + "/" + name;
}

//****************************************************************************/
// labeling: connectivity 4 vs 8
//****************************************************************************/
static void test_labeling() {
  cv::Mat img = cv::Mat::zeros(10, 10, CV_8UC1);
  img.at<uchar>(2,2) = 255; img.at<uchar>(3,3) = 255; // touching only on the diagonal
  cvl::blobs_t b4, b8;
  cvl::labeling::get(img, cvl::labeling::NEAREST_NEIGHBOURS, 1, b4, img);
  cvl::labeling::get(img, cvl::labeling::NEXT_NEAREST_NEIGHBOURS, 1, b8, img);
  CHECK(b4.size() == 2);   // 4-connectivity keeps them separate
  CHECK(b8.size() == 1);   // 8-connectivity merges them
}

//****************************************************************************/
// blobs_t::finalize: labels follow row-major baricenter order
//****************************************************************************/
static void test_finalize_order() {
  cv::Mat img = cv::Mat::zeros(100, 100, CV_8UC1);
  cv::rectangle(img, cv::Rect(80, 80, 5, 5), cv::Scalar(200), cv::FILLED);
  cv::rectangle(img, cv::Rect(10, 10, 5, 5), cv::Scalar(200), cv::FILLED);
  cv::rectangle(img, cv::Rect(50, 40, 5, 5), cv::Scalar(200), cv::FILLED);
  cvl::blobs_t blobs;
  cvl::labeling::get(img, cvl::labeling::NEXT_NEAREST_NEIGHBOURS, 1, blobs, img);
  CHECK(blobs.size() == 3);
  CHECK(blobs[0].baricenter.y < blobs[1].baricenter.y);
  CHECK(blobs[1].baricenter.y < blobs[2].baricenter.y);
  CHECK(blobs[0].label == 0 && blobs[1].label == 1 && blobs[2].label == 2);
}

//****************************************************************************/
// background: known per-pixel results
//****************************************************************************/
static void test_background() {
  std::vector<cv::Mat> frames;
  int vals[5] = {10, 20, 30, 40, 50};
  for(int v : vals) frames.push_back(cv::Mat(8, 8, CV_8UC1, cv::Scalar(v)));
  cv::Mat dst;
  dst.release(); cvl::background::get(frames, dst, "MEDIAN");    CHECK(dst.at<uchar>(4,4) == 30);
  dst.release(); cvl::background::get(frames, dst, "MODA");      CHECK(dst.at<uchar>(4,4) == 10);
  dst.release(); cvl::background::get(frames, dst, "AVG");       CHECK(dst.at<uchar>(4,4) == 30);
  dst.release(); cvl::background::get(frames, dst, "LIGHTEST");  CHECK(dst.at<uchar>(4,4) == 50);
  dst.release(); cvl::background::get(frames, dst, "DARKEST");   CHECK(dst.at<uchar>(4,4) == 10);
  dst.release(); cvl::background::get(frames, dst, "NONE");      CHECK(dst.at<uchar>(4,4) == 30);
  dst.release(); cvl::background::get(frames, dst, "FIXED", 25); CHECK(dst.at<uchar>(4,4) == 30);
}

//****************************************************************************/
// threshold: every dispatcher produces a binary image
//****************************************************************************/
static void test_threshold() {
  cv::Mat grad(64, 64, CV_8UC1);
  for(int y=0; y<64; ++y) for(int x=0; x<64; ++x) grad.at<uchar>(y,x) = (uchar)(x*4);
  const char * algos[] = {"GLOBAL", "OTSU", "MEDIAN", "MODA", "PERCENTILE", "ADAPTIVE"};
  double p1[] = {128, 0, 1.0, 1.0, 50, 11};
  for(int a=0; a<6; ++a) {
    cv::Mat src = grad.clone(), out;
    cvl::threshold::get(src, out, algos[a], p1[a], 2, -1);
    double mn, mx; cv::minMaxLoc(out, &mn, &mx);
    CHECK(out.size() == src.size() && out.type() == CV_8UC1);
    CHECK((mn == 0 || mn == 255) && (mx == 0 || mx == 255));
  }
}

//****************************************************************************/
// blobSplitting: split, watershed and fixed modes
//****************************************************************************/
static void test_blobsplitting() {
  auto makeScene = [](cv::Mat & img){
    img = cv::Mat::zeros(20, 40, CV_8UC1);
    cv::rectangle(img, cv::Rect(5, 5, 8, 8),  cv::Scalar(200), cv::FILLED);
    cv::rectangle(img, cv::Rect(25, 5, 8, 8), cv::Scalar(200), cv::FILLED);
    for(int x=13; x<25; ++x) img.at<uchar>(8, x) = 100; // dark bridge
  };
  cv::Mat img; makeScene(img);
  cv::Mat bin; cv::threshold(img, bin, 50, 255, cv::THRESH_BINARY);

  cvl::blobs_t blobs;
  cvl::labeling::get(bin, cvl::labeling::NEXT_NEAREST_NEIGHBOURS, 1, blobs, img);
  CHECK(blobs.size() == 1);   // everything connected through the bridge

  cvl::blobsplitting::get(blobs, img, img, "TO_WHITE", 100, 20, 5, false, false, 0, cvl::labeling::NEXT_NEAREST_NEIGHBOURS);
  CHECK(blobs.size() == 2);

  // watershed refinement path
  cvl::labeling::get(bin, cvl::labeling::NEXT_NEAREST_NEIGHBOURS, 1, blobs, img);
  cvl::blobsplitting::get(blobs, img, img, "TO_WHITE", 100, 20, 5, false, true, 0, cvl::labeling::NEXT_NEAREST_NEIGHBOURS);
  CHECK(blobs.size() == 2);
  CHECK(blobs[0].size() > 0 && blobs[1].size() > 0);

  // FIXED mode
  cvl::labeling::get(bin, cvl::labeling::NEXT_NEAREST_NEIGHBOURS, 1, blobs, img);
  cvl::blobsplitting::get(blobs, img, img, "TO_WHITE", 100, 20, 5, false, false, 0, cvl::labeling::NEXT_NEAREST_NEIGHBOURS);
  cvl::blobsplitting::get(blobs, img, img, "FIXED", 0, 20, 5, false, false, 4, cvl::labeling::NEXT_NEAREST_NEIGHBOURS);
  CHECK(blobs.size() == 2);
}

//****************************************************************************/
// clustering: two well-separated groups
//****************************************************************************/
static void test_clustering() {
  cvl::blobs_t blobs;
  int pos[4][2] = {{0,0},{1,1},{50,50},{51,51}};
  for(auto & p : pos) { cvl::blob_t b; b.add(p[0], p[1], 100); blobs.add(b); }
  blobs.finalize();
  std::vector<cvl::blobs_t> clusters;
  cvl::clustering::NN(blobs, clusters, 2.0);
  CHECK(clusters.size() == 2);
  CHECK(clusters[0].size() == 2 && clusters[1].size() == 2);
}

//****************************************************************************/
// convert: 16 -> 8 modes
//****************************************************************************/
static void test_convert() {
  cv::Mat s16(4, 4, CV_16UC1);
  for(int i=0; i<16; ++i) s16.at<ushort>(i/4, i%4) = (ushort)(1000 + i*100);
  cv::Mat d8;
  cvl::convert::from16to8(s16, d8, cvl::convert::LINEAR_MINMAX);
  double mn, mx; cv::minMaxLoc(d8, &mn, &mx);
  CHECK(mn == 0 && mx == 255);
  cvl::convert::from16to8(s16, d8, cvl::convert::LINEAR);
  cvl::convert::from16to8(s16, d8, cvl::convert::LINEAR_CUTWINGS, 0.1);
  cvl::convert::from16to8(s16, d8, cvl::convert::LINEAR_CUTEXTREMAL);
  auto ab = cvl::convert::from16to8(s16, d8, cvl::convert::UPPER);
  CHECK(std::isinf(ab.first));
  cvl::convert::from16to8(s16, d8, cvl::convert::LOWER);
}

//****************************************************************************/
// contours: internal vs external pixel counts on a 3x3 square
//****************************************************************************/
static void test_contours() {
  cv::Mat img = cv::Mat::zeros(5, 5, CV_8UC1);
  cv::rectangle(img, cv::Rect(1, 1, 3, 3), cv::Scalar(255), cv::FILLED);
  std::vector<cv::Point> ci, ce;
  cvl::contours::get(img, cvl::contours::INTERNAL, ci);
  cvl::contours::get(img, cvl::contours::EXTERNAL, ce);
  CHECK(ci.size() == 8);   // border pixels of the filled square
  CHECK(ce.size() == 12);  // background ring around it
}

//****************************************************************************/
// baricenter: +0.5 pixel-center convention
//****************************************************************************/
static void test_baricenter() {
  std::vector<cv::Point3i> px = { {5, 7, 100} };
  cv::Point2d g = cvl::baricenter::get(px, "GEOMETRIC");
  CHECK(g.x == 5.5 && g.y == 7.5);
  cv::Point2d m = cvl::baricenter::get(px, "MAX_INTENSITY");
  CHECK(m.x == 5.5 && m.y == 7.5);
}

//****************************************************************************/
// subpixel: refine a synthetic checkerboard saddle
//****************************************************************************/
static void test_subpixel() {
  cv::Mat cb(64, 64, CV_8UC1);
  for(int y=0; y<64; ++y) for(int x=0; x<64; ++x)
    cb.at<uchar>(y,x) = ((x < 32) ^ (y < 32)) ? 200 : 50;
  cv::GaussianBlur(cb, cb, cv::Size(5,5), 0);
  std::vector<cv::Point2f> corners = { {30.5f, 30.5f} };
  cvl::subpixel::get(cb, corners, 15, "old");
  CHECK(std::fabs(corners[0].x - 31.5f) < 1.0f);
  CHECK(std::fabs(corners[0].y - 31.5f) < 1.0f);
  std::vector<cv::Point2f> corners2 = { {30.5f, 30.5f} };
  cvl::subpixel::get(cb, corners2, 40, "new"); // must not crash
  CHECK(corners2.size() == 1);
}

//****************************************************************************/
// deflickering: does not blow up the intensity range
//****************************************************************************/
static void test_deflickering() {
  std::vector<cv::Mat> frames;
  int vals[5] = {100, 110, 90, 105, 95};
  for(int v : vals) frames.push_back(cv::Mat(16, 16, CV_8UC1, cv::Scalar(v)));
  asDeflickering defl;
  defl.init(frames, 5, 3, 3, "", false);
  defl.deflicker(frames);
  double lo = 255, hi = 0;
  for(auto & f : frames) { double mn, mx; cv::minMaxLoc(f, &mn, &mx); lo = std::min(lo, mn); hi = std::max(hi, mx); }
  CHECK(hi - lo <= 20);
}

//****************************************************************************/
// loader / corners_t / chessboard_t: round-trip through text files
//****************************************************************************/
static void test_loaders() {
  {
    FILE * f = fopen(tmpPath("loader.txt").c_str(), "w");
    fprintf(f, "1 2 3\n# comment\n4 5 6\n");
    fclose(f);
    std::vector<std::pair<int,int>> rows;
    cvl::loader ld;
    ld(tmpPath("loader.txt"), "1 3", [&rows](const cvl::loader::data & d){ rows.push_back({ d.get<int>(0), d.get<int>(1) }); });
    CHECK(rows.size() == 2);
    CHECK(rows[0].first == 1 && rows[0].second == 3);
    CHECK(rows[1].first == 4 && rows[1].second == 6);
  }
  {
    FILE * f = fopen(tmpPath("corners.txt").c_str(), "w");
    fprintf(f, "id 0 z 1.5 2.5\nid 1 z 3.5 4.5\n");
    fclose(f);
    cvl::corners_t crn;
    crn.load(tmpPath("corners.txt"));
    CHECK(crn.size() == 2);
    CHECK(crn[0][0] == cv::Point2d(1.5, 2.5));
    CHECK(crn[1][0] == cv::Point2d(3.5, 4.5));
  }
  {
    FILE * f = fopen(tmpPath("chess.txt").c_str(), "w");
    fprintf(f, "f1 0 0 0 10.0 20.0\nf1 1 0 0 30.0 40.0\nf2 0 0 0 50.0 60.0\n");
    fclose(f);
    cvl::chessboard_t cbt;
    cbt.load(tmpPath("chess.txt"));
    CHECK(cbt.objectPoints.size() == 2);
    CHECK(cbt.imagePoints[0].size() == 2 && cbt.imagePoints[1].size() == 1);
    CHECK(cbt.imagePoints[1][0] == cv::Point2d(50.0, 60.0));
  }
}

//****************************************************************************/
// colors / hex
//****************************************************************************/
static void test_colors() {
  CHECK(cvl::colors::RGBtoHex(255, 0, 0) == "#FF0000");
  CHECK(cvl::utils::decToHex(0) == "00");
  CHECK(cvl::utils::decToHex(255) == "FF");
  CHECK(cvl::colors::get("red") == cv::Vec3b(0, 0, 255));
}

//****************************************************************************/
// io helpers
//****************************************************************************/
static void test_io() {
  CHECK(cvl::io::basename("/a/b/c.png") == "c.png");
  CHECK(cvl::io::subdirName("/a/b/c.png") == "b");
  CHECK(cvl::io::isToSkip("   # x") == true);
  CHECK(cvl::io::isToSkip("data") == false);
}

//****************************************************************************/
// opencv helpers and drawing
//****************************************************************************/
static void test_opencv_helpers() {
  cv::Rect r1(0, 0, 4, 4), r2(2, 2, 6, 6);
  CHECK(cvl::minEnclosureRoi(r1, r2) == (r1 | r2));

  cv::Point p1(0, 0), p2(10, 0);
  cvl::shrinkLine(p1, p2, 2);
  CHECK(p1 == cv::Point(2, 0) && p2 == cv::Point(8, 0));

  // scaleLine: midpoint computed in double (center 2.5, not rounded to 2)
  cv::Point q1(1, 0), q2(4, 0);
  cvl::scaleLine(q1, q2, 2.0);
  CHECK(q2.x == 5);

  std::vector<cv::Point> tp = { {0,0}, {5,0}, {3,2} };
  CHECK(cvl::getTopRightPoint(tp) == cv::Point(5, 0));

  CHECK(cvl::utils::median(std::vector<double>{3, 1, 2}) == 2.0);
  CHECK(cvl::utils::median(std::vector<double>{4, 1, 3, 2}) == 2.5);

  cv::Mat canvas(100, 100, CV_8UC3, cv::Scalar::all(0));
  cvl::drawString(canvas, "123", cv::Point(10, 10), cvl::yellow);
  cvl::drawNumber(canvas, 42, cv::Point(10, 30), cvl::red);
  cvl::drawParabola(canvas, cv::Vec3d(0.01, -1, 50), cvl::lime);
  cvl::drawParabola(canvas, cv::Vec3d(1e8, 1e8, 1e8), cvl::lime); // huge y: must not UB/crash
  cvl::drawRotatedText(canvas, "test", cv::Point(50, 50), 30, 0.4, cv::Scalar(0, 255, 0));
  cvl::drawDistanceMarker(canvas, cv::Point(10, 90), cv::Point(90, 90), "8 cm", cv::Scalar(255, 0, 0));
  cvl::draw::dashedCircle(canvas, cv::Point(50, 50), 20, 5, 5, cv::Scalar(0, 0, 255));
  cvl::draw::cross(canvas, cv::Point2f(50, 50), 3, cv::Scalar(255, 255, 0));

  cv::Mat c1(8, 8, CV_8UC1, cv::Scalar(7));
  cv::Mat h;
  cvl::histogram8bit(c1, h);
  CHECK(h.at<float>(7) == 64.0f);
  cv::Mat s16(4, 4, CV_16UC1, cv::Scalar(500));
  s16.at<ushort>(0,0) = 100;
  int mn, mx;
  cvl::histogram16bit(s16, h, mn, mx);
  CHECK(mn == 100 && mx == 500);
}

//****************************************************************************/
// enhance dispatchers (single-channel, must not throw)
//****************************************************************************/
static void test_enhance() {
  cv::Mat r(64, 64, CV_8UC1);
  cv::randu(r, 0, 255);
  cv::Mat t;
  t = r.clone(); cvl::enhance::get(t, "LAPLACIAN", 1.1);
  t = r.clone(); cvl::enhance::get(t, "UNSHARPMASK", 4, 1, 5, 3); // even size must not throw
  t = r.clone(); cvl::enhance::get(t, "CONTRAST", 1.2, 10);
  t = r.clone(); cvl::enhance::get(t, "AUTO_CONTRAST", 1);
  t = r.clone(); cvl::enhance::get(t, "EQUALIZE_HISTOGRAM", 0);
  t = r.clone(); cvl::enhance::get(t, "CLAHE", 2, 8, 8);
  CHECK(t.size() == r.size() && t.type() == CV_8UC1);
}

//****************************************************************************/
// denoising dispatchers
//****************************************************************************/
static void test_denoising() {
  cv::Mat r(64, 64, CV_8UC1);
  cv::randu(r, 0, 255);
  cv::Mat t;
  t = r.clone(); cvl::denoising::get(t, "GAUSSIAN", 3);
  t = r.clone(); cvl::denoising::get(t, "MEDIAN", 3);
  t = r.clone(); cvl::denoising::get(t, "MORPHOLOGY", 3);
  t = r.clone(); cvl::denoising::get(t, "LOWLIGHTFILTER", 1.5);
  t = r.clone(); cvl::denoising::get(t, "STARFLAG", 3, 2);
  CHECK(t.size() == r.size() && t.type() == CV_8UC1);
}

//****************************************************************************/
// blob draw round-trip: drawn mask has exactly `size()` pixels set
//****************************************************************************/
static void test_blob_draw() {
  cv::Mat img = cv::Mat::zeros(10, 10, CV_8UC1);
  cv::rectangle(img, cv::Rect(2, 3, 4, 3), cv::Scalar(200), cv::FILLED);
  cvl::blobs_t blobs;
  cvl::labeling::get(img, cvl::labeling::NEXT_NEAREST_NEIGHBOURS, 1, blobs, img);
  CHECK(blobs.size() == 1);
  const cvl::blob_t & b = blobs[0];
  cv::Mat roiImg = cv::Mat::zeros(b.getImageSize(), CV_8UC1);
  b.drawMask(roiImg, cv::Point(b.roi.x, b.roi.y));
  CHECK((size_t)cv::countNonZero(roiImg) == b.size());

  std::vector<cv::Point2f> bc;
  blobs.getBaricenters(bc); // renamed API
  CHECK(bc.size() == 1);
}

//****************************************************************************/
// main
//****************************************************************************/
int main() {

  // su macOS il driver OpenCL Apple/Intel crasha in modo intermittente
  // (fuori dal codice CVL); cv::ocl::setUseOpenCL(false) non basta perche'
  // vale solo per il thread chiamante e il detector subpixel usa parallel_for_,
  // quindi disabilito il runtime OpenCL a livello di processo prima che
  // OpenCV lo inizializzi
  setenv("OPENCV_OPENCL_RUNTIME", "disabled", 1);

  char tmpl[] = "/tmp/cvl_test_XXXXXX";
  char * dir = mkdtemp(tmpl);
  if(dir == nullptr) { fprintf(stderr, "cannot create temp dir\n"); return 1; }
  g_tmpDir = dir;

  test_labeling();
  test_finalize_order();
  test_background();
  test_threshold();
  test_blobsplitting();
  test_clustering();
  test_convert();
  test_contours();
  test_baricenter();
  test_subpixel();
  test_deflickering();
  test_loaders();
  test_colors();
  test_io();
  test_opencv_helpers();
  test_enhance();
  test_denoising();
  test_blob_draw();

  // best-effort cleanup of the temp files
  std::string rm = "rm -rf " + g_tmpDir;
  if(system(rm.c_str()) != 0) fprintf(stderr, "warning: could not remove %s\n", g_tmpDir.c_str());

  printf("ALL %d CHECKS PASSED\n", checks);
  return 0;
}
