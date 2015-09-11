#pragma once

#include "ofMain.h"

#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxOpenNI.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkit.h"


using namespace ofxCv;
using namespace cv;


class ContourTracker {
public:
    void setup();
    void update();
    
    bool hadUsers;
    
    // kinect
    ofxOpenNI kinect;
    ofxCv::ContourFinder contourFinder;
    ofxKinectProjectorToolkit kpt;
    ofShortPixels depthPixels;
    
    ofxCvGrayscaleImage bgImage;
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayThreshNear;
    ofxCvGrayscaleImage grayThreshFar;
    
    ofParameter<float> nearThreshold;
    ofParameter<float> farThreshold;
    ofParameter<float> minArea;
    ofParameter<float> maxArea;
    ofParameter<int> smoothness;
    ofParameter<float> threshold;
    ofParameter<float> persistence;
    ofParameter<float> maxDistance;
    
};