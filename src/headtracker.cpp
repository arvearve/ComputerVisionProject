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


void FaceTracker::track() {
    if(gpu::getCudaEnabledDeviceCount() > 0) {
        gpuCap();
    }
    else {
        printf("Could not find any CUDA enabled devices. Resorting to CPU face tracking.\n");
        cpuCap();
    }
}

/* 
 * Face tracking using the GPU.
 * Not implemented properly yet,
 * Just some example code from documentation.
 */
void FaceTracker::gpuCap(){
    gpu::CascadeClassifier_GPU cascade_gpu("data/haarcascades/haarcascade_eye.xml");
    Mat image_cpu;

    if(!webcam.isOpened())  // Make sure camera works properly
        return;

    while (true) {
        webcam >> image_cpu;
        gpu::GpuMat image_gpu(image_cpu);
        gpu::GpuMat objbuf;

        int detections_number = cascade_gpu.detectMultiScale( image_gpu, objbuf);

        Mat obj_host;
        // download only detected number of rectangles
        objbuf.colRange(0, detections_number).download(obj_host);

        Rect* faces = obj_host.ptr<Rect>();
        for(int i = 0; i < detections_number; ++i) {
            cv::rectangle(image_cpu, faces[i], Scalar(255));
        }

        imshow("Faces", image_cpu);
        if(waitKey(1) >= 0) {
            break;
        }
    }
}

void FaceTracker::cpuCap(){
    if(!webcam.isOpened()) { return; } // check if we succeeded

    CascadeClassifier eye_cascade;
    if(!eye_cascade.load("data/haarcascades/haarcascade_eye.xml")){
        printf("Error: Could not load cascades for eye detection.");
    }
    Mat frame;

    while(true) {
        auto start = std::chrono::system_clock::now();
        webcam >> frame;
        vector<Rect> eyes;
        vector<Point> centerpoints;
        float scaleFactor = 1.1;
        int minNeighbours = 6;
        eye_cascade.detectMultiScale(frame, eyes, scaleFactor, minNeighbours);

        /* Draw a gray circle at each detected eye, and note its centerpoint.
         *
         * Potential improvement: We could detect a face as well and only detect eyes inside the face.
         * But currently this is too slow.
         */
        for (auto eye : eyes) {
            Point centerpoint(eye.x + eye.width/2, eye.y + eye.height/2);
            centerpoints.push_back(centerpoint);
            circle(frame, centerpoint, 5, Scalar(128,128,128));
        }

        /* We need at least two eyes to determine head center 
         * Often more than 2 eyes are detected. In that case we just use the first two (at the moment).
         */
        if (centerpoints.size()>1) {
            Point left_eye = centerpoints[0];  // Wild guess
            Point right_eye = centerpoints[1]; // Wild guess

            // Midpoint between the eyes.
            Point midPoint = Point((left_eye.x + right_eye.x)/2, (left_eye.y + right_eye.y)/2);
            int distance = (int)euclideanDist(left_eye, right_eye);

            /* At this point we save the detected camera position.
             * OpenGL should use this value to position the camera in the scene.
             */
            camera = Point3i(midPoint.x, midPoint.y, distance);

            // Draw a green crossed circle representing 3d camera position.
            circle(frame, midPoint, distance/2, Scalar(0,255,0));
            line(frame, midPoint-Point(distance/2, 0), midPoint+Point(distance/2, 0), Scalar(0,2550));
            line(frame, midPoint-Point(0, distance/2), midPoint+Point(0, distance/2), Scalar(0,2550));
        }

        // Draw fps counter
        auto end = std::chrono::system_clock::now();
        ms duration = std::chrono::duration_cast<ms>(end - start);
        string duration_string = std::to_string(1000/ (duration.count()+1)) + "fps";
        putText(frame, duration_string, Point(0,12), CV_FONT_HERSHEY_PLAIN, 1.1 , Scalar(255,255,255));

        imshow("face", frame);
        if(waitKey(1) >= 0) {
            break;
        }
    }
}


int main(int argc, char** argv) {
    FaceTracker ft = FaceTracker();
    ft.track();
    return 0;
}
