Head tracking virtual reality using single camera
=================================================

We aim to use a devices webcamera to track the user's head and position
the camera in a 3d scene correspondingly. This gives the illusion of looking
into a real 3d scene "inside" the display.

Inspired by Johhny Chung Lee's [work](https://www.youtube.com/watch?v=Jd3-eiid-Uw) with the Nintendo Wii.


Requirements
------------
* cmake, make, c++11 compatible compiler
* CUDA
* OpenCV compiled with CUDA and OpenGL support.

Building & Running
------------------
```bash
$ cd build
$ cmake .. && make
$ ./headtracker
```
