/* INCLUDES FOR THIS PROJECT */
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "dataStructures.h"
#include "matching2D.hpp"

using namespace std;

/* MAIN PROGRAM */
int main(int argc, const char *argv[])
{

    /* INIT VARIABLES AND DATA STRUCTURES */

    // https://knowledge.udacity.com/questions/105491
    // Create an output filestream object
    /*std::ofstream result;
    //set it write and append mode
    result.open ("../data/results.csv", std::fstream::out | std::fstream::app);
    // Send the various column header names to the created csv file
    result << "Detector" << ","<< "Descriptor" << ","<< "DetectorTime" << ","<< "DescriptorTime"<< ","<< "DetectectedKeypoints" << ","<< "MatchedKeypoints"<<","<<","<< "TotalTime"<<","<< endl;
    */
    // data location
    string dataPath = "../";

    // camera
    string imgBasePath = dataPath + "images/";
    string imgPrefix = "KITTI/2011_09_26/image_00/data/000000"; // left camera, color
    string imgFileType = ".png";
    int imgStartIndex = 0; // first file index to load (assumes Lidar and camera names have identical naming convention)
    int imgEndIndex = 9;   // last file index to load
    int imgFillWidth = 4;  // no. of digits which make up the file index (e.g. img-0001.png)

    // misc
    int dataBufferSize = 3;//2;       // no. of images which are held in memory (ring buffer) at the same time
    vector<DataFrame> dataBuffer; // list of data frames which are held in memory at the same time
    bool bVis = false;            // visualize results
    bool bVisGlobal = false;
    bool bVisMatchesGlobal = false;

    /* MAIN LOOP OVER ALL IMAGES */
    cout << "|Detector|Keypoints|Time(ms)|Descriptor|Keypoints|Time(ms)|Matches|" << endl;
    cout << "|---|---|---|---|---|---|---|" << endl;

    vector<string> detector_types = {"SHITOMASI", "HARRIS", "FAST", "BRISK", "ORB", "AKAZE", "SIFT"};
    vector<string> descriptor_types = {"BRISK", "BRIEF", "ORB", "FREAK", "AKAZE", "SIFT"};

    for (string detectorType:detector_types)
    {
        for (string descriptorType:descriptor_types)
        {
            bVis = bVisGlobal;

            if (
                (detectorType.compare("AKAZE") != 0 && descriptorType.compare("AKAZE") == 0 ) ||
                (detectorType.compare("AKAZE") == 0 && descriptorType.compare("AKAZE") != 0 ) ||
                (detectorType.compare("SIFT")  == 0 && descriptorType.compare("ORB") == 0 )
            )
            {
                /*
                https://knowledge.udacity.com/questions/105392
                https://knowledge.udacity.com/questions/122170
                AKAZE descriptors worked only with AKAZE detectors
                SIFT+ORB generates memory error
                */
                continue;
            }
            dataBuffer.clear();

            for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex; imgIndex++)
            {
                /* LOAD IMAGE INTO BUFFER */

                // assemble filenames for current index
                ostringstream imgNumber;
                imgNumber << setfill('0') << setw(imgFillWidth) << imgStartIndex + imgIndex;
                string imgFullFilename = imgBasePath + imgPrefix + imgNumber.str() + imgFileType;

                // load image from file and convert to grayscale
                cv::Mat img, imgGray;
                img = cv::imread(imgFullFilename);
                cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

                //// STUDENT ASSIGNMENT
                //// TASK MP.1 -> replace the following code with ring buffer of size dataBufferSize

                // push image into data frame buffer
                DataFrame frame;
                frame.cameraImg = imgGray;
                if (dataBuffer.size() >= dataBufferSize)
                {
                    dataBuffer.erase(dataBuffer.begin());
                }
                dataBuffer.push_back(frame);

                //// EOF STUDENT ASSIGNMENT
                //cout << "#1 : LOAD IMAGE INTO BUFFER done" << endl;

                /* DETECT IMAGE KEYPOINTS */

                // extract 2D keypoints from current image
                vector<cv::KeyPoint> keypoints; // create empty feature list for current image

                //// STUDENT ASSIGNMENT
                //// TASK MP.2 -> add the following keypoint detectors in file matching2D.cpp and enable string-based selection based on detectorType
                //// -> HARRIS, FAST, BRISK, ORB, AKAZE, SIFT

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
                //// EOF STUDENT ASSIGNMENT

                //// STUDENT ASSIGNMENT
                //// TASK MP.3 -> only keep keypoints on the preceding vehicle

                // only keep keypoints on the preceding vehicle
                bool bFocusOnVehicle = true;
                cv::Rect vehicleRect(535, 180, 180, 150);
                if (bFocusOnVehicle)
                {
                    vector<cv::KeyPoint> filtered_keypoints;
                    for (cv::KeyPoint &kp: keypoints)
                    {
                        if(vehicleRect.contains(kp.pt))
                        {
                            filtered_keypoints.push_back(kp);
                        }
                    }
                    //cout << detectorType << " n= " << keypoints.size() << " keypoints, filtered= " << filtered_keypoints.size() << endl;
                    keypoints = filtered_keypoints;
                }

                //// EOF STUDENT ASSIGNMENT

                // optional : limit number of keypoints (helpful for debugging and learning)
                bool bLimitKpts = false;
                if (bLimitKpts)
                {
                    int maxKeypoints = 50;

                    if (detectorType.compare("SHITOMASI") == 0)
                    { // there is no response info, so keep the first 50 as they are sorted in descending quality order
                        keypoints.erase(keypoints.begin() + maxKeypoints, keypoints.end());
                    }
                    cv::KeyPointsFilter::retainBest(keypoints, maxKeypoints);
                    cout << " NOTE: Keypoints have been limited!" << endl;
                }

                // push keypoints and descriptor for current frame to end of data buffer
                (dataBuffer.end() - 1)->keypoints = keypoints;
                //cout << "#2 : DETECT KEYPOINTS done" << endl;

                /* EXTRACT KEYPOINT DESCRIPTORS */

                //// STUDENT ASSIGNMENT
                //// TASK MP.4 -> add the following descriptors in file matching2D.cpp and enable string-based selection based on descriptorType
                //// -> BRIEF, ORB, FREAK, AKAZE, SIFT

                cv::Mat descriptors;
                //string descriptorType = "BRISK"; // BRIEF, ORB, FREAK, AKAZE, SIFT
                descKeypoints((dataBuffer.end() - 1)->keypoints, (dataBuffer.end() - 1)->cameraImg, descriptors, descriptorType);
                //// EOF STUDENT ASSIGNMENT

                // push descriptors for current frame to end of data buffer
                (dataBuffer.end() - 1)->descriptors = descriptors;

                //"#3 : EXTRACT DESCRIPTORS done" << endl;

                if (dataBuffer.size() > 1) // wait until at least two images have been processed
                {

                    /* MATCH KEYPOINT DESCRIPTORS */

                    vector<cv::DMatch> matches;

                    //// STUDENT ASSIGNMENT
                    //// TASK MP.5 -> add FLANN matching in file matching2D.cpp
                    vector<string> matcherTypes = {"MAT_BF", "MAT_FLANN"};
                    vector<string> descriptorTypes = {"DES_BINARY", "DES_HOG"};
                    vector<string> selectorTypes = {"SEL_NN", "SEL_KNN"};
                    string matcherType = matcherTypes.at(0);
                    string descriptorType1 = (descriptorType.compare("SIFT") == 0?descriptorTypes.at(1):descriptorTypes.at(0));
                    string selectorType = selectorTypes.at(1);
                    //// TASK MP.6 -> add KNN match selection and perform descriptor distance ratio filtering with t=0.8 in file matching2D.cpp

                    matchDescriptors((dataBuffer.end() - 2)->keypoints, (dataBuffer.end() - 1)->keypoints,
                                    (dataBuffer.end() - 2)->descriptors, (dataBuffer.end() - 1)->descriptors,
                                    matches, descriptorType1, matcherType, selectorType);

                    //// EOF STUDENT ASSIGNMENT

                    // store matches in current data frame
                    (dataBuffer.end() - 1)->kptMatches = matches;

                    //cout << "#4 : MATCH KEYPOINT DESCRIPTORS done" << endl;

                    // visualize matches between current and previous image
                    bVis = bVisMatchesGlobal;
                    if (bVis)
                    {
                        cv::Mat matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
                        cv::drawMatches((dataBuffer.end() - 2)->cameraImg, (dataBuffer.end() - 2)->keypoints,
                                        (dataBuffer.end() - 1)->cameraImg, (dataBuffer.end() - 1)->keypoints,
                                        matches, matchImg,
                                        cv::Scalar::all(-1), cv::Scalar::all(-1),
                                        vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

                        string windowName = detectorType + "/" + descriptorType + " matching keypoints";
                        cv::namedWindow(windowName, 7);
                        cv::imshow(windowName, matchImg);
                        cout << "Press key to continue to next image" << endl;
                        cv::waitKey(0); // wait for key to be pressed
                    }
                    bVis = false;
                }
                else
                {
                    cout << "|" << endl;
                }

            } // eof loop over all images
        }
    }

    return 0;
}
