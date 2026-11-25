# Changelog

## v4.0 — 2026-11-25

Changes since v3.2.5. This is the first public release: the library no longer
depends on MPL, part of the public API has been renamed or merged, and it now
comes with a test suite, static analysis and an example. See
**Breaking changes** before upgrading.

### New

- **Self-contained.** The only dependency is OpenCV 4. The helpers that came
  from MPL (logging, path and file I/O, the column loader, the clustering
  primitives) now live in `utils.hpp`, as `cvl::log`, `cvl::io`, `cvl::loader_t`
  and `cvl::utils`.
- **Include guard**: the single headers refuse to be included directly, the
  entry point is `<cvl/cvl.hpp>`.
- **`enhance`**: gamma correction (`gamma()`, and `"GAMMA"` in `get()`).
- **`blobsplitting`**: the `"ERODE"` and `"FIXED"` modes, next to `"TO_WHITE"`
  and `"TO_DARK"`; the watershed step and the radius of the fixed mode are
  arguments of `get()`.
- **`deflickering_t`**: additive or multiplicative correction, chosen in
  `init()`; the local means are computed once per frame with `getLocalMean()`,
  so a sliding window only pays for the frame that enters it.
- **`blobs_t::save()`**: the intensity written in the last column can be the
  blob maximum (default), `"MIN"` or `"MEAN"`.
- **`contours`**: contour scaling (`scale()`).
- **`opencv`**: `open()` with a resize percentage, `resize()`, `scaleRect()`,
  `scaleLine()`, `shrinkLine()`, `getTopRightPoint()`, `drawRotatedText()`,
  `drawDistanceMarker()`.
- **Example**: `src/main.cpp`, built with `make example`. It needs no input
  data.
- **Tooling**: `make test` and `make test_asan` (91 checks on synthetic
  inputs), `make lint` (`cppcheck` + `clang-tidy`).

### Changed

- **Error handling is uniform**: every error goes through `cvl::log::error()`,
  which prints `cvl::<module>::<function>() error: ...` to `stderr` and calls
  `abort()`. Before, some paths called `exit()` and some `abort()`.
- **Input validation**: empty images, wrong image types, masks of the wrong
  size, unknown algorithm names and out-of-range arguments are now reported
  instead of reaching OpenCV or reading out of bounds.
- `labeling::get()` keeps its labels in one contiguous matrix instead of
  allocating one row at a time.
- Every free function is `inline`: the headers can be included in more than one
  translation unit.
- The library builds without warnings with `-Wall -Wextra -Wpedantic -Wshadow`.
- Uniform layout of the headers (banners, section order, naming); see the
  conventions at the top of each file.

### Fixed

- `colors::getRGBHex()` returned the red and blue channels swapped: the palette
  is BGR and was converted as if it were RGB.
- `convert::from16to8()` in `"UPPER"` mode rounded instead of taking the high
  byte (384 gave 2 instead of 1).
- `drawParabola()` cast out-of-range values to `int` (undefined behavior with
  large coefficients).
- `subpixel`: the second constructor of the detector parameters left some
  fields uninitialized.
- `enhance::unsharpMask()` on a vector of images used different defaults from
  the single-image version.
- `pose`: `recoverPose()`, `solvePnP()` and `stereoCalibrate()` took their
  output matrices by value, so the computed poses never reached the caller.

### Breaking changes

- **MPL is gone.** `blob_t` no longer derives from `mpl::graph::node_t`, and
  the headers no longer include `<mpl/...>`. Code that relied on those includes
  coming in through CVL must include them itself.
- **Headers removed**:
  - `targetMass.hpp`: `cvl::targetmass` is merged into `cvl::blobsplitting`
    (`"ERODE"`, `"FIXED"` and the iterative modes of `get()`);
  - `subpixelNew.hpp`: merged into `subpixel.hpp`, the detector is reached
    through `subpixel::get(..., "new")`;
  - `features.hpp`: `cvl::features::findCircles()` is removed, call
    `cv::HoughCircles()` directly.
- **`blobsplitting::get()`** takes two more arguments, `useWatershed` and
  `radius`, before `labelingNeighboursType`.
- **Deflickering**: the global class `asDeflickering` is now
  `cvl::deflickering_t`, with a new interface:
  `init(deflickerTime, deflickerMatrixSize, mode)`, `getLocalMean(image)` and
  `deflicker(images, localMeans, output)`. It no longer loads a mask or keeps
  the image list.
- **`enhance::unsharpMask()`** on a vector of images now has `amount = 3` as
  default, like the single-image version (it was 1).
- **Renames**:
  - `blobs_t::getBarycenters()` is `getBaricenters()`;
  - the namespace `cvl::util` is `cvl::utils`;
  - `mpl::loader` and `mpl::loader::data` are `cvl::loader_t` and
    `cvl::loader_t::data_t`.
- **`convert::from16to8()`** in `"UPPER"` mode gives different values where the
  low byte is 128 or more (see **Fixed**).
- **`colors::getRGBHex()`** now returns the correct `#RRGGBB` string: callers
  that compensated for the swapped channels must stop doing so.
- Include only `<cvl/cvl.hpp>`: the single headers refuse to be included
  directly.

### Known limitations

See [TODO.md](TODO.md) (in Italian). In short: not thread-safe, `enhance` and
`denoising` are only smoke-tested, and the chessboard detection and the stereo
pose functions have no tests yet.
