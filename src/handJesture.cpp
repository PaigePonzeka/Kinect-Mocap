#include "handJesture.h"
//#include "zhelpers.hpp"

//the ZMQ socket used to transfer data to the browser
/*
 zmq::context_t context (1);
 zmq::socket_t socket (context, ZMQ_PUB);
 */
//--------------------------------------------------------------

//array that stores the shapes on the board
Shape* Shape::board[10];
int Shape::max_hits = 150;
float Shape::min_w_white_block =400.0f;
float Shape::min_h_white_block = 50.0f;
bool setSIZE = true;

// Struct that stores arrays of Colors and Sounds
ofColor colors[10];
ofSoundPlayer sounds[5];
ofSoundPlayer twilight_sounds[5];
ofSoundPlayer groovy_sounds[5];
ofSoundPlayer asian_sounds[5];
ofSoundPlayer chordal_sounds[5];
ofSoundPlayer generic_sounds[5];
ofSoundPlayer retro_sounds[5];
ofSoundPlayer jazz_sounds[5];
ofSoundPlayer race_sounds[5];

//An array of the last hand positions
ofPoint hand_trail_one[20];
ofPoint hand_trail_two[20];
int HAND_TRAIL_SIZE = 20;
int trail_one;
int trail_two;
int intial_trail_one;
int intial_trail_two;


// Themes Values
const int TWILIGHT_THEME = 0;
const int GROOVY_THEME = 1;
const int ASIAN_THEME = 2;
const int JAZZ_THEME = 3;
const int CHORDAL_THEME = 4;
const int GENERIC_THEME = 5;
const int RETRO_THEME = 6;
const int RACE_THEME = 7;
string string_themes[][15] ={ 
/*twilight*/{"CAKE?","hello","hi","welcome","there's more","do more","more","come on","done yet?","so close"}
/*groovy*/,{"groovy","look","see","colors","pretty","playful","change them","explore","explode","keep going"}
/*asian*/,{"TOUCH!","well done","how nice","comfortable?","you get it?","so easy","too easy?","what now?","will it change?","maybe"}
/*jazzy*/,{"jazzy","never ends","forever","scared?","apathetic?","whatever","leave","who needs you?","that's it","screw it"}
/*generic*/,{"generic","you there?","im bored","hit me","come on","you wimp","more","HIT ME","that's it","one more time"}
/*chordal*/,{"chordal","sorry","my bad","ok?","forgiven?","accept it","thanks","come back","see ya","bye"}
/*retro*/,{"RAW","play with me","i'm lonely","don't go","why stay?","help","one more","i lied","two more","i lied"}
/*race*/,{"boom","hey","that hurts","ouch","ahh","stop","pain","why?","numb","finish"}
};
int max_number_themes = 7;
int current_theme=0;



//screen size
int screen_width= 1024;
int screen_height=768;
float centroidX;//hand location
float centroidY;
float prev_x = 0.0f;//previous location for velocity calc
float prev_y = 0.0f;
float RGB2[] = {0.0f,128.0f,255.0f};//limit color options

int checkMouseDownCount = 0;
int cornerCount = 0;

bool Currently_Grabbing_One_Shape = true;

const int MOUSE_CLICK_FRAME = 8;
const int HAND_MODE_NORMAL = 0;
const int HAND_MODE_MOVE = 1;
const int HAND_MODE_CLICK = 2;
const int HAND_MODE_DRAG = 3;
const int POSITION_HISTORY_SIZE = 4;
const int JESTURE_FIRE_BUFFER = 20;

const int SHAPE_SIZE_MAX = 200;//only used for rand()% expr so keep them ints
const int SHAPE_SIZE_MIN = 50;


void HandJesture::initShapeBoard(){
    //set intial board sounds
	for(int i = 0; i<11 ; i++){
		
        //random value for the theme 
        int color_value = ofRandom(1, 5);
        /*Edit the shape sizes to user ofrandom instead of rand... wasn't working*/
		Shape::board[i] = new Shape(
									float(ofRandom(0.0f,screen_width-SHAPE_SIZE_MAX)),float(ofRandom(0.0f,screen_height-SHAPE_SIZE_MAX)),//random location 
                                    float(ofRandom(SHAPE_SIZE_MIN, SHAPE_SIZE_MAX)),//random width 
							 float(ofRandom(SHAPE_SIZE_MIN, SHAPE_SIZE_MAX)),	//random height
							 colors[color_value].r, colors[color_value].g, colors[color_value].b, float(ofRandom(0.0f, 255.0f)));//random color
		if(i == 10){
			Shape::board[i]->setColor(255.0, 255.0, 255.0, 100.0);
			if(setSIZE){
				Shape::board[i]->setHeight(120.0f);
				Shape::board[i]->setWidth( Shape::min_w_white_block);
			}
			else{
				Shape::board[i]->setHeight(float(ofRandom(Shape::min_h_white_block,SHAPE_SIZE_MAX)));
				Shape::board[i]->setWidth(float(ofRandom( Shape::min_w_white_block,SHAPE_SIZE_MAX)));
			}
		}
		printf("loc [%i] (%f ,%f)\n",i,Shape::board[i]->getLocation_x(),Shape::board[i]->getLocation_y());
		while(Shape::board[i]->locationError(i,false)){
			Shape::board[i]->setLocation(float(ofRandom(0.0f,screen_width-SHAPE_SIZE_MAX)), float(ofRandom(0.0f,screen_height-SHAPE_SIZE_MAX)));
			printf("error [%i] new loc (%f ,%f)\n",i,Shape::board[i]->getLocation_x(),Shape::board[i]->getLocation_y());
            
            
		}
	}
}
/*Update the color of all the shapes on the board (ro reflect the new theme)*/
void HandJesture::updateShapeColors()
{
    for(int i = 0; i<10 ; i++){
        int color_value = ofRandom(1, 6);
        Shape::board[i]->setColor(colors[color_value].r, colors[color_value].g, colors[color_value].b, 1);
        
        //update sounds
        Shape::board[i]->setSounds(sounds);
    }
}
/*Load sounds for each theme*/
void HandJesture::loadSounds()
{
    
    /* 
     Twilight
     */
    // background sounds
    twilight_sounds[0].loadSound("sound/Twilight/background.mp3");
    twilight_sounds[0].setVolume(0.25f);
    twilight_sounds[0].setMultiPlay(true);
    twilight_sounds[0].setLoop(true);
    
    //explosion sounds
    twilight_sounds[1].loadSound("sound/Twilight/explosion.mp3");
    twilight_sounds[1].setVolume(0.50f);
    twilight_sounds[1].setMultiPlay(true);
    twilight_sounds[1].setLoop(false);
    
    //collision sounds
    twilight_sounds[2].loadSound("sound/Twilight/collision.mp3");
    twilight_sounds[2].setVolume(0.50f);
    twilight_sounds[2].setMultiPlay(true);
    twilight_sounds[2].setLoop(false);
       
    /* 
     Groovy
     */
    // background sounds
	groovy_sounds[0].loadSound("sound/Groovy/background.mp3");
    groovy_sounds[0].setVolume(0.25f);
	groovy_sounds[0].setMultiPlay(true);
    groovy_sounds[0].setLoop(true);

    //explosion sounds
    groovy_sounds[1].loadSound("sound/Groovy/explosion.mp3");
    groovy_sounds[1].setVolume(0.50f);
    groovy_sounds[1].setMultiPlay(true);
    groovy_sounds[1].setLoop(false);
    
    //collision sounds
    groovy_sounds[2].loadSound("sound/Groovy/collision.mp3");
    groovy_sounds[2].setVolume(0.50f);
    groovy_sounds[2].setMultiPlay(true);
    groovy_sounds[2].setLoop(false);
    
    /* 
     Asian
     */
    // background sounds
    asian_sounds[0].loadSound("sound/Asian/background.mp3");
    asian_sounds[0].setVolume(0.50f);
    asian_sounds[0].setMultiPlay(true);
    asian_sounds[0].setLoop(true);
    
    //explosion sounds
    asian_sounds[1].loadSound("sound/Asian/explosion.mp3");
    asian_sounds[1].setVolume(0.50f);
    asian_sounds[1].setMultiPlay(true);
    asian_sounds[1].setLoop(false);
    
    //collision sounds
    asian_sounds[2].loadSound("sound/Asian/collision.mp3");
    asian_sounds[2].setVolume(0.50f);
    asian_sounds[2].setMultiPlay(true);
    asian_sounds[2].setLoop(false);
    
    /* 
     Jazz
     */
    // background sounds
    jazz_sounds[0].loadSound("sound/Jazz/background.mp3");
    jazz_sounds[0].setVolume(0.25f);
    jazz_sounds[0].setMultiPlay(true);
    jazz_sounds[0].setLoop(true);
    
    //explosion sounds
    jazz_sounds[1].loadSound("sound/Jazz/explosion.mp3");
    jazz_sounds[1].setVolume(0.50f);
    jazz_sounds[1].setMultiPlay(true);
    jazz_sounds[1].setLoop(false);
    
    //collision sounds
    jazz_sounds[2].loadSound("sound/Jazz/collision.mp3");
    jazz_sounds[2].setVolume(0.50f);
    jazz_sounds[2].setMultiPlay(true);
    jazz_sounds[2].setLoop(false);
    
    /* 
     Chordal
     */
    // background sounds
    chordal_sounds[0].loadSound("sound/Chordal/background.mp3");
    chordal_sounds[0].setVolume(0.25f);
    chordal_sounds[0].setMultiPlay(true);
    chordal_sounds[0].setLoop(true);
    
    //explosion sounds
    chordal_sounds[1].loadSound("sound/Chodral/explosion.mp3");
    chordal_sounds[1].setVolume(0.50f);
    chordal_sounds[1].setMultiPlay(true);
    chordal_sounds[1].setLoop(false);
    
    //collision sounds
    chordal_sounds[2].loadSound("sound/Chodral/collision.mp3");
    chordal_sounds[2].setVolume(0.50f);
    chordal_sounds[2].setMultiPlay(true);
    chordal_sounds[2].setLoop(false);
    
    /* 
     Generic
     */
    // background sounds
    generic_sounds[0].loadSound("sound/Generic/background.mp3");
    generic_sounds[0].setVolume(0.25f);
    generic_sounds[0].setMultiPlay(true);
    generic_sounds[0].setLoop(true);
    
    //explosion sounds
    generic_sounds[1].loadSound("sound/Generic/explosion.mp3");
    generic_sounds[1].setVolume(0.50f);
    generic_sounds[1].setMultiPlay(true);
    generic_sounds[1].setLoop(false);
    
    //collision sounds
    generic_sounds[2].loadSound("sound/Generic/collision.mp3");
    generic_sounds[2].setVolume(0.50f);
    generic_sounds[2].setMultiPlay(true);
    generic_sounds[2].setLoop(false);

    /* 
     Retro
     */
    // background sounds
    retro_sounds[0].loadSound("sound/Retro/background.mp3");
    retro_sounds[0].setVolume(0.25f);
    retro_sounds[0].setMultiPlay(true);
    retro_sounds[0].setLoop(true);
    
    //explosion sounds
    retro_sounds[1].loadSound("sound/Retro/explosion.mp3");
    retro_sounds[1].setVolume(0.50f);
    retro_sounds[1].setMultiPlay(true);
    retro_sounds[1].setLoop(false);
    
    //collision sounds
    retro_sounds[2].loadSound("sound/Retro/collision.mp3");
    retro_sounds[2].setVolume(0.50f);
    retro_sounds[2].setMultiPlay(true);
    retro_sounds[2].setLoop(false);
    
    /* 
     Race
     */
    // background sounds
    race_sounds[0].loadSound("sound/Race/background.mp3");
    race_sounds[0].setVolume(0.25f);
    race_sounds[0].setMultiPlay(true);
    race_sounds[0].setLoop(true);
    
    //explosion sounds
    race_sounds[1].loadSound("sound/Race/explosion.mp3");
    race_sounds[1].setVolume(0.50f);
    race_sounds[1].setMultiPlay(true);
    race_sounds[1].setLoop(false);
    
    //collision sounds
    race_sounds[2].loadSound("sound/Race/collision.mp3");
    race_sounds[2].setVolume(0.50f);
    race_sounds[2].setMultiPlay(true);
    race_sounds[2].setLoop(false);

}
/*Allocates sounds and colors based on selected theme*/
void HandJesture::setTheme(int theme)
{
     // if the background sound is already playing stop it
    if(sounds[0].getIsPlaying())
    {
        printf("TURNING OFF BACKGROUND SOUND \n");
        sounds[0].stop();
    }
    if(theme==TWILIGHT_THEME)
    {
        printf("Setting Theme: Twilight \n");
        //background color -black
        colors[0].r=0;
        colors[0].g=0;
        colors[0].b=0;
        
        //shape colors
        // Yellow #ffcc33
        colors[1].r=255;
        colors[1].g=204;
        colors[1].b=51;
        
        // Blue #6699ff
        colors[2].r=102;
        colors[2].g=153;
        colors[2].b=255;
        
        // Purple #652ead
        colors[3].r=101;
        colors[3].g=46;
        colors[3].b=173;
        
        // Lt.Blue #296FA4
        colors[4].r=41;
        colors[4].g=111;
        colors[4].b=164;
        
        
        // Lt.Yellow #FFE38F
        colors[5].r=255;
        colors[5].g=227;
        colors[5].b=143;
        
        // sounds
        // intialize the background board sound
        sounds[0]=twilight_sounds[0];
        sounds[1]=twilight_sounds[1];
        sounds[2]=twilight_sounds[2];
               
    }
    else if(theme==GROOVY_THEME)
    {
        printf("Setting Theme: Groovy \n");
        // background color -black
        colors[0].r=0;
        colors[0].g=0;
        colors[0].b=0;
        
        // shape colors
        // Yellow #ffff00
        colors[1].r=255;
        colors[1].g=255;
        colors[1].b=0;
        
        // Green #9FEE00
        colors[2].r=159;
        colors[2].g=238;
        colors[2].b=0;
        
        // Blue #0099FF
        colors[3].r=0;
        colors[3].g=153;
        colors[3].b=255;
        
        // Purple #7109AA
        colors[4].r=113;
        colors[4].g=9;
        colors[4].b=170;
        
        
        // Pink #CD0074
        colors[5].r=205;
        colors[5].g=0;
        colors[5].b=116;
        
        // sounds
        // intialize the background board sound
        sounds[0]=groovy_sounds[0];
        sounds[1]=groovy_sounds[1];
        sounds[2]=groovy_sounds[2];
    }
    else if(theme==ASIAN_THEME)
    {
        printf("Setting Theme: ASIAN \n");
        // background color -black
        colors[0].r=0;
        colors[0].g=0;
        colors[0].b=0;
        
        // shape colors
        // Yellow #FFAA11
        colors[1].r=255;
        colors[1].g=170;
        colors[1].b=17;
        
        // Red #CC3322
        colors[2].r=204;
        colors[2].g=51;
        colors[2].b=34;
        
        // Deep Red #A61B1A
        colors[3].r=166;
        colors[3].g=27;
        colors[3].b=26;
        
        // brown #A6591A
        colors[4].r=166;
        colors[4].g=89;
        colors[4].b=26;
        
        
        // Gold #FFD700
        colors[5].r=255;
        colors[5].g=215;
        colors[5].b=0;
        
        // sounds
        // intialize the background board sound
        sounds[0]=asian_sounds[0];
        sounds[1]=asian_sounds[1];
        sounds[2]=asian_sounds[2];
    }
    else if(theme == JAZZ_THEME)
    {
        printf("Changing Theme To: Business \n");
        // background color -white
        colors[0].r=255;
        colors[0].g=255;
        colors[0].b=255;
        
        // shape colors
        // purple #cb99ff
        colors[1].r=203;
        colors[1].g=153;
        colors[1].b=255;
        
        // peach #FFE791
        colors[2].r=255;
        colors[2].g=231;
        colors[2].b=145;
        
        // pink #FF91F1
        colors[3].r=255;
        colors[3].g=145;
        colors[3].b=241;
        
        // green #E9FF91
        colors[4].r=233;
        colors[4].g=255;
        colors[4].b=145;
        
        
        // blue #AB9FFF
        colors[5].r=171;
        colors[5].g=159;
        colors[5].b=255;
        
        // set the default sounds to busneiss sounds
        sounds[0]=jazz_sounds[0];
        sounds[1]=jazz_sounds[1];
        sounds[2]=jazz_sounds[2];
    }

    else if(theme == CHORDAL_THEME)
    {
        printf("Changing Theme To: CHORDAL\n");
        //background color -black
        colors[0].r=0;
        colors[0].g=0;
        colors[0].b=0;
        
        // shape colors
        // Pink #D2315D
        colors[1].r=210;
        colors[1].g=49;
        colors[1].b=93;
        
        // Yellow #F7C808
        colors[2].r=247;
        colors[2].g=200;
        colors[2].b=8;
        
        // Aqua #22B5Bf
        colors[3].r=34;
        colors[3].g=181;
        colors[3].b=191;
        
        // Orange #E98813
        colors[4].r=233;
        colors[4].g=136;
        colors[4].b=19;
        
        // Green #88c134
        colors[5].r=136;
        colors[5].g=193;
        colors[5].b=52;
        
        // set the default sounds to chordal sounds
        sounds[0]=chordal_sounds[0];
        sounds[1]=chordal_sounds[1];
        sounds[2]=chordal_sounds[2];
        
    }
    else if(theme==GENERIC_THEME)
    {
        printf("Setting a Generic Theme \n");
        //background color -black
        colors[0].r=0;
        colors[0].g=0;
        colors[0].b=0;
        
        //shape colors
        //purple #90F
        colors[1].r=153;
        colors[1].g=0;
        colors[1].b=255;
        
        //lime green #CF0
        colors[2].r=204;
        colors[2].g=255;
        colors[2].b=0;
        
        //pink #F0C
        colors[3].r=255;
        colors[3].g=0;
        colors[3].b=204;
        
        //red #ff0037
        colors[4].r=255;
        colors[4].g=0;
        colors[4].b=55;
        
        
        //blue-purple #8813FF
        colors[5].r=136;
        colors[5].g=19;
        colors[5].b=255;
        
        // sounds
        // intialize the background board sound
        sounds[0]=generic_sounds[0];
        sounds[1]=generic_sounds[1];
        sounds[2]=generic_sounds[2];
    }

    else if(theme == RETRO_THEME)
    {
        printf("Changing Theme To: RETRO \n");
        //background color -black
        colors[0].r=0;
        colors[0].g=0;
        colors[0].b=0;
        
        //shape colors
        //Green #00ff00
        colors[1].r=0;
        colors[1].g=255;
        colors[1].b=0;
        
        //Orange #FF7400
        colors[2].r=255;
        colors[2].g=116;
        colors[2].b=0;
        
        //Cyan #00ffff
        colors[3].r=0;
        colors[3].g=255;
        colors[3].b=255;
        
        //yellow #f8ff00
        colors[4].r=248;
        colors[4].g=255;
        colors[4].b=0;
        
        //pink #FF00FF
        colors[5].r=255;
        colors[5].g=0;
        colors[5].b=255;
        
        // set the default sounds to retro sounds
        sounds[0]=retro_sounds[0];
        sounds[1]=retro_sounds[1];
        sounds[2]=retro_sounds[2];
        
    }
    else if(theme == RACE_THEME)
    {
        printf("Changing Theme To: RACE \n");
        //background color -black
        colors[0].r=0;
        colors[0].g=0;
        colors[0].b=0;
        
        // shape colors
            // Green #B4FF00
            colors[1].r=180;
            colors[1].g=255;
            colors[1].b=0;
            
            // Sea-Green #1EFF83 
            colors[2].r=30;
            colors[2].g=255;
            colors[2].b=131;
            
            // Sky-Blue #37E4F5
            colors[3].r=55;
            colors[3].g=228;
            colors[3].b=245;
            
            // Teal #DAc7c7
            colors[4].r=218;
            colors[4].g=199;
            colors[4].b=199;
            
            //shade teal #66FFE1
            colors[5].r=102;
            colors[5].g=255;
            colors[5].b=225;
        
        //set the default sounds to Race sounds
        sounds[0]=race_sounds[0];
        sounds[1]=race_sounds[1];
        sounds[2]=race_sounds[2];
        }
    else
    {
        printf("Invalid Theme");
        return;
    }
   
       
    sounds[0].play();
}

void HandJesture::setup() {
    trail_one = 0;
    trail_two = 0;
    intial_trail_one = 0;
    intial_trail_two = 0;
    //soundClick.loadSound("sound/4beat.mp3");
    //soundClick.setVolume(100);
    loadSounds();
	ofSetLogLevel(0);
	ofLog(OF_LOG_VERBOSE, "Start setup()");
    jestureFiredCount = 0;
	toNormalModeCount = 0;
	setTheme(current_theme);
    debug = false;
	showConfigUI = true;
	mirror = true;
    showUserFeedback=true;
    	
	// Setup Kinect
	angle = 5;
	//kinect.init(true);  //shows infrared image
	kinect.init();
	//kinect.setVerbose(true);
	kinect.open();
	kinect.setCameraTiltAngle(angle);
	
	// Setup ofScreen
	//ofSetFullscreen(true);
	ofSetFrameRate(30);
	ofBackground(0, 0, 0);
	ofSetWindowShape(800, 600);
	
	// For GAMEBOARD
	HandJesture::initShapeBoard();
	
	// For images
	grayImage.allocate(kinect.width, kinect.height);
	checkGrayImage.allocate(kinect.width, kinect.height);
	grayThresh.allocate(kinect.width, kinect.height);
	
	// For hand detection *these values are set in calibration*
	nearThreshold = 5;
	farThreshold = 30;
	detectCount = 0;
	detectTwoHandsCount = 0;
	
    //screen display values, increase to increase screen size
	displayWidth = 1280;
	displayHeight = 800;
	
	// Fonts
	msgFont.loadFont("Courier New.ttf",14, true, true);
	msgFont.setLineHeight(20.0f);
	
	blockFont.loadFont("Courier New.ttf",36, true, true);
	blockFont.setLineHeight(200.0f);
    
	/*
     try {
     socket.bind ("tcp:*:14444");
     s_sendmore (socket, "event");
     s_send (socket, "{type:\"up\"}");
     std::cout << "Open Zeromq socket" << endl;
     }
     catch (zmq::error_t e) {
     std::cerr << "Cannot bind to socket: " <<e.what() << endl;
     exit();
     }
     */
	
	// Sounds
	//soundDetect.loadSound("sound/16582__tedthetrumpet__kettleswitch1.aif");
	//soundDetect.setVolume(100);
	//soundRelease.loadSound("sound/2674__dmooney__TAPE32.wav");
	//soundRelease.setVolume(100);
	
	// setup config gui
	/*set up infromation for the setup screen*/
	gui.setup();
	gui.config->gridSize.x = 300;
	gui.addTitle("KINECT SETTINGS");
	gui.addSlider("Tilt Angle", angle, -30, 30);
	gui.addToggle("Mirror Mode", mirror);
	gui.addTitle("DETECT RANGE");
	gui.addSlider("Near Distance", nearThreshold, 5, 20);
	gui.addSlider("Far Distance", farThreshold, 20, 60);
	gui.addTitle("MOUSE CONTROLL");
	gui.addSlider("Display Width", displayWidth, 600, 1980);
	gui.addSlider("Display height", displayHeight, 600, 1980);
	gui.setDefaultKeys(true);
	gui.loadFromXML();
	gui.show();

    
}

//--------------------------------------------------------------
void HandJesture::update() {
    
	kinect.update();
	checkDepthUpdated();
	//prints gray pixels where the image is detected
	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	grayThresh.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	
	unsigned char * pix = grayImage.getPixels();
	unsigned char * grayThreshPix = grayThresh.getPixels();
	int numPixels = grayImage.getWidth() * grayImage.getHeight();
    
	int maxThreshold = 255 - nearThreshold;// default 250
	int minThreshold = 255 - farThreshold; // default 225
	
	int nearestDepth = 0;
	for(int i = 0; i < numPixels; i++){
		if (minThreshold < pix[i] && pix[i] < maxThreshold && pix[i] > nearestDepth) {
			nearestDepth = pix[i];
		}
	}
    
	for(int i = 0; i < numPixels; i++){
		//if( pix[i] < nearThreshold && pix[i] > farThreshold ){
		if(minThreshold < pix[i] 
           && pix[i] < nearestDepth+2 
           && pix[i] > nearestDepth - 10 ){
			//grayThreshPix[i] = floor((5 - (nearestDepth - pix[i]))/5.0f*255.0f);
			pix[i] = 255; // white
		}else{
			pix[i] = 0;
			//grayThreshPix[i] = 0;
		}
	}
	
	//update the cv image
	grayImage.flagImageChanged();
	//grayThresh.flagImageChanged();
	if (mirror) {
		grayImage.mirror(false, true);
	}
	//grayThresh.mirror(false, true);
	
    contourFinder.findContours(grayImage, 1500, (kinect.width*kinect.height)/4, 2, false);
	
	if (showConfigUI) {
		return;
	}
	
	int detectHands = contourFinder.nBlobs;
	
	if (detectHands == 2) {
		detectTwoHandsCount = min(60, ++detectTwoHandsCount);
	} else {
		detectTwoHandsCount = max(0, --detectTwoHandsCount);
	}
	
	if (detectHands > 0) {
		detectCount = min(50, ++detectCount);
	} else {
		detectCount = max(0, --detectCount);
	}
	
	if (detectingHands) {
		if (detectCount < 10) {
			detectingHands = false;
			sendEvent("UnRegister", "\"mode\":\"single\"");
			for (int j = 0; j < hands.size(); j++) {
				hands[j]->unRegister();
			}
			//soundRelease.play();
		}
	} else {
		if (detectCount > 30) {
			detectingHands = true;
			sendEvent("Register", "\"mode\":\"single\"");
			//soundDetect.play();
		}
	}
    
	//ofLog(OF_LOG_VERBOSE, ofToString(detectTwoHandsCount));
	if (detectingTwoHands) {
		//ofLog(OF_LOG_VERBOSE, "detecTwo");
		if (detectTwoHandsCount < 15) {
			detectingTwoHands = false;
			sendEvent("Register", "\"mode\":\"double\"");
		}
	} else {
		//ofLog(OF_LOG_VERBOSE, "Not...");
		if (detectTwoHandsCount > 30) {
			detectingTwoHands = true;
			sendEvent("Register", "\"mode\":\"double\"");
            
        }
	}
	
	if (detectingHands && detectHands == 1) {
        
		for (int i = 0; i < contourFinder.nBlobs; i++){
            
            
            
			int centerX = contourFinder.blobs[i].centroid.x;
			int centerY = contourFinder.blobs[i].centroid.y;
           
                        
			// Apply lowpass filter
			float x = centerX;
			float y = centerY;
			cornerCount = contourFinder.blobs[i].nPts;
			
			
			 centroidX = 0;
			 centroidY = 0;
			float addCount = 0;
			for (int j = 0; j < contourFinder.blobs[i].nPts; j+=5){
				addCount++;
				centroidX += contourFinder.blobs[i].pts[j].x;
				centroidY += contourFinder.blobs[i].pts[j].y;
			}
			centroidX = centroidX/addCount;
			centroidY = centroidY/addCount;			
			
            /*Contour finder to find and update the location of the hands*/
			if (hands.size() == 0) {
                Hand *hand = new Hand(true, displayWidth, displayHeight);
				hand->setIsActive(true);
				hand->update(ofPoint(x, y), cornerCount, ofPoint(x, y));
				hands.push_back(hand);
			} else {
				for (int j = 0; j < hands.size(); j++) {
					hands[j]->update(ofPoint(x, y), cornerCount, ofPoint(centroidX, centroidY));
				}
			}
            
			/*
             stringstream ss;
             ss  << "\"x\":"  << x
             << ",\"y\":" << y
             << ",\"z\":" << min(100, (int)kinect.getDistanceAt(centerX, centerY));
             //cout << "move: " << ss.str() << endl;
             sendEvent("Move", ss.str());
             */
		}
	}
}

void HandJesture::sendEvent(const std::string& etype, const std::string& edata) {
	/*
     s_sendmore (socket, "event");
     stringstream ss;
     ss << "{\"type\":\"" << etype << "\",\"data\":{" << edata << "}}";
     s_send (socket, ss.str());
     //std::cout << ss.str() << endl;
     */
}


/**
 * Check depth data is updated.
 *
 * If not updated, close and reopen the Kinect.
 */
void HandJesture::checkDepthUpdated(){
    if (ofGetFrameNum() % 150 == 0) {
		//ofLog(OF_LOG_VERBOSE, "check depth updated");
        unsigned char * nextDepth = kinect.getDepthPixels();
		
        if (ofGetFrameNum() != 150) {
			//ofLog(OF_LOG_VERBOSE, "Start compare depth pixels");
			unsigned char * currentDepthPixels = checkGrayImage.getPixels();
			
		    int pixNum = kinect.width * kinect.height;
            for (int i=0; i<pixNum; i++) {
                if (nextDepth[i] != currentDepthPixels[i]) {
                    break;
				}
				if (i > pixNum / 2) {
					ofLog(OF_LOG_ERROR, "Depth pixels could not be refreshed. Reset Kinect");
					kinect.close();
					kinect.open();
					kinect.setCameraTiltAngle(angle);
					break;
				}
			}                  
		}
		checkGrayImage.setFromPixels(nextDepth, kinect.width, kinect.height);
	}
}
//---------------------------Hand Trail Values--------------------------//
/*Takes an int i (hand) and stores the hand_location point in the array*/
void HandJesture::storeHandTrail(int i, ofPoint hand_location)
{
    //check for the for loop value 
    if (i == 0)
    {
        if(trail_one >= HAND_TRAIL_SIZE)
        {
            trail_one = 0;
            intial_trail_one = 20;
        }
        hand_trail_one[trail_one]=hand_location;
        trail_one+=1;
        intial_trail_one += 1;
        if(intial_trail_one >= HAND_TRAIL_SIZE)
        {
            intial_trail_one = HAND_TRAIL_SIZE;
        }
        //printf("Storing X: %f, Y: %f, at :%i \n",hand_location.x, hand_location.y,intial_trail_two);
    }
    else if(i == 1)
    {
        if(trail_two >= HAND_TRAIL_SIZE)
        {
            trail_two = 0;
            intial_trail_two = 20;
        }
        hand_trail_two[trail_two]=hand_location;
        trail_two+=1;
        intial_trail_two += 1;
        if(intial_trail_two >= HAND_TRAIL_SIZE)
        {
            intial_trail_two = HAND_TRAIL_SIZE;
        }
        //printf("Storing X: %f, Y: %f, at :%i \n ",hand_location.x, hand_location.y, intial_trail_two);
    }
    if((intial_trail_one >= HAND_TRAIL_SIZE) || (intial_trail_two >= HAND_TRAIL_SIZE))
    {
        resetHandTrail();
    }
}

/*runs a loop through each hand and prints the hand trail*/
void HandJesture::printHandTrail()
{
    //the set width for the circle trail behind the hand
    int trail_width = 7;
    for(int i =0; i<intial_trail_one; i++)
    {
       // printf("Print Hand Trail!!!!: %i \n",i);
        ofSetColor(0, 255, 0);
        ofNoFill(); 
        ofEllipse(hand_trail_one[i].x, hand_trail_one[i].y, trail_width, trail_width);
    }
    for(int k =0; k<intial_trail_two; k++)
    {
        //printf("Print Hand Trail: %i \n",k);
        ofSetColor(0, 0, 255);
        ofNoFill(); 
        ofEllipse(hand_trail_two[k].x, hand_trail_two[k].y, trail_width, trail_width);
    }
}

/*resets the hand trail values so they aren't printed anymore*/
void HandJesture::resetHandTrail()
{
    intial_trail_one = 0;
    intial_trail_two = 0;
}

//----------------------------------------------------------------------------------------------------------------
void HandJesture::draw() {
    	ofSetColor(255, 255, 255);
	
	if (showConfigUI==true ) {
        //draw a black background for the gui
        ofBackground(0,0,0);	
		kinect.drawDepth(400, 0, 400, 300);
		gui.draw();
		sounds[0].setVolume(0.00f);
		msgFont.drawString("Press Space Key to start.", 20, ofGetHeight()-60);
        
        
		ofPushMatrix();
		ofTranslate(400, 300, 0);
		glScalef(.6, .6, 1.0f); 
        for (int i = 0; i < contourFinder.nBlobs; i++){
            ofPushMatrix();
            contourFinder.blobs[i].draw(0,0);
            //draws the hand centering dot
			ofSetColor(255, 0 , 0);
            ofFill();
            ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 20, 20);
			
			 centroidX = 0;
			 centroidY = 0;
			float addCount = 0;
			for (int j = 0; j < contourFinder.blobs[i].nPts; j+=5){
				addCount++;
				centroidX += contourFinder.blobs[i].pts[j].x;
				centroidY += contourFinder.blobs[i].pts[j].y;
			}
			centroidX = centroidX/addCount;
			centroidY = centroidY/addCount;
			ofCircle(centroidX, centroidY, 10);

			ofPopMatrix();
        }
		ofPopMatrix();
	} 
	else {
        //set the background full screen
       // ofSetFullscreen(true);
    
        //get the elapsed time and switch the theme
        int time =ofGetElapsedTimeMillis();
        
       // printf("%d \n",time%1000);
      /*  if(time
           %300==0)
        {
            //printf("change theme");
            current_theme++;
            if(current_theme>max_number_themes)
            {
                current_theme=0;
            }
            setTheme(current_theme);
            //update the shape colors
            updateShapeColors();
        }*/
        //draw a theme background
        ofBackground(colors[0].r,colors[0].g,colors[0].b);	
        sounds[0].setVolume(0.25f);
        //if showUserFeedback is on show the kinect input
        if(showUserFeedback)
        {
            ofSetColor(55, 55 , 55);
            grayImage.draw(0, 0,screen_width, screen_height);
        }	

		//msgFont.drawString("Press Space Key to start.", 20, ofGetHeight()-60);
		
		ofPushMatrix();//play the backgorund sound
		//ofTranslate(400, 300, 0);
		glScalef(1, 1, 1.0f); 

        /*loop through each blob (hand) found*/
        for (int i = 0; i < contourFinder.nBlobs; i++){
            ofPushMatrix();
            
            //contourFinder.blobs[i].draw(0,0);
            
            //set the hand centering dot color (white)
			ofSetColor(30, 30,30);
            ofFill();
            //ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 20, 20);

			 centroidX = 0;
			 centroidY = 0;
            
			float addCount = 0;
            //sets the x and y value for the contours
			for (int j = 0; j < contourFinder.blobs[i].nPts; j+=5){
				addCount++;
				centroidX += contourFinder.blobs[i].pts[j].x;
				centroidY += contourFinder.blobs[i].pts[j].y;
                
                //check the speed of the hand movement
               // HandJesture::checkSpeedMove(centroidX, centroidY);
			}
            /*These are the values */
			centroidX = centroidX/addCount;
			centroidY = centroidY/addCount;
			
			/*scales points to the screen size*/
			centroidX = screen_width*(centroidX/620);
			centroidY = screen_height*(centroidY/460);
			
			if((prev_x ==0.0f)&&(prev_y==0.0f)){
				prev_x = centroidX;
				prev_y = centroidY;
			}
			
			//draw circle at the location of the hand
            ofSetColor(255, 255, 255);
            ofCircle(centroidX, centroidY, 20);
            
            ofPoint hand_location;
            hand_location.x=centroidX;
            hand_location.y=centroidY;
           
			HandJesture::drawShapes(i);
            
			//store the current hand location values into an array
            storeHandTrail(i,hand_location);
            //print the resedual hand location values
            //printHandTrail();
            
			prev_x = centroidX;
			prev_y = centroidY;
			 
			
            /*Gets a boolean value that indicates whether the hand has been made into a fist "clicked"*/
			HandJesture::checkClick(cornerCount,i);
            
            /*For test purposes only - print the location of the the X and Y location of each hand*/
			//printf("Hand Location X: %f Y: %f \n", centroidX,centroidY);
            ofPopMatrix();
        }
		//draw interactive shapes if there is no hand
		if(contourFinder.nBlobs==0)HandJesture::drawShapes(0);
		
		
		ofPopMatrix();
        
	}
    //set the font color
	ofSetColor(0, 0, 0);
    //print the videos frames per second
	//msgFont.drawString("fps: "+ ofToString(ofGetFrameRate()), 20, ofGetHeight()-40);
    
	ofNoFill();
}
//-----------------------------------------------------------------------------------
/*
 *print all the shape objects after checking interaction
 */
void HandJesture::drawShapes(int hand)
{	
	for(int i = 0; i<11 ; i++)
	{
        //setting the colors to fill for the shape
		ofSetColor(Shape::board[i]->getRed(), Shape::board[i]->getGreen(), Shape::board[i]->getBlue());
		ofFill();
		
		//hand-shape interaction
		if(Shape::board[i]->hoveredOver(centroidX, centroidY)){
			/*Shape::board[i]->createExplosion();/*/Shape::board[i]->collision_AntiMagnet(centroidX, centroidY);
		}
		
		//shape-shape interaction
		Shape::board[i]->move();
		Shape::board[i]->checkCollision(i);
		Shape::board[i]->updateTrail();

		//error checking
		if( Shape::board[i]->checkDamage()){
			current_theme++;
			if(current_theme>max_number_themes) {current_theme = 0;}
			setTheme(current_theme);
			updateShapeColors();
		}
		if(Shape::board[i]->locationError(11,true)){ 
			printf("shape location error [%i] (%f, %f)\n",i,Shape::board[i]->getLocation_x(),Shape::board[i]->getLocation_y());
			while(Shape::board[i]->locationError(11,true))
				Shape::board[i]->setLocation(float(ofRandom(0.0f,screen_width-SHAPE_SIZE_MAX)), float(ofRandom(0.0f,screen_height-SHAPE_SIZE_MAX)));
		}
		//friction physics
		Shape::board[i]->slow();
		
		//explosion process
		if(Shape::board[i]->isExploding())
		{
			for(int j = 0; j<10 ;j++){
				ofSetColor(Shape::board[i]->getRed(), Shape::board[i]->getGreen(), Shape::board[i]->getBlue(),
						   Shape::board[i]->bubbles[j]->alpha);
				ofCircle(Shape::board[i]->bubbles[j]->x, Shape::board[i]->bubbles[j]->y, Shape::board[i]->bubbles[j]->size);
			}
			if(Shape::board[i]->popBubbles())Shape::board[i]->doneExploding();
		}
		else
		{
		//draw the shape
		ofRect(Shape::board[i]->getLocation_x(),Shape::board[i]->getLocation_y(),
			   Shape::board[i]->getWidth(), Shape::board[i]->getHeight());
		}	
		//draw the tail
		int t =Shape::board[i]->index();
		float alpha = 0;
		do{
			alpha += 10;
			if(t>0)t--;
			else t = 9;
			ofSetColor(Shape::board[i]->getRed(), Shape::board[i]->getGreen(), Shape::board[i]->getBlue(),
					   alpha);
			ofRect(Shape::board[i]->trail[t]->x,Shape::board[i]->trail[t]->y,
				   Shape::board[i]->getWidth(), Shape::board[i]->getHeight());
			
		}while( t != Shape::board[i]->index());
		
		//write number of the square on the screen (just for testing but what do you think?)
		ofSetColor(0, 0, 0);
		ofFill();
		//shows words
		if(i==10)blockFont.drawString(string_themes[current_theme][Shape::board[i]->hit_count/(Shape::max_hits/10)],Shape::board[i]->getLocation_x()/*+(Shape::board[i]->getWidth()/2)*/,
									  Shape::board[i]->getLocation_y()+(Shape::board[i]->getHeight()*.75));
		//shows theme
		//if(i==10)blockFont.drawString(string_themes[current_theme][0],Shape::board[i]->getLocation_x()/*+(Shape::board[i]->getWidth()/2)*/,
		//							  Shape::board[i]->getLocation_y()+(Shape::board[i]->getHeight()*.75));
		//

	}
}
//-------------------------------------------------------------
//Do on program exit
void HandJesture::exit(){
	kinect.close();
	ofLog(OF_LOG_NOTICE, "Closing Kinect and Exiting");
}


//--------------------------------------------------------------
void HandJesture::keyPressed (int key)
{
	//ofLog(OF_LOG_VERBOSE, ofToString(key));
	switch (key)
	{
	
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
		case '<':		
		case ',':		
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
		//case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
		case '-':		
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
        case 'd':
            //debug mode on or off
            debug ? debug=false : debug=true;
            break;
		case 'r':
			// reboot kinect
			kinect.close();
			kinect.open();
			kinect.setCameraTiltAngle(angle);
            break;
        case 't':
            //changes the theme
            current_theme++;
            if(current_theme > max_number_themes) //if current_theme has reached the max # of themes-reset
            {
                current_theme = 0;
            }
            //set the new theme
            setTheme(current_theme);
            //update the shape colors
            updateShapeColors();
            break;
		case ' ':
			showConfigUI = !showConfigUI;
			if (showConfigUI) {
                //set the calibration window shape
				ofSetWindowShape(800, 600);
                
                //set the background sound to stop
                HandJesture::background_sound.stop();
                
			} else {
				ofSetWindowShape(1024, 768);
                
                //set the camera tilt
				kinect.setCameraTiltAngle(angle);
                
                //set the background sound to play
               // HandJesture::background_sound.play();
			}
			break;
        //show the corner user feedback screen
        case 's':
			showUserFeedback=!showUserFeedback;
			break;
        //move the kinect camera
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;     
		case OF_KEY_LEFT:
			if(Shape::max_hits>100)Shape::max_hits-=10;
			printf("max_hits=%i\n",Shape::max_hits);
			break;
		case OF_KEY_RIGHT:
			Shape::max_hits+=10;
			printf("max_hits=%i\n",Shape::max_hits);
			break;
        //move the kinect camera
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}
//-----------------------------------------------------------------------
/*Checks to see if the hand has been clicked*/
void HandJesture::checkClick(int cornerCount,int hand) {
	cornerCountHistory.push_back(cornerCount);
	if (cornerCountHistory.size() > 6) {
		cornerCountHistory.erase(cornerCountHistory.begin());
	} else {
		return;
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
        //printf("\n MOUSE DOWN \n \n");
		handMode = HAND_MODE_CLICK;
		//checkMouseDownCount = 0;
		//if(!Currently_Grabbing_One_Shape)
			for( int s=0; s<10; s++){
				if(!(Shape::board[s]->isGrabbed())&&(Shape::board[s]->hoveredOver(centroidX,centroidY)))
				{
					Shape::board[s]->grabShape(hand);
					Currently_Grabbing_One_Shape = true;
				}
			}
		return;
	}
	if (cornerNums > currentCornerNums + 150) {
		if (handMode == HAND_MODE_DRAG) {
			//fireMouseUp();
             printf("\n MOUSE UP \n \n");
		//	soundClick.play();
			handMode = HAND_MODE_NORMAL;
			for( int s=0; s<10; s++)
				Shape::board[s]->releaseShapeFrom(hand);
			Currently_Grabbing_One_Shape = false;
			return;
		} else if (handMode == HAND_MODE_CLICK) {
			//fireMouseClick();
			//soundClick.play();
			printf("\n MOUSE UP \n \n");
			for( int s=0; s<10; s++)
				Shape::board[s]->releaseShapeFrom(hand);
			Currently_Grabbing_One_Shape = false;
			handMode = HAND_MODE_NORMAL;
			return;
		}
	}
	if (handMode == HAND_MODE_CLICK) {
		checkMouseDownCount++;
		if (checkMouseDownCount > MOUSE_CLICK_FRAME) {
			handMode = HAND_MODE_DRAG;
           //  printf("\n MOUSE DOWN \n \n");
			//soundClick.play();
			checkMouseDownCount = 0;
			//if(!Currently_Grabbing_One_Shape)
				for( int s=0; s<10; s++){
					if(!(Shape::board[s]->isGrabbed())&&(Shape::board[s]->hoveredOver(centroidX,centroidY)))
					{
						Shape::board[s]->grabShape(1);
						Currently_Grabbing_One_Shape = true;
					}
				}
		}
	}
	return;
}
/*Check The Speed of the hand movement
 And set the speed of the background beats*/
void HandJesture::checkSpeedMove(float x, float y) {
    // removing code to update the speed of the beats
    float slowest_beat_speed = .75;
    float fastest_beat_speed = 1.25;
    float widthStep = ofGetWidth() / 3.0f;
	if (x >= widthStep && x < widthStep*2){
        float speed = 0.5f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*0.75f;
        //make sure speed doesn't fall above or below a certain threshold
        if(speed < slowest_beat_speed)
        {
            speed = slowest_beat_speed;
        }
        if(speed > fastest_beat_speed)
        {
            speed = fastest_beat_speed;
        }

		background_sound.setSpeed(speed);
        //printf("setting Background Beat Speed: %f \n",speed);
	} 

}
/*void HandJesture::setBeatSpeed(int x, int y, int button){
	// continuously control the speed of the beat sample via drag, 
	// when in the "beat" region:
	float widthStep = ofGetWidth() / 3.0f;
	if (x >= widthStep && x < widthStep*2){
		beats.setSpeed( 0.5f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*1.0f);
	} 
    
}*/

//--------------------------------------------------------------
void HandJesture::mouseMoved(int x, int y)
{}

//--------------------------------------------------------------
void HandJesture::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void HandJesture::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void HandJesture::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void HandJesture::windowResized(int w, int h)
{}

