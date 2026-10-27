# TODO

Open items for CVL, organized by priority (Critica / Alta / Media / Bassa).
Nothing here is a known bug: these are pending decisions and deferred
improvements, kept out of the README on purpose.

## Alta — decisioni di progetto aperte

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

## Media

- **Bump di versione.** La revisione del 16/7/2026 ha rimosso l'overload pubblico
  `colors::RGBtoHex(const cv::Vec3b &)` (trappola BGR/RGB, zero utilizzatori nei
  progetti locali): è una rottura di API. Decidere il bump di
  `CVL_LIB_VERSION` in `cvl.hpp` (proposta: v3.2.6 → v3.3.0) al prossimo commit.

- **Test mancanti** (dettagli e razionale in `tests/README.md`):
  - Strengthen the smoke-level areas into oracle tests (`enhance`, `denoising`,
    `subpixel "new"`) with hand-verified reference outputs.
  - Add `tests/data/` with a real chessboard image + expected corners to cover
    `chessboard::get` and the stereo `pose::*` functions end-to-end.
  - Fork-based death tests for the `log::error()` paths, including the checks
    added in the 2026-07-16 review: deflicker time/local means mismatch, `threshold::*`
    mask validation, `starflag` type check, `corners_t` frame bound,
    `contours::get` mode validation.

## Bassa

- **Policy `-Wconversion` (decisione presa, da non riaprire senza motivo).**
  Con `-Wconversion` il codice CVL genera ~183 warning di conversioni implicite
  (double→int su coordinate, int→uchar su intensità), esaminati a campione e
  intenzionali. La build di riferimento resta `-Wall -Wextra -Wpedantic`
  (pulita, con gli header OpenCV via `-isystem`). Non silenziare in massa con
  cast cosmetici; eventuali cast espliciti solo su file mirati e con revisione.

- **`utils::medianFirstNNDistance` con un solo elemento restituisce `FLT_MAX`**
  (comportamento documentato: un solo blob → un solo cluster in
  `clustering::NN`). Valutare se un giorno serva un contratto più esplicito.

- **Ottimizzazioni future** (only worth doing if they become real bottlenecks;
  ogni cambio va accompagnato da un benchmark):
  - **Spatial index for clustering.** `cvl::utils::connectedComponents` and
    `cvl::utils::medianFirstNNDistance` (in `utils.hpp`) are brute-force, O(n²) in
    the number of blobs. For large blob counts a uniform spatial grid (or a k-d
    tree) would bring the neighbor search down to roughly O(n).
  - **OpenCV-based connected components.** `cvl::labeling::get` uses a hand-written
    union-find over a contiguous label matrix. Replacing the core with
    `cv::connectedComponentsWithStats` (SIMD-optimized, supports 4/8-connectivity)
    would likely be faster; it is a larger refactor because the per-blob pixel
    lists still have to be collected afterwards.
