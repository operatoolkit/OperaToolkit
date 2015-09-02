#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1280, 800);
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofSetBackgroundAuto(false);
    
    projector.setup("projector", 0, 0, 1280, 800, false);
    
    static string maskShaderText =
    STRINGIFY(
              uniform sampler2DRect tex;
              uniform sampler2DRect depthTex;
              uniform float depthThresh;
              void main()
              {
                  vec3 col = texture2DRect(tex, gl_TexCoord[0].xy).rgb;
                  float depth = texture2DRect(depthTex, 0.5*gl_TexCoord[0].xy).r;
                  float d = depth < depthThresh ? 1.0 : 0.0;
                  gl_FragColor = vec4(col, d);
              }
    );

    depthShader.setupShaderFromSource(GL_FRAGMENT_SHADER, maskShaderText);
    depthShader.linkProgram();
    
    kinect0.open(true, true, 0);    // kinect0.open(true, true, 0, 2);
    kinect0.start();
    img.setup(kinect0.getProtonect(), 2);
    
    
    gui.setup();
    gui.setName("ThousandArms");
    gui.add(appFrameSkip.set("app frame skip", 1, 1, 30));
    gui.add(debug.set("debug", false));
    gui.add(recording.set("recording", true));
    gui.add(totalNumImages.set("total number images", 300, 100, 500));
    gui.add(depthThreshold.set("depth threshold", 3700, 0, 8000));
    gui.add(frameRate.set("frame rate", 5, 1, 15));
    gui.add(numImages.set("number images draw", 10, 1, 30));
    gui.add(imageSkip.set("image skip", 5, 1, 15));
    gui.add(frameSkip.set("frame skip", 5, 1, 30));
    
    indexImage = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    if (recording) {
        kinect0.update();
    }
    
    if (kinect0.isFrameNew() && recording)
    {
        colorTex0.loadData(kinect0.getColorPixelsRef());
        depthTex0.loadData(kinect0.getDepthPixelsRef());
        depthTex0.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
        img.update(depthTex0, colorTex0, false);
        
        ofFbo newFbo;
        newFbo.allocate(1024, 848);
        newFbo.begin();
        ofClear(0, 0);
        depthShader.begin();
        depthShader.setUniform1f("depthThresh", depthThreshold);
        depthShader.setUniformTexture("depthTex", depthTex0, 1);
        img.getRegisteredTexture(false).draw(0, 0);
        depthShader.end();
        newFbo.end();

        fbo.push_back(newFbo);
    }

    while (fbo.size() > totalNumImages) {
        fbo.erase(fbo.begin());
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (debug && fbo.size() > 0) {
        ofBackground(0);
        fbo[fbo.size()-1].draw(0, 0);
    }
    gui.draw();
    
    projector.begin();
    ofBackground(0);
    if (ofGetFrameNum() % appFrameSkip == 0 && fbo.size() > 0)
    {
        ofBackground(0);
        for (int i=0; i < numImages; i++)
        {
            int idx = (indexImage + i * imageSkip) % fbo.size();
            fbo[idx].draw(0, 0);
        }
        indexImage = (indexImage + frameSkip) % fbo.size();
    }
    projector.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
