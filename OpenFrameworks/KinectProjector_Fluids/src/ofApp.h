#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxOpenNI.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxFluid.h"


// this must match the display resolution of your projector
#define PROJECTOR_RESOLUTION_X 1280
#define PROJECTOR_RESOLUTION_Y 1024

using namespace ofxCv;
using namespace cv;


struct Contour
{
    Contour(vector<ofVec2f> & points, ofPoint center, int label);
    void setPoints(vector<ofVec2f> & points, ofPoint center);
    void draw();
    int getNumPoints();
    
    vector<ofVec2f> points;
    ofPoint center;
    int label;
    int age;
    ofColor color;
};


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
    ofxCvGrayscaleImage grayThreshNear;
    ofxCvGrayscaleImage grayThreshFar;
    ofxCv::ContourFinder contourFinder;
    bool hadUsers;
    
    map<int, Contour*> users;
    ofxFluid fluid;
    vector<vector<ofVec2f> > pContourPoints;
    vector<vector<ofVec2f> > displace;
    ofParameter<float> simplify;
    ofParameter<float> displaceLerp;
    ofParameter<int> numContourPts;
    ofParameter<int> skip;
    ofParameter<int> maxUsers;
    ofParameter<float> dissipation, velDissipation;
    ofParameter<float> displacement;
    ofParameter<float> strength;
    ofParameter<float> gravityX, gravityY;
    ofParameter<bool> useUserImage;
    ofParameter<float> nearThreshold;
    ofParameter<float> farThreshold;
    ofParameter<float> minArea;
    ofParameter<float> maxArea;
    ofParameter<int> smoothness;
};
