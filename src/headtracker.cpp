//
//  facetracker.cpp
//  NygaardHW3
//
//  Created by Arve Nygård on 10/05/15.
//  Copyright (c) 2015 Arve Nygård. All rights reserved.
//

#include "headtracker.h"

typedef std::chrono::milliseconds ms;

float euclideanDist(Point& p, Point& q) {
    Point diff = p - q;
    return (int)sqrt(diff.x*diff.x + diff.y*diff.y);
}

HeadTracker::~HeadTracker(){
    keepTracking = false;
    processThread.join();
    printf("asked to stop tracking\n");
}


void HeadTracker::track() {
    keepTracking = true;
    if(!webcam.isOpened()) { // Make sure camera works properly
        return;
    }
    if(gpu::getCudaEnabledDeviceCount() > 0) {
        printf("Found CUDA enabled devices. Using GPU for face tracking!\n");
        gpuTrack();
    }
    else {
        printf("Could not find any CUDA enabled devices. Resorting to CPU face tracking.\n");
        cpuTrack();
    }
}

/* 
 * Face tracking using the GPU.
 */
void HeadTracker::gpuTrack(){
    gpu::CascadeClassifier_GPU face_cascade("data/lbpcascades/lbpcascade_frontalface.xml");
    Mat frame_gray, frame;

    while (keepTracking) {
        auto start = std::chrono::system_clock::now();
        webcam >> frame;
        cvtColor(frame, frame_gray, CV_BGR2GRAY);  // convert to gray image as face detection does NOT use color info
        equalizeHist(frame_gray,frame_gray);
        gpu::GpuMat gray_gpu(frame_gray);  // copy the gray image to GPU memory
        gpu::GpuMat facebuf;
        float scaleFactor = 1.2;
        int minNeighbours = 6;
        int face_count = face_cascade.detectMultiScale(gray_gpu, facebuf, scaleFactor, minNeighbours);

        Mat obj_host;
        // download detected number of rectangles from the GPU
        facebuf.colRange(0, face_count).download(obj_host);
        Rect* faces_gpu = obj_host.ptr<Rect>();

        /* Loop through faces.
         * Currently we just use the last face.
         * Ideally, we should do some analysis to decide what position
         * to use when multiple faces are detected. (Biggest one?)
         */
        for(int i = 0; i < face_count; ++i) {
            Rect face = faces_gpu[i];
            Point centerpoint(face.x + face.width/2, face.y + face.height/2);
            detectedPosition = Point2i(centerpoint.x, centerpoint.y);
        }
        frame.copyTo(output);
        auto end = std::chrono::system_clock::now();
        ms duration = std::chrono::duration_cast<ms>(end - start);
        fps = (duration.count() == 0)?0:(1000/duration.count());
    }
    printf("finished tracking\n");
}

void HeadTracker::cpuTrack(){
    CascadeClassifier eye_cascade;
    if(!eye_cascade.load("data/lbpcascades/lbpcascade_frontalface.xml")){
        printf("Error: Could not load cascades for face detection.");
    }
    Mat frame;

    while(keepTracking) {
        auto start = std::chrono::system_clock::now();
        webcam >> frame;
        vector<Rect> faces;
        vector<Point> centerpoints;
        float scaleFactor = 1.1;
        int minNeighbours = 6;
        eye_cascade.detectMultiScale(frame, faces, scaleFactor, minNeighbours);

        /*
         * Grab the last face (because reasons)
         */
        if (faces.size() > 0) {
            Rect face = faces[0];
            Point centerpoint(face.x + face.width/2, face.y + face.height/2);
            detectedPosition = Point2i(centerpoint.x, centerpoint.y);
        }

        frame.copyTo(output);

        // Draw fps counter
        auto end = std::chrono::system_clock::now();
        ms duration = std::chrono::duration_cast<ms>(end - start);
        fps = (duration.count() == 0)?0:(1000/duration.count());
    }
    printf("finished tracking\n");
}
