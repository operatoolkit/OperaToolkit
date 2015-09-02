#pragma once

#define STRINGIFY(x) #x

#include "ofMain.h"
#include "ofxMultiKinectV2.h"
#include "ofxGui.h"
#include "ofxSecondWindow.h"
#include "GpuRegistration.h"


class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxSecondWindow projector;
    
    ofxMultiKinectV2 kinect0;
    ofTexture colorTex0;
    ofTexture depthTex0;
    ofShader depthShader;
    GpuRegistration img;
    vector<ofFbo> fbo;
    
    ofParameter<int> appFrameSkip;
    ofParameter<bool> debug;
    ofParameter<bool> recording;
    ofParameter<int> totalNumImages;
    ofParameter<int> frameRate;
    ofParameter<int> numImages;
    ofParameter<int> frameSkip;
    ofParameter<int> imageSkip;
    ofParameter<float> depthThreshold;
    
    ofxPanel gui;

    int indexImage;
};
