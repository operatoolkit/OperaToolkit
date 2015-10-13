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
    projector.setup("main", 0, 0, PROJECTOR_RESOLUTION_X/2, PROJECTOR_RESOLUTION_Y/2, false);
//    projector.setup("main", ofGetScreenWidth(), 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, true);
    
    // setup gui
    gui.setup("parameters");
    gui.add(useUserImage.set("useUserImage", true));
    gui.add(nearThreshold.set("nearThreshold", 9, 0, 255));
    gui.add(farThreshold.set("farThreshold", 0, 0, 255));
    gui.add(minArea.set("minArea", 1000, 0, 5000));
    gui.add(maxArea.set("maxArea", 70000, 15000, 150000));
    gui.add(smoothness.set("smoothness", 3, 1, 30));
    gui.add(numNew.set("numNew", 1, 1, 10));
    gui.add(frameSkip.set("frameSkip", 3, 1, 10));
    gui.add(maxAgeMin.set("maxAgeMin", 50, 5, 50));
    gui.add(maxAgeMax.set("maxAgeMax", 100, 20, 100));
    gui.add(speedMin.set("speedMin", 1, 1, 5));
    gui.add(speedMax.set("speedMax", 4, 3, 20));
    gui.add(lengthMin.set("lengthMin", 15, 5, 30));
    gui.add(lengthMax.set("lengthMax", 80, 20, 120));
    gui.add(skipMin.set("skipMin", 5, 1, 10));
    gui.add(skipMax.set("skipMax", 10, 5, 30));
    gui.add(marginMin.set("marginMin", 12, 0, 40));
    gui.add(marginMax.set("marginMax", 30, 0, 100));
    gui.add(noiseFactorMin.set("noiseFactorMin", 0.01, 0.001, 0.02));
    gui.add(noiseFactorMax.set("noiseFactorMax", 0.03, 0.02, 0.1));
    gui.add(ageFactorMin.set("ageFactorMin", 0.01, 0, 0.01));
    gui.add(ageFactorMax.set("ageFactorMax", 0.03, 0, 0.02));
    gui.add(lineWidthMin.set("lineWidthMin", 1, 0, 3));
    gui.add(lineWidthMax.set("lineWidthMax", 3, 1, 8));
    gui.add(maxAlphaMin.set("maxAlphaMin", 200, 0, 200));
    gui.add(maxAlphaMax.set("maxAlphaMax", 255, 100, 255));
    gui.add(updateRateMin.set("updateRateMin", 1, 1, 5));
    gui.add(updateRateMax.set("updateRateMax", 3, 2, 10));
    gui.add(lerpRateMin.set("lerpRateMin", 0.4, 0, 1));
    gui.add(lerpRateMax.set("lerpRateMax", 0.6, 0, 1));
    gui.add(colorMarginMin.set("colorMarginMin", 30, 0, 100));
    gui.add(colorMarginMax.set("colorMarginMax", 30, 0, 100));
    gui.add(dilate.set("dilate", 1, 0, 2.0));
    gui.add(curved.set("curved", true));
    gui.add(match.set("match", true));
    gui.setPosition(0, 0);
    gui.loadFromFile("settings.xml");
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
    
    if (useUserImage)
    {
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
    }
    else
    {
        grayImage.setFromPixels(kinect.getDepthRawPixels());
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
        
        // determine found contours
        contourFinder.findContours(grayImage);
        
        for(int i = 0; i < contourFinder.size(); i++) {
            vector<cv::Point> points = contourFinder.getContour(i);
            int label = contourFinder.getLabel(i);
            eligibleUsers.push_back(label);
            
            vector<ofVec2f> calibratedPoints;
            getCalibratedContour(depthPixels, points, calibratedPoints);
            if (!users.count(label)) {
                users[label] = new Contour(calibratedPoints, toOf(contourFinder.getCenter(i)), label);
            }
            else {
                users[label]->setPoints(calibratedPoints, toOf(contourFinder.getCenter(i)));
            }
        }
    }
    
    // update ribbons
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
    ofTranslate(-640, 480);
    grayImage.draw(0, 0);
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