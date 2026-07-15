# TODO

Open items for CVL. Nothing here is a bug: these are pending decisions and
deferred improvements, kept out of the README on purpose.

## Open design decisions

1. **`subpixel` detector `strength`: 40 vs 150.**
   In `include/subpixel.hpp` (`detail::filterKeyPoints`) the historical code set
   `strength = 150` on a local struct that was never used, so the effective value
   has always been the default **40** (kept for behavioral compatibility, see the
   note in the code). Decide the intended value by validating both on real
   calibration data.

2. **Otsu + mask semantics.**
   `threshold::otsu()` applies the mask to the *output only*: masked-out pixels do
   not affect the computed threshold. A mask-aware variant exists but is unused
   (`threshold::utils::otsu(src, excludeValue)`). Decide whether the mask should
   influence the threshold computation and wire it in (or drop the unused helper).

## Future optimizations

Only worth doing if they become real bottlenecks:

- **Spatial index for clustering.** `cvl::utils::connectedComponents` and
  `cvl::utils::medianFirstNNDistance` (in `utils.hpp`) are brute-force, O(n²) in
  the number of blobs. For large blob counts a uniform spatial grid (or a k-d
  tree) would bring the neighbor search down to roughly O(n).
- **OpenCV-based connected components.** `cvl::labeling::get` uses a hand-written
  union-find over a contiguous label matrix. Replacing the core with
  `cv::connectedComponentsWithStats` (SIMD-optimized, supports 4/8-connectivity)
  would likely be faster; it is a larger refactor because the per-blob pixel
  lists still have to be collected afterwards.
- **Integral images for deflickering.** `asDeflickering::deflicker` (in
  `deflickering.hpp`) recomputes the mean of a
  `deflickerMatrixSize`×`deflickerMatrixSize` window for every pixel and every
  frame, giving roughly O(W·H·T·k²). Precomputing one integral image per frame
  (`cv::integral`) would make each window mean O(1), bringing the whole pass down
  to about O(W·H·T). This is the heaviest bottleneck on real video.

## Tests

- Strengthen the smoke-level areas into oracle tests (`enhance`, `denoising`,
  `subpixel "new"`) with hand-verified reference outputs.
- Add `tests/data/` with a real chessboard image + expected corners to cover
  `chessboard::get` and the stereo `pose::*` functions end-to-end.
- Fork-based death tests for the `log::error()` paths.
  (Details in `tests/README.md`.)
