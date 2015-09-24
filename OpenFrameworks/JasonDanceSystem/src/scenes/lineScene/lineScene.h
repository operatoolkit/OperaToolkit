//
//  lineScene.h
//  openNiSample007
//
//  Created by Jason Levine on 2014-02-03.
//
//

#include "scene.h"

class lineScene : public scene {
public:
    void setup(openNIManager * _oni, flowManager * _flow );
    void update(int width, int height);
    void draw(float x, float y, float scale);
    
    openNIManager * oni;
    flowManager * flow;
    
    ofxUICanvas *gui;
    void toggleGUI(){ gui->toggleVisible(); }
    void hideGUI(){ gui->setVisible(false); }
    void saveGUI( string filename ) { gui->saveSettings(filename); }
    void loadGUI( string filename ) { gui->loadSettings(filename); }
    
    //------------------------------------------------------------
    
    void drawLine(ofVec3f jointA, ofVec3f jointB);
    void drawLines(ofVec3f jointA, ofVec3f jointB, float spacing);
    void drawBodyLines();
    void drawMoire();
    
    float width, height;
    float lineWidth, lineLength;
    float linesWidth, linesLength;
    float distanceScale;
    
    bool llArm, rlArm,
    luArm, ruArm,
    llLeg, rlLeg,
    luLeg, ruLeg;
    
    bool bDrawBody, bDrawMoire;
    float skeletonSmoothing;
    
//    ofFloatColor bgColor;
    ofFloatColor lineColor;
    
};
