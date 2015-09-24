//
//  laserScene.h
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-02.
//
//

#pragma once

#include "scene.h"

class laserScene : public scene {
public:
    void setup(openNIManager * _oni, flowManager * _flow );
    void update(int width, int height);
    void draw(float x, float y, float scale);
    
    
    
    openNIManager * oni;
    flowManager * flow;
    
    ofxUICanvas *gui;
//    void guiEvent(ofxUIEventArgs &e);
    void toggleGUI(){ gui->toggleVisible(); }
    void hideGUI(){ gui->setVisible(false); }
    void saveGUI( string filename ) { gui->saveSettings(filename); }
    void loadGUI( string filename ) { gui->loadSettings(filename); }
    
    float threshold;
    float lineScale;
    float lineWidth;
    float lineWidthMult;
    
    ofFloatColor lineColor;
//    bool bgColorChanged;

};
