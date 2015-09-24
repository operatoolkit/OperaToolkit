#pragma once

#include "ofMain.h"

class particle
{
public:
    ofVec2f pos;
    vector<ofVec2f> posHistory;
    ofVec2f vel;
    ofVec2f frc;// frc is also know as acceleration (newton says "f=ma")

    float alpha;
    float size;

    particle();
    virtual ~particle(){};

    void resetForce();
    void addForce(float x, float y);
    void addRepulsionForce(float x, float y, float radius, float scale);
    void addAttractionForce(float x, float y, float radius, float scale);
    void addRepulsionForce(particle &p, float radius, float scale);
    void addAttractionForce(particle &p, float radius, float scale);
    
    void addDampingForce();
    
    void setInitialCondition(float px, float py, float vx, float vy);
    void update(int tailLength);
    void draw(ofFloatColor col);

    void bounceOffWalls(float minx, float miny, float maxx, float maxy);
    void wrapAround(float minx, float miny, float maxx, float maxy);

    float damping;
};

