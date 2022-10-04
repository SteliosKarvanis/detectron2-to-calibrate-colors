//
// Created by renan on 03/07/22.
//

#ifndef CT_EXAME_SpinnakerFrameGrabber_H
#define CT_EXAME_SpinnakerFrameGrabber_H

#include "Spinnaker.h"
#include <opencv2/opencv.hpp>

enum PixelFormat {
    YUYV = 0,
    RGB,
    SEGMENTED,
    UYV,
    UYVY,
    HSV
};

class SpinnakerFrameGrabber{
public:
    SpinnakerFrameGrabber(bool verbose = true, unsigned int camIndex = 0);
    ~SpinnakerFrameGrabber();

    bool check();
    bool connectWithCamera(int cols, int rows, bool verbose = true, unsigned int camIndex = 0);
    bool startCapturing();
    int getImageHeight();
    int getImageWidth();
    unsigned char *grabFrame();
    unsigned int checkNumberOfCameras();
    void blockConnection();
    void setFrameRate(float desiredFrameRate);
    void setImageSize(int width, int height);
    void setPixelFormat(PixelFormat pixelFormat);
    void useBuffer(int numBuffers);
    void setNodeEnum(const Spinnaker::GenApi::INodeMap &nodeMap, const Spinnaker::GenICam::gcstring &nodeLabel, const Spinnaker::GenICam::gcstring &value);

private:
    bool successCommunicationWithCamera;
    Spinnaker::SystemPtr pSystem;
    Spinnaker::CameraPtr pCam;
    Spinnaker::ImagePtr image;
    int cols;
    int rows;

    void checkExpection(Spinnaker::Exception e);
    void printCameraInfo(const Spinnaker::CameraPtr& pCam);
};

#endif //CT_EXAME_SpinnakerFrameGrabber_H
