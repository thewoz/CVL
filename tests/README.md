# CVL test suite

`test_cvl.cpp` is a single self-contained executable, built and run with
`make test` from the repository root. It creates its own inputs (synthetic
images and small text files in a `mkdtemp` temp dir) and asserts on the result;
on failure it prints `FAIL <file>:<line>  <condition>` and exits non-zero.

## Two kinds of test

Not every check is equally strong. Each assertion falls into one of two classes:

- **Oracle** — the input is chosen so that the correct output is known
  analytically, and the test asserts the *exact* value (e.g. the median of
  `{10,20,30,40,50}` is `30`). A regression that changes the result fails.
- **Property / smoke** — the exact output is not known (or not meaningful), so
  the test asserts a weaker invariant: "the output is binary", "the size/type is
  preserved", "the value stays in range", or simply "it does not crash / does not
  trigger undefined behavior". These catch crashes and gross regressions but
  **not** subtle numerical errors.

The distinction matters: a green suite means "no crash and no gross regression",
which is *not* the same as "every algorithm is numerically correct". The
property/smoke areas are the ones most worth strengthening with real
ground-truth data later.

## Coverage by module

| Module / function | Kind | What is actually checked |
| --- | --- | --- |
| `labeling::get` | Oracle | 2 diagonal pixels → 2 blobs (4-conn) / 1 blob (8-conn); a 3-blob scene → labels in row-major baricenter order |
| `blobs_t::finalize` | Oracle | baricenters computed, then labels reassigned 0,1,2 in row-major order |
| `background` (MEDIAN/MODA/AVG/LIGHTEST/DARKEST/NONE/FIXED) | Oracle | per-pixel result of constant frames `{10,20,30,40,50}` |
| `baricenter::get` (GEOMETRIC, MAX_INTENSITY) | Oracle | single pixel `(5,7)` → `(5.5,7.5)` (+0.5 convention) |
| `contours::get` | Oracle | filled 3×3 square → 8 internal / 12 external boundary pixels |
| `convert::from16to8` (LINEAR_MINMAX) | Oracle | min→0, max→255; UPPER returns `(INF,INF)` |
| `clustering::NN` | Oracle | 4 points in two far-apart pairs → 2 clusters of 2 |
| `colors` (`RGBtoHex`, `BGRtoHex`, `getRGBHex`, `get`), `utils::decToHex` | Oracle | `RGBtoHex(255,0,0)=="#FF0000"`, `get("red")==(0,0,255)` BGR, `decToHex(0)=="00"`; **regression: `getRGBHex("red")=="#FF0000"`** (before the fix the BGR palette went through `RGBtoHex` and came out with R/B swapped) |
| `utils::median` | Oracle | `{3,1,2}→2`, `{4,1,3,2}→2.5` |
| `io::basename/subdirName/isToSkip` | Oracle | exact string / bool results; **regression: `subdirName("a/b.png")=="a"`** (top-level relative dirs used to return `""`) |
| `io::expandPath` | Property | a path with `//` and `/./` inside the temp dir is normalized and the file stays reachable via `fopen` |
| `opencv`: `minEnclosureRoi`, `shrinkLine`, `scaleLine`, `getTopRightPoint`, `histogram8bit/16bit` | Oracle | union rect; endpoint math; **`scaleLine` guards the int-midpoint fix**; exact histogram bins and 16-bit min/max |
| `loader`, `corners_t`, `chessboard_t` | Oracle | text files with known columns → exact parsed values, frame grouping |
| `blob_t::draw*` | Oracle (property) | `drawMask` sets exactly `size()` pixels; `blobs_t::getBaricenters` returns one point |
| `threshold::get` (all 6 algos) | Property | output has size/type of input and is strictly binary (0/255) |
| `blobsplitting::get` (TO_WHITE, watershed, FIXED) | Property | a bridged double-square splits into 2 non-empty blobs |
| `subpixel::get` ("old") | Property | refined corner lands within 1 px of the true saddle of a synthetic checkerboard |
| `subpixel::get` ("new") | Smoke | runs without crashing, keeps the corner count |
| `deflickering` | Property | flickering constant frames → intensity spread stays bounded (≤ 20) |
| `enhance::get` (all algos) | Smoke | single-channel input, no throw, size/type preserved |
| `denoising::get` (all algos) | Smoke | no throw, size/type preserved |
| `drawParabola`, `drawString`, `drawNumber`, `drawRotatedText`, `drawDistanceMarker`, `draw::dashedCircle`, `draw::cross` | Smoke | run on a real canvas without crashing; `drawParabola` is fed huge coefficients to guard the out-of-range cast fix |

## Not tested (and why)

These are deliberately left out; they need resources or data the suite cannot
synthesize in-process:

- **`pose::recoverPose` / `solvePnP` / `stereoCalibrate`** — require calibrated
  stereo correspondences with a known pose to check against. Needs a real
  ground-truth dataset.
- **`chessboard::get`** (checkerboard / circle-grid detection) — needs real
  calibration-target images; a synthetic pattern is not representative of the
  detector's behavior.
- **`window_t`** — GUI, needs a display/highgui backend. Only its *compile-time*
  contract is verified elsewhere (copy ctor/assignment are `= delete`, so copying
  a window is a compile error).
- **I/O to disk**: `open`, `open16bitC1`, `save`, `resize`-from-file — filesystem
  and codec dependent. `expandPath` has a normalization property test in the
  suite (see the table), but its `~`/absolute expansion is still not asserted
  (it depends on `$HOME`/cwd).
- **Error paths** — every `cvl::log::error()` call ends in `abort()`,
  so bad-argument / empty-input branches cannot be exercised from inside the same
  process without forking. The validation checks (empty blob collections,
  non-CV_8UC1 inputs, empty loader column lists, and the 2026-07-16 additions:
  deflicker window mismatch, `threshold::*` mask validation, `starflag` type
  check, `corners_t` frame bound, `contours::get` mode validation) are
  therefore unverified by the suite. They were each exercised manually with
  standalone programs during the review (clear message + abort confirmed).
- **Exact numeric output of the filters** — `enhance` and `denoising` are only
  smoke-tested. Their pixel-level output is not compared against a reference.
- **Randomness** — `colors::getRandom` / `getRandomColor` now share a seeded
  `std::mt19937` (`cvl::utils::rng`), but the suite does not assert on the random
  values themselves.

## Sanitizer runs

The suite runs clean under AddressSanitizer + UndefinedBehaviorSanitizer:

```sh
c++ -std=c++17 -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined \
    -D_DEBUG_CVL_LIBS_ $(pkg-config --cflags opencv4) -Iinclude \
    tests/test_cvl.cpp -o tests/test_cvl_san $(pkg-config --libs opencv4)
OPENCV_OPENCL_RUNTIME=disabled ASAN_OPTIONS=detect_leaks=0 ./tests/test_cvl_san
```

Note on OpenCL: on macOS the Apple Intel GPU OpenCL driver crashes
intermittently during kernel handling (SEGV/GPU hang inside
`AppleIntelKBLGraphicsGLDriver`, outside CVL code), with or without sanitizers.
The suite therefore sets `OPENCV_OPENCL_RUNTIME=disabled` via `setenv` at the
top of `main()`, which disables OpenCL process-wide before OpenCV initializes
it. (`cv::ocl::setUseOpenCL(false)` is NOT enough: it only affects the calling
thread, and the subpixel detector runs under `parallel_for_`.)
`detect_leaks=0` silences one-time allocations inside OpenCV/highgui.

## How to strengthen this later

The highest-value additions, roughly in order:

1. Turn the smoke areas (`enhance`, `denoising`, `subpixel "new"`) into oracle
   tests by checking against small hand-verified reference outputs.
2. Add a `tests/data/` folder with a real chessboard image + expected corners to
   cover `chessboard::get` and the stereo `pose::*` functions end-to-end.
3. Fork-based death tests for the `log::error` paths (assert the process exits
   with a non-zero code on invalid input).
