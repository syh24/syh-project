/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
#include <stack>
#include <queue>


using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isbfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS",false,false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if(title == "Open") {
		readFile();
	}
	if(title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if(title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			isdfs = 1;
			draw_flag = 1;
			DFS();
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
		
	}

	if(title == "Show BFS") {
		if (isOpen) {
			BFS();
			draw_flag = 1;
			isbfs = 1;
			doTopmost(bChecked); // Use the checked value directly
		}
		else
			cout << "you must open file first" << endl;
	}

	if(title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if(title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	
	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here
	if (draw_flag) {
		int da = 5, db = 5;
		for (i = 0; i < maze_row; i++) {
			for (j = 0; j < maze_col; j++) {
				if (input[i][j].p == '1' && i%2 == 0 && j%2 == 1)
				{
					ofDrawLine(da, db, da + 40, db);
					da += 40;
				}
				if (input[i][j].p == '1' && i%2 == 1)
				{
					ofDrawLine(da, db, da, db + 40);
					if (j == maze_col - 1) db += 40;
				}
				if (input[i][j].p == '0')
				{
					if (j % 2) da += 40;
				}
			}
			da = 5;	
		}
	}

	if (isdfs)
	{
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	if (isbfs)
	{
		ofSetLineWidth(5);
		if (isOpen)
			bfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	if(bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight()-20);
	}
} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if(bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else { 
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU)); 
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if(bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if(bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	// Escape key exit has been disabled but it can be checked here
	if(key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if(bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}
} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 'r') {
		bfsdraw();
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName.c_str());
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);

			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			ofBuffer buffer(file);

			// Input_flag is a variable for indication the type of input.
			// If input_flag is zero, then work of line input is progress.
			// If input_flag is one, then work of dot input is progress.
			int input_flag = 0;

			// Idx is a variable for index of array.
			int idx = 0;

			// Read file line by line
			int cnt = 0;


			// TO DO
			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다
			freeMemory();
			ofFile fp1(fileName);
			char a;
			string s;
			getline(fp1, s);
			idx = s.length() - 1;
			ofFile fp2(fileName);
			while (getline(fp2,s))
			{
				cnt++;
			}
			ofFile fp3(fileName);
			input = new maze*[cnt];
			for (int i = 0; i < cnt; i++)
				input[i] = new maze[idx];
			for (int i = 0; i < cnt; i++)
			{
				getline(fp3, s);
				for (int j = 0; j < idx; j++)
				{
					if (s[j] == '+' || s[j] == '-' || s[j] == '|')
						input[i][j].p = '1';
					else
						input[i][j].p = '0';
				}
			}
			maze_col = idx;
			maze_row = cnt;
			init_maze();
			construct_maze();
		}	
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::construct_maze()
{
	for (int i = 1; i < maze_row; i += 2) {
		for (int j = 1; j < maze_col; j += 2) {
			if (input[i][j - 1].p == '0')
				input[i][j].l = 1;
			if (input[i][j + 1].p == '0')
				input[i][j].r = 1;
			if (input[i+1][j].p == '0')
				input[i][j].d = 1;
			if (input[i-1][j].p == '0')
				input[i][j].u = 1;
		}
	}
}
void ofApp::init_maze()
{
	bfs = 0;
	a = 0;
	k = 0;
	for (int i = 1; i < maze_row; i+=2) {
		for (int j = 1; j < maze_col; j+=2) {
			input[i][j].u = 0;
			input[i][j].d = 0;
			input[i][j].l = 0;
			input[i][j].r = 0;
		}
	}
	visited = new int*[maze_row];
	for (int i = 0; i < maze_row; i++)
		visited[i] = new int[maze_col];
	
	for (int i = 0; i < maze_row; i++)
		for (int j = 0; j < maze_col; j++)
			visited[i][j] = -1;
	for (int i = 0; i < 1000; i++)
	{
		x[i] = 0;
		y[i] = 0;
		tr[i].x = 0;
		tr[i].y = 0;
		btr[i].x = 0;
		btr[i].y = 0;
	}
}
void ofApp::freeMemory() {

	//TO DO
	// malloc한 memory를 free해주는 함수
	delete input;
	delete visited;
}

bool ofApp::DFS()//DFS탐색을 하는 함수
{
	//TO DO
	//DFS탐색을 하는 함수 ( 3주차)
	int cnt , k;
	int i=1, j=1;
	visited[1][1] = 0;
	s.push(make_pair(1, 1));
	while (!s.empty()) {
				k = 1;
				i = s.top().first;
				j = s.top().second;
				tr[a].y = s.top().first;
				tr[a].x = s.top().second;
				a++;
				s.pop();
				if (i == (maze_row - 2) && j == (maze_col - 2))
				{
					s.push(make_pair(i, j));
					break;
				}
				while (1) {
					cnt = 0;
					if (k) {
						s.push(make_pair(i, j));
						k = 0;
					}
					if (i == (maze_row - 2) && j == (maze_col - 2))	break;
					if (input[i][j].d) {
						if (visited[i + 2][j] < 0) {
							s.push(make_pair(i + 2, j));
							tr[a].y = s.top().first;
							tr[a].x = s.top().second;
							visited[i + 2][j]++;
							i += 2;
							a++;
							cnt++;
						}
					}
					if (i == (maze_row - 2) && j == (maze_col - 2))	break;
					if (input[i][j].u) {
						if (visited[i - 2][j] < 0) {
							s.push(make_pair(i - 2, j));
							tr[a].y = s.top().first;
							tr[a].x = s.top().second;
							visited[i - 2][j]++;
							i -=2 ;
							a++;
							cnt++;
						}
					}
					if (i == (maze_row - 2) && j == (maze_col - 2))	break;
					if (input[i][j].l) {
						if (visited[i][j - 2] < 0) {
							s.push(make_pair(i, j - 2));
							tr[a].y = s.top().first;
							tr[a].x = s.top().second;
							visited[i][j - 2]++;
							j -= 2;
							a++;
							cnt++;
						}
					}
					if (i == (maze_row - 2) && j == (maze_col - 2))	break;
					if (input[i][j].r) {
						if (visited[i][j + 2] < 0) {
							s.push(make_pair(i, j + 2));
							tr[a].y = s.top().first;
							tr[a].x = s.top().second;
							visited[i][j + 2]++;
							j += 2;
							a++;
							cnt++;
						}
					}
					if (cnt == 0) {
						s.pop();
						break;
					}
				}
		}
	return true;
}
bool ofApp::BFS() {
	int i, j,k,cnt;
	i = 1, j = 1;
	visited[1][1]++;
	tr[0].y = i; tr[0].x = j;
	a++;
	q.push(make_pair(i, j));
	while (!q.empty()) {
		i = q.front().first;
		j = q.front().second;
		q.pop();
		if (i == (maze_row - 2) && j == (maze_col - 2))
			break;
			if (i == (maze_row - 2) && j == (maze_col - 2))	break;
			if (input[i][j].d) {
				if (visited[i + 2][j] < 0) {
					q.push(make_pair(i + 2, j));
					tr[a].y = q.back().first;
					tr[a].x = q.back().second;
					visited[i + 2][j]++;
					a++;
				}
			}
			if (input[i][j].u) {
				if (visited[i - 2][j] < 0) {
					q.push(make_pair(i - 2, j));
					tr[a].y = q.back().first;
					tr[a].x = q.back().second;
					visited[i - 2][j]++;
					a++;
				}
			}
			if (input[i][j].l) {
				if (visited[i][j - 2] < 0) {
					q.push(make_pair(i, j - 2));
					tr[a].y = q.back().first;
					tr[a].x = q.back().second;
					visited[i][j - 2]++;
					a++;
				}
			}
			if (input[i][j].r) {
				if (visited[i][j + 2] < 0) {
					q.push(make_pair(i, j + 2));
					tr[a].y = q.back().first;
					tr[a].x = q.back().second;
					visited[i][j + 2]++;
					a++;
				}
			}	
	}
	int ta, tb; 
	btr[bfs].x = maze_col - 2;
	btr[bfs].y = maze_row - 2;
	for (i = a - 2; i >= 0; i--) {
		ta = tr[i].x;
		tb = tr[i].y;
		if (btr[bfs].x == ta && btr[bfs].y - tb == 2) {
			if (input[btr[bfs].y][btr[bfs].x].u)
			{
				bfs++;
				btr[bfs].x = ta;
				btr[bfs].y = tb;
				continue;
			}
		}
		if (btr[bfs].x == ta && tb - btr[bfs].y == 2) {
			if (input[btr[bfs].y][btr[bfs].x].d)
			{
				bfs++;
				btr[bfs].x = ta;
				btr[bfs].y = tb;
				continue;
			}
		}
		if (btr[bfs].x - ta == 2 && btr[bfs].y == tb) {
			if (input[btr[bfs].y][btr[bfs].x].l)
			{
				bfs++;
				btr[bfs].x = ta;
				btr[bfs].y = tb;
				continue;
			}
		}
		if (btr[bfs].x - ta == -2 && btr[bfs].y == tb) {
			if (input[btr[bfs].y][btr[bfs].x].r)
			{
				bfs++;
				btr[bfs].x = ta;
				btr[bfs].y = tb;
			}
		}
	}
	btr[bfs].x = tr[0].x;
	btr[bfs].y = tr[0].y;
	return true;
}
void ofApp::dfsdraw()
{
	//TO DO 
	//DFS를 수행한 결과를 그린다. (3주차 내용)
	while (!s.empty())
	{
		y[k] = s.top().first;
		x[k] = s.top().second;
		s.pop();
		k++;
	}
	for (int i = 0; i < a - 1; i++) {
		ofSetColor(200);
		ofDrawLine(5 + tr[i].x * 20, 5 + tr[i].y * 20, 5 + tr[i + 1].x * 20, 5 + tr[i + 1].y * 20);
	}
	for (int i = 0; i< k-1; i++) {
		ofSetColor(ofColor::orangeRed);
		ofDrawLine(5 + x[i] * 20, 5 + y[i] * 20, 5 + x[i+1] * 20, 5 + y[i+1] * 20);	
	}
}
void ofApp::bfsdraw() {
	for (int i = 0; i < a - 1; i++) {
		for (int j = i + 1; j < a; j++) {
			if (tr[i].x == tr[j].x && tr[i].y - tr[j].y == 2) {
				if (input[tr[i].y][tr[i].x].u)
				{
					ofSetColor(200);
					ofDrawLine(5+ 20* tr[i].x, 5+ 20 * tr[i].y, 5+ 20 * tr[j].x, 5+ 20 * tr[j].y);
				}
			}
			if (tr[i].x == tr[j].x && tr[j].y - tr[i].y == 2) {
				if (input[tr[i].y][tr[i].x].d)
				{
					ofSetColor(200);
					ofDrawLine(5+ 20 * tr[i].x, 5+ 20 * tr[i].y, 5+ 20 * tr[j].x,5+ 20 * tr[j].y);
				}
			}
			if (tr[i].x - tr[j].x ==2 && tr[i].y == tr[j].y) {
				if (input[tr[i].y][tr[i].x].l)
				{
					ofSetColor(200);
					ofDrawLine(5+ 20 * tr[i].x, 5+ 20 * tr[i].y, 5 + 20 * tr[j].x, 5 + 20 * tr[j].y);
				}
			}
			if (tr[j].x - tr[i].x == 2 && tr[i].y == tr[j].y) {
				if (input[tr[i].y][tr[i].x].r)
				{
					ofSetColor(200);
					ofDrawLine(5 + 20 * tr[i].x, 5 + 20 * tr[i].y, 5 + 20 * tr[j].x, 5 + 20 * tr[j].y);
				}
			}
		}
	}
	for (int i = 0; i < bfs; i++) {
		ofSetColor(ofColor::red);
		ofDrawLine(5 + btr[i].x * 20, 5 + btr[i].y * 20, 5 + btr[i + 1].x * 20, 5 + btr[i + 1].y * 20);
	}
}
	

