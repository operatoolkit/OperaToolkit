#pragma once

#include "ofMain.h"
#include "SuperColliderLooper.h"


class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    SuperCollider sc3;
    SuperColliderLooper looper;
};
