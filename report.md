# Sensor Fusion Nanodegree

|Projects|
|---|
|[Lidar Obstacle Detection](https://github.com/ladrians/sfND_Lidar_Obstacle_Detection/blob/master/report.md)|
|[Camera Based 2D Feature Tracking](https://github.com/ladrians/sfND_2D_Feature_Tracking/blob/master/report.md)|
|[3D Object Tracking](https://github.com/ladrians/sfND_3D_Object_Tracking/blob/master/report.md)|
|[Radar Target Generation and Detection](https://github.com/ladrians/sfND_Radar_Target_Generation_and_Detection/blob/master/README.md)|
|[Unscented Kalman Filter Highway](https://github.com/ladrians/sfND_Unscented_Kalman_Filter/blob/master/report.md)|

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

The results are:

 * [Shi-Tomasi sample](./data/shi_tomasi_corners01.png)
 * [Harris sample](./data/harris_corners01.png)
 * [FAST sample](./data/fast_corners01.png)
 * [BRISK sample](./data/brisk_corners01.png)
 * [ORB sample](./data/orb_corners01.png)
 * [AKAZE sample](./data/akaze_corners01.png)
 * [SIFT sample](./data/sift_corners01.png)

#### Keypoint Removal

Section `MP.3` removes all keypoints outside of a pre-defined rectangle with dimensions: `(535, 180, 180, 150)`.

```cpp
vector<cv::KeyPoint> filtered_keypoints;
for (cv::KeyPoint &kp: keypoints)
{
    // https://knowledge.udacity.com/questions/112624
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

#### Detector Keypoints Analysis

Section `MP.7` counts the number of keypoints on the preceding vehicle for all 10 images and take note of the distribution of their neighborhood size, using all the detectors implemented.

|Detector|Average Keypoints|Average Time(ms)|
|---|---|---|
|Shi-Tomasi|1342.3|16.86845|
|Harris|173.7|19.3867775|
|FAST|4920.4|2.976255|
|BRISK|2711.6|392.601175|
|ORB|500|8.842625|
|AKAZE|1342.9|84.624945|
|SIFT|1386|108.1360375|

The `Harris` and `ORB` detectors where the ones with the least ammout of keypoints while `FAST`, `SIFT` got the most.

The distribution of their neighborhood size can be checked with the associated [images](./data/), best results in this matter are `FAST`, `AKAZE` and `SIFT` detectors.

#### Descriptor Keypoints Analysis

Section `MP.8` counts the number of matched keypoints for all 10 images using all possible combinations of detectors and descriptors. In the matching step, the BF approach is used with the descriptor distance ratio (variable `ratio_thresh`) set to `0.8`.

|Descriptor|Average Keypoints|Average Time(ms)|Average Matches|
|---|---|---|---|
|BRIEF|178.5857143|2.611125714|129.6349206|
|BRISK|177.2714286|2.366152857|114.6349206|
|FREAK|167.8428571|43.67435143|109.0634921|
|ORB|185.2333333|4.670678333|135.1296296|
|SIFT|138.7|52.85753|89.11111111|

The `ORB`, `BRIEF` and `BRISK` descriptors got the better results.

#### MP.9 Recommendations

Section `MP.9` compares the time it takes for keypoint detection and descriptor extraction. From the previous tables

 * Best detectors: `FAST`, `SIFT`.
 * Best descriptors: `ORB`, `BRIEF`, `BRISK`.

The selection are as follows

 * `FAST` + `ORB`.
 * `FAST` + `BRIEF`.
 * `SIFT` + `BRISK`.

For more information check the [data.xlsx](./data/data.xlsx) file.

### Links

 * [Original Repository](https://github.com/udacity/SFND_2D_Feature_Tracking)
 * [Project Rubric](https://review.udacity.com/#!/rubrics/2549/view)

### Appendix

Detailed execution by descriptor / detector.

|Detector|Keypoints|Time(ms)|Descriptor|Keypoints|Time(ms)|Matches|
|---|---|---|---|---|---|---|
|Shi-Tomasi|1370|20.9835|BRISK|125|1.4278||
|Shi-Tomasi|1301|18.4651|BRISK|118|2.5446|95|
|Shi-Tomasi|1361|18.1078|BRISK|123|2.4739|88|
|Shi-Tomasi|1358|19.3971|BRISK|120|2.37|80|
|Shi-Tomasi|1333|18.2879|BRISK|120|3.5536|90|
|Shi-Tomasi|1284|22.3868|BRISK|113|2.4855|82|
|Shi-Tomasi|1322|18.0228|BRISK|114|2.5337|79|
|Shi-Tomasi|1366|16.8659|BRISK|123|2.4803|85|
|Shi-Tomasi|1389|16.211|BRISK|111|2.4035|86|
|Shi-Tomasi|1339|16.8063|BRISK|112|2.4004|82|
|Shi-Tomasi|1370|21.6224|BRIEF|125|80.5991||
|Shi-Tomasi|1301|18.5684|BRIEF|118|2.5992|115|
|Shi-Tomasi|1361|18.8873|BRIEF|123|1.7221|111|
|Shi-Tomasi|1358|17.1322|BRIEF|120|1.7904|104|
|Shi-Tomasi|1333|17.1816|BRIEF|120|2.395|101|
|Shi-Tomasi|1284|19.0577|BRIEF|113|1.9301|102|
|Shi-Tomasi|1322|20.0692|BRIEF|114|1.6549|102|
|Shi-Tomasi|1366|17.2158|BRIEF|123|1.8442|100|
|Shi-Tomasi|1389|17.506|BRIEF|111|2.0523|109|
|Shi-Tomasi|1339|17.9038|BRIEF|112|1.9114|100|
|Shi-Tomasi|1370|21.0485|ORB|125|113.062||
|Shi-Tomasi|1301|18.52|ORB|118|1.2239|106|
|Shi-Tomasi|1361|18.3203|ORB|123|0.6699|102|
|Shi-Tomasi|1358|17.9579|ORB|120|1.0538|99|
|Shi-Tomasi|1333|20.8956|ORB|120|1.1848|102|
|Shi-Tomasi|1284|19.1954|ORB|113|1.1464|103|
|Shi-Tomasi|1322|18.2686|ORB|114|1.5535|97|
|Shi-Tomasi|1366|18.269|ORB|123|1.2839|98|
|Shi-Tomasi|1389|17.9476|ORB|111|1.1011|104|
|Shi-Tomasi|1339|17.9163|ORB|112|1.0573|97|
|Shi-Tomasi|1370|20.787|FREAK|125|47.6837||
|Shi-Tomasi|1301|16.9147|FREAK|118|54.7787|86|
|Shi-Tomasi|1361|10.0729|FREAK|123|41.006|90|
|Shi-Tomasi|1358|9.7158|FREAK|120|46.9841|86|
|Shi-Tomasi|1333|10.0621|FREAK|120|42.3174|88|
|Shi-Tomasi|1284|9.6886|FREAK|113|44.1429|86|
|Shi-Tomasi|1322|9.342|FREAK|114|41.9085|80|
|Shi-Tomasi|1366|9.3878|FREAK|123|41.024|81|
|Shi-Tomasi|1389|10.3823|FREAK|111|43.3316|86|
|Shi-Tomasi|1339|9.367|FREAK|112|40.7289|85|
|Harris|115|58.6394|BRISK|17|0.5449||
|Harris|98|17.723|BRISK|14|1.6047|12|
|Harris|113|17.1792|BRISK|18|1.6032|10|
|Harris|121|16.4875|BRISK|21|1.6465|14|
|Harris|160|17|BRISK|26|1.6653|15|
|Harris|383|29.3307|BRISK|43|1.8153|16|
|Harris|85|17.0616|BRISK|18|0.3436|16|
|Harris|210|19.0684|BRISK|31|1.7121|15|
|Harris|171|17.7978|BRISK|26|1.7|23|
|Harris|281|21.663|BRISK|34|1.7004|21|
|Harris|115|21.3585|BRIEF|17|0.6298||
|Harris|98|18.4084|BRIEF|14|1.4926|14|
|Harris|113|17.275|BRIEF|18|1.5392|11|
|Harris|121|16.4884|BRIEF|21|1.5241|15|
|Harris|160|16.7256|BRIEF|26|1.55|20|
|Harris|383|27.8917|BRIEF|43|1.5601|24|
|Harris|85|16.0524|BRIEF|18|2.0915|26|
|Harris|210|24.6894|BRIEF|31|2.227|16|
|Harris|171|18.3624|BRIEF|26|2.0576|24|
|Harris|281|21.1034|BRIEF|34|0.4257|23|
|Harris|115|20.7076|ORB|17|1.0148||
|Harris|98|19.1637|ORB|14|1.0043|12|
|Harris|113|17.9628|ORB|18|1.0086|12|
|Harris|121|17.9104|ORB|21|1.0677|15|
|Harris|160|17.4213|ORB|26|0.8871|18|
|Harris|383|28.9475|ORB|43|0.9117|24|
|Harris|85|17.218|ORB|18|1.0399|20|
|Harris|210|20.271|ORB|31|0.8682|15|
|Harris|171|18.666|ORB|26|0.9058|24|
|Harris|281|23.1116|ORB|34|0.9864|22|
|Harris|115|21.2452|FREAK|17|46.9758||
|Harris|98|21.6584|FREAK|14|40.5535|13|
|Harris|113|10.6875|FREAK|18|40.7121|13|
|Harris|121|11.0748|FREAK|21|45.2681|15|
|Harris|160|10.5227|FREAK|26|42.4614|15|
|Harris|383|21.2006|FREAK|43|42.2848|17|
|Harris|85|10.0116|FREAK|18|41.858|20|
|Harris|210|11.675|FREAK|31|40.9844|12|
|Harris|171|11.589|FREAK|26|40.7941|21|
|Harris|281|14.1206|FREAK|34|41.2367|18|
|FAST|5063|44.2178|BRISK|419|5.4923||
|FAST|4952|1.9405|BRISK|427|5.208|256|
|FAST|4863|1.8162|BRISK|404|4.4988|243|
|FAST|4840|1.7789|BRISK|423|3.9827|241|
|FAST|4856|2.0501|BRISK|386|4.1053|239|
|FAST|4899|2.0707|BRISK|414|4.6815|215|
|FAST|4870|1.853|BRISK|418|6.144|251|
|FAST|4868|2.0478|BRISK|406|4.5017|248|
|FAST|4996|2.0577|BRISK|396|4.8009|243|
|FAST|4997|2.0811|BRISK|401|4.3594|247|
|FAST|5063|2.992|BRIEF|419|4.7578||
|FAST|4952|2.3878|BRIEF|427|4.4679|320|
|FAST|4863|1.8268|BRIEF|404|3.4613|332|
|FAST|4840|2.4324|BRIEF|423|2.7567|299|
|FAST|4856|2.4011|BRIEF|386|2.5093|331|
|FAST|4899|1.6947|BRIEF|414|1.4493|276|
|FAST|4870|1.7922|BRIEF|418|1.4227|327|
|FAST|4868|1.7987|BRIEF|406|1.6191|324|
|FAST|4996|1.7637|BRIEF|396|1.7217|315|
|FAST|4997|1.7777|BRIEF|401|2.6893|307|
|FAST|5063|2.1151|ORB|419|1.9979||
|FAST|4952|2.0317|ORB|427|1.9591|307|
|FAST|4863|1.6788|ORB|404|1.6143|308|
|FAST|4840|1.6836|ORB|423|1.4943|298|
|FAST|4856|2.0143|ORB|386|2.0032|321|
|FAST|4899|1.6697|ORB|414|1.3764|283|
|FAST|4870|1.7954|ORB|418|1.2986|315|
|FAST|4868|1.762|ORB|406|1.2673|323|
|FAST|4996|1.7556|ORB|396|1.2696|302|
|FAST|4997|1.8|ORB|401|1.7184|311|
|FAST|5063|2.0731|FREAK|419|53.4844||
|FAST|4952|1.9523|FREAK|427|49.5748|251|
|FAST|4863|1.7161|FREAK|404|42.8337|247|
|FAST|4840|1.784|FREAK|423|43.2384|233|
|FAST|4856|1.779|FREAK|386|42.7761|255|
|FAST|4899|1.7042|FREAK|414|46.0568|231|
|FAST|4870|1.7732|FREAK|418|42.7117|265|
|FAST|4868|1.7576|FREAK|406|42.3013|251|
|FAST|4996|1.7016|FREAK|396|42.416|253|
|FAST|4997|1.722|FREAK|401|43.1927|247|
|BRISK|2757|447.486|BRISK|264|5.3191||
|BRISK|2777|415.594|BRISK|282|2.7842|171|
|BRISK|2741|388.883|BRISK|282|2.7648|176|
|BRISK|2735|384.369|BRISK|277|2.8708|157|
|BRISK|2757|407.135|BRISK|297|2.9502|176|
|BRISK|2695|386.323|BRISK|279|2.7508|174|
|BRISK|2715|383.875|BRISK|289|2.8767|188|
|BRISK|2628|390.918|BRISK|272|2.6491|173|
|BRISK|2639|384.393|BRISK|266|2.6682|171|
|BRISK|2672|386.016|BRISK|254|2.527|184|
|BRISK|2757|392.436|BRIEF|264|2.3678||
|BRISK|2777|398.796|BRIEF|282|2.1058|178|
|BRISK|2741|393.511|BRIEF|282|0.9415|205|
|BRISK|2735|385.569|BRIEF|277|1.0065|185|
|BRISK|2757|383.17|BRIEF|297|1.2719|179|
|BRISK|2695|391.784|BRIEF|279|1.2264|183|
|BRISK|2715|382.95|BRIEF|289|1.0552|195|
|BRISK|2628|384.936|BRIEF|272|1.8278|207|
|BRISK|2639|385.208|BRIEF|266|1.1279|189|
|BRISK|2672|384.488|BRIEF|254|0.9225|183|
|BRISK|2757|390.674|ORB|264|21.4511||
|BRISK|2777|390.842|ORB|282|3.7717|162|
|BRISK|2741|391.055|ORB|282|4.02|175|
|BRISK|2735|383.221|ORB|277|5.3627|158|
|BRISK|2757|384.529|ORB|297|4.1787|167|
|BRISK|2695|387.627|ORB|279|4.0107|160|
|BRISK|2715|394.173|ORB|289|3.9616|182|
|BRISK|2628|397.265|ORB|272|3.7885|167|
|BRISK|2639|395.201|ORB|266|3.6918|171|
|BRISK|2672|439.499|ORB|254|4.7524|172|
|BRISK|2757|393.07|FREAK|242|48.9649||
|BRISK|2777|386.436|FREAK|260|46.7266|160|
|BRISK|2741|385.816|FREAK|263|42.7966|177|
|BRISK|2735|401.09|FREAK|264|48.8874|155|
|BRISK|2757|403.365|FREAK|274|42.3239|173|
|BRISK|2695|384.671|FREAK|256|41.3583|161|
|BRISK|2715|386.514|FREAK|269|43.4083|183|
|BRISK|2628|382.252|FREAK|255|41.1196|169|
|BRISK|2639|383.887|FREAK|243|40.994|178|
|BRISK|2672|385.02|FREAK|237|40.939|168|
|ORB|500|91.3032|BRISK|83|2.5049||
|ORB|500|7.3971|BRISK|93|1.2342|73|
|ORB|500|6.7798|BRISK|95|1.5744|74|
|ORB|500|5.8203|BRISK|103|1.1734|79|
|ORB|500|5.7944|BRISK|101|1.1472|85|
|ORB|500|6.2832|BRISK|116|1.3675|79|
|ORB|500|6.7729|BRISK|120|1.8536|92|
|ORB|500|5.844|BRISK|120|1.299|90|
|ORB|500|5.8306|BRISK|119|1.285|88|
|ORB|500|6.4054|BRISK|120|1.6068|91|
|ORB|500|9.6067|BRIEF|92|1.9774||
|ORB|500|7.8745|BRIEF|102|0.7076|49|
|ORB|500|6.5417|BRIEF|106|0.6446|43|
|ORB|500|6.3804|BRIEF|113|0.6897|45|
|ORB|500|5.9997|BRIEF|109|0.7189|59|
|ORB|500|5.649|BRIEF|125|0.6349|53|
|ORB|500|5.6971|BRIEF|130|0.781|78|
|ORB|500|5.7374|BRIEF|129|0.7772|68|
|ORB|500|6.184|BRIEF|127|0.7256|84|
|ORB|500|7.4719|BRIEF|128|0.6997|66|
|ORB|500|9.9101|ORB|92|5.6458||
|ORB|500|6.9194|ORB|102|3.8194|67|
|ORB|500|7.7666|ORB|106|4.591|70|
|ORB|500|5.8235|ORB|113|3.8824|72|
|ORB|500|6.1015|ORB|109|3.8066|84|
|ORB|500|8.384|ORB|125|4.5759|91|
|ORB|500|6.468|ORB|130|4.2441|101|
|ORB|500|8.4743|ORB|129|4.5909|92|
|ORB|500|6.1871|ORB|127|3.7482|93|
|ORB|500|7.7144|ORB|128|3.6827|93|
|ORB|500|9.5101|FREAK|46|52.1927||
|ORB|500|7.6138|FREAK|53|46.4286|42|
|ORB|500|5.648|FREAK|56|41.18|36|
|ORB|500|5.7815|FREAK|65|41.0636|44|
|ORB|500|5.9977|FREAK|55|41.2444|47|
|ORB|500|5.9826|FREAK|64|43.7245|44|
|ORB|500|6.5484|FREAK|66|41.4454|51|
|ORB|500|5.8333|FREAK|71|42.9168|52|
|ORB|500|5.8314|FREAK|73|41.2552|48|
|ORB|500|5.836|FREAK|71|40.7972|56|
|AKAZE|1351|132.627|BRISK|166|1.8152||
|AKAZE|1327|86.5124|BRISK|157|2.6158|137|
|AKAZE|1311|85.286|BRISK|161|1.6384|125|
|AKAZE|1351|89.6202|BRISK|155|1.6045|129|
|AKAZE|1360|81.1683|BRISK|163|1.6776|129|
|AKAZE|1347|81.1228|BRISK|164|1.6322|131|
|AKAZE|1363|94.3334|BRISK|173|1.7533|132|
|AKAZE|1331|83.4483|BRISK|175|1.7889|142|
|AKAZE|1357|98.1819|BRISK|177|1.7544|146|
|AKAZE|1331|82.3817|BRISK|179|1.8241|144|
|AKAZE|1351|113.517|BRIEF|166|0.8614||
|AKAZE|1327|87.5553|BRIEF|157|1.8941|141|
|AKAZE|1311|83.3454|BRIEF|161|1.2543|134|
|AKAZE|1351|82.893|BRIEF|155|0.8807|131|
|AKAZE|1360|81.8838|BRIEF|163|1.0381|130|
|AKAZE|1347|85.867|BRIEF|164|1.0597|134|
|AKAZE|1363|81.3676|BRIEF|173|1.152|146|
|AKAZE|1331|78.6912|BRIEF|175|1.1048|150|
|AKAZE|1357|82.4711|BRIEF|177|0.9829|148|
|AKAZE|1331|84.4338|BRIEF|179|0.9183|152|
|AKAZE|1351|111.267|ORB|166|2.8961||
|AKAZE|1327|92.553|ORB|157|3.6883|131|
|AKAZE|1311|94.647|ORB|161|2.7789|129|
|AKAZE|1351|91.6073|ORB|155|2.6941|127|
|AKAZE|1360|98.2547|ORB|163|2.7758|117|
|AKAZE|1347|95.3623|ORB|164|2.635|130|
|AKAZE|1363|83.9926|ORB|173|2.3848|131|
|AKAZE|1331|79.917|ORB|175|3.6172|137|
|AKAZE|1357|82.5119|ORB|177|3.5631|135|
|AKAZE|1331|81.442|ORB|179|2.601|145|
|AKAZE|1351|112.494|FREAK|166|42.7666||
|AKAZE|1327|76.8036|FREAK|157|57.0543|126|
|AKAZE|1311|80.6963|FREAK|161|46.0007|129|
|AKAZE|1351|65.8204|FREAK|155|42.8393|127|
|AKAZE|1360|61.2978|FREAK|163|45.7353|121|
|AKAZE|1347|70.805|FREAK|164|43.397|122|
|AKAZE|1363|53.1201|FREAK|173|42.9143|133|
|AKAZE|1331|56.808|FREAK|175|42.7728|144|
|AKAZE|1357|65.5763|FREAK|177|42.2237|147|
|AKAZE|1331|53.3143|FREAK|179|43.4038|138|
|SIFT|1437|430.034|BRISK|137|1.7058||
|SIFT|1371|132.948|BRISK|132|1.4205|64|
|SIFT|1381|88.6354|BRISK|124|1.276|66|
|SIFT|1336|71.8391|BRISK|138|1.72|63|
|SIFT|1303|108.583|BRISK|134|1.684|67|
|SIFT|1370|106.12|BRISK|140|1.4245|59|
|SIFT|1396|128.271|BRISK|137|1.3085|64|
|SIFT|1382|101.811|BRISK|148|1.4668|64|
|SIFT|1462|101.736|BRISK|159|1.695|67|
|SIFT|1422|113.632|BRISK|137|1.8344|80|
|SIFT|1437|164.092|BRIEF|138|1.0021||
|SIFT|1371|133.227|BRIEF|132|0.8719|86|
|SIFT|1381|107.474|BRIEF|124|0.7689|78|
|SIFT|1336|104.037|BRIEF|138|0.9083|77|
|SIFT|1303|103.444|BRIEF|134|0.8693|86|
|SIFT|1370|106.466|BRIEF|140|0.9872|69|
|SIFT|1396|108.396|BRIEF|137|0.8891|74|
|SIFT|1382|106.904|BRIEF|148|0.9099|76|
|SIFT|1462|105.477|BRIEF|159|0.9504|70|
|SIFT|1422|103.59|BRIEF|137|0.8141|88|
|SIFT|1437|159.859|FREAK|137|41.7679||
|SIFT|1371|136.773|FREAK|131|44.5062|65|
|SIFT|1381|103.213|FREAK|123|41.5585|72|
|SIFT|1336|102.788|FREAK|137|40.0334|65|
|SIFT|1303|102.575|FREAK|133|40.8675|67|
|SIFT|1370|105.505|FREAK|139|40.8556|59|
|SIFT|1396|102.709|FREAK|135|41.1711|59|
|SIFT|1382|107.139|FREAK|147|49.3414|64|
|SIFT|1462|94.2318|FREAK|158|42.6307|65|
|SIFT|1422|109.271|FREAK|135|39.9769|79|
|SIFT|1437|175.616|SIFT|138|116.283||
|SIFT|1371|65.0474|SIFT|132|49.1089|82|
|SIFT|1381|64.3432|SIFT|124|50.3537|81|
|SIFT|1336|65.356|SIFT|138|48.0907|86|
|SIFT|1303|52.8564|SIFT|134|43.8238|94|
|SIFT|1370|50.7479|SIFT|140|42.5458|90|
|SIFT|1396|50.6028|SIFT|137|42.2224|81|
|SIFT|1382|48.9931|SIFT|148|44.5072|82|
|SIFT|1462|50.7849|SIFT|159|41.8963|102|
|SIFT|1422|50.3135|SIFT|137|49.7435|104|