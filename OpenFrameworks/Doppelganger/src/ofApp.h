#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxLayerMask.h"
#include "ofxSecondWindow.h"


class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    
    ofxSecondWindow projector;
    
    ofxKinect kinect;
    
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayThreshNear;
    ofxCvGrayscaleImage grayThreshFar;
    
    ofxLayerMask masker;
    vector<ofFbo> fbo;
    
    ofParameter<bool> debug;
    ofParameter<bool> recording;
    ofParameter<int> totalNumImages;
    ofParameter<int> nearThreshold;
    ofParameter<int> farThreshold;
    ofParameter<bool> oscillate;
    ofParameter<int> delay;
    ofParameter<ofVec2f> translate;
    ofParameter<ofVec2f> scale;
    
    ofxPanel gui;
    
    int indexImage;
    int indexDraw;
};
