# CVL

CVL is a simple, header-only Computer Vision Library built on top of OpenCV.
It provides convenience wrappers for common CV tasks such as background modeling,
thresholding, blob processing, denoising, enhancement, and drawing utilities.

## Requirements

- OpenCV (headers `opencv2/*`)

## Installation

CVL is header-only. Add the `include/` directory to your include path.

### CMake

```cmake
target_include_directories(my_target PRIVATE /path/to/CVL/include)
```

## Usage

Include the main header to access all modules:

```cpp
#include <cvl/cvl.hpp>
```

### Minimal example

```cpp
#include <cvl/cvl.hpp>

int main() {
  cv::Mat image = cvl::open("image.png", cv::IMREAD_GRAYSCALE);

  cv::Mat thresholded;
  cvl::threshold::get(image, thresholded, "OTSU", 0, 0, 0);

  cv::imwrite("out.png", thresholded);
  return 0;
}
```

## Modules

- `background` – background estimation (median, mode, average, etc.)
- `baricenter` – barycenter computations
- `blob` / `blobs` – blob data structures and utilities
- `blobSplitting` – blob splitting via thresholding + watershed
- `chessboard` – chessboard/circle grid detection
- `clustering` – nearest-neighbor clustering
- `contours` – contour extraction from masks
- `convert` – 16-bit to 8-bit conversions
- `corners` – loading tracked corners
- `deflickering` – temporal deflickering
- `denoising` – denoising filters
- `draw` – drawing primitives
- `enhance` – contrast/CLAHE/sharpening
- `features` – feature helpers (Hough circles)
- `labeling` – connected components labeling
- `opencv` – OpenCV helpers and colors
- `pose` – pose and stereo utilities
- `subpixel` – subpixel corner refinement
- `targetMass` – split/erode blobs by mass
- `threshold` – thresholding utilities
- `window` – OpenCV window wrapper

## Documentation

A minimal API reference is available in [`docs/REFERENCE.md`](docs/REFERENCE.md).
