#include "Ribbons.h"


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


Ribbon::Ribbon(Contour *contour,
               int maxAge, int speed, int length, int skip,
               int margin, float noiseFactor, float ageFactor,
               float lineWidth, int maxAlpha, int updateRate,
               float lerpRate, float dilate, int colorMargin, bool curved, bool match)
{
    this->contour = contour;
    this->maxAge = maxAge;
    this->speed = speed;
    this->length = length;
    this->skip = skip;
    this->margin = margin;
    this->noiseFactor = noiseFactor;
    this->ageFactor = ageFactor;
    this->lineWidth = lineWidth;
    this->maxAlpha = maxAlpha;
    this->updateRate = updateRate;
    this->lerpRate = lerpRate;
    this->dilate = dilate;
    this->colorMargin = colorMargin;
    this->curved = curved;
    this->match = match;
    
    offRed = ofRandom(-colorMargin, colorMargin);
    offGreen = ofRandom(-colorMargin, colorMargin);
    offBlue = ofRandom(-colorMargin, colorMargin);
    
    idx = ofRandom(contour->points.size());
    age = 0;
    active = true;
    
    for (int i=0; i<length; i++) {
        int j = (idx + i*skip) % contour->points.size();
        addPoint(j);
    }
    idx = (idx + length * skip) % contour->points.size();
}

void Ribbon::update()
{
    if (ofGetFrameNum() % updateRate == 0) {
        idx = (idx + speed) % contour->points.size();
        addPoint(idx);
        points.erase(points.begin());
        lookup.erase(lookup.begin());
        lookupMatched.erase(lookupMatched.begin());
    }
    age++;
    if (age >= maxAge) {
        active = false;
    }
}

void Ribbon::addPoint(int p)
{
    if (dilate != 1.0) {
        points.push_back(ofVec2f(contour->center.x + dilate * (contour->points[p].x - contour->center.x) + margin * ofSignedNoise(p * noiseFactor + 25, ageFactor * age - 9, -22),
                                 contour->center.y + dilate * (contour->points[p].y - contour->center.y) + margin * ofSignedNoise(p * noiseFactor + 17, ageFactor * age + 6, -50)));
    }
    else {
        points.push_back(ofVec2f(contour->points[p].x + margin * ofSignedNoise(p * noiseFactor + 25, ageFactor * age - 9, -22),
                                 contour->points[p].y + margin * ofSignedNoise(p * noiseFactor + 17, ageFactor * age + 6, -50)));
    }
    lookup.push_back(p);
    lookupMatched.push_back((float) p / contour->points.size());
}

void Ribbon::draw()
{
    ofPushStyle();
    ofNoFill();
    ofSetLineWidth(lineWidth);
    ofSetColor(contour->color.r + offRed,
               contour->color.g + offGreen,
               contour->color.b + offBlue,
               ofMap(abs(age - maxAge*0.5), 0, maxAge*0.5, 255, 0));
    
    ofBeginShape();
    for (int i=0; i<points.size(); i++) {
        if (match) {
            idxMatched = floor(contour->points.size() * lookupMatched[i]);
            points[i].x = ofLerp(points[i].x, contour->points[idxMatched].x +
                                 margin * ofSignedNoise(i * noiseFactor, ageFactor * age, 5), lerpRate);
            points[i].y = ofLerp(points[i].y, contour->points[idxMatched].y +
                                 margin * ofSignedNoise(i * noiseFactor, ageFactor * age, 10), lerpRate);
        }
        else {
            points[i].x = ofLerp(points[i].x, contour->points[lookup[i]].x +
                                 margin * ofSignedNoise(i * noiseFactor, ageFactor * age, 5), lerpRate);
            points[i].y = ofLerp(points[i].y, contour->points[lookup[i]].y +
                                 margin * ofSignedNoise(i * noiseFactor, ageFactor * age, 10), lerpRate);
        }
        curved ? ofCurveVertex(points[i].x, points[i].y) : ofVertex(points[i].x, points[i].y);
    }
    ofEndShape();
    ofPopStyle();
}
