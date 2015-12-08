#include "stdafx.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// Needed on MsWindows
#include <windows.h>
#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>

// Download glut from: http://www.opengl.org/resources/libraries/glut/
#include "glut.h"
#include "float2.h"

//-------------------
//------GLOBAL-------
//-------------------
boolean keysPressed[256] = { false };
bool lineConnect = false;
int selected = -1;


class Curve
{
public:
	// return curve point at parameter t in [0,1]
	virtual float2 getPoint(float t) = 0;
	virtual float2 getDerivative(float t) = 0;
	virtual void drawControlPoints(){

	}

	virtual void addControlPoints(float x, float y){}

	virtual void deleteControlPoints(float x, float y){	}

	virtual void setColor(){}

	virtual void draw(int sel)
	{	
		glPointSize(4.0);
		if (sel == selected){
			glColor3d(0.0,0.0, 1.0);
		}
		else{
			setColor();
		}
		glBegin(GL_LINE_STRIP);
		for (float i = 0.00; i < 1.00; i+=0.005){
			float2 temp(getPoint(i));
			glVertex2d(temp.x, temp.y);
		}
		glEnd();
	}	
};

class PolyLine : public Curve {
public:
	std::vector<float2> points;

	float2 getPoint(float t) { 
		float pos = t*points.size();
		float2 r = points.at(pos);
		return r;
	}
	float2 getDerivative(float t) { return float2(); }
	
	void setColor(){
		glColor3d(1.0, 0.0, 0.0);
	}

	void draw(int sel){
		glPointSize(5);
		if (sel == selected){
			glColor3d(0.0, 0.0, 1.0);
		}
		else{
			setColor();
		}
		glLineWidth(3.0);
		glBegin(GL_LINE_STRIP);
		for (unsigned int i = 0; i < points.size() - 1; i++){
			glVertex2d(points.at(i).x, points.at(i).y);
			glVertex2d(points.at(i + 1).x, points.at(i + 1).y);
		}
		glEnd();
	}

	void addControlPoints(float x, float y){
		points.push_back(float2(x, y));
	}

	void deleteControlPoints(float x, float y){
		for (unsigned int i = 0; i < points.size(); i++){
			float2 position = points.at(i);
			if (position.x > x - 0.02 && position.x < x + 0.02 && position.y > y - 0.02 && position.y < y + 0.02){
				points.erase(points.begin() + i);
			}
		}
	}

	void drawControlPoints(){
		glPointSize(8.0);
		glColor3d(0.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < points.size(); i++){
			glVertex2d(points.at(i).x, points.at(i).y);
		}
		glEnd();
	}
};

class Freeform : public Curve
{
protected:
	std::vector<float2> controlPoints;
public:
	virtual float2 getPoint(float t) = 0;
	virtual void addControlPoints(float2 p)
	{
		controlPoints.push_back(p);
	}
	void drawControlPoints(){
		// draw points at control points
		glPointSize(8.0);
		glColor3d(0.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < controlPoints.size(); i++){
			glVertex2d(controlPoints.at(i).x, controlPoints.at(i).y);
		}
		glEnd();
	}
};



class BezierCurve : public Freeform
{
public:
	float2 getDerivative(float t) { return float2(); }

	void setColor(){
		glColor3d(1.0, 1.0, 0.0);
	}

	static double bernstein(int i, int n, double t) {
		if (n == 1) {
			if (i == 0) return 1 - t;
			if (i == 1) return t;
			return 0;
		}
		if (i < 0 || i > n) return 0;
		return (1 - t) * bernstein(i, n - 1, t)
			+ t  * bernstein(i - 1, n - 1, t);
	}

	void addControlPoints(float x, float y)
	{
		controlPoints.push_back(float2(x,y));
	}

	void deleteControlPoints(float x, float y){
		for (unsigned int i = 0; i < controlPoints.size(); i++){
			float2 position = controlPoints.at(i);
			if (position.x > x - 0.02 && position.x < x + 0.02 && position.y > y - 0.02 && position.y < y + 0.02){
				controlPoints.erase(controlPoints.begin() + i);
			}
		}
	}

	float2 getPoint(float t)
	{
		float2 r(0.0, 0.0); 
		// for every control point
		for (unsigned int i = 0; i < controlPoints.size(); i++){
			float weight = bernstein(i, controlPoints.size() - 1, t);
			r += controlPoints.at(i) * weight;
		}
		// compute weight using the Bernstein formula
		// add control point to r, weighted
		return r;
	}

};

class LagrangeCurve : public Freeform
{
public:
	std::vector<float> knots;
	float2 getDerivative(float t) { return float2(); }

	void setColor(){
		glColor3d(0.0, 1.0, 0.0);
	}

	float2 getPoint(float t)
	{
		float2 r(0.0, 0.0);
		float weight = 1;
		// for every control point
		// compute weight using the Lagrange formula
		// add control point to r, weighted
		for (unsigned int i = 0; i < knots.size(); i++){
			for (unsigned int j = 0; j < knots.size(); j++){
				if (i != j){
					weight *= (t - knots.at(j)) / (knots.at(i) - knots.at(j));
					
				}
			}
			r += controlPoints.at(i)*weight;
			weight = 1;
		}	
		return r;
	}

	void addControlPoints(float x, float y){
		controlPoints.push_back(float2(x,y));
		knots.clear();
		if (controlPoints.size() > 1) {
			for (unsigned int i = 0; i<controlPoints.size(); i++)
			{
				float ti = (float)i / (controlPoints.size() - 1);
				knots.push_back(ti);
			}
		}

	}

	void deleteControlPoints(float x, float y){
		for (unsigned int i = 0; i < controlPoints.size(); i++){
			float2 position = controlPoints.at(i);
			if (position.x > x - 0.02 && position.x < x + 0.02 && position.y > y - 0.02 && position.y < y + 0.02){
				controlPoints.erase(controlPoints.begin() + i);
			}
		}
		knots.clear();
		if (controlPoints.size() > 1) {
			for (unsigned int i = 0; i<controlPoints.size(); i++)
			{
				float ti = (float)i / (controlPoints.size() - 1);
				knots.push_back(ti);
			}
		}
	}
};

std::vector<Curve*> curves;

class Scene
{
public:
	void addObject(Curve* curve) {
		curves.push_back(curve);
		selected = curves.size() - 1;
	}
	~Scene() {
		for (unsigned int i = 0; i<curves.size(); i++)
			delete curves.at(i);
	}
	void draw() {
		for (unsigned int i = 0; i < curves.size(); i++){
			curves.at(i)->draw(i);
			curves.at(i)->drawControlPoints();
		}
	}

	void addControlPoints(float x, float y){
		for (unsigned int i = 0; i < curves.size(); i++){
			curves.at(i)->addControlPoints(x, y);
		}
	}
};

Scene scene;

void onMouse(int button, int state, int x, int y) {
	int viewportRect[4];
	glGetIntegerv(GL_VIEWPORT, viewportRect);
	float newx = (x * 2.0) / viewportRect[2] - 1.0;
	float newy = (-y * 2.0) / viewportRect[3] + 1.0;

	if (state == GLUT_UP){
		for (unsigned int i = 0; i < sizeof(keysPressed); i++){
			keysPressed[i] = false;
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){		

		if (keysPressed['d']){
			lineConnect = false;			
			curves.at(selected)->deleteControlPoints(newx, newy);
		}

		if (keysPressed['a']){
			lineConnect = !lineConnect;
		}
		
		if (keysPressed['p']){
			lineConnect = !lineConnect;
			if (lineConnect){
				PolyLine *polyline = new PolyLine();
				polyline->addControlPoints(
					newx, newy);
				scene.addObject(polyline);
			}
		}

		if (keysPressed['b']){
			lineConnect = !lineConnect;
			if (lineConnect){
				BezierCurve *bezier = new BezierCurve();
				bezier->addControlPoints(
					newx, newy);
				scene.addObject(bezier);
			}
		}

		if (keysPressed['l']){
			lineConnect = !lineConnect;
			
			if (lineConnect){
				LagrangeCurve *lagrange = new LagrangeCurve();
				lagrange->addControlPoints(
					newx, newy);
				scene.addObject(lagrange);
			}
		}

		if (lineConnect){
			curves.at(selected)->addControlPoints(
				newx, newy);
		}	
		else{
			for (unsigned int i = 0; i < curves.size(); i++){
				for (float j = 0.01; j < 1.00; j += 0.01){		
					float2 position = curves.at(i)->getPoint(j);
					if (position.x > newx - 0.2 && position.x < newx + 0.2 && position.y > newy - 0.2 && position.y < newy + 0.2){
						selected = i;
					}
				}
			}
		}
	
	}
	glutPostRedisplay();
}

void print(int x, int y, char*string){
	glRasterPos2f(x, y);
	int len = (int)strlen(string);
	for (int i = 0; i < len; i++)
	{
		glColor3d(0.0, 0.0, 0.0);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, string[i]);
	}
}


void onReshape(int winWidth, int winHeight) {
	glViewport(0, 0, winWidth, winHeight);
}

void spaceSelect(){
	if (selected < curves.size()-1){
		selected += 1;
	}
	else { selected = 0; }
	glutPostRedisplay();
}

void onKeyPressed(unsigned char key, int x, int y){
	keysPressed[key] = true;
	if (key == ' '){
		lineConnect = false;
		spaceSelect();
	}
}

void onKeyReleased(unsigned char key, int x, int y){
	
}

void onIdle(){
	glutPostRedisplay();
}

void onDisplay() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	print(-1.0, -1.0, "Press a key to toggle actions. (e.g. 'p' turns on polyline, then turns off) Press 'd' once to delete a control point.");
	scene.draw();
	glutSwapBuffers();                     		// Swap buffers for double buffering

}

int main(int argc, char *argv[]){
	glutInit(&argc, argv);                 		// GLUT initialization
	glutInitWindowSize(640, 480);				// Initial resolution of the MsWindows Window is 600x600 pixels 
	glutInitWindowPosition(100, 100);            // Initial location of the MsWindows window
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);    // Image = 8 bit R,G,B + double buffer + depth buffer

	glutCreateWindow("Curves Editor");        	// Window is born
	
	glutIdleFunc(onIdle);
	glutDisplayFunc(onDisplay);
	glutKeyboardFunc(onKeyPressed);
	glutKeyboardUpFunc(onKeyReleased);
	glutMouseFunc(onMouse);
	glutMainLoop();                    			// Event loop  
	return 0;

}