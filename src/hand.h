/*
 *  hand.h
 *  jestureCap
 *
 *  Created by Takashi on 11/02/12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HAND
#define _HAND

#include "ofMain.h"

class Hand{
	
public:
 
    Hand(bool isPrimary, int dispWidth, int dispHeight);

    ~Hand();
    //static bool hand_clicked_left;
    //static bool hand_clicked_right;
    void update(ofPoint currentPoint, int cornerCount, ofPoint currentCentroid);

	void setCentroid(ofPoint centroid);
	void setIsActive(bool isActive);
	ofPoint getCurrentPos(ofPoint newPoint);
	void setPos(ofPoint currentPoint);
	void unRegister();
	ofPoint getPos(); 
	bool getIsPrimary();

    bool dead;
	
private:
	void fireMouseClick();
	void fireMouseDown();
	void fireMouseUp();
	void fireMouseDrag();
	void fireMouseMove();
	bool checkClick(int cornerCount);
	bool checkSpeedMove();
	
	
	CGPoint	calcMousePosition();
	
	// Position
	ofPoint					currentTmpPos;
    ofPoint					currentPos;
	ofPoint					clickedPos;
	vector<ofPoint>			posHistory;
	vector<ofPoint>			adjustPosHistory;
	
	// couner count use for click
	int						handMode;
	int						mouseDownCount;
	int						toNormalModeCount;
	int						jestureFiredCount;
	int						currentCornerNums;
	vector<int>				cornerCountHistory;
	
	// reference
	Hand *					primaryHand;
	
	int						displayHeight;
	int						displayWidth;
	float					previousFrameCount;
	bool					isSuspendEvent;
	
	// sounds
	//ofSoundPlayer		soundClick;
	
	bool isActive;
	
	bool isPrimary; //true: can move mouse, false: can click mouse
};

#endif