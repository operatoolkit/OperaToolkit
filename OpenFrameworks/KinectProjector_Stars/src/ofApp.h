#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxOpenNI.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxBox2d.h"


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

    struct IdCircle
    {
        ofPtr<ofxBox2dCircle> circle;
        int idx;
        IdCircle(ofPtr<ofxBox2dCircle> circle, int idx);
    };
    
    void addImage(string path);
    void addCircle(int index);
    void spurt(int index, int amount);

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
    ofxBox2d box2d;
    vector<IdCircle*> circles;
    vector<ofPolyline> lines;
    vector<ofPtr<ofxBox2dEdge> > edges;
    vector<ofImage> img;
    bool hasSpurted;
    float spurtHack;

    ofParameter<bool> useUserImage;
    ofParameter<float> nearThreshold;
    ofParameter<float> farThreshold;
    ofParameter<float> minArea;
    ofParameter<float> maxArea;
    ofParameter<int> smoothness;
    ofParameter<ofColor> contourColor;
    ofParameter<int> rate;
    ofParameter<bool> isFalling;
    ofParameter<float> tolerance;
    ofParameter<float> circleDensity;
    ofParameter<float> circleBounce;
    ofParameter<float> circleFriction;
};
