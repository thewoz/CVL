# CVL

CVL is a header-only **C**omputer **V**ision **L**ibrary built on top of OpenCV.
It provides convenience wrappers and small algorithms for common CV tasks such as
background modeling, thresholding, connected-component labeling, blob processing
and splitting, denoising, image enhancement, sub-pixel corner refinement and
drawing utilities.

## Requirements

A **C++17** compiler and the library **OpenCV 4**


## Installation

CVL is header-only. Either add the `include/` directory to your include path, or use
the provided `Makefile`, which symlinks `include/` to `/usr/local/include/cvl`:

```sh
make install     # creates /usr/local/include/cvl -> ./include
make uninstall   # removes the symlink
```

## Usage

Include the umbrella header to access all modules (everything lives under the `cvl` namespace):

```cpp
#include <cvl/cvl.hpp>
```

By default the headers are included as `<cvl/...>`. 
If you define `_DEBUG_CVL_LIBS_` before including `cvl.hpp`, they are included with local quotes (`"..."`) instead, which is handy when working on the library itself.

### Minimal example

```cpp
#include <cvl/cvl.hpp>

int main() {
  cv::Mat image = cvl::open("image.png", cv::IMREAD_GRAYSCALE);

  cv::Mat thresholded;
  cvl::threshold::get(image, thresholded, "OTSU", 0, 0, 0);

  cvl::blobs_t blobs;
  cvl::labeling::get(thresholded, cvl::labeling::NEXT_NEAREST_NEIGHBOURS, /*minClusterMass=*/5, blobs, image);

  cv::Mat out(image.size(), CV_8UC1, cv::Scalar::all(0));
  blobs.drawMask(out);

  cvl::save(out, "out.png");
  return 0;
}
```

## Modules

| Header | Namespace | Purpose |
| --- | --- | --- |
| `background` | `cvl::background` | Background estimation (lightest, darkest, median, mode, average, bucket, fixed) |
| `baricenter` | `cvl::baricenter` | Baricenter computations (geometric / intensity weighted) |
| `blob` / `blobs` | `cvl` | `blob_t` / `blobs_t` data structures and drawing helpers |
| `blobSplitting` | `cvl::blobsplitting` | Blob splitting via iterative thresholding + watershed |
| `chessboard` | `cvl::chessboard`, `cvl` | Checkerboard / circle-grid detection and `chessboard_t` |
| `clustering` | `cvl::clustering` | Nearest-neighbor clustering of blobs |
| `colors` | `cvl` | Named BGR color palette and the `colors` lookup class |
| `contours` | `cvl::contours` | Internal / external contour extraction, contour scaling |
| `convert` | `cvl::convert` | 16-bit → 8-bit conversions (several modes) |
| `corners` | `cvl` | `corners_t` loader for tracked corners |
| `deflickering` | global | `asDeflickering` temporal deflickering helper |
| `denoising` | `cvl::denoising` | Denoising filters (gaussian, median, morphology, bilateral, starflag, …) |
| `draw` | `cvl::draw` | Drawing primitives (dashed circle, cross) |
| `enhance` | `cvl::enhance` | Contrast / CLAHE / unsharp / histogram equalization |
| `labeling` | `cvl::labeling` | Connected-components labeling into blobs |
| `opencv` | `cvl` | OpenCV helpers (I/O, resize, histograms, text/markers, operators) |
| `pose` | `cvl::pose` | Pose / stereo utilities |
| `subpixel` | `cvl::subpixel` | Sub-pixel corner refinement (old `cv::cornerSubPix` + new ROCHADE-like detector) |
| `threshold` | `cvl::threshold` | Thresholding utilities (otsu, adaptive, global, median, mode, percentile) |
| `utils` | `cvl::io`, `cvl::log`, `cvl::utils`, `cvl` | Self-contained helpers: path/file I/O, logging, `loader`, clustering primitives, `itoa`/`decToHex` |
| `window` | `cvl` | `window_t`, an OpenCV window wrapper with event hooks |

## Caveats

- Unless stated otherwise, functions assume single-channel 8-bit (`CV_8UC1`) images;
  color or 16-bit inputs are only supported where explicitly documented
  (`convert`, `histogram16bit`, `open16bitC1`, `background::lightest/darkest/avg`, …).
- Error handling is fail-fast: `cvl::log::error()` prints to `stderr` and calls
  `exit(EXIT_FAILURE)`. The library does not throw recoverable exceptions.
- The library is **not thread-safe** (lazy-initialized color palette, global
  detector parameters in `subpixel`, shared window registry in `window_t`).
- Baricenters follow a pixel-center convention: the `baricenter::*` functions add
  **+0.5** to the averaged integer pixel coordinates.

## Testing

A smoke-test suite lives in `tests/test_cvl.cpp` and exercises every module
against known inputs. Run it from the repository root:

```sh
make test
```

It builds against the working-tree headers (no install needed) and prints
`ALL <n> CHECKS PASSED` on success, or the failing file/line and aborts with a
non-zero exit code on a regression. Requires `pkg-config` with an `opencv4`
package.

## Future optimizations

Ideas left for later, only worth doing if they become real bottlenecks:

- **Spatial index for clustering.** `cvl::utils::connectedComponents` and `cvl::utils::medianFirstNNDistance` (in `utils.hpp`) are brute-force, O(n²) in the number of blobs. For large blob counts a uniform spatial grid (or a k-d tree) would bring the neighbor search down to roughly O(n).
- **OpenCV-based connected components.** `cvl::labeling::get` uses a hand-written union-find over a contiguous label matrix. Replacing the core with `cv::connectedComponentsWithStats` (SIMD-optimized, supports 4/8-connectivity) would likely be faster; it is a larger refactor because the per-blob pixel lists still have to be collected afterwards.
- **Integral images for deflickering.** `asDeflickering::deflicker` (in `deflickering.hpp`) recomputes the mean of a `deflickerMatrixSize`×`deflickerMatrixSize` window for every pixel and every frame, giving roughly O(W·H·T·k²). Precomputing one integral image per frame (`cv::integral`) would make each window mean O(1), bringing the whole pass down to about O(W·H·T). This is the heaviest bottleneck on real video.

