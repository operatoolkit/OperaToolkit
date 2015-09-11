#pragma once

#include "ofMain.h"


struct Contour
{
    Contour(vector<ofVec2f> & points, ofPoint center, int label);
    void setPoints(vector<ofVec2f> & points, ofPoint center);
    void draw();
    int getNumPoints();
    
    vector<ofVec2f> points;
    ofPoint center;
    int label;
    int age;
    ofColor color;
};


class Ribbon
{
public:
    Ribbon(Contour *contour,
           int maxAge, int speed, int length, int skip,
           int margin, float noiseFactor, float ageFactor,
           float lineWidth, int maxAlpha, int updateRate,
           float lerpRate, float dilate, int colorMargin, bool curved, bool match);
    
    void update();
    void addPoint(int p);
    void draw();
    
    Contour * getContour() {return contour;}
    bool getActive() {return active;}
    
private:
    
    int idx, idxMatched;
    int age, maxAge;
    int speed;
    int length;
    int skip;
    bool active;
    int margin;
    float lineWidth;
    int maxAlpha;
    float noiseFactor, ageFactor;
    int updateRate;
    float lerpRate;
    float dilate;
    bool curved, match;
    int colorMargin;
    int offRed, offGreen, offBlue;
    
    Contour *contour;
    vector<ofVec2f> points;
    vector<int> lookup;
    vector<float> lookupMatched;
};