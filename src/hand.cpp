/*
 *  hand.cpp
 *  jestureCap
 *
 *  Created by Takashi on 11/02/12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "hand.h"
#include "handJesture.h"

int mouseDownCount = 0;
const int MOUSE_CLICK_FRAME = 8;
const int HAND_MODE_NORMAL = 0;
const int HAND_MODE_MOVE = 1;
const int HAND_MODE_CLICK = 2;
const int HAND_MODE_DRAG = 3;
const int POSITION_HISTORY_SIZE = 4;
const int JESTURE_FIRE_BUFFER = 20;


Hand::Hand(bool isPrimary, int dispWidth, int dispHeight) {
	isPrimary = isPrimary;
	isActive = false;
	displayHeight = dispHeight;
	displayWidth = dispWidth;
	mouseDownCount = 0;
	jestureFiredCount = 0;
	toNormalModeCount = 0;
	handMode = HAND_MODE_NORMAL; // NORMAL
	//centroid = ofPoint(0, 0);
	previousFrameCount = ofGetFrameNum();
	isSuspendEvent = false;
	
	
	//soundClick.loadSound("sound/16582__tedthetrumpet__kettleswitch1.aif");
	//soundClick.setVolume(100);

}

Hand::~Hand(){}

void Hand::update(ofPoint pos, int cornerCount, ofPoint currentCentroid) {
	
	if (ofGetFrameNum() - previousFrameCount > 15) {
		isSuspendEvent = true;
		jestureFiredCount = 10;
	} else {
		isSuspendEvent = false;
		jestureFiredCount = max(0, --jestureFiredCount);
	}
	previousFrameCount = ofGetFrameNum();
		
	currentTmpPos = getCurrentPos(pos);
	//ofLog(OF_LOG_VERBOSE, "HAND_MODE" + ofToString(handMode) + " : " + ofToString(currentTmpPos.x));
	
	if (handMode == HAND_MODE_NORMAL) {
		if (!checkClick(cornerCount)) {
			setPos(currentTmpPos);
			fireMouseMove();
			checkSpeedMove();
		}
	} else if (handMode == HAND_MODE_MOVE) {
		setPos(currentTmpPos);
		fireMouseMove();
		checkSpeedMove();
	} else if (handMode == HAND_MODE_DRAG) {
		if (!checkClick(cornerCount)) {
			setPos(currentTmpPos);
			fireMouseDrag();
		}	
	}else if (handMode == HAND_MODE_CLICK) {
		checkClick(cornerCount);
	}
}

void Hand::unRegister() {
	
	
	if (handMode == HAND_MODE_DRAG) {
		fireMouseUp();
	}
	isActive = false;
	handMode = HAND_MODE_NORMAL;
}

/*returns the point to move the mouse cursor*/
CGPoint Hand::calcMousePosition() {
	float x = currentPos.x;
	float y = currentPos.y;
		
    x = max(0.0, x*100.0/640.0-15.0f);
	x = min(x, 70.0f)/70*100;
	y = max(0.0, y*100.0/480.0-10.0f);
	y = min(y, 70.0f)/70*100;
	
	CGPoint pt;
	pt.x = x/100*displayWidth;
	pt.y = y/100*displayHeight;
	
	return pt;
}
bool Hand::checkSpeedMove() {
	
	if (posHistory.size() < POSITION_HISTORY_SIZE) {
		false;
	}
	
	float x = 0;
	float y = 0;
	
	for (int j = 0; j < posHistory.size()-1; j++) {
		x += posHistory.at(j).x - posHistory.at(j+1).x;
		y += posHistory.at(j).y - posHistory.at(j+1).y;
        float widthStep = ofGetWidth() / 3.0f;
        if (x >= widthStep && x < widthStep*2){
           // beats.setSpeed( 0.5f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*1.0f);
           // HandJesture::background_sound.setSpeed( 0.5f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*1.0f);
        } 

	}
	
	
	if (abs(x) + abs(y) > 12) {
		handMode = HAND_MODE_MOVE;
		toNormalModeCount = 0;
		return true;
	}
	
	if (handMode == HAND_MODE_MOVE) {
		toNormalModeCount++;
		if (toNormalModeCount > 10) {
			//ofLog(OF_LOG_VERBOSE, "TO NORMAL MODE");
			handMode = HAND_MODE_NORMAL;
			return false;
		}
		return true;
	}
	handMode = HAND_MODE_NORMAL;
	return false;
}

/*Checks to see if the hand has been clicked*/
bool Hand::checkClick(int cornerCount) {
	cornerCountHistory.push_back(cornerCount);
	if (cornerCountHistory.size() > 6) {
		cornerCountHistory.erase(cornerCountHistory.begin());
	} else {
		return false;
	}
		
	int oldCornerNums = 0;
	int cornerNums = 0;
	for (int i=0; i<cornerCountHistory.size(); i++) {
		if (i < 4) {
			oldCornerNums += cornerCountHistory[i];
		} else {
			cornerNums += cornerCountHistory[i];
		}
	}
	oldCornerNums = oldCornerNums/4;
	cornerNums = cornerNums/2;
			
	
	if (handMode == HAND_MODE_NORMAL && cornerNums + 150 < oldCornerNums) {
		// mouse down
		currentCornerNums = cornerNums;
		//fireMouseDown();
		//clikcedPos = calcMousePosition();
		//isMouseDown = true;
		handMode = HAND_MODE_CLICK;
		mouseDownCount = 0;
		return true;
	}
	if (cornerNums > currentCornerNums + 150) {
		if (handMode == HAND_MODE_DRAG) {
			fireMouseUp();
			//soundClick.play();
			handMode = HAND_MODE_NORMAL;
			return true;
		} else if (handMode == HAND_MODE_CLICK) {
			fireMouseClick();
			//soundClick.play();
			handMode = HAND_MODE_NORMAL;
			return true;
		}
	}
	if (handMode == HAND_MODE_CLICK) {
		mouseDownCount++;
		if (mouseDownCount > MOUSE_CLICK_FRAME) {
			handMode = HAND_MODE_DRAG;
			fireMouseDown();
			//soundClick.play();
			mouseDownCount = 0;
		}
	}
	return false;
}

void Hand::fireMouseMove() {

}

/*This is there the mouse clicked */
void Hand::fireMouseDown() {
	//cout << "Mouse Down!!+++++++" << endl;
    //set mouseclick to true
   // handClicked = true;

}

void Hand::fireMouseUp() {
	//cout << "Mouse Up!!-----------" << endl;
	//set mouseclick to false;
  //  handClicked=false;
}

void Hand::fireMouseDrag() {
    
}

void Hand::fireMouseClick() {
	fireMouseDown();
	fireMouseUp();
}

/*get the current position of the hand*/
ofPoint Hand::getCurrentPos(ofPoint newPos) {
	
	if (posHistory.size() == 0) {
		return newPos;
	}
	
	float x = newPos.x;
	float y = newPos.y;
	
	for (int j = 0; j < posHistory.size(); j++) {
		x += posHistory.at(j).x;
		y += posHistory.at(j).y;
	}
	
	x = x/(posHistory.size()+1);
	y = y/(posHistory.size()+1);
	
	return ofPoint(x, y);	
}

void Hand::setPos(ofPoint pos) {
	currentPos = pos;
	
	posHistory.push_back(pos);
	if (posHistory.size() > POSITION_HISTORY_SIZE) {
		posHistory.erase(posHistory.begin());
	}	
}

void Hand::setIsActive(bool active) {
	isActive = active;
}

ofPoint Hand::getPos() {
	return currentPos;
}

bool Hand::getIsPrimary() {
	return isPrimary;
}

