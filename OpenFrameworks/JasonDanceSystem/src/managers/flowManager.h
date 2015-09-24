//
//  flowManager.h
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-02.
//
//

#pragma once
#include "ofxCv.h"
#include "ofMain.h"

class flowManager {
public:

	void setup();
	void update(ofPixels &maskPix);
	void draw(int x, int y, int w, int h);
	ofxCv::FlowPyrLK pyrLk;
    
    vector<ofPoint> features;
    vector<ofPoint> current;
    vector<ofVec2f> motion;

};
