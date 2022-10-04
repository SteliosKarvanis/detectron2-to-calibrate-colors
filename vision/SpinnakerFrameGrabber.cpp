//
// Created by renan on 03/07/22.
//

#include <iostream>
#include "SpinnakerFrameGrabber.h"

#define MAX_WIDTH 656
#define MAX_HEIGHT 516

using namespace Spinnaker;

SpinnakerFrameGrabber::SpinnakerFrameGrabber(bool verbose, unsigned int camIndex) {
    this->connectWithCamera(MAX_WIDTH, MAX_HEIGHT, verbose, camIndex);
}

SpinnakerFrameGrabber::~SpinnakerFrameGrabber() {
    pCam->EndAcquisition();
    pCam->DeInit();
}

bool SpinnakerFrameGrabber::check() {
    return successCommunicationWithCamera;
}

bool SpinnakerFrameGrabber::connectWithCamera(int cols, int rows, bool verbose, unsigned int camIndex) {
    pSystem = System::GetInstance();
    CameraList camList = pSystem->GetCameras();
    this->cols = cols;
    this->rows = rows;

    unsigned int numCameras = camList.GetSize();
    if (verbose)
        std::cout << "Spinnaker: Number of cams: " << numCameras << "\n";

    if (numCameras == 0) {
        successCommunicationWithCamera = false;
        if (verbose) {
            std::cout << "PointGreyFrameGrabber:" << std::endl;
            std::cout << "\tNo Point Grey camera detected" << std::endl;
        }
    } else {
        successCommunicationWithCamera = true;

        try{
            pCam = camList.GetByIndex(camIndex);

            if (verbose)
                printCameraInfo(pCam);
        }
        catch (Spinnaker::Exception &e) {
            checkExpection(e);
            std::cout << "Get pCam by index\n";
            camList.Clear();
            pSystem->ReleaseInstance();
            return false;
        }

        try{
            pCam->Init();
            camList.Clear();
        }
        catch (Spinnaker::Exception &e) {
            checkExpection(e);
            std::cout << "pCam Init\n";
            camList.Clear();
            pSystem->ReleaseInstance();
            return false;
        }

        try {
            if (verbose)
                fprintf(stderr, "Opened %s - %s\n", pCam->DeviceModelName.GetValue().c_str(), pCam->DeviceSerialNumber.GetValue().c_str());
            pCam->AcquisitionMode.SetValue(Spinnaker::AcquisitionMode_Continuous);
        }
        catch (Spinnaker::Exception &e) {
            checkExpection(e);
            std::cout << "Error in Acquisition\n";
            camList.Clear();
            pSystem->ReleaseInstance();
            return false;
        }

        setImageSize(cols, rows);
        setPixelFormat(PixelFormat::UYVY);
        setFrameRate(60);

        try {
            Spinnaker::GenApi::INodeMap& snodeMap = pCam->GetTLStreamNodeMap();
            Spinnaker::GenApi::CEnumerationPtr ptrBufferHandlingMode = snodeMap.GetNode("StreamBufferHandlingMode");

            Spinnaker::GenApi::CEnumEntryPtr ptrBufferHandlingModeNewest = ptrBufferHandlingMode->GetEntryByName("NewestFirst");
            int64_t bufferHandlingModeNewest = ptrBufferHandlingModeNewest->GetValue();
            ptrBufferHandlingMode->SetIntValue(bufferHandlingModeNewest);
        }
        catch (Spinnaker::Exception &e) {
            checkExpection(e);
            std::cout << "Error in Buffer Handling\n";
            camList.Clear();
            pSystem->ReleaseInstance();
            return false;
        }

    }

    return successCommunicationWithCamera;
}

bool SpinnakerFrameGrabber::startCapturing() {
    try {
        pCam->BeginAcquisition();
        return true;
    }
    catch (Spinnaker::Exception &e) {
        checkExpection(e);
        std::cout << "Error in startCapturing\n";
        pSystem->ReleaseInstance();
        return false;
    }
}

int SpinnakerFrameGrabber::getImageWidth() {
//    Spinnaker::GenApi::INodeMap& snodeMap = pCam->GetNodeMap();
//    Spinnaker::GenApi::CIntegerPtr ptrWidth = snodeMap.GetNode("Width");
//
//    return ptrWidth->GetValue();
    return this->cols;
}

int SpinnakerFrameGrabber::getImageHeight() {
//    Spinnaker::GenApi::INodeMap& snodeMap = pCam->GetNodeMap();
//    Spinnaker::GenApi::CIntegerPtr ptrHeight = snodeMap.GetNode("Height");

//    return ptrHeight->GetValue()
    return this->rows;
}

unsigned char *SpinnakerFrameGrabber::grabFrame() {
    ImagePtr pResultImage;

    try {
        pResultImage = pCam->GetNextImage();
    }
    catch (Spinnaker::Exception &e) {
        checkExpection(e);
        std::cout << "setFramerate\n";
        pSystem->ReleaseInstance();
    }


    try {
        auto* data = (unsigned char*)pResultImage->GetData();
        return data;
    }
    catch (Spinnaker::Exception &e) {
        checkExpection(e);
        std::cout << "grabFrame\n";
        pSystem->ReleaseInstance();
    }

}

unsigned int SpinnakerFrameGrabber::checkNumberOfCameras() {
    return pSystem->GetCameras().GetSize();
}

void SpinnakerFrameGrabber::blockConnection() {
    this->successCommunicationWithCamera = false;
}

void SpinnakerFrameGrabber::setFrameRate(float desiredFrameRate) {
    Spinnaker::GenApi::INodeMap& nodeMap = pCam->GetNodeMap();
    Spinnaker::GenApi::CFloatPtr ptrAcquisitionFrameRate = nodeMap.GetNode("AcquisitionFrameRate");

    try {
        ptrAcquisitionFrameRate->SetValue(desiredFrameRate);
    }
    catch (Spinnaker::Exception &e) {
        checkExpection(e);
        std::cout << "setFramerate\n";
        pSystem->ReleaseInstance();
    }
}

void SpinnakerFrameGrabber::setImageSize(int width, int height) {
    Spinnaker::GenApi::INodeMap& nodeMap = pCam->GetNodeMap();

    try {
        Spinnaker::GenApi::CIntegerPtr ptrWidth = nodeMap.GetNode("Width");
        if (IsAvailable(ptrWidth) && IsWritable(ptrWidth))
        {
            int64_t widthInc = ptrWidth->GetInc();

            if (width % widthInc != 0)
            {
                width = (width / widthInc) * widthInc;
            }

            ptrWidth->SetValue(width);
        }
    }
    catch (Spinnaker::Exception &e) {
        checkExpection(e);
        std::cout << "width\n";
        pSystem->ReleaseInstance();
    }

    try {
        Spinnaker::GenApi::CIntegerPtr ptrHeight = nodeMap.GetNode("Height");
        if (IsAvailable(ptrHeight) && IsWritable(ptrHeight))
        {
            int64_t heightInc = ptrHeight->GetInc();

            if (height % heightInc != 0)
            {
                height = (height / heightInc) * heightInc;
            }

            ptrHeight->SetValue(height);
        }
        ptrHeight->SetValue(height);
    }
    catch (Spinnaker::Exception &e) {
        checkExpection(e);
        std::cout << "width\n";
        pSystem->ReleaseInstance();
    }
}

void SpinnakerFrameGrabber::setPixelFormat(PixelFormat pixelFormat) {
//    Some spinnaker pixel format is not working
    Spinnaker::GenApi::INodeMap& nodeMap = pCam->GetNodeMap();

//    GenApi::StringList_t symbolic;
//
//    pCam->PixelFormat.GetSymbolics(symbolic);
//
//    for (auto item : symbolic) {
//        std:: cout << item << " ";
//    }
//
//    std::cout << "\n";

    try {
        switch (pixelFormat) {
            case PixelFormat::UYV:
                setNodeEnum(nodeMap, "PixelFormat", "YCbCr8_CbYCr");
                break;
            case PixelFormat::UYVY:
                setNodeEnum(nodeMap, "PixelFormat", "YCbCr422_8_CbYCrY");
                break;
            case PixelFormat::RGB:
                setNodeEnum(nodeMap, "PixelFormat", "RGB8");
                break;
        }

    }
    catch (Spinnaker::Exception &e) {
        checkExpection(e);
        std::cout << "SetPixelFormat\n";
        pSystem->ReleaseInstance();
    }
}

void SpinnakerFrameGrabber::useBuffer(int numBuffers) {
    Spinnaker::GenApi::INodeMap& sNodeMap = pCam->GetTLStreamNodeMap();

    Spinnaker::GenApi::CIntegerPtr ptrBufferCount = sNodeMap.GetNode("StreamBufferCountManual");

    try {
        ptrBufferCount->SetValue(numBuffers);
    }
    catch (Spinnaker::Exception &e) {
        checkExpection(e);
        std::cout << "SetPixelFormat\n";
        pSystem->ReleaseInstance();
    }

}

void SpinnakerFrameGrabber::checkExpection(Exception e) {
    if (e.GetError() != SPINNAKER_ERR_SUCCESS) {
        std::cout << e.GetFullErrorMessage() << "\n";
    }
}

void SpinnakerFrameGrabber::setNodeEnum(const GenApi::INodeMap &nodeMap, const GenICam::gcstring &nodeLabel,
                                        const GenICam::gcstring &value) {
    Spinnaker::GenApi::CEnumerationPtr ptrNodeEnum = nodeMap.GetNode(nodeLabel.c_str());
    if (!GenApi::IsWritable(ptrNodeEnum)) {
        std::cout << "CameraFlir: error, unable to set " << nodeLabel << " to " << value << "\n";
        return;
    }

    Spinnaker::GenApi::CEnumEntryPtr ptrNodeEnumEntry = ptrNodeEnum->GetEntryByName(value.c_str());
    if (!GenApi::IsReadable(ptrNodeEnumEntry)) {
        std::cout << "CameraFlir: error, unable to set " << nodeLabel << " to " << value << ", not readable." << "\n";
        return;
    }

    const int64_t valueEntry = ptrNodeEnumEntry->GetValue();

    ptrNodeEnum->SetIntValue(valueEntry);
}

void SpinnakerFrameGrabber::printCameraInfo(const CameraPtr& pCam) {
    Spinnaker::GenApi::INodeMap &nodeMap = pCam->GetNodeMap();

    std::cout << "\n*** DEVICE INFORMATION ***\n\n";

    try {
        Spinnaker::GenApi::FeatureList_t features;
        Spinnaker::GenApi::CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
        if (IsAvailable(category) && IsReadable(category)) {
            category->GetFeatures(features);

            Spinnaker::GenApi::FeatureList_t::const_iterator it;
            for (it = features.begin(); it != features.end(); ++it) {
                Spinnaker::GenApi::CNodePtr pfeatureNode = *it;
                std::cout << pfeatureNode->GetName() << " : ";
                Spinnaker::GenApi::CValuePtr pValue = static_cast<Spinnaker::GenApi::CValuePtr>(pfeatureNode);
                std::cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable\n");
            }
        }
        else {
            std::cout << "Device control information not available.\n";
        }
    }
    catch (Spinnaker::Exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
}


