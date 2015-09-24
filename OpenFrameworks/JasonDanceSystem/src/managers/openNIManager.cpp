//
//  openNIManager.cpp
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-02.
//
//

#include "openNIManager.h"

//--------------------------------------------------------------
void openNIManager::setup() {
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
//    openNIDevice.setup();
    openNIDevice.setupFromONI("/Users/gene/Code/of_v0.8-4.4_osx_release/templates_old/Kinect/openni_oniRecording/bin/data/alecsroom.oni");
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.setDepthColoring(COLORING_GREY);
    openNIDevice.setRegister(false);
    openNIDevice.setMirror(false);
    openNIDevice.addUserGenerator();
    openNIDevice.setMaxNumUsers(2);
//    depthThresh.set(255, 70);
//    depthThresh.setUseDepthPixels(true);
//    openNIDevice.addDepthThreshold(depthThresh);
    openNIDevice.start();
    
    
    openNIDevice.setUseMaskPixelsAllUsers(true);
    openNIDevice.setUsePointCloudsAllUsers(false);
    
//    grayImage.allocate(640, 480);
//	grayThreshNear.allocate(640, 480);
//	grayThreshFar.allocate(640, 480);
    
    nearThresh = 255;
    farThresh = 20;
    
    
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
 
    maskPix.allocate(640, 480, 1);
    
    bUseDepth = true;
}

//--------------------------------------------------------------
void openNIManager::update(){
    openNIDevice.update();

    if (bUseDepth) {
        if (openNIDevice.isNewFrame()) {
            maskPix = openNIDevice.getDepthPixels();
            maskPix.setNumChannels(1);
            maskPix.resize(320, 240);
            
			for(int i = 0; i < maskPix.size(); i++) {
				if(maskPix[i] < nearThresh && maskPix[i] > farThresh) {
					maskPix[i] = 255;
				} else {
					maskPix[i] = 0;
				}
			}
        }
    }
    else {
        // get number of current users
        int numUsers = openNIDevice.getNumTrackedUsers();
        
        // iterate through users
        for (int i = 0; i < numUsers; i++){
            
            // get a reference to this user
            ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
            
            // draw the mask texture for this user
            //        user.drawMask();
            
            // PIXEL REFERENCE
            if (user.isFound()) {
                maskPix = user.getMaskPixels();
                maskPix.resize(320, 240);
            }

//            if (user.isSkeleton()) {
                headPos = user.getJoint(JOINT_HEAD).getProjectivePosition();
                neckPos = user.getJoint(JOINT_NECK).getProjectivePosition();
                torsoPos = user.getJoint(JOINT_TORSO).getProjectivePosition();
                leftShoulderPos = user.getJoint(JOINT_LEFT_SHOULDER).getProjectivePosition();
                rightShoulderPos = user.getJoint(JOINT_RIGHT_SHOULDER).getProjectivePosition();
                leftHipPos = user.getJoint(JOINT_LEFT_HIP).getProjectivePosition();
                rightHipPos = user.getJoint(JOINT_RIGHT_HIP).getProjectivePosition();
                leftKneePos = user.getJoint(JOINT_LEFT_KNEE).getProjectivePosition();
                rightKneePos = user.getJoint(JOINT_RIGHT_KNEE).getProjectivePosition();
                leftFootPos = user.getJoint(JOINT_LEFT_FOOT).getProjectivePosition();
                rightFootPos = user.getJoint(JOINT_RIGHT_FOOT).getProjectivePosition();
                leftElbowPos = user.getJoint(JOINT_LEFT_ELBOW).getProjectivePosition();
                leftHandPos = user.getJoint(JOINT_LEFT_HAND).getProjectivePosition();
                rightElbowPos = user.getJoint(JOINT_RIGHT_ELBOW).getProjectivePosition();
                rightHandPos = user.getJoint(JOINT_RIGHT_HAND).getProjectivePosition();
                
//            }

        }
    }
}

//--------------------------------------------------------------
void openNIManager::draw(float x, float y, float scale){

    
    ofSetColor(255, 255, 255);
    
    ofPushMatrix();
    ofTranslate(x, y);
    ofTranslate(160, 120);
    ofScale(scale, scale);
    ofTranslate(-160, -120);
    ofImage maskImg = maskPix;
    maskImg.draw(0, 0);
    ofPopMatrix();


    // draw some info regarding frame counts etc
	ofSetColor(0, 255, 0);
	string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate()) + " Device FPS: " + ofToString(openNIDevice.getFrameRate());
    
    string thresholds = "NEAR = " + ofToString(nearThresh) + " FAR = " + ofToString(farThresh);
    
	verdana.drawString(msg, 20, openNIDevice.getNumDevices() * 480 - 20);
    verdana.drawString(thresholds, 20, 500);
}

//--------------------------------------------------------------
void openNIManager::userEvent(ofxOpenNIUserEvent & event){
    // show user event messages in the console
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void openNIManager::exit(){
    openNIDevice.stop();
}

//--------------------------------------------------------------
bool openNIManager::isNewFrame() {
    return openNIDevice.isNewFrame();
}

//--------------------------------------------------------------
bool openNIManager::isFound() {
    int numUsers = openNIDevice.getNumTrackedUsers();
    bool userFound = false;
    // iterate through users
    for (int i = 0; i < numUsers; i++){
        ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
        userFound |= user.isFound();
    }
    
    return userFound;
}

//--------------------------------------------------------------
bool openNIManager::isSkeleton() {
    int numUsers = openNIDevice.getNumTrackedUsers();
    bool userSkeleton = false;
    // iterate through users
    for (int i = 0; i < numUsers; i++){
        ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
        userSkeleton |= user.isSkeleton();
    }
    
    return userSkeleton;
}



//--------------------------------------------------------------
void openNIManager::keyPressed(int key){
    switch (key) {
        case 'R':
        {
            int numUsers = openNIDevice.getNumTrackedUsers();
            
            // iterate through users
            for (int i = 0; i < numUsers; i++){
                openNIDevice.resetUserTracking(i, true);
            }
            break;
        }
            
            
        case ',':
            if (farThresh > 0)farThresh--;
            break;
            
        case '.':
            if (farThresh < 255) farThresh++;
            break;
            
        case 'l':
            if (nearThresh > 0) nearThresh--;
            break;
            
        case ';':
            if (nearThresh < 255) nearThresh++;
            break;
            
        case 'D':
            bUseDepth = !bUseDepth;
            break;
  
    }
}
