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



using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {
	nill = (node*)malloc(sizeof(struct node));
	nill->color = BLACK;
	root = nill;
	init_game();
	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isgame = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

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
	hPopup = menu->AddPopupMenu(hMenu, "Game");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Play Game",false,false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show Rank"); // Not checked (default)
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
	if(title == "Play Game") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			isgame = 1;
			draw_flag = 1;
			init_game();
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;	
	}
	if(title == "Show Rank") {
		if (isOpen) {
			draw_flag = 0;
			isgame = 0;
			rank_flag = 1;
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
	//�����¿� Ű�� flag���� Ȱ��ȭ �Ǿ��� ��� ball�� ��ġ�� �˸°� �������ְ� mv++���ش�
	if (mvleft_flag) {
		ball_x -= 2;
		if (!input[ball_y][ball_x].l) mvleft_flag = false;
		mv++;
	}
	if (mvright_flag) {
		ball_x += 2;
		if (!input[ball_y][ball_x].r) mvright_flag = false;
		mv++;
	}
	if (mvup_flag) {
		ball_y -= 2;
		if (!input[ball_y][ball_x].u) mvup_flag = false;
		mv++;
	}
	if (mvdown_flag) {
		ball_y += 2;
		if (!input[ball_y][ball_x].d) mvdown_flag = false;
		mv++;
	}
	//ball�� ���������� �����Ͽ��� ��� finish_flag�� 1�� set���ش�
	if (ball_x == maze_col - 2 && ball_y == maze_row - 2) finish_flag = true;
}


//--------------------------------------------------------------
void ofApp::draw() {
	//ȭ�鿡 ��� ������� ������ ���ڿ���
	char str1[256];
	char str2[256];
	char str3[256];
	char str4[256];

	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	// DRAW MAZE; 
	// ����� �ڷᱸ���� �̿��� �̷θ� �׸���.
	// add code here
	if (draw_flag) { //if draw_flag == 1 draw maze
		int da = 5, db = 5; //�̷��� ù��ġ�� (5,5)�� �ʱ�ȭ
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
	if (draw_flag)
	{	
		ofSetColor(ofColor::cyan); //�� ����
		ofDrawRectangle((maze_col - 2) * 20 - 13, (maze_row - 2) * 20 - 13, 36, 36);  //�̷ΰ� ������ �κ��� �簢������ ���� ���ϰ� �׷ȴ�.
	}
	if (isgame)
	{
		if (isOpen) {
			//game�� �����߰� ������ ���ȴٸ�
			//������ ������ ��� ball���� ���������� �׷��ش�
			if (finish_flag) {
				ofSetColor(ofColor::black);
				ofCircle(ball_x * 20 + 5, ball_y * 20 + 5, 10);
			}
			else {//���� �����߿��� ball�� ���������� �׷��ش�
				ofSetColor(ofColor::red);
				ofCircle(ball_x * 20 + 5, ball_y * 20 + 5, 10);
			}
			//ball�� �����̴� Ƚ���� �ǽð����� üũ���ش�
			ofSetColor(ofColor::black);
			ofDrawBitmapString("Moving times: " + ofToString(mv), 20, maze_row * 20);
		}
		else
			cout << "You must open file first" << endl;
	}
	//������ ������ ��� �޼����� â�� ���
	if(finish_flag && !regame_flag && !rank_flag) {
		ofSetColor(ofColor::black);
		// Show keyboard duplicates of menu functions
		sprintf(str1, "Register Rank? (y/n)");
		myFont.drawString(str1, 20, maze_row*22);
	}
	// regame_flag�� set�Ǿ��� ��� �޼����� â�� ���
	if (regame_flag && !rank_flag) {
		ofSetColor(ofColor::black);
		sprintf(str2, "if you want regame press button 'g'");
		myFont.drawString(str2, 20, maze_row * 22);
	}
	//rank_flag�� set�Ǿ��� ���
	if (rank_flag) {
		p = 0; //p���� 0���� �ʱ�ȭ
		rank = 1; //rank�� 1�� �ʱ�ȭ
		ofSetColor(ofColor::black);
		sprintf(str3, "  rank             moving times");
		myFont.drawString(str3, 5, 20);
		tree_print(root); //rank��� �������
	}
	if (rank_flag) {
		ofSetColor(ofColor::black);
		sprintf(str4, "if you delete rank press button 'd'");
		myFont.drawString(str4, 300, 100);
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
	// y�� ������ mv���� rb tree�ȿ� �ְ� regame_flag�� 1�� set
	if (key == 'y') {
		rb_insert(mv);
		regame_flag = true;
	}
	// n�� ������ regame_flag�� 1�� set
	if (key == 'n') {
		regame_flag = true;
	}
	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}
	//�����¿� ���� ������ ��� flag�� 1�� set����
	// �����¿� ���� ���ÿ� ������ ���ϰ��ϰ� ������ ������ �ʾ��� ��츸 ���� �� ����
	if (key == OF_KEY_LEFT){
		if (input[ball_y][ball_x].l && !mvright_flag && !mvup_flag && !mvdown_flag && !finish_flag) {
			mvleft_flag = true;
		}
	}
	if (key == OF_KEY_DOWN){
		if (input[ball_y][ball_x].d && !mvright_flag && !mvup_flag && !mvleft_flag && !finish_flag) {
			mvdown_flag = true;
		}
	}
	if (key == OF_KEY_UP) {
		if (input[ball_y][ball_x].u && !mvright_flag && !mvleft_flag && !mvdown_flag && !finish_flag) {
			mvup_flag = true;
		}
	}
	if (key == OF_KEY_RIGHT) {
		if (input[ball_y][ball_x].r && !mvleft_flag && !mvup_flag && !mvdown_flag && !finish_flag) {
			mvright_flag = true;
		}
	}
	//g�� ������ �� ������ ������ϱ� ���� ������ �ʱ�ȭ ���ش�
	if (key == 'g' && regame_flag) {
		init_game();
	}
	if (key == 'd' && rank_flag) {
		int k;
		cin >> k;
		rb_delete(rank_search(k));
	}
} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	//�����¿��� flag������ 0���� set
	if (key == OF_KEY_LEFT) {
		mvleft_flag = false;
	}
	if (key == OF_KEY_RIGHT) {
		mvright_flag = false;
	}
	if (key == OF_KEY_UP) {
		mvup_flag = false;
	}
	if (key == OF_KEY_DOWN) {
		mvdown_flag = false;
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

			ofFile file(fileName); //���� ������

			//������ ���������� ���� ���
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
			// .maz ������ input���� �޾Ƽ� ������ �ڷᱸ���� �ִ´�
			freeMemory(); //�Ҵ�� �޸� ����
			memfree(root); //rb tree�ʱ�ȭ
			init_rb(); //rb tree �ٽ� �����
			ofFile fp1(fileName); //���� ������
			string s;  //���� �� ���� ������ ���ڿ� ����
			getline(fp1, s);
			//�̷��� ���α��̸� idx�� ����
			idx = s.length() - 1;
			ofFile fp2(fileName); //���� ����
			//�̷��� ���̸� cnt�� ����
			while (getline(fp2,s))
			{
				cnt++;
			}
			ofFile fp3(fileName); //���� ������
			//�̷��� row, col��ŭ input �迭�� �޸� �Ҵ����ش�
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
			maze_col = idx; //���������� maze_col�� idx�� �Ѱ���
			maze_row = cnt; //���������� maze_row�� cnt�� �Ѱ���
			construct_maze();  // �Լ� ȣ��
			init_game(); //�Լ�ȣ��
		}	
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::init_game() {
	mv = 0;  //������ Ƚ�� �ʱ�ȭ
	//ball�� x,y��ǥ 1,1�� �ʱ�ȭ
	ball_x = 1;
	ball_y = 1;
	regame_flag = false;
	//�����¿� flag �ʱ�ȭ
	mvleft_flag = false;
	mvright_flag = false;
	mvup_flag = false;
	mvdown_flag = false;
	finish_flag = false; //���� ������ reset
	rank_flag = false; //rank_flag reset
}
void ofApp::construct_maze()
{
	//input�Լ��� �����¿� ���� ��� 0���� �ʱ�ȭ ���ش�
	for (int i = 1; i < maze_row; i += 2) {
		for (int j = 1; j < maze_col; j += 2) {
			input[i][j].u = 0;
			input[i][j].d = 0;
			input[i][j].l = 0;
			input[i][j].r = 0;
		}
	}
	//�̷��� �����¿� ���⿡ ���� ������ 0 ���� ������ 1�� ����
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
//rb tree�� �ʱ�ȭ ���ִ� �Լ�
void ofApp::init_rb() {
	nill = (node*)malloc(sizeof(struct node));
	nill->color = BLACK;
	root = nill;
}
// rb tree�� �Ҵ�� ��� ������ �޸𸮸� �����Ѵ�
void ofApp::memfree(struct node* x) {
	if (x == nill) return;
	memfree(x->left);
	memfree(x->right);
	free(x);
	return;
}
void ofApp::freeMemory() {
	// malloc�� memory�� free���ִ� �Լ�
	delete input;
}
//tree�� key���� ã���ִ� �Լ�
struct ofApp::node *ofApp::tree_search(int key) {
	struct node *x; 
	x = root; //x�� root ����
	//x�� Ű���� ã���� ���� while���� �����ش�
	while (x != nill && x->key != key) {
		if (key < x->key) x = x->left;
		else x = x->right;
	}
	return x;
}
struct ofApp::node *ofApp::rank_search(int rank) {
	struct node *x;
	x = root; //x�� root ����
	//x�� Ű���� ã���� ���� while���� �����ش�
	while (x != nill && x->rank != rank) {
		if (rank < x->rank) x = x->left;
		else x = x->right;
	}
	return x;
}
//x�� successor�� �����ִ� �Լ�
struct ofApp::node *ofApp::tree_mim(struct node* x) {
	while (x->left != nill)
		x = x->left;
	return x;
}
//left rotate �������ִ� �Լ�
void ofApp::left_rotate(struct node *x) {
	struct node *y; //y����
	y = x->right; //y�� ���� x�� ������ �ڽ����� 
	x->right = y->left; //x->right�� y�� �ڽİ�����
	//y�� ���� �ڽ��� ���� null�� �ƴ϶�� y�� �����ڽ��� �θ��带 x�� �Ҵ�
	if (y->left != nill) {
		y->left->parent = x;
	}
	y->parent = x->parent; //y�� �θ��带 x�� �θ���� �Ҵ��Ѵ�
	//x�� root��� y�� ��Ʈ�� �����Ѵ�
	if (x->parent == nill) {
		root = y;
	}
	//x�� x�� �θ����� ���� ��忴�ٸ� x�� �θ��� ���� �ڽ��� y�� �ȴ�
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	//x�� x�� �θ����� ������ ��忴�ٸ� x�� �θ��� ������ �ڽ��� y�� �ȴ�
	else
		x->parent->right = y;
	y->left = x; //x�� y�� ���� �ڽ�
	x->parent = y;	//y�� x�� �θ� ��尡 �ǰ��Ѵ�
}
void ofApp::right_rotate(struct node *x) {
	struct node *y; //y ����
	y = x->left;	//y�� ���� x�� ���� �ڽ�����
	x->left = y->right;	//x->left�� y�� �ڽİ�����
	//y�� ������ �ڽ��� ���� null�� �ƴ϶�� y�� �������ڽ��� �θ��带 x�� �Ҵ�
	if (y->right!= nill) {
		y->right->parent = x;
	}
	y->parent = x->parent; //y�� �θ��带 x�� �θ���� �Ҵ��Ѵ�
	//x�� root��� y�� ��Ʈ�� �����Ѵ�
	if (x->parent == nill) {
		root = y;
	}
	//x�� x�� �θ����� ���� ��忴�ٸ� x�� �θ��� ���� �ڽ��� y�� �ȴ�
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	//x�� x�� �θ����� ������ ��忴�ٸ� x�� �θ��� ������ �ڽ��� y�� �ȴ�
	else
		x->parent->right = y;
	y->right = x; //x�� y�� ������ �ڽ�
	x->parent = y;	//y�� x�� �θ� ��尡 �ǰ��Ѵ�
}
//rb tree�ȿ� key���� �־��ִ� �Լ�
void ofApp::rb_insert(int key) {
	struct node *z, *x, *y;
	z = (node*)malloc(sizeof(struct node)); //�޸� �Ҵ�
	z->key = key; //key�� �Ѱ���
	z->color = RED;  //�־��ִ� ����� ���� �׻� red
	//������, ���� �ڸ� ��� nill
	z->left = nill;
	z->right = nill;
	x = root;
	y = nill;
	//x�� ���� �־��� �ڸ��� �θ� y������ ���� 
	while (x != nill) {
		y = x;
		if (z->key <= x->key) x = x->left;
		else x = x->right;
	}
	if (y == nill) root = z;  //y�� ���� null�̸� z�� root ��尡 �ȴ�
	//y�� ���� �ڽ����� ������ �ڽ����� �����Ѵ�.
	else if (z->key <= y->key) y->left = z;
	else y->right = z;
	z->parent = y;
	rb_insertfix(z); //���ǿ� ���ݵǴ� ��쿡 insertfix�Լ��� ȣ���Ѵ�.
}
void ofApp::rb_insertfix(struct node *z) {
	while (z->parent->color == RED) { //z�� �θ��尡 red�� �ƴҶ� ���� while�� ���� red-red�� ���� ����
		if (z->parent == z->parent->parent->left) { //z�� �Ҿƹ��� ����� �����ڽ��� z�� �θ����� ���
			if (z->parent->parent->right->color == RED) { //z�� �Ҿƹ����� ������ �ڽ� ���� ������ ���
				z->parent->color = BLACK; //z�� �θ��� ���� ������
				z->parent->parent->right->color = BLACK; //z�� �Ҿƹ����� ������ �ڽ��� ���������� �ٲ۴�
				z->parent->parent->color = RED; //z�� �Ҿƹ��� ���� �������� �ٲ۴�
				z = z->parent->parent; //z�� z�� �Ҿƹ��� ���� �ٲ۴�
			}
			else {//z�� �Ҿƹ��� ����� ������ �ڽ��� ���� �������� ��
				if (z == z->parent->right) { //z�� z�� �θ� ������ �ڽ��� ��� z��带 �������� leftrotate������ �Ѵ�
					z = z->parent;
					left_rotate(z);
				}

				z->parent->color = BLACK; //z�� �θ����� ���� ���������� �ٲ۴� 
				z->parent->parent->color = RED; //z�� �Ҿƹ�������� ���� �������� �ٲ۴�
				right_rotate(z->parent->parent); //z�� �Ҿƹ�����带 �������� rightrotate������ ���ش�
			}
		}
		else {//z�� �Ҿƹ��� ����� ������ �ڽ��� z�� �θ����� ���
			if (z->parent->parent->left->color == RED) {//z�� �Ҿƹ����� ���� �ڽ��� ������ ���
				z->parent->color = BLACK;	//z�� �θ����� ���� ���������� �ٲ۴�
				z->parent->parent->left->color = BLACK; //z�� �Ҿƹ��� ���� �ڽ��� ������ ���������� �ٲ۴�
				z->parent->parent->color = RED;	//z�� �Ҿƹ��� ���� red�� �ٲ۴�
				z = z->parent->parent; //z�� z�� �Ҿƹ��� ���� �ٲ۴�
			}
			else {//z�� �Ҿƹ����� ���� �ڽ��� �������� ���
				if (z == z->parent->left) {//z�� z�� �θ� ���� �ڽ��� ��� z��带 �������� rightrotate������ �Ѵ�
					z = z->parent;
					right_rotate(z);
				}

				z->parent->color = BLACK;	//z�� �θ���� ���������� �ٲ۴�
				z->parent->parent->color = RED;	//z�� �Ҿƹ��� ���� �������� �ٲ۴�	
				left_rotate(z->parent->parent);	//z�� �Ҿƹ��� �������� leftrotate������ ���ش�
			}
		}
	}
	root->color = BLACK;	//root�� ���� ���������� ������ش�
}
void ofApp::rb_delete(struct node *z) {
	struct node *y, *x; //y�� x�� ����
	int yOriginalColor; //y�� ������ ��
	y = z; //y�� z�� ����
	yOriginalColor = y->color; //y�� ���� �� ����
	
	//z�� �ڽ��� ���ų� ������ �ڽĸ� �ִ� ���
	if (z->left == nill) {
		x = z->right;  //x�� z�� ������ �ڽ� ����
		rb_transplant(z, z->right);
	}
	//z�� ���� �ڽĸ� �ִ� ���
	else if (z->right == nill) {
		x = z->left; //x�� z�� ���� �ڽ��� ����
		rb_transplant(z, z->left);
	}
	//z�� �ڽ��� �Ѵ� �ִ� ���
	else {
		y = tree_mim(z->right);  //z�� successor�� ã�Ƽ� y�� ���� bst�� ����
		yOriginalColor = y->color; //y�� ���� �� ����
		x = y->right; //x�� y�� ������ �ڽ� ����
		
		//y�� �θ��尡 z�� ���ٸ� x�� �θ��带 y�� ����
		if (y->parent == z) {
			x->parent = y;
		}
		//y�� �θ��尡 z�� �ٸ��ٸ�
		else {
			//y�� ������ �ڽ��� ����Ʈ���� transplant ��
			rb_transplant(y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}
		
		rb_transplant(z, y); //y�� z�� transplant ��
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	//y�� ���� �������̿��ٸ� deletefix ȣ��
	if (yOriginalColor == BLACK) {
		rb_deletefix(x);
	}
}
void ofApp::rb_deletefix(struct node *x) {
	struct node *w;
	while (x != root && x->color == BLACK) {
		//x�� �����ڽ� ����� ���
		if (x == x->parent->left) {
			w = x->parent->right;
			//w�� red�� ���
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				left_rotate(x->parent);
				w = x->parent->right;
			}
			//w�� ���ڳడ ��� black�� ���
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->parent;
			}
			else {
				//w�� �������̰� w�� �������ڽ��� ���� ���̰� ������ ������ ���
				if (w->right->color == BLACK) {
					w->color = RED;
					w->left->color = BLACK;
					right_rotate(w);
					w = x->parent->right;
				}
				//w�� �������̰� ������ �ڽ��� ������ ���
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				left_rotate(x->parent);
				x = root;
			}
		}
		//x�� �������ڽ� ����� ���
		else {
			w = x->parent->left;

			//w�� ���� ������ ���
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				right_rotate(x->parent);
				w = x->parent->left;
			}
			//w�� �ڽ��� �Ѵ� black�� ���
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->parent;
			}
			else {
				//w�� �������̰� w�� �������ڽ��� ���� ���̰� ������ ������ ���
				if (w->left->color == BLACK) {
					w->color = RED;
					w->right->color = BLACK;
					left_rotate(w);
					w = x->parent->left;
				}
				//w�� �������̰� ������ �ڽ��� ������ ���
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				right_rotate(x->parent);
				x = root;
			}
		}

	}
	//x�� ���� black���� ����
	x->color = BLACK;
}
void ofApp::rb_transplant(struct node *u, struct node *v) {
	if (u->parent == nill) root = v;  //u�� ��Ʈ�� ���, v�� root������ ����
	else if (u == u->parent->left) u->parent->left = v; //u�� �����ڽ��� ��� u�� �θ��� ���� �ڽ��� v�� ����
	else u->parent->right = v; //u�� �������ڽ��� ��� u�� �θ��� ������ �ڽ��� v�� ����
	v->parent = u->parent;
}
// tree�� key���� ������������ ȭ�鿡 �׷��ִ� �Լ�
void ofApp::tree_print(struct node* x)
{
	if (x == nill) return;
	tree_print(x->left);
	x->rank = rank;
	ofDrawBitmapString(ofToString(x->rank), 20, 40 + p);
	ofDrawBitmapString(ofToString(x->key), 150, 40+p);
	rank++;
	p += 20;
	tree_print(x->right);
	return;
}
