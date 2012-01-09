#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxSimpleGuiToo.h"
#include "hand.h"
#include "shape.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <vector>



class HandJesture : public ofBaseApp
{
	
public:
	
    
	void setup();
	void update();
	void draw();
	void exit();
	
    
	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    void loadSounds();
	void checkClick(int cornerCount,int hand);
    void storeHandTrail(int i, ofPoint hand_location);
    void resetHandTrail();
    void checkSpeedMove(float x, float y);
    void drawPointCloud();
	void drawHands();
    void updateShapeColors();
	void drawHandCircle();
    void drawShapes(int hand);
	void drawBackground();
	void sendEvent(const std::string& event, const std::string& data);
	void detectCorner();
	void checkDepthUpdated();
	void initShapeBoard();
    
    void printHandTrail();
	void setTheme(int theme);
	//static Shape* board[10];
    
	// Kinect
	ofxKinect kinect;
	int angle;
	
	ofxCvColorImage		colorImage;
	ofxCvGrayscaleImage		checkGrayImage;
	
	ofxCvGrayscaleImage 	grayImage;
	ofxCvGrayscaleImage 	grayThresh;
	ofxCvGrayscaleImage		grayThreshPrev;
	
	ofxCvContourFinder 	contourFinder;
	struct themes;
    //debug values etc
	bool				debug;
	bool				showConfigUI;
	bool				mirror;
    bool                showUserFeedback;
    
    // for image
	int 				nearThreshold;
	int					farThreshold;
	
	// for mouse controll
	int					displayWidth;
	int					displayHeight;
    
    // couner count use for click
    int						handMode;
	int						checkMouseDownCount;
	int						toNormalModeCount;
	int						jestureFiredCount;
	int						currentCornerNums;
	vector<int>				cornerCountHistory;

	
	// sounds
	ofSoundPlayer		soundDetect;
	ofSoundPlayer		soundRelease;

	
	// for detection
	int					detectCount;
	int					detectTwoHandsCount;
	bool				detectingHands;
	bool				detectingTwoHands;
	vector<Hand *>		hands;
	
	
	// fonts
	ofTrueTypeFont		msgFont;
	ofTrueTypeFont		blockFont;
    
    // sounds
	ofSoundPlayer		soundClick;
    ofSoundPlayer       beats;
    ofSoundPlayer       background_sound;
    
    vector<ofPoint>			posHistory;
	vector<ofPoint>			adjustPosHistory;
};

#endif
