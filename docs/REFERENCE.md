# CVL – API Reference (minimal)

This page summarizes the main functions available in the library headers.
All functions are header-only and live under the `include/` directory.

## Background (`include/background.hpp`)

- `background::lightest(src, dst)` – per-pixel maximum background.
- `background::darkest(src, dst)` – per-pixel minimum background.
- `background::median(src, dst)` – per-pixel median background.
- `background::moda(src, dst)` – per-pixel mode (most frequent) background.
- `background::avg(src, dst)` – per-pixel average background.
- `background::bucket(src, dst)` – mask pixels close to the median.
- `background::fixed(src, dst, value)` – copy pixels above a threshold.
- `background::get(src, dst, algo, value)` – dispatcher (LIGHTEST, DARKEST, MEDIAN, MODA, AVG, FIXED, NONE).

## Baricenter (`include/baricenter.hpp`)

- `baricenter::geometric(pixels)` – geometric barycenter.
- `baricenter::maxIntensity(pixels)` – max-intensity point.
- `baricenter::lowIntensity(pixels)` – min-intensity point.
- `baricenter::maxIntensityWeighted(pixels)` – intensity-weighted barycenter.
- `baricenter::lowIntensityWeighted(pixels)` – inverted-intensity barycenter.
- `baricenter::get(pixels, algo)` – dispatcher.

## Blob (`include/blob.hpp`)

Class `cvl::blob_t`:

- `blob_t::add(x, y, i)` – add a pixel (x,y,intensity).
- `blob_t::size()` – number of pixels.
- `blob_t::getImageSize()` – ROI size.
- `blob_t::clear()` – reset pixels.
- `blob_t::finalize()` – compute ROI, barycenter, min/max, average.
- `blob_t::print(out)` – print blob info.
- `blob_t::draw(...)` – draw the blob.
- `blob_t::drawInv(...)` – draw inverted intensities.
- `blob_t::drawMask(dst)` – draw a white mask.
- `blob_t::drawLabel(dst)` – draw the label at the barycenter.
- `blob_t::saveContours(out, camera, frame)` – save contours.
- `blob_t::println(out)` – print pixels.
- `blob_t::operator<` – barycenter ordering.
- `blob_t::operator[]` – indexed pixel access.
- `operator-(A, B)` – distance between barycenters.

## Blobs (`include/blobs.hpp`)

Class `cvl::blobs_t`:

- `blobs_t::size()` – number of blobs.
- `blobs_t::clear()` – reset collection.
- `blobs_t::erase(index)` – remove a blob.
- `blobs_t::add(blob)` / `add(blobs)` – add blobs.
- `blobs_t::insert(index, blob)` – insert a blob.
- `blobs_t::begin()` / `end()` – iterators.
- `blobs_t::finalize()` – sort and recompute stats.
- `blobs_t::draw(...)` – draw blobs.
- `blobs_t::drawMask(dst)` – draw masks.
- `blobs_t::drawBaricenters(dst)` – draw barycenters.
- `blobs_t::drawLabels(dst)` – draw labels.
- `blobs_t::drawContours(dst)` – draw contours.
- `blobs_t::enclosingCircle(center, radius)` – enclosing circle of barycenters.
- `blobs_t::getConvexHull(hull)` – barycenter hull.
- `blobs_t::boundingRect(roi)` – barycenter bounding box.
- `blobs_t::getBarycenters(out)` – list of barycenters.
- `blobs_t::save(outPoints, camera, frame, outFrame)` – save blob data.
- `blobs_t::saveContours(out, camera, frame)` – save contours.

## Blob splitting (`include/blobSplitting.hpp`)

- `blobsplitting::utils::split(...)` – split a blob by iterative thresholding.
- `blobsplitting::get(...)` – split a collection with watershed (TO_DARK/TO_WHITE).

## Chessboard (`include/chessboard.hpp`)

- `chessboard::get(image, patternSize, corners, mode, algorithm)` – detect chessboard or circle grid.
- `chessboard::get(path, ...)` – image loading + detection.
- `chessboard::saveImage(image, file, corners, boardSize, status)` – save image with corners.
- `chessboard::saveCorners(out, file, corners, boardSize)` – save corners to stream.

Class `cvl::chessboard_t`:

- `scale(factor)` – scale object points.
- `load(filepath)` – load object/image points from file.

## Clustering (`include/clustering.hpp`)

- `clustering::NN(blobs, clusters, factor)` – NN clustering.
- `clustering::follow(blobs, coords, clusters, factor)` – track closest cluster.

## Contours (`include/contours.hpp`)

- `contours::get(image, mode, contours)` – extract internal/external contours.

## Convert (`include/convert.hpp`)

- `convert::from16to8(src, dst, alpha, beta)` – 16→8 with scale/offset.
- `convert::from16to8(src, dst, mode, value)` – 16→8 with modes.

## Corners (`include/corners.hpp`)

Class `cvl::corners_t`:

- `load(filepath)` – load tracked corners.
- `size()` – number of frames.
- `operator[]` – per-frame access.

## Deflickering (`include/deflickering.hpp`)

Class `asDeflickering`:

- `init(images, window, time, matrixSize, maskPath, invert)` – initialize.
- `deflicker(images)` – apply deflickering.

## Denoising (`include/denoising.hpp`)

- `denoising::morphology(image, size)` – opening morphology.
- `denoising::gaussian(src, size)` – Gaussian blur.
- `denoising::blur(src, size)` – box blur.
- `denoising::bilateralFilter(src, diameter, sigmaColor, sigmaSpace)` – bilateral filter.
- `denoising::anisotropic(src, amount, sensitivity)` – anisotropic diffusion.
- `denoising::median(src, size)` – median blur.
- `denoising::lowlightFilter(src, factor)` – low-light noise filtering.
- `denoising::starflag(src, workImage, kernelSize, denoiseMatrix)` – Starflag algorithm.
- `denoising::get(src, algo, param, time)` – dispatcher.

## Draw (`include/draw.hpp`)

- `draw::dashedCircle(image, center, radius, dashWidth, dashSpacing, color, thickness)` – dashed circle.
- `draw::cross(image, center, length, color, thickness)` – diagonal cross.

## Enhance (`include/enhance.hpp`)

- `enhance::histogramEqualization(src)` – histogram equalization.
- `enhance::contrast(image, alpha, beta)` – linear contrast.
- `enhance::autoContrast(image, clipHistPercent)` – automatic contrast.
- `enhance::unsharpMask(src, size, sigma, threshold, amount)` – unsharp mask.
- `enhance::laplacianUnsharpMask(src, amount)` – Laplacian sharpening.
- `enhance::clahe(src, clipLimit, tilesGridSize)` – CLAHE.
- `enhance::get(src, algo, params...)` – dispatcher.

## Features (`include/features.hpp`)

- `features::findCircles(image, minDist, higherThreshold, accumulatorThreshold, minRadius, maxRadius)` – Hough circles.

## Labeling (`include/labeling.hpp`)

- `labeling::get(src, neighboursType, minClusterMass, blobs, original, offset)` – connected components labeling.

## OpenCV utilities (`include/opencv.hpp`)

- `drawString(dst, str, pos, color)` – draw numeric strings.
- `drawNumber(dst, number, pos, color)` – draw an integer.
- `drawParabola(dst, coeffs, color, offset)` – draw parabola.
- `cross(dst, pos, color)` – small cross marker.
- `minEnclosureRoi(roi1, roi2)` – enclosing ROI.
- `getRandomColor(minBright)` – random color.
- `RGBtoHex(R, G, B)` – hex string conversion.
- `save(image, path)` – save image.
- `open(path, mode)` – open image.
- `open16bitC1(path)` – open 16-bit mono image.

## Pose (`include/pose.hpp`)

- `pose::recoverPose(...)` – recover relative pose.
- `pose::solvePnP(...)` – solvePnP for stereo.
- `pose::stereoCalibrate(...)` – stereo calibration with fixed intrinsics.

## Subpixel (`include/subpixel.hpp` and `include/subpixelNew.hpp`)

- `subpixel::get(image, corners, roiSize, algorithm)` – subpixel refinement.
- `cornerSubPixNew(image, corners, roiSize)` – new algorithm refinement.

## Target mass (`include/targetMass.hpp`)

- `targetmass::fixed(blobs, dst, radius)` – draw radius targets at barycenters.
- `targetmass::erode(blobs, src, targetMass, minMass, neighboursType)` – erode blobs.
- `targetmass::split(blobs, src, targetMass, toDark, minMass, neighboursType)` – split blobs by mass.

## Threshold (`include/threshold.hpp`)

- `threshold::otsu(src, dst, mask)` – Otsu threshold.
- `threshold::adaptive(src, dst, method, type, blockSize, C, mask)` – adaptive threshold.
- `threshold::global(src, dst, threshold, mask)` – global threshold.
- `threshold::median(src, dst, factor, valueToSkip, mask)` – median-based threshold.
- `threshold::moda(src, dst, factor, valueToSkip, mask)` – mode-based threshold.
- `threshold::percentile(src, dst, percentile, valueToSkip, mask)` – percentile threshold.
- `threshold::get(src, dst, algo, param1, param2, param3, mask)` – dispatcher.

## Window (`include/window.hpp`)

Class `cvl::window_t`:

- `init(name, flags)` – create window.
- `show(image, [title], time)` – show image.
- `setTitle(str)` – set title.
- `toggleFullScreen()` / `setFullScreen()` / `unsetFullScreen()` – fullscreen.
- `update(image, title)` – update image.
- `resize(width, height)` – resize window.
- `move(x, y)` – move window.
- `broadcast()` – broadcast event.
- `setCursor(type)` – set cursor if supported.
