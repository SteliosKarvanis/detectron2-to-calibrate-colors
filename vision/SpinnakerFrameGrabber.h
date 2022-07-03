//
// Created by renan on 03/07/22.
//

#ifndef CT_EXAME_SpinnakerFrameGrabber_H
#define CT_EXAME_SpinnakerFrameGrabber_H

#include "/opt/spinnaker/include/Spinnaker.h"
#include <opencv2/opencv.hpp>

using namespace Spinnaker;

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
    void setNodeEnum(const GenApi::INodeMap &nodeMap, const GenICam::gcstring &nodeLabel, const GenICam::gcstring &value);

private:
    bool successCommunicationWithCamera;
    SystemPtr pSystem;
    CameraPtr pCam;
    ImagePtr image;
    int cols;
    int rows;

    void checkExpection(Exception e);
    void printCameraInfo(const CameraPtr& pCam);
};

#endif //CT_EXAME_SpinnakerFrameGrabber_H
