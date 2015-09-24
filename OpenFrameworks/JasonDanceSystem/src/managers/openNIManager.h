//
//  openNIManager.h
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-02.
//
//
#pragma once

#include "ofxOpenNI.h"
#include "ofxCvGrayscaleImage.h"
#include "ofMain.h"

class openNIManager {
public:
    void setup();
	void update();
	void draw(float x, float y, float scale);
    void exit();
    
    bool isNewFrame();
    bool isFound();
    bool isSkeleton();
    void setDepthThresh();
    
    ofVec3f getLeftElbowPos() { return leftElbowPos; }
    ofVec3f getLeftHandPos() { return leftHandPos; }
    

	void keyPressed(int key);
    void userEvent(ofxOpenNIUserEvent & event);
    
	ofxOpenNI openNIDevice;
    
    ofTrueTypeFont verdana;
    
    ofPixels maskPix;
    
    bool bUseDepth;
    
    int nearThresh, farThresh;
    
    ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
    
    //center
    
    ofVec3f headPos, neckPos, torsoPos;
    
    //upper body
    ofVec3f leftShoulderPos, rightShoulderPos;
    
    ofVec3f leftElbowPos, rightElbowPos;
    
    ofVec3f leftHandPos, rightHandPos;
    
    //lower body
    
    ofVec3f leftHipPos, rightHipPos;
    
    ofVec3f leftKneePos, rightKneePos;
    
    ofVec3f leftFootPos, rightFootPos;

    
};
