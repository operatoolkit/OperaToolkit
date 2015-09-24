//
//  flowManager.cpp
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-02.
//
//

#include "flowManager.h"

//--------------------------------------------------------------
void flowManager::setup() {

    pyrLk.setMaxFeatures( 300 );
    pyrLk.setQualityLevel( 0.01 );
    pyrLk.setMinDistance( 4 );
    pyrLk.setWindowSize( 32 );
    pyrLk.setMaxLevel( 3 );
    
    ofPixels firstFrame;
    firstFrame.allocate(320, 240, 1);
    
//    pyrLk.calcOpticalFlow(firstFrame, firstFrame);
    
    features.clear();
    current.clear();
    motion.clear();
}

//--------------------------------------------------------------
void flowManager::update(ofPixels &maskPix){
    
    if (ofGetFrameNum() % 60 == 0) pyrLk.resetFeaturesToTrack();
    
    pyrLk.calcOpticalFlow(maskPix);
    
    features = pyrLk.getFeatures();
    current = pyrLk.getCurrent();
    motion = pyrLk.getMotion();

    
}

//--------------------------------------------------------------
void flowManager::draw(int x, int y, int w, int h){

    pyrLk.draw(x,y,w,h);
    
}
