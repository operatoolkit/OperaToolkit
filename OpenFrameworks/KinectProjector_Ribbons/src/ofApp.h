#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxOpenNI.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkit.h"

#include "ContourTracker.h"
#include "Ribbons.h"


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
    
    map<Contour*, vector<Ribbon*> > ribbons;
    map<int, Contour*> users;
    ofParameter<int> maxAgeMin, maxAgeMax;
    ofParameter<int> speedMin, speedMax;
    ofParameter<int> lengthMin, lengthMax;
    ofParameter<int> skipMin, skipMax;
    ofParameter<int> marginMin, marginMax;
    ofParameter<float> noiseFactorMin, noiseFactorMax;
    ofParameter<float> ageFactorMin, ageFactorMax;
    ofParameter<float> lineWidthMin, lineWidthMax;
    ofParameter<int> maxAlphaMin, maxAlphaMax;
    ofParameter<float> lerpRateMin, lerpRateMax;
    ofParameter<int> colorMarginMin, colorMarginMax;
    ofParameter<int> updateRateMin, updateRateMax;
    ofParameter<float> dilate;
    ofParameter<bool> curved, match;
    ofParameter<int> numNew;
    ofParameter<int> frameSkip;
};
