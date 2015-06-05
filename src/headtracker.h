//
//  facetracker.h
//  NygaardHW3
//
//  Created by Arve Nygård on 10/05/15.
//  Copyright (c) 2015 Arve Nygård. All rights reserved.
//

#ifndef __NygaardHW3__facetracker__
#define __NygaardHW3__facetracker__

#include <stdio.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/gpu/gpumat.hpp>
#include <thread>

using namespace cv;
class HeadTracker {
public:

    Point2i detectedPosition; // Position of face (between eyes)
    Point2f normalizedPosition;
    int detectedDistance;
    int fps;
    HeadTracker():detectedPosition(Point2i(0,0)), detectedDistance(0) {
        webcam = VideoCapture(0);
        // To increase camera FPS and processing speed, capture a small image.
        webcam.set(CV_CAP_PROP_FRAME_WIDTH, 320);
        webcam.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
        webcam.set(CV_CAP_PROP_CONVERT_RGB, false); // We will convert to grayscale anyway.
        output = Mat::zeros(240, 320, CV_8UC3);
        processThread = std::thread(&HeadTracker::track, this);
    };
    ~HeadTracker();
    void track();
    void stopTrack();
    Mat output;
private:
    std::thread processThread;
    void gpuTrack();
    void cpuTrack();
    VideoCapture webcam;
    bool keepTracking;
    /* GPU */

};
#endif /* defined(__NygaardHW3__facetracker__) */
