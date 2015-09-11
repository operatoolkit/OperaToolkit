#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxOpenNI.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkit.h"

#include "ContourTracker.h"


// this must match the display resolution of your projector
#define PROJECTOR_RESOLUTION_X 1280
#define PROJECTOR_RESOLUTION_Y 800

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    
private:
    
    void getCalibratedContour(ofShortPixels & depthPixels, vector<cv::Point> & points, vector<ofVec2f> & calibratedPoints);
    void clean();
    
    ofxSecondWindow projector;
    ofxPanel gui;
    
    // kinect
    ofxOpenNI kinect;
    ofxKinectProjectorToolkit kpt;
    ofShortPixels depthPixels;
    ofxCvGrayscaleImage grayImage;
    ofxCv::ContourFinder contourFinder;
    bool hadUsers;
    
    ofParameter<float> minArea;
    ofParameter<float> maxArea;
    ofParameter<int> smoothness;
    
    // bg lines
    ofParameter<int> mode;
    ofParameter<float> refreshAlpha;
    ofParameter<ofColor> color;
    ofParameter<int> skip;
    ofParameter<int> numPoints;
    ofParameter<bool> horiz;
    ofParameter<float> mult;
    ofParameter<float> offset;
    ofParameter<float> strokeWeight;
    ofParameter<float> alpha;
    
    vector<ofPoint> points;
    float lerpRate;

};
