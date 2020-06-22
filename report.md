# Sensor Fusion Nanodegree

## Camera Based 2D Feature Tracking Project
Luciano Silveira
June, 2020

### Specification

<img src="images/keypoints.png" width="820" height="248" />

The objective is to build a `feature racking` component using state-of-the-art `detector/descriptor` combinations to see which ones perform best.

### Data Buffer

Section `MP.1` implements a vector for data buffering whose size does not exceed a limit of 3 elements (variable `dataBufferSize`).

```cpp
int dataBufferSize = 3;//2; // no. of images which are held in memory (ring buffer) at the same time
...
if (dataBuffer.size() >= dataBufferSize)
{
    dataBuffer.erase(dataBuffer.begin());
}
dataBuffer.push_back(frame);
```

A `ring buffer` is used to optimize memory load, it pushes-in new elements on one end and removes elements on the other end.

### Keypoints

#### Keypoint Detection

Section `MP.2` implements the `HARRIS`, `FAST`, `BRISK`, `ORB`, `AKAZE`, and `SIFT` detectors. Each one is selectable by setting the correct index on the `detector_types` vector variable.

```cpp
vector<string> detector_types = {"SHITOMASI", "HARRIS", "FAST", "BRISK", "ORB", "AKAZE", "SIFT"};
detectorType = detector_types.at(0);
...
if (detectorType.compare("SHITOMASI") == 0)
{
    detKeypointsShiTomasi(keypoints, imgGray, bVis);
}
else if (detectorType.compare("HARRIS") == 0)
{
    detKeypointsHarris(keypoints, imgGray, bVis);
}
else
{
    detKeypointsModern(keypoints, imgGray, detectorType, bVis);
}
```

#### Keypoint Removal

Section `MP.3` removes all keypoints outside of a pre-defined rectangle with dimensions: `(535, 180, 180, 150)`.

```cpp
vector<cv::KeyPoint> filtered_keypoints;
for (cv::KeyPoint &kp: keypoints)
{
    if(vehicleRect.contains(kp.pt))
    {
        filtered_keypoints.push_back(kp);
    }
}
cout << detectorType << " n= " << keypoints.size() << " keypoints, filtered= " << filtered_keypoints.size() << endl;
keypoints = filtered_keypoints;
```

### Descriptors

#### Keypoint Descriptors

Section `MP.4` implements the `BRIEF`, `ORB`, `FREAK`, `AKAZE` and `SIFT` descriptors. Each one is selectable by setting the correct index on the `descriptor_types` vector variable.

```cpp
vector<string> descriptor_types = {"BRISK", "BRIEF", "ORB", "FREAK", "AKAZE", "SIFT"};
descriptorType = descriptor_types.at(0);
```

#### Descriptor Matching

Section `MP.5` implements `FLANN` matching as well as k-nearest neighbor selection. Both methods are selectable using the respective index on the associated vector variables.

```cpp
vector<string> matcherTypes = {"MAT_BF", "MAT_FLANN"};
matcherType = matcherTypes.at(0);
```

#### MP.6 Descriptor Distance Ratio

Section `MP.6` uses the `K-Nearest-Neighbor matching` to implement the descriptor distance ratio test, which looks at the ratio of best vs. second-best match to decide whether to keep an associated pair of keypoints.

```cpp
vector<string> descriptorTypes = {"DES_BINARY", "DES_HOG"};
vector<string> selectorTypes = {"SEL_NN", "SEL_KNN"};
descriptorType = descriptorTypes.at(0);
selectorType = selectorTypes.at(0);
```

### Performance

The algorithms were tested in different combinations and compared with regard to some performance measures.

#### Performance Evaluation 1

Section `MP.7` counts the number of keypoints on the preceding vehicle for all 10 images and take note of the distribution of their neighborhood size, using all the detectors implemented.

#### Performance Evaluation 2

Section `MP.8` counts the number of matched keypoints for all 10 images using all possible combinations of detectors and descriptors. In the matching step, the BF approach is used with the descriptor distance ratio (variable `ratio_thresh`) set to `0.8`.

#### MP.9 Performance Evaluation 3

Section `MP.9` compares the time it takes for keypoint detection and descriptor extraction in the following table.

### Comments

The TOP3 detector / descriptor combinations to recommended are as follows:

|Detector|Time|Keypoints|Time|
|---|---|---|---|
| | | | |
| | | | |

### Links

 * [Original Repository](https://github.com/udacity/SFND_2D_Feature_Tracking)
 * [Project Rubric](https://review.udacity.com/#!/rubrics/2549/view)
