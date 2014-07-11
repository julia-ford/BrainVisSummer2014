
#include <cassert>
#include <iostream>
#include <gl/glew.h>
#include <gl/glut.h>
#include "colorscheme.h"
#include "displaymanager.h"
#include "keymouselog.h"
#include "myGLexp.h"
#include "mystdexp.h"
#include "selectionboxmanager.h"
#include "trackball.h"
#include "triallog.h"
using namespace std;
using namespace mystdexp;

/*
	1) set training or study in testbed.cpp
	2) set DATARESOLUTION in trialManager.cpp
*/

int width  = 1920; /**< default screen width */
int height = 1140; /**< default screen height */

int tButton = -1;
int tState  = -1;

float buttonWidthScale = 1/12.f;
float buttonHeightScale = 3/8.f;
float barLegendWidthScale = 1/48.f;
float barLegendHeightScale = 0.618f;
float barLegendXOffsetScale = 6.8/8.f;
float mapLegendWidthScale = 1/6.f;
float mapLegendXOffsetScale = 0.02f;
float mapLegendYOffsetScale = 4.5/6;
float initZoom = -180.f;
vec3 panOffset(0,0,0);
vec3 pressedLoc;
vec3 tmpOffset(0,0,0);
vec3 panStep(3,3,3);

// Program configuration options.
bool debugMode  = true;
bool isTraining = false;

int participantIdx = 0; /**< index of the current participant */
int  startTrialIdx = 0; /**< index of the first trial to do */

#define TRAININGBUNDLEQUEST 5
// do not change
bool isMousePressedInBox = false;


// material
GLfloat materialAmbient  [] = {0.0,0.0,0.0,1};
GLfloat materialDiffuse  [] = {0.8,0.8,0.8,1};
GLfloat materialSpecular [] = {0.2,0.2,0.2,1};
GLfloat materialShininess[] = {50};

DisplayManager displayManager;
SelectionBoxManager selBoxManager;
TrialLog trialLog;
KeyMouseLog keyMouseLog;

// things used for lesion task

GLdouble minX,minY,maxX,maxY;
int markerX, markerY;
bool markerValid = false;
string markerString = "X";
int markerSize = 33;
#define TRAININGBUNDLEQUEST 5

/// Move the marker to the specified screen coordinates.
/// Also marks the marker as valid.
/// @param x the x coordinate for the marker
/// @param y the y coordinate for the marker
void placeMarker(int x,int y){
	markerX = x;
	markerY = y;
	markerValid = true;
}

/// "Erases" the marker by marking it as invalid.
/// Does not change the markers coordinates.
void eraseMarker(){
	markerValid = false;
}

/// Determines the screen coordinates of the "currect" location.
/// @param minX where to store the minimum x coordinate of the correct area
/// @param minY where to store the minimum y coordinate of the correct area
/// @param maxX where to store the minimum x coordinate of the correct area
/// @param maxY where to store the minimum y coordinate of the correct area
void getCorrectLoc(GLdouble &minX, GLdouble &minY, GLdouble &maxX, GLdouble &maxY){
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLdouble ax[8],ay[8], az;
	// min and max
	vec3 correctLoc[2];
	correctLoc[0] = displayManager.GetCurrentTrialDataPtr()->GetCorrectAnswerMinLocation();
	correctLoc[1] = displayManager.GetCurrentTrialDataPtr()->GetCorrectAnswerMaxLocation();
	glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,width/(float)height,1,1000);
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();{
		tbMatrix();
		
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);

		for(int i = 0;i<8;i++){
			int xIdx = i/4;
			int yIdx = (i/2)%2;
			int zIdx = i%2;
			gluProject( correctLoc[xIdx].x,correctLoc[yIdx].y,correctLoc[zIdx].z,
					modelview, projection, viewport, 
					&ax[i], &ay[i], &az);
		}
	}glPopMatrix();
	
	minX = minY = 99999.f;
	maxX = maxY = -99999.f;
	for(int i = 0;i<8;i++){
		if(ax[i]<minX){
			minX = ax[i];
		}
		if(ax[i]>maxX){
			maxX = ax[i];
		}
		if(ay[i]<minY){
			minY = ay[i];
		}
		if(ay[i]>maxY){
			maxY = ay[i];
		}
	}
}

/// Checks if the marker is in a valid position.
/// @param minX the minimum x coordinate for the marker
/// @param minY the minimum y coordinate for the marker
/// @param maxX the maximum x coordinate for the marker
/// @param maxY the maximum y coordinate for the marker
bool isMarkerCorrect(GLdouble minX, GLdouble minY, GLdouble maxX, GLdouble maxY){
	if(markerX < minX){ return false; }
	if(markerX > maxX){ return false; }
	if(height-markerY < minY){ return false; }
	if(height-markerY > maxY){ return false; }
	return true;
}

/// Draws a 2D box.
/// @param minX the left edge of the box
/// @param minY the bottom edge of the box
/// @param maxX the right edge of the box
/// @param maxY the top edge of the box
void draw2DBox(GLdouble minX, GLdouble minY, GLdouble maxX, GLdouble maxY){
	glPushAttrib(GL_VIEWPORT_BIT | GL_LIGHTING_BIT);{
		glViewport(0,0,width,height);
		glDisable(GL_LIGHTING);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();{
			glLoadIdentity();
			glOrtho(0,width,0,height,-5,5);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();{
				glLoadIdentity();
				glBegin(GL_LINE_LOOP);{
					glVertex2f(minX,minY);
					glVertex2f(maxX,minY);
					glVertex2f(maxX,maxY);
					glVertex2f(minX,maxY);
				}glEnd();
			}glPopMatrix();
			glMatrixMode(GL_PROJECTION);
		}glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}glPopAttrib();
}

// end things used for lesion task

void updateBoxes(){
	MriTask task = displayManager.GetCurrentTrialInfoPtr()->GetMriTask();
	if(displayManager.GetCurrentTrialInfoPtr()->IsEmpty() || displayManager.GetCurrentTrialInfoPtr()->GetQuestIndex() == TRAININGBUNDLEQUEST){
		selBoxManager.SetNumBoxes(0);
	}
	else if(task == FA || task == BUNDLE_TRACE){
		int numBoxes = displayManager.GetCurrentTrialDataPtr()->GetNumChoices();
		if(task == FA){
			selBoxManager.SetNumBoxes(numBoxes+1);
			selBoxManager.SetString("Similiar",numBoxes);
		}
		else selBoxManager.SetNumBoxes(numBoxes);
	}
	else if(task == BUNDLE_SAME){
		selBoxManager.SetNumBoxes(2);
		selBoxManager.SetString("NO",0);
		selBoxManager.SetString("YES",1);
	}
	else if(task == BUNDLE_NAME){
		selBoxManager.SetNumBoxes(5);
		selBoxManager.SetString("CC",0);
		selBoxManager.SetString("CST",1);
		selBoxManager.SetString("ILF",2);
		selBoxManager.SetString("IFO",3);
		selBoxManager.SetString("CG",4);
	}
	else{
		assert(task == LESION);
		selBoxManager.SetNumBoxes(0);
	}
}

void resetView(){
    tbInit(GLUT_LEFT_BUTTON);
	zoom = 5;
	tbZoom(initZoom);
	tmpOffset.set(0,0,0);
	panOffset.set(0,0,0);
}

void goToNextTrial(){
	// can only end after it is started
	// i.e has rendered at least one frame
	if(trialLog.IsStarted()){

		if(displayManager.GetCurrentTrialInfoPtr()->GetMriTask() == LESION){
			if(markerValid){

			}
			else if(displayManager.GetCurrentTrialInfoPtr()->IsEmpty()){
			}
			else{
				return;
			}
		}
		trialLog.EndTrial(selBoxManager.GetFinnalAnswer(),isMarkerCorrect(minX,minY,maxX,maxY));

		// all ends for one participant
		if(displayManager.IsLastDisplay()){
			trialLog.EndParticipant();
			exit(0);
		}
		displayManager.NextDisplay();
		displayManager.Update();
		//displayManager.PrintTrialInfo();
		updateBoxes();
		trialLog.NextTrial(displayManager.GetCurrentTrialInfoPtr(),
			displayManager.GetCurrentTrialDataPtr());
		// reset view
		resetView();
		eraseMarker();
	}
}


void goToPreviousTrial(){
	trialLog.EndTrial(selBoxManager.GetFinnalAnswer());

    displayManager.PreviousDisplay();
	displayManager.Update();
	//displayManager.PrintTrialInfo();
	updateBoxes();
	trialLog.NextTrial(displayManager.GetCurrentTrialInfoPtr(),
		displayManager.GetCurrentTrialDataPtr());
}

/** This function appears to initialize the lighting on the 3D structures. */
void initLight(){
	// These variables appear to be declared to explain magic numbers.
    float constant  = 1;
    float linear    = 1;
    float quadratic = 1;
    float position[] = {0.0,0.0,1.0,0};
    float ambient [] = {0.0,0.0,0.0,1};
    float diffuse [] = {0.9,0.9,0.9,1};
	float specular[] = {0.9,0.9,0.9,1};

	// TODO: Why is this commented out?
    //glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, constant );
    //glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, linear );
    //glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadratic );

	// These functions are from glew.h.
    glLightfv( GL_LIGHT0, GL_POSITION, position );
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
	glEnable ( GL_LIGHT0 );
}

void drawGround(){
	float halfWidth = 180;
	float halfHeight = 420;
	float yOffset = -90;

	// draw floor
	glColor3f(0.8,0.8,0.8);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_QUADS);{
		glNormal3f(0,1,0);
		glVertex3f(-halfWidth,yOffset,-halfHeight);
	
		glNormal3f(0,1,0);
		glVertex3f(halfWidth,yOffset,-halfHeight);
	
		glNormal3f(0,1,0);
		glVertex3f(halfWidth,yOffset,halfHeight);
	
		glNormal3f(0,1,0);
		glVertex3f(-halfWidth,yOffset,halfHeight);
	}glEnd();

	// draw grid
	int numxgrid = 15;
	int numygrid = 15;
	float tinyOffset = 0.001;
	glColor3f(1,1,1);
	glLineWidth(3);
	for(int i = 0;i<=numxgrid;i++){
		float stepx = i*halfWidth/numxgrid*2-halfWidth;
		glBegin(GL_LINES);{
			glVertex3f(stepx,yOffset+tinyOffset,-halfHeight);
			glVertex3f(stepx,yOffset+tinyOffset,halfHeight);
		}glEnd();
	}
	
	for(int i = 0;i<=numygrid;i++){
		float stepy = i*halfHeight/numygrid*2-halfHeight;
		glBegin(GL_LINES);{
			glVertex3f(-halfWidth,yOffset+tinyOffset,stepy);
			glVertex3f(halfWidth,yOffset+tinyOffset,stepy);
		}glEnd();
	}
}

void draw(){
	// Background color will be gray.
    glClearColor(0.5,0.5,0.5,0.0);

	// Clear the screen, using the color given above,
	// and without clearing the foreground.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   materialAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   materialDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  materialSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	// the main part
	glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,width/(float)height,1,1000);
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();{
		vec3 offset = panOffset+tmpOffset;
		
		 // TODO: Why is this commented out?
		//task 1 2: for CG whole
		//glRotatef(-270,0,0,1);
		//glRotatef(-90,0,1,0); 
		 //task 3 4 for CG whole glRotatef(270,0,0,1); glRotatef(-90,0,1,0);
		 //for CC WHOLE shape X encoding 
		//keqin_1_1//use IFO whole
		//glScalef(1.2,1.2,1.2); glRotatef(-30,1,0,0);  glRotatef(270,0,0,1); glRotatef(-90,0,1,0); 
		
		//keqin_1_2:task3, use ILF WHOLE
		//glScalef(1.2,1.2,1.2); glRotatef(-270,0,0,1); glRotatef(90,0,1,0);
		
		//keqin_2 IFO BUNDLE// glScalef(1.2,1.2,1.2); glRotatef(-270,0,0,1); glRotatef(90,0,1,0);

		glTranslatef(offset.x,offset.y,offset.z);
		tbMatrix();
		displayManager.Render();
	}glPopMatrix();

	
	// and the ground
	glEnable(GL_LIGHTING);
	glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,width/(float)height,1,1000);
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();{
		glLoadIdentity();
		gluLookAt(0,0,1,0,0,0,0,1,0);
		drawGround();
	}glPopMatrix();
	glDisable(GL_LIGHTING);

	// marker for lesion task
	if(displayManager.GetCurrentTrialInfoPtr()->GetMriTask() == LESION){
		getCorrectLoc(minX,minY,maxX,maxY);
		if(markerValid){
			glColor3f(1,0,0);
			myGLexp::myGLPrintString(markerX,height-(markerY+markerSize/2),markerString,1,markerSize);

			if(displayManager.GetCurrentTrialInfoPtr()->IsTraining() || debugMode){
				// Green marker if correct.
				if(isMarkerCorrect(minX,minY,maxX,maxY)){ glColor3f(0,1,0); }
				// Red marker if wrong.
				else { glColor3f(1,0,0); }
				// Draw marker either way.
				draw2DBox(minX,minY,maxX,maxY);
			}
		}
	}

	// the answer
	glColor3f(1,1,1);
	glLineWidth(2);
	bool _isTraining = displayManager.GetCurrentTrialInfoPtr()->IsTraining();
	int _currentSel = selBoxManager.GetCurrentSelection();
	int _numSel = selBoxManager.GetNumBoxes();
	if(( _isTraining || debugMode)
		&& (_currentSel>=0 || _numSel ==0)){

		if(displayManager.GetCurrentTrialInfoPtr()->GetMriTask() != LESION 
			&& !displayManager.GetCurrentTrialInfoPtr()->IsEmpty()){
			if(_currentSel!=displayManager.GetCurrentTrialDataPtr()->GetCorrectAnswer()){
				myGLexp::myGLPrintString(10,height/2,"Correct Answer: "+
					displayManager.GetCurrentTrialDataPtr()->GetCorrectAnswerString(),
					0,30);

				if(displayManager.GetCurrentTrialInfoPtr()->GetMriTask() == FA){
					float diff = displayManager.GetCurrentTrialDataPtr()->GetDifficulty();
					myGLexp::myGLPrintString(10,height/2-35,"Difference: Box1 - Box2:",0,30);
					myGLexp::myGLPrintString(10,height/2-70,convertFloat(diff),0,30);
					myGLexp::myGLPrintString(10,height/2-105,"Similiar threshold 0.08.",
					0,30);
				}
			}
			else{
				myGLexp::myGLPrintString(10,height/2,"Correct! Good Job!",
					0,30);
			}
		}
	}

	// encoding hint
	if(	displayManager.GetCurrentTrialInfoPtr()->GetQuestIndex() != TRAININGBUNDLEQUEST
		&& !displayManager.GetCurrentTrialInfoPtr()->IsEmpty()){
		glColor3f(1,1,1);
		myGLexp::myGLPrintString(width-30,height-30,
			"Current Encoding: "+toString(displayManager.GetCurrentTrialInfoPtr()->GetRetinalChannel()),
				2,20);
	}

	//question string
	glColor3f(1,1,1);
	if(displayManager.GetCurrentTrialInfoPtr()->IsEmpty()){
		myGLexp::myGLPrintString(width/2,height/2,displayManager.GetQuestionString(),
			1,20);
	}
	else myGLexp::myGLPrintString(width/2,20,displayManager.GetQuestionString(),
		1,20);

	// TODO: Why is this commented out?
	// progress
	//myGLexp::myGLPrintString(width-55,35,
	//	convertInt(displayManager.GetCurrentTrialIndex()+1)+"/"+
	//	convertInt(displayManager.GetNumTrialsPerparticipant()),1,20);
	if(displayManager.GetCurrentTrialInfoPtr()->GetQuestIndex() != TRAININGBUNDLEQUEST&&
		!displayManager.GetCurrentTrialInfoPtr()->IsEmpty()){
		myGLexp::myGLPrintString(width-55,35,displayManager.GetProgressString(),2,20);
	}


	// the map axes
	int legendSize = width<height?width:height;
	legendSize*=mapLegendWidthScale;
    glViewport(mapLegendXOffsetScale*width,mapLegendYOffsetScale*height,
		legendSize,legendSize);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glMatrixMode(GL_PROJECTION);
	glPushMatrix();{
		glLoadIdentity();
		gluPerspective(45,1,1,1000);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();{
			// cancel the zooming by trackball
		
			glTranslatef(0, 0, -zoom);
			tbMatrix();
			glLineWidth(2);
			// TODO: Why is this commented out?
			//glScalef(2,2,2);
			displayManager.RenderAxes();
		}glPopMatrix();
		glMatrixMode(GL_PROJECTION);
	}glPopMatrix();

	// draw bar legend
    glViewport(width*barLegendXOffsetScale,(1-barLegendHeightScale)/2*height,
		barLegendWidthScale*width,barLegendHeightScale*height);
    glMatrixMode(GL_PROJECTION);
	glPushMatrix();{
		glLoadIdentity();
		glOrtho(-1,1,-1,1,-5,5);
		// TODO: Why is this commented out?
		//gluPerspective(60,1,1,1000);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();{
			glLoadIdentity();
			gluLookAt(0,0,1,0,0,0,0,1,0);
			displayManager.RenderLegend();
		}glPopMatrix();
		glMatrixMode(GL_PROJECTION);
	}glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glDisable( GL_COLOR_MATERIAL ) ;

	// buttons
	selBoxManager.SetPosition(vec3(width*(1-buttonWidthScale),0.2*height,0),
		vec3(width*buttonWidthScale,height*buttonHeightScale,1));
	selBoxManager.Render();

    glutSwapBuffers();
	trialLog.StartTrial();
}

/// This appears to be an event triggered by any mouse button being pressed.
/// @param button indicates which button was pressed to trigger the event
/// @param state ???? I dunno
/// @param x the x coordinate of the mouse on the screen
/// @param y the y coordinate of the mouse on the screen
void mousePress(int button, int state, int x, int y){
	// Make a record of the event happening.
	keyMouseLog.MousePressed(button,state,x,y);

	// Update global variables with most recently pressed buttton and state.
	tButton = button;
	tState = state;

	// Tell the selection box manager about the event.
	selBoxManager.MousePress(tButton,tState,x,height-y);

	if(selBoxManager.IsIn(x,height-y) && state == GLUT_DOWN){
		isMousePressedInBox = true;
		trialLog.EndTimeing();
	}
	else{
		isMousePressedInBox = false;
	}
	if(selBoxManager.HasAnswered()){
		goToNextTrial();
	}
	if(tButton == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN){
		pressedLoc.set(x,y,0);
	}
	if(tButton == GLUT_MIDDLE_BUTTON && state == GLUT_UP){
		panOffset += tmpOffset;
		tmpOffset.set(0,0,0);
	}
	tbMouse(button, state, x, y);
	// TODO: Why is this commented out?
	//if(selBoxManager.NeedRepaint()){
		glutPostRedisplay();
	//}
}

/// This is an event triggered when a user clicks and drags on the screen.
/// This causes the tracball stuff to make the 
/// @param x the x coordinate of the mouse on the screen
/// @param y the y coordinate of the mouse on the screen
void mouseDrag(int x,int y){
	if(!isMousePressedInBox){
		tbMotion(x, y, tButton);
		keyMouseLog.MouseDrag(x,y);
	}
	if(tButton == GLUT_MIDDLE_BUTTON){
		tmpOffset.set(x-pressedLoc.x,y-pressedLoc.y,0);
		tmpOffset.scale(0.1,-0.1,0);
	}
	glutPostRedisplay();
	eraseMarker();
}

void mouseMove(int x,int y){
	// TODO: Why is this commented out?
	//keyMouseLog.MouseMove(x,y);
	selBoxManager.MouseMove(tButton,tState,x,height-y);
	if(selBoxManager.NeedRepaint()){
		glutPostRedisplay();
	}
}
// TODO: Why is this commented out?
/*
extern GLfloat zoom;
extern GLfloat tbx;
extern GLfloat tby;
extern GLfloat tb_transform[4][4];
*/
void saveView(){
	ofstream viewFile("view.txt");
	viewFile << zoom << endl
		<< tbx << endl
		<< tby <<endl
		<< panOffset.x << endl
		<< panOffset.y << endl
		<< panOffset.z << endl
		<< tmpOffset.x << endl
		<< tmpOffset.y << endl
		<< tmpOffset.z << endl;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			viewFile << tb_transform[i][j] << endl;
	viewFile.close();
}

void loadView(){
	ifstream viewFile("view.txt");
	viewFile >> zoom
		>> tbx
		>> tby
		>> panOffset.x
		>> panOffset.y
		>> panOffset.z
		>> tmpOffset.x
		>> tmpOffset.y
		>> tmpOffset.z;
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			viewFile >> tb_transform[i][j];
	viewFile.close();
}

void keyPress( unsigned char key, int _x, int _y ){
	keyMouseLog.KeyPressed(key,_x,_y);
    switch ( key ){
        case 27:            // Esc
            exit(0);
			break;
        case ' ':
			if(debugMode){
				goToPreviousTrial();
			}
			break;
        case 'q':
        case 'Q':
			placeMarker(_x,_y);
			break;
        case 'z':
        case 'Z':
			tbZoom(20);
			break;
        case 'x':
        case 'X':
			tbZoom(-20);
			break;
			// TODO: Why is this commented out?
   //     //case GLUT_KEY_UP:
   //     case 'w':
   //     case 'W':
			//panOffset.y += panStep.y;
			//break;
   //     //case GLUT_KEY_DOWN:
   //     case 's':
   //     case 'S':
			//panOffset.y -= panStep.y;
			//break;
   //     //case GLUT_KEY_LEFT:
   //     case 'a':
   //     case 'A':
			//panOffset.x -= panStep.x;
			//break;
   //     //case GLUT_KEY_RIGHT:
   //     case 'd':
   //     case 'D':
			//panOffset.x += panStep.x;
			//break;
        case 'h':
        case 'H':
			resetView();
			break;
		case 'k':
		case 'K':
			saveView();
			break;
		case 'l':
		case 'L':
			loadView();
			break;
    };

    glutPostRedisplay();
}

void keyRelease( unsigned char key, int _x, int _y ){
	keyMouseLog.KeyReleased(key,_x,_y);
}

/** This function sets up some of the GL stuff, and also the trackball. */
void glInit(){
	// Setup for the trackball class from trackball.h.
    tbAnimate(GL_TRUE);
    tbInit(GLUT_LEFT_BUTTON);

	// These functions are from glew.h.
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	// This function is defined in this file, testbed.cpp.
	initLight();

	// From glew.h.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,(float)width/height,1,1000);

	// From GLU.h.
    glMatrixMode(GL_MODELVIEW);

	// From trackball.cpp.
	// Making the initial view good.
	tbZoom(initZoom);
}

/// Updates the content of the window whenever the window size is changed.
/// @param _width the new width of the window
/// @param _height the new height of the window
void reshape(int _width, int _height){
	// Adjust the global variables.
    width  = _width;
    height = _height;

	// Adjust the trackball (from trackball.cpp) to fit.
    tbReshape(width, height);

	// Readjust the GL stuff to the new window size.
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,(float)width/height,1,1000);
    glMatrixMode(GL_MODELVIEW);
}

/// The main function which is run at startup.
/// @param argc "argument count," the number of command-line arguments read
/// @param argv the array containing the arguments
int main(int argc, char** argv){

	// This block checks the number of arguments read, and requests more
	// information if less than 2 arguments were given.
	if(argc <= 1){

		// The next four code lines print messages to the console.
		// "<< endl" adds a newline character.
		std::cout << "Please type in the participant idx" << endl;
		std::cout << "\ttestbed.exe <pIdx> <trialIdx>" << endl;
		std::cout << "\tpIdx: participant Idx" << endl;
		std::cout << "\ttrialIdx: startTrial Idx, default 0" << endl;
		// In spite of the request for "idx" (which I think means ID #),
		// the above part of the code does not accept input.

		// "debugMode" is a variable set at the top of the code
		if(!debugMode){ exit(0); }

	} // End of argument count checking.

	// This stuff sets up the glut environment.
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow( "Ranking" );
	glutDisplayFunc( draw );
    glutKeyboardFunc(keyPress);
	glutKeyboardUpFunc(keyRelease);
    glutMouseFunc(mousePress);
    glutMotionFunc(mouseDrag);
	glutPassiveMotionFunc(mouseMove);
    glutReshapeFunc(reshape);

	// No full-screen in debug mode.
    if(!debugMode){ glutFullScreen(); }

	// Defined in this file; does GL setup.
	glInit();

	// Defined in glew.h; does glew setup.
	glewInit();

	// Let the display manager know what modes this is running in.
	displayManager.SetTraining(isTraining);
	displayManager.SetStudy(!isTraining);
	displayManager.SetDebugMode(debugMode);

	// The directories where the data to be loaded is found.
	string  dataDir = "RankingData/traces/data";
	string traceDir = "RankingData/templates";

	// Decide what data to load based on the mode being run.
	if(!isTraining) {
		// Used for everything.
		displayManager.LoadData(dataDir.c_str(),
			(traceDir+"/normal_s3_switched.tensorinfo").c_str(),
			(traceDir+"/forallfb.data").c_str());
		// Used only for Boys Surface Color Schemes.
		BoysSurfaceColorScheme::LoadFromFile(
			(traceDir+"/normal_s3_boy.data").c_str());
		// Used only for Boys Surface Color Schemes.
		displayManager.LoadLegend((traceDir+"/boysLegend").c_str());
	} else {
		// Used for everything.
		displayManager.LoadData(dataDir.c_str(),
			(traceDir+"/normal_s4_switched.tensorinfo").c_str(),
			(traceDir+"/forallfb.data").c_str());
		// Used only for Boys Surface Color Schemes.
		BoysSurfaceColorScheme::LoadFromFile(
			(traceDir+"/normal_s4_boy.data").c_str());
		// Used only for Boys Surface Color Schemes.
		displayManager.LoadLegend((traceDir+"/boysLegend").c_str());
	} displayManager.BuildDataBase();

	// If there is more than one command line argument, parse argument #1
	// as an integer, and use it as the participant index.
	if(argc>1) { participantIdx = atoi (argv[1]); }

	// If there are more than two command line arguments, parse argument #2
	// as an integer, and use it as the index of the first trial to run.
	if(argc>2) {  startTrialIdx = atoi (argv[2]); }

	// Tell the display manager the index of the participant and the index
	// of the first trial to run.
	displayManager.SetParticipantIndex(participantIdx);
	displayManager.SetTrialIndex(startTrialIdx);

	// Update the display manager so that it uses the new values, and print
	// the trial information.
	displayManager.Update();
	displayManager.PrintTrialInfo();
	
	// Determine where the selection boxes should go on the screen and update
	// them to reflect the trial info & new position.
	selBoxManager.SetPosition(vec3(width*(1-buttonWidthScale),0.2*height,0),
		vec3(width*buttonWidthScale,height*buttonHeightScale,1));
	updateBoxes();

	// Load up the first trial.
	trialLog.StartParticipant(participantIdx,isTraining);
	trialLog.NextTrial(displayManager.GetCurrentTrialInfoPtr(),
		displayManager.GetCurrentTrialDataPtr());
	trialLog.SetDebugOut(debugMode);

	// Prepare the output file.
	if(isTraining) {
		keyMouseLog.SetOutput("Eventlog_"+convertInt(participantIdx)+".log");
	} else {
		keyMouseLog.SetOutput("TrainingEventlog_"+convertInt(participantIdx)+".log");
	}
	keyMouseLog.SetDebugOut(debugMode);

	// This is defined in glut.h.
    glutMainLoop();
}