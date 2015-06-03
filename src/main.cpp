//
//  glmain.cpp
//  project
//
//  Created by Arve Nygård on 15/05/15.
//  Copyright (c) 2015 Arve Nygård. All rights reserved.
//

#include "main.h"
#include "headtracker.h"
#include <thread>
#define FLIP_HORIZONTAL 1

Mat drawParallax(const Point2i headPosition);
Mat backgroundImage = imread("data/landscape.jpg");

/* Draws tracking debug info on an image.
 *
 * Draws FPS counter
 * Draws a cross on the detected location
 * Mirrors the image
 */
Mat drawStats(const FaceTracker &ft, Mat &frame){
    Point2i detectedPosition = Point(ft.detectedPosition);
    string duration_string = std::to_string(ft.fps) + "fps";
    line(frame, detectedPosition-Point(5, 0), detectedPosition+Point(5, 0), Scalar(0,2550));
    line(frame, detectedPosition-Point(0, 5), detectedPosition+Point(0, 5), Scalar(0,2550));
    Mat result;
    flip(frame, result, FLIP_HORIZONTAL);
    putText(result, duration_string, Point(0,12), CV_FONT_HERSHEY_PLAIN, 1.1 , Scalar(255,255,255));
    return result;
}


int main(int argc, char** argv) {
    FaceTracker ft = FaceTracker();
    namedWindow("debug");
    namedWindow("scene");
    /* Start a thread and continuously run face tracking */
    std::thread trackerThread(&FaceTracker::track, &ft);

    /* Read tracked values, update display */
    while(true){
        Mat cameraView = drawStats(ft, ft.output);
        imshow("debug", cameraView);
        imshow("scene", drawParallax(ft.detectedPosition));
        moveWindow("scene", 750, 0);
        if(waitKey(1) >= 0) {
            ft.stopTrack();
            break;
        }
    }

    trackerThread.join(); // Let the thread clean up before we exit.
    return 0;
}


/* Parallax effect based on head position
 * Returns a region of backgroundImage.
 * The region size is 640*480
 * The regions position is inversely related to the user's head position.
 */
Mat drawParallax(Point2i headPosition){
    float scaleFactor = 0.5;
    headPosition.x = backgroundImage.cols/2 + headPosition.x*scaleFactor;
    headPosition.y = backgroundImage.rows/2 + headPosition.y*scaleFactor;
    Point2i offsetX = Point2i(320, 0), offsetY = Point2i(0, 240);
    Point2i topLeft     = headPosition - offsetX - offsetY,
            bottomRight = headPosition + offsetX + offsetY;
    Rect ROI(topLeft, bottomRight);
    return backgroundImage(ROI);
}