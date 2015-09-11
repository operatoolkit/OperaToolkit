#include "ofApp.h"


void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetWindowShape(1280, 960);
    
    // set up kinect
    //kinect.setup();
    kinect.setupFromONI("/Users/gene/Code/of_v0.8-4.4_osx_release/templates_old/Kinect/openni_oniRecording/bin/data/alecsroom.oni");
    //kinect.setupFromONI("/Users/gene/Desktop/oni/2012-06-25-14-08-00-860.oni");
    kinect.addImageGenerator();
    kinect.addDepthGenerator();
    kinect.addUserGenerator();
    kinect.setMaxNumUsers(5);
    kinect.setUseMaskPixelsAllUsers(true);
    kinect.setMaskPixelFormatAllUsers(OF_PIXELS_MONO);
    kinect.setUseDepthRawPixels(true);
    kinect.setRegister(true);
    kinect.setMirror(false);
    kinect.start();
    
    grayImage.allocate(kinect.getWidth(), kinect.getHeight());
    
    // setup calibration
    kpt.loadCalibration("/Users/Gene/Desktop/calibration.xml");
    
    // setup projection window
    projector.setup("main", 0, 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, false);
    //projector.setup("main", ofGetScreenWidth(), 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, true);
    
    // setup gui
    gui.setup("backgrounding");
    
    gui.add(mode.set("mode", &mode, 0, 6);
    gui.add(color.set("color", &color);
    gui.add(refreshAlpha.set("refreshAlpha", &refreshAlpha, 0.0f, 255.0f);
    gui.add(skip.set("skip", &skip, 1, 30);
    gui.add(numPoints.set("numPoints", &numPoints, 1, 300);
    gui.add(horiz.set("horizontal", &horiz);
    gui.add(mult.set("mult", &mult, 0.0f, 30.0f);
    gui.add(offset.set("offset", &offset, (float)-TWO_PI, (float)TWO_PI);
    gui.add(strokeWeight.set("strokeWeight", &strokeWeight, 0.5f, 8.0f);
    gui.add(alpha.set("alpha", &alpha, 0.0f, 255.0f);
    gui.add(lerpRate.set("lerpRate", &lerpRate, 0.0f, 1.0f);
    gui.add(contourSmoothness.add("contour smooth", &contourSmoothness, 1.0f, 10.0f);
    gui.add("contour dilation", &contourDilation, 0.01f, 2.0f);

}

void ofApp::update() {
    contourFinder.setMinArea(minArea);
    contourFinder.setMaxArea(maxArea);
    
    kinect.update();
    
    if(!kinect.isNewFrame()) {
        clean();
        return;
    }
    
    depthPixels = kinect.getDepthRawPixels();
    
    vector<int> eligibleUsers;
    int numUsers = kinect.getNumTrackedUsers();
    for (int i=0; i<numUsers; i++)
    {
        ofxOpenNIUser & user = kinect.getTrackedUser(i);
        if (!user.getMaskPixels().isAllocated()) {continue;}
        
        user.setMaskPixelFormat(OF_PIXELS_MONO);
        grayImage.setFromPixels(user.getMaskPixels());
        contourFinder.findContours(grayImage);
        
        if (contourFinder.size() == 0) {continue;}
        eligibleUsers.push_back(user.getXnID());
        
        vector<ofVec2f> calibratedPoints;
        getCalibratedContour(depthPixels, contourFinder.getContour(0), calibratedPoints);
        
        if (!users.count(user.getXnID())) {
            users[user.getXnID()] = new Contour(calibratedPoints, toOf(contourFinder.getCenter(0)), user.getXnID());
        }
        else {
            users[user.getXnID()]->setPoints(calibratedPoints, toOf(contourFinder.getCenter(0)));
        }
        
        for (int j=1; j<contourFinder.size(); j++) {
            if (contourFinder.getContour(j).size() > users[user.getXnID()]->getNumPoints()) {
                getCalibratedContour(depthPixels, contourFinder.getContour(j), calibratedPoints);
                users[user.getXnID()]->setPoints(calibratedPoints, toOf(contourFinder.getCenter(j)));
            }
        }
    }
    
    if (ofGetFrameNum() % frameSkip == 0 && eligibleUsers.size() > 0)
    {
        int labelToAdd = eligibleUsers[floor(ofRandom(eligibleUsers.size()))];
        int maxAge = ofRandom(maxAgeMin, maxAgeMax);
        int speed = ofRandom(speedMin, speedMax);
        int length = ofRandom(lengthMin, lengthMax);
        int skip = ofRandom(skipMin, skipMax);
        int margin = ofRandom(marginMin, marginMax);
        float noiseFactor = ofRandom(noiseFactorMin, noiseFactorMax);
        float ageFactor = ofRandom(ageFactorMin, ageFactorMax);
        float lineWidth = ofRandom(lineWidthMin, lineWidthMax);
        int maxAlpha = ofRandom(maxAlphaMin, maxAlphaMax);
        int updateRate = ofRandom(updateRateMin, updateRateMax);
        float lerpRate = ofRandom(lerpRateMin, lerpRateMax);
        int colorMargin = ofRandom(colorMarginMin, colorMarginMax);
        Ribbon *ribbon = new Ribbon(users[labelToAdd], maxAge, speed, length, skip,
                                    margin, noiseFactor, ageFactor,
                                    lineWidth, maxAlpha, updateRate,
                                    lerpRate, dilate, colorMargin, curved, match);
        ribbons[users[labelToAdd]].push_back(ribbon);
    }
    
    clean();
}

void ofApp::clean() {
    
    // remove old ribbons and contours
    map<Contour*, vector<Ribbon*> >::iterator itc = ribbons.begin();
    for (; itc != ribbons.end(); ++itc) {
        vector<Ribbon*>::iterator itr = itc->second.begin();
        while (itr != itc->second.end()) {
            if (!(*itr)->getActive()) {
                delete *itr;
                itc->second.erase(itr);
            }
            else {
                ++itr;
            }
        }
        if (itc->second.size() == 0) {
            delete users[itc->first->label];
            users.erase(itc->first->label);
            ribbons.erase(itc);
        }
    }
    
    // trick to reset user generator in case all users are lose
    if (kinect.getNumTrackedUsers()) {
        hadUsers = true;
    }
    else if (!kinect.getNumTrackedUsers() && hadUsers){
        hadUsers = false;
        kinect.setPaused(true);
        kinect.removeUserGenerator();
        kinect.addUserGenerator();
        kinect.setPaused(false);
    }
}

void ofApp::draw() {
    // gui
    ofPushMatrix();
    ofPushStyle();
    kinect.drawImage(0, 0);
    ofTranslate(640, 0);
    kinect.drawDepth();
    kinect.drawSkeletons();
    ofPopStyle();
    ofPopMatrix();
    gui.draw();
    
    // Ribbons
    projector.begin();
    
    ofPushMatrix();
    ofPushStyle();
    ofClear(0, 0);
    
    map<Contour*, vector<Ribbon*> >::iterator it = ribbons.begin();
    for (; it != ribbons.end(); ++it) {
        for (auto r : it->second) {
            r->update();
            r->draw();
        }
    }
    
    ofPopStyle();
    ofPopMatrix();
    
    projector.end();
}

void ofApp::getCalibratedContour(ofShortPixels & depthPixels, vector<cv::Point> & points, vector<ofVec2f> & calibratedPoints) {
    for (int j=0; j<points.size(); j+=smoothness) {
        ofPoint depthPoint = ofPoint(points[j].x, points[j].y,
                                     depthPixels[(int)(points[j].x + (int) points[j].y * kinect.getWidth())]);
        ofVec3f worldPoint = kinect.projectiveToWorld(depthPoint);
        ofVec2f projectedPoint = kpt.getProjectedPoint(worldPoint);
        ofPoint mappedPoint(projectedPoint.x * projector.getWidth(),
                            projectedPoint.y * projector.getHeight());
        calibratedPoints.push_back(mappedPoint);
    }
}

void ofApp::keyPressed(int key) {
    
}