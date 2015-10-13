#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxOpenNI.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkit.h"

//  display resolution of your projector (default)
#define PROJECTOR_RESOLUTION_X 1280
#define PROJECTOR_RESOLUTION_Y 800
#define DEBUG_MODE true

using namespace ofxCv;
using namespace cv;


struct Contour {
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


class TV {
public:
    TV(int x, int y, int w, int h);
    void update();
    bool setActive(bool active);
    void draw();

    int x, y, w, h;
    bool active, pActive;
    float alph;
    ofColor color;
};


class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void exit();
    
private:
    void setupTVs();

    void eventToProject(bool & b);
    void eventResizeProjector(int & v);
    void eventResizeTVs(int & v);

    void loadPreset();
    void savePreset();

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
    
    // app objects
    map<int, Contour*> users;
    map<int, TV*> tvs;
    int nx, ny;
    
    // parameters, gui
    ofParameter<int> projectorWidth;
    ofParameter<int> projectorHeight;
    ofParameter<bool> toProject;

    ofParameter<bool> useUserImage;
    ofParameter<float> nearThreshold;
    ofParameter<float> farThreshold;
    ofParameter<float> minArea;
    ofParameter<float> maxArea;
    ofParameter<int> smoothness;
    
    ofParameter<int> tvWidth;
    ofParameter<int> tvHeight;
};
