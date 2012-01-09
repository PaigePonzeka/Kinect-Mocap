/*
 *  shape.cpp
 *  jestureCap
 *
 *  Created by elizabeth pelka on 4/3/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "shape.h"

//barriers for walls
const float RWALL = 1024.0f; 
const float LWALL = 0.0f;
const float TWALL = 0.0f;
const float BWALL = 768.0f;

ofSoundPlayer shape_sounds[5];
int COLLISION_SOUND =2;
int EXPLOSION_SOUND =1;

const int SHAPE_SIZE_MAX = 200;//only used for rand()% expr so keep them ints
const int SHAPE_SIZE_MIN = 50;

float RGB[] = {0.0f,128.0f,255.0f};//limit color options

//to inhance reaction of the hoverOver effect (never set below 1)
const int SPEED = 4;


/*
 *constructors
 */
Shape::Shape(){}
Shape::Shape(float init_location_x,float init_location_y, float init_height, float init_width, 
			 float init_R, float init_G, float init_B, float init_A)
{
	location_x = init_location_x;
	location_y = init_location_y;
	height = init_height;
	width = init_width;
	shape_color.red = init_R;
	shape_color.green = init_G;
	shape_color.blue = init_B;
	shape_color.alpha = init_A;
	velocity_x = 0;
	velocity_x = 0;
	hit_count =0;
	grabbed = false;
	exploding = false;	
	trail_index = 0;
	for(int i = 0; i<10; i++){
		trail[i] = new SHADOW;
	}
}
/*
 * Set the shape sounds //changes with theme
 */
void Shape::setSounds(ofSoundPlayer new_sounds[5])
{
    for(int i =0; i<5; i++)
    {
        Shape::shape_sounds[i]=new_sounds[i];
    }
}
/*
 * changes location of the shape based on its velocity and checks to see if it has hit a wall (in which case it bounces)
 */
void Shape::move()
{	
	float newx, newy;
	//checks to to if something went wrong with velocity while loading and resets
	if((velocity_x>1000)||(velocity_y >1000)){
		printf("velocity soo high (%f ,%f)",velocity_x,velocity_y);
		setVelocity(0.0f, 0.0f);
	}
	
	//increment locations
	location_x += velocity_x;
	location_y += velocity_y;
	
	//hits LEFT or RIGHT wall
	if(outsideBounds_x(LWALL,RWALL)){
		hit_count++;
		//move location back
		location_y-=(velocity_y);
		location_x-=(velocity_x);
		
		//move shape to touch wall it will hit (find y value through ratio)
		if(velocity_x >0) {
			newx = RWALL-width;
			newy = location_y+(((newx-location_x)*velocity_y)/velocity_x);
		}
		else{
			newx = LWALL;
			newy = location_y-((location_x *velocity_y)/velocity_x);
		}	
		
		//set it
		setLocation(newx, newy);

		//change velocity direction
		velocity_x/= -1;

	}
	//hits TOP or BOTTOM wall
	if(outsideBounds_y(TWALL,BWALL)){
		hit_count++;
		
		//move location back
		location_x-=(velocity_x);
		location_y-=(velocity_y);
		
		//move shape to touch wall it will hit (find x value through ratio)
		if(velocity_y >0) {
			newy = BWALL-height;
			newx = location_x+(((newy-location_y)*velocity_x)/velocity_y);
		}
		else{
			newy = TWALL;
			newx = location_x-((location_y *velocity_x)/velocity_y);
		}
		
		//set it
		setLocation(newx, newy);
		
		//change velocity direction
		velocity_y/= -1;

	}
	
}
void Shape::moveBack()
{
	//move location back
	location_y-=(velocity_y);
	location_x-=(velocity_x); 

}

/*
 * decreases the velocity of the shape by some percent (friction)
 */
void Shape::slow()
{	
	//percent it will decrease speed by
	float percent= .10f;
	
	if(velocity_x!=0){
		if((velocity_x<1)&&(velocity_x>-1)) velocity_x = 0;
		if(velocity_x>0)velocity_x-=(velocity_x * percent);
		if(velocity_x<0)velocity_x+=(velocity_x * percent);
	}
	if(velocity_y!=0){
		if((velocity_y<1)&&(velocity_y>-1)) velocity_y = 0;
		if(velocity_y>0)velocity_y-=(velocity_y * percent);
		if(velocity_y<0)velocity_y+=(velocity_y * percent);
	}

}
/*
 * compares all other shape locations with that of itself to see if it will collide
 */
void Shape::checkCollision(int index)
{	float shapeT,shapeB,shapeL,shapeR;
	
	for(int i=0; i<10; i++){
		if(i==index)continue;
		shapeL = Shape::board[i]->getLocation_x();
		shapeR = shapeL + Shape::board[i]->getWidth();
		
		shapeT = Shape::board[i]->getLocation_y();
		shapeB = shapeT + Shape::board[i]->getHeight();
		
		// check for shapes overlapping 
		if(insideBounds(shapeL, shapeR, shapeT, shapeB)){
            //if the collision is already playing - stop it
            if(shape_sounds[COLLISION_SOUND].getIsPlaying())
            {
                //stop the sound
                shape_sounds[COLLISION_SOUND].stop();
                //printf("Stopping the sound\n");
            }
			hit_count++;
			Shape::board[i]->hit_count++;
            //play a collision sound
           // printf("COLLISION!\n");
            //shape_sounds[COLLISION_SOUND].play();
           // printf("Play Collision! \n");
			collision_Bounce(i);
			/***** this is where we can put switch collisions *****/
		}
		
	}
}
/*
 * transfers momentum between shape at index i and this shape
 */
void Shape::collision_Bounce(int i){
	//printf("--- hit shape [%i]\n",i);
	//move the shape back
	location_y-=(velocity_y);
	location_x-=(velocity_x);
	
	//switch the two shapes velocities
	float velx = getVelocity_x();
	float vely = getVelocity_y();
	setVelocity(Shape::board[i]->getVelocity_x(), Shape::board[i]->getVelocity_y());
	Shape::board[i]->setVelocity(velx, vely);
}	
/*
 * repells shapes increasingly dependent on how close to the center of the shape the points (x,y) are
 */
void Shape::collision_AntiMagnet(float x, float y){
	float horz_Middle, vert_Middle;
	horz_Middle = getLocation_y()+(getHeight()/2);
	vert_Middle = getLocation_x()+(getWidth()/2);
	
	//velocity based on distance from center
	float vely = (y-horz_Middle)/(getHeight()/2);
	float velx = (x-vert_Middle)/(getWidth()/2);
	
	//inverse velocity
	if(velx > 0) velx-=1;
	else velx+=1;
	if(vely >0) vely-=1;
	else vely+=1;
	
	//speed up the affect
	velx*=SPEED;
	vely*=SPEED;
	
	//increment velocity 
	incVelocity(velx, vely);
	
	//printf("incremented by (%f, %f)  velocity --(%f, %f)\n",velx,vely,velocity_x,velocity_y);
}

/*
  * if hit_count is enough it explodes it and generates a new shape
  *
  */
bool Shape::checkDamage()
{
	//shape is not special and has been hit too much = EXPLODE
	if((hit_count>4)&&(this!=Shape::board[10])){
		createExplosion();
		hit_count = 0;
		width = ofRandom(50, 200);
		height = ofRandom(50, 200);
		while(locationError(11,true)){
			location_y = ofRandom(TWALL, BWALL-200);
			location_x = ofRandom(LWALL, RWALL-200);
		}
	}
	//SPECIAL shape is hit too much = CHANGE THEME
	if((hit_count>max_hits)&&(this==Shape::board[10])){
		hit_count = 0;
		printf("hit special\n");
		if(Shape::board[10]->getWidth()>Shape::min_w_white_block)Shape::board[10]->setWidth(Shape::board[10]->getWidth()-10);
		if(Shape::board[10]->getHeight()>Shape::min_h_white_block)Shape::board[10]->setHeight(Shape::board[10]->getHeight()-10);
		return TRUE;//going to change theme
	}
	return FALSE;
}
/*
 * getter for explosion
 */
bool Shape::isExploding()
{
	return exploding;
}
/*
 * setter for explosion
 */
void Shape::doneExploding()
{
	exploding = false;
}
/*
 * fills bubble array for explosion
 */
void Shape::createExplosion()
{	
	if(this == Shape::board[10]){
		setWidth(BWALL);
		setHeight(RWALL);
	}
	exploding = true;
    //printf("Explosion");
   // if (shape_sounds[EXPLOSION_SOUND].getIsPlaying())
    //{
        //if the sound is still playing
        //stop it 
      //  shape_sounds[EXPLOSION_SOUND].stop();
        
    //}
    //shape_sounds[EXPLOSION_SOUND].play();
	for(int i = 0; i<10 ;i++){
        
		bubbles[i] = new SHADOW;
		bubbles[i]->x = ofRandom(getLocation_x(), getLocation_x()+getWidth());
		bubbles[i]->y = ofRandom(getLocation_y(), getLocation_y()+getHeight());
		bubbles[i]->size = ofRandom(10, 50);
		bubbles[i]->alpha = ofRandom(200, 255);
        
	}
}
/*
 * animation for bubbles explosion
 */
bool Shape::popBubbles(){
	bool done = true;
	for(int i = 0; i<10 ;i++){
		if(bubbles[i]->alpha >0){
			bubbles[i]->x += ofRandom(0,5);
			bubbles[i]->y += ofRandom(0,5);
			bubbles[i]->size +=5 ;
			bubbles[i]->alpha -=20;
			done = false;
		}
	}
	return done;
}
/*
 * makes 10 shadows of the shape trailing behind it
 */
void Shape::updateTrail()
{
	if(trail_index<9) trail_index++;
	else trail_index = 0;
	trail[trail_index]->x = location_x;
	trail[trail_index]->y = location_y;
}
int Shape::index(){
	return trail_index;
}

/*
 * checks that a new shape's location is not overlapping another shape
 * breakpoints and print outs warn against outside the walls locations
 */
bool Shape::locationError(int index, bool allow_one)
{
	float shapeT,shapeB,shapeL,shapeR;
    
	if(outsideBounds_x(LWALL,RWALL))
		printf("left or right error [%i] (%f, %f) --- (%f, %f)\n",index,getLocation_x(),getLocation_y(),getVelocity_x(),getVelocity_y());
		
	if(outsideBounds_y(TWALL,BWALL))
		printf("top or bottom error [%i] (%f, %f) --- (%f, %f)\n",index,getLocation_x(),getLocation_y(),getVelocity_x(),getVelocity_y());
	int count = 0;
	for(int i=0; i<index; i++){
		
		shapeL = Shape::board[i]->getLocation_x();
		shapeR = shapeL + Shape::board[i]->getWidth();
		
		shapeT = Shape::board[i]->getLocation_y();
		shapeB = shapeT + Shape::board[i]->getHeight();
		
		if(insideBounds(shapeL, shapeR, shapeT, shapeB)){
			count++;
			if((allow_one)&&(count<2))continue;
			return true;
		}
	}
	return false;
}
/*
 * returns true if a hand is hovering over a shape
 */
bool Shape::hoveredOver(float x, float y)
{
	float left = location_x;
	float right = location_x + width;
	float top = location_y;
	float bottom = location_y + height;
	
	if((x > left)&&(x < right)&&
	   (y > top)&&(y < bottom)){
		return true;
	}
    if(((x-20) > left)&&((x-20) < right)&&
	   ((y-20) > top)&&((y-20) < bottom)){
		return true;
	}
    if(((x +20) > left)&&((x +20)< right)&&
	   ((y+20) > top)&&((y+20) < bottom)){
		return true;
	}
	else return false;
	
}
/*
 * check for shape being outside certain barriers (used for walls)
 */		   
bool Shape::outsideBounds(float left, float right, float top, float bottom)
{
	if(((location_x < left)||((location_x + width) > right))&&
	   ((location_y < top)||((location_y + height) > bottom))){
		return true;
	}
	else return false;
}
/*
 * checks for being outside x barriers (helper in ousideBounds)
 */		   
bool Shape::outsideBounds_x(float left,float right)
{
	if((location_x < left)||((location_x + width) > right)){
		return true;
	}
	else return false;
}
/*
 * checks for being outside y barriers (helper in ousideBounds)
 */
bool Shape::outsideBounds_y(float top, float bottom)
{
	if((location_y < top)||((location_y + height) > bottom)){
		return true;
	}
	else return false;
}
/*
 * checks for being inside some barriers (used in hoveredOver)
 */
bool Shape::insideBounds(float left, float right, float top, float bottom)
{
	if((insideBounds_x(left, right))&&(insideBounds_y(top, bottom))){
		return true;
	}
	else return false;
}
/*
 * checks for being inside x barriers (helper in insideBounds)
 */
bool Shape::insideBounds_x(float left, float right)
{
	if((location_x > left)&&((location_x) < right)||
	   ((location_x+width) > left)&&((location_x+width) < right)||
	   (location_x < left)&&((location_x+width) > right)||
		(location_x > left)&&((location_x+width) < right))
	{
		return true;
	}
	else return false;
}
/*
 * checks for being inside y barriers (helper in insideBounds)
 */
bool Shape::insideBounds_y( float top, float bottom)
{
	if((location_y > top)&&((location_y) < bottom)||
	   ((location_y + height)> top)&&((location_y + height) < bottom)||
	   (location_y < top)&&((location_y + height) > bottom)||
	   (location_y > top)&&((location_y + height) < bottom)){
		return true;
	}
	else return false;
}

float Shape::getLocation_x()
{
	return location_x;
}

float Shape::getLocation_y()
{
	return location_y;
}

float Shape::getWidth()
{
	return width;
}

float Shape::getHeight()
{
	return height;
}

float Shape::getRed()
{
	return shape_color.red;
}

float Shape::getGreen()
{
	return shape_color.green;
}

float Shape::getBlue()
{
	return shape_color.blue;
}

float Shape::getAlpha()
{
	return shape_color.alpha;
}
void Shape::setHeight(float h){
	height = h;
}
void Shape::setWidth(float w){
	width = w;
}

void Shape::incVelocity(float vel_x, float vel_y)
{
	if(getVelocity_x()<0) vel_x/=-1;
	if(getVelocity_y()<0) vel_y/=-1;
	
	setVelocity(velocity_x+vel_x, velocity_y+vel_y);
}
void Shape::setVelocity(float vel_x, float vel_y)
{
	velocity_x = vel_x;
	velocity_y = vel_y;
}
float Shape::getVelocity_x(){
	return velocity_x;
}
float Shape::getVelocity_y(){
	return velocity_y;
}
void Shape::setColor(float R, float G, float B, float A)
{
	shape_color.red = R;
	shape_color.green = G;
	shape_color.blue = B;
	shape_color.alpha = A;
}
void Shape::setLocation(float x, float y)
{
   //put in check for nan (not a number) 
    //if nan happens then do ofRandom until you get a valid number
    while(isnan(x))
    {
        x=float(ofRandom(0.0f,RWALL-SHAPE_SIZE_MAX));

    }
    while(isnan(y))
    {
       y=float(ofRandom(0.0f,BWALL-SHAPE_SIZE_MAX)); 
    }
	location_x = x;
	location_y = y;
}

void Shape::grabShape(int hand)
{
	grabbed_by = hand;
	grabbed = true;
}

void Shape::releaseShapeFrom(int hand)
{
	if(grabbed_by == hand){
		grabbed_by = 0;
		grabbed = false;
	}
}
bool Shape::isGrabbed()
{
	return grabbed;
}
bool Shape::isGrabbedBy(int hand)
{	
	return((grabbed_by == hand)&&grabbed);
}