#pragma once

#include "ofMain.h"
#include <iostream>
#include <fstream>
using namespace std;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);	
		void processOpenFileSelection(ofFileDialogResult openFileResult);
		ofFile TF;
		int n1, n2;
		int **k;
		int **s;
		int draw_flag = 0;
		int  water_flag=0;
};


