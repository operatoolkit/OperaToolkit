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
    
    
    
    ///
    
    decoder.decode("/Users/gene/Downloads/fireball_001_71x100 (3).gif");
    file = decoder.getFile();

    
    
    // set up kinect
    //kinect.setup();
    kinect.setupFromONI("/Users/gene/Code/of_v0.8-4.4_osx_release/of-tools/KinectProjectorToolkit/alecsroom.oni");
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
    //projector.setup("main", ofGetScreenWidth(), 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, true);
    
    // setup falling assets
    addImage("/Users/gene/Downloads/lisa_chung.png");
    
    // setup box2d
    box2d.init();
    box2d.enableEvents();
    box2d.setGravity(0, 0);
    //box2d.createGround(ofPoint(0, secondWindow.getHeight()), ofPoint(secondWindow.getWidth(), secondWindow.getHeight()));
    ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
    
    // setup gui
    gui.setup("parameters");
    gui.add(useUserImage.set("useUserImage", true));
    gui.add(nearThreshold.set("nearThreshold", 9, 0, 255));
    gui.add(farThreshold.set("farThreshold", 0, 0, 255));
    gui.add(minArea.set("minArea", 1000, 0, 5000));
    gui.add(maxArea.set("maxArea", 70000, 15000, 150000));
    gui.add(smoothness.set("smoothness", 3, 1, 30));
    gui.add(isFalling.set("isFalling", true));
    gui.add(rate.set("rate", 5, 1, 20));
    gui.add(tolerance.set("tolerance", 0.3f, 0.0f, 100.0f));
    gui.add(circleDensity.set("circleDensity", 0.3f, 0.0f, 1.0f));
    gui.add(circleBounce.set("circleBounce", 0.5f, 0.0f, 1.0f));
    gui.add(circleFriction.set("circleFriction", 0.1f, 0.0f, 1.0f));
    gui.add(contourColor.set("contourColor", ofColor(255, 225, 30, 200), ofColor(0), ofColor(255)));
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
    
    
    // update box2d
    lines.clear();
    edges.clear();
    
    map<int, Contour*>::iterator it = users.begin();
    for (; it != users.end(); ++it)
    {
        lines.push_back(ofPolyline());
        for (int j=0; j<it->second->points.size(); j++) {
            lines.back().addVertex(it->second->points[j].x, it->second->points[j].y);
        }
        ofPtr <ofxBox2dEdge> edge = ofPtr<ofxBox2dEdge>(new ofxBox2dEdge);
        lines.back().simplify(tolerance);
        for (int i=0; i<lines.back().size(); i++) {
            edge.get()->addVertex(lines.back()[i]);
        }
        edge.get()->create(box2d.getWorld());
        edges.push_back(edge);
    }
    
    // add some falling circles every so often
    if(isFalling && (int) ofRandom(0, rate) == 0 && img.size() > 0) {
        addCircle(ofRandom(img.size()));
    }
    
    if (spurtHack > 0.5 && !hasSpurted) {
        spurt(ofRandom(img.size()), 5);
        hasSpurted = true;
    }
    if (spurtHack < 0.5) {
        hasSpurted = false;
    }
    
    // destroy circles
    vector<IdCircle*>::iterator itc = circles.begin();
    while (itc != circles.end())
    {
        if (!ofRectangle(0, -200, projector.getWidth()+400, projector.getHeight()+400).inside((*itc)->circle->getPosition()) ||
            ((ItemData*)(*itc)->circle->getData())->t == explosionTime)
        {
            (*itc)->circle->destroy();
            circles.erase(itc);
        }
        else if (((ItemData*)(*itc)->circle->getData())->bHit) {
            ((ItemData*)(*itc)->circle->getData())->t++;
            ++itc;
        }
        else {
            ++itc;
        }
    }
    box2d.update();
    
    // get rid of old contours
    map<int, Contour*>::iterator itu = users.begin();
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

void ofApp::contactStart(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        if (!(e.a->GetType() == b2Shape::e_circle) &&
            (e.b->GetType() == b2Shape::e_circle)) {
            ItemData * aData = (ItemData*)e.b->GetBody()->GetUserData();
            aData->bHit = true;
        }
        else if (!(e.b->GetType() == b2Shape::e_circle) &&
                 (e.a->GetType() == b2Shape::e_circle)) {
            ItemData * aData = (ItemData*)e.a->GetBody()->GetUserData();
            aData->bHit = true;
        }
    }
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
    
    ofPushStyle();
    ofEnableAlphaBlending();
    map<int, Contour*>::iterator it = users.begin();
    for (; it != users.end(); ++it) {
        ofBeginShape();
        ofFill();
        ofSetColor(contourColor);
        for (int j=0; j<it->second->points.size(); j+=smoothness) {
            ofCurveVertex(it->second->points[j].x, it->second->points[j].y);
        }
        ofEndShape();
    }
    ofPopStyle();
    
    for (int i=0; i<circles.size(); i++)
    {
        ofPushMatrix();
        ofPushStyle();
        
        ofFill();
        //ofSetHexColor(0xc0dd3b);
        ofSetColor(255);
        
        ofVec2f pos = circles[i]->circle.get()->getPosition();
        float rad = circles[i]->circle.get()->getRadius();
        float ang = circles[i]->circle.get()->getRotation();
        
        ofSetRectMode(OF_RECTMODE_CENTER);
        ofTranslate(pos.x, pos.y);
        ofRotate(ang);
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        
        ItemData *data = (ItemData*) circles[i]->circle->getData();
        float size = ofMap(data->t, 0, explosionTime, 2*rad, 0);

        img[circles[i]->idx % img.size()].draw(0, 0, size, size);
        
        if (data->t > 0) {
            ofSetColor(255);
            file.drawFrame(ofMap(data->t, 0, explosionTime, 1, file.getNumFrames()-1), 0, 0);
        }
        
        ofDisableBlendMode();
        
        ofPopStyle();
        ofPopMatrix();
    }
    
    projector.end();
}

ofApp::IdCircle::IdCircle(ofPtr<ofxBox2dCircle> circle, int idx) {
    this->circle = circle;
    this->idx = idx;
}

void ofApp::addImage(string path)
{
    ofImage newImg;
    newImg.loadImage(path);
    newImg.resize(40, 40);
    img.push_back(newImg);
}

void ofApp::addCircle(int index)
{
    ofPtr<ofxBox2dCircle> circle = ofPtr<ofxBox2dCircle>(new ofxBox2dCircle);
    circle.get()->setPhysics(circleDensity, circleBounce, circleFriction);
    if (ofRandom(1) < 0.5) {
        circle.get()->setup(box2d.getWorld(), 1, ofGetHeight() * ofRandom(0.2, 0.8), ofRandom(15, 40));
        circle.get()->setVelocity(ofRandom(4, 12), 0);
    }
    else {
        circle.get()->setup(box2d.getWorld(), projector.getWidth()-1, ofGetHeight() * ofRandom(0.2, 0.8), ofRandom(15, 40));
        circle.get()->setVelocity(-ofRandom(4, 12), 0);
    }
    circle.get()->setData(new ItemData());
    circles.push_back(new IdCircle(circle, index));
}

void ofApp::spurt(int index, int amount)
{
    for (int i=0; i<amount; i++) {
        addCircle(index);
    }
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