#include "ofApp.h"


Contour::Contour(vector<ofVec2f> & points, ofPoint center, int label)
{
    this->points = points;
    this->center = center;
    this->label = label;
    age = 0;
    color = ofColor(ofRandom(60,255), ofRandom(60,255), ofRandom(60,255));
}

void Contour::setPoints(vector<ofVec2f> & points, ofPoint center)
{
    this->points = points;
    this->center = center;
}

int Contour::getNumPoints()
{
    return points.size();
}

void Contour::draw()
{
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(2);
    ofSetColor(color);
    ofBeginShape();
    for (int j=0; j<points.size(); j++) {
        ofVertex(points[j].x, points[j].y);
    }
    ofEndShape();
    ofPopStyle();
}

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetWindowShape(1280, 960);
    
    maxUsers = 5;
    
    // set up kinect
    //kinect.setup();
    kinect.setupFromONI("/Users/gene/Code/of_v0.8-4.4_osx_release/templates_old/Kinect/openni_oniRecording/bin/data/alecsroom.oni");
    //kinect.setupFromONI("/Users/gene/Desktop/oni/2012-06-25-14-08-00-860.oni");
    
    kinect.addImageGenerator();
    kinect.addDepthGenerator();
    kinect.addUserGenerator();
    kinect.setMaxNumUsers(maxUsers);
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
    
    // hold previous joints
    for (int i=0; i<maxUsers; i++) {
        vector<ofVec2f> newVec;
        //newVec.resize(openNi->getJointNames().size());
        pContourPoints.push_back(newVec);
        displace.push_back(newVec);
    }
    
    // Initial Allocation
    fluid.allocate(projector.getWidth(), projector.getHeight(), 0.5);
    
    // Seting the gravity set up & injecting the background image
    fluid.dissipation = 0.99;
    fluid.velocityDissipation = 0.99;
    
    fluid.setGravity(ofVec2f(0.0,0.0));
    //fluid.setGravity(ofVec2f(0.0,0.0098));
    
    //  Set obstacle
    fluid.begin();
    ofSetColor(0,0);
    ofSetColor(255);
    ofCircle(projector.getWidth()*0.5, projector.getHeight()*0.35, 40);
    fluid.end();
    fluid.setUseObstacles(false);
    
    // Adding constant forces
    /*
     fluid.addConstantForce(ofPoint(gfx.getWidth()*0.5,
     gfx.getHeight()*0.85),
     ofPoint(0,-2),
     ofFloatColor(0.5,0.1,0.0),
     10.f);
     */

    gui.setup("parameters");
    gui.add(useUserImage.set("useUserImage", true));
    gui.add(nearThreshold.set("nearThreshold", 9, 0, 255));
    gui.add(farThreshold.set("farThreshold", 0, 0, 255));
    gui.add(minArea.set("minArea", 1000, 0, 5000));
    gui.add(maxArea.set("maxArea", 70000, 15000, 150000));
    gui.add(smoothness.set("smoothness", 3, 1, 30));
    gui.add(simplify.set("simplify", 40.0f, 0.0f, 100.0f));
    gui.add(numContourPts.set("numContourPts", 10, 3, 100));
    gui.add(skip.set("skip", 5, 1, 100));
    gui.add(displaceLerp.set("displaceLerp", 0.1f, 0.0f, 1.0f));
    gui.add(dissipation.set("dissipation", 0.99f, 0.0f, 1.0f));
    gui.add(velDissipation.set("vel dissipation", 0.99f, 0.0f, 1.0f));
    gui.add(displacement.set("displacement", 10.0f, 0.0f, 100.0f));
    gui.add(strength.set("strength", 4.8f, 0.0f, 10.0f));
    gui.add(gravityX.set("gravityX",  0.0f, -0.02f, 0.02f));
    gui.add(gravityY.set("gravityY", 0.0f, -0.02f, 0.02f));
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
    
    
    // update fluids
    for (int i=0; i<pContourPoints.size(); i++) {
        while (pContourPoints[i].size() < numContourPts) {
            pContourPoints[i].push_back(ofVec2f(0,0));
        }
        if (pContourPoints[i].size() > numContourPts) {
            pContourPoints[i].resize(numContourPts);
        }
    }
    
    for (int i=0; i<displace.size(); i++) {
        while (displace[i].size() < numContourPts) {
            displace[i].push_back(ofVec2f(0,0));
        }
        if (displace[i].size() > numContourPts) {
            displace[i].resize(numContourPts);
        }
    }
    
    map<int, Contour*>::iterator itu = users.begin();
    int k = 0;
    for (; itu != users.end(); ++itu) {
        if (k >= maxUsers) continue;
        ofPolyline pl;
        
        for (int j=0; j<itu->second->points.size(); j++) {
            pl.addVertex(itu->second->points[j]);
        }
        //pl.addVertices((vector<ofPoint>&) currentContours[i]);
        pl.simplify(simplify);
        
        vector<ofPoint> verts = pl.getVertices();
        ofVec2f newDisplace;
        for (int j=0; j<numContourPts; j++) {
            int idx = ofMap(j, 0, numContourPts, 0, verts.size());
            ofVec2f pt = verts[idx];
            newDisplace = pt - pContourPoints[k][j];
            displace[k][j].set(ofLerp(displace[k][j].x, newDisplace.x, displaceLerp),
                               ofLerp(displace[k][j].y, newDisplace.y, displaceLerp));
            
            pContourPoints[k][j] = pt;
            
            if (j%skip==0) {
                ofPoint m = pt;
                ofPoint d = displace[k][j]*displacement;
                ofPoint c = ofPoint(320, 240) - m;
                c.normalize();
                fluid.addTemporalForce(m,
                                       d,
                                       ofFloatColor(c.x,c.y,0.5)*sin(ofGetElapsedTimef()),
                                       strength);
            }
        }
        k++;
    }
    
    //fluid.setGravity(ofVec2f(gravityX, gravityY));
    fluid.dissipation = dissipation;
    fluid.velocityDissipation = velDissipation;
    
    fluid.update();
    
    // get rid of old contours
    itu = users.begin();
    while (itu != users.end()) {
        bool found = false;
        for (auto e : eligibleUsers) {
            if (e == itu->first) {
                found = true;
            }
        }
        if (!found) {
            delete itu->second;
            users.erase(itu++);
        }
        else {
            ++itu;
        }
    }
    
    clean();
}

void ofApp::clean() {    
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
    ofClear(0, 0);
    fluid.draw();
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