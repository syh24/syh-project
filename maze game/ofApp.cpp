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
	//상하좌우 키의 flag들이 활성화 되었을 경우 ball의 위치를 알맞게 변경해주고 mv++해준다
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
	//ball이 도착지점에 도착하였을 경우 finish_flag를 1로 set해준다
	if (ball_x == maze_col - 2 && ball_y == maze_row - 2) finish_flag = true;
}


//--------------------------------------------------------------
void ofApp::draw() {
	//화면에 띄울 문장들을 저장할 문자열들
	char str1[256];
	char str2[256];
	char str3[256];
	char str4[256];

	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;
	// DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here
	if (draw_flag) { //if draw_flag == 1 draw maze
		int da = 5, db = 5; //미로의 첫위치를 (5,5)로 초기화
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
		ofSetColor(ofColor::cyan); //색 설정
		ofDrawRectangle((maze_col - 2) * 20 - 13, (maze_row - 2) * 20 - 13, 36, 36);  //미로가 끝나는 부분을 사각형으로 보기 편하게 그렸다.
	}
	if (isgame)
	{
		if (isOpen) {
			//game을 시작했고 파일이 열렸다면
			//게임이 끝났을 경우 ball색을 검은색으로 그려준다
			if (finish_flag) {
				ofSetColor(ofColor::black);
				ofCircle(ball_x * 20 + 5, ball_y * 20 + 5, 10);
			}
			else {//게임 진행중에는 ball을 빨강색으로 그려준다
				ofSetColor(ofColor::red);
				ofCircle(ball_x * 20 + 5, ball_y * 20 + 5, 10);
			}
			//ball이 움직이는 횟수를 실시간으로 체크해준다
			ofSetColor(ofColor::black);
			ofDrawBitmapString("Moving times: " + ofToString(mv), 20, maze_row * 20);
		}
		else
			cout << "You must open file first" << endl;
	}
	//게임이 끝났을 경우 메세지를 창에 띄움
	if(finish_flag && !regame_flag && !rank_flag) {
		ofSetColor(ofColor::black);
		// Show keyboard duplicates of menu functions
		sprintf(str1, "Register Rank? (y/n)");
		myFont.drawString(str1, 20, maze_row*22);
	}
	// regame_flag가 set되었을 경우 메세지를 창에 띄움
	if (regame_flag && !rank_flag) {
		ofSetColor(ofColor::black);
		sprintf(str2, "if you want regame press button 'g'");
		myFont.drawString(str2, 20, maze_row * 22);
	}
	//rank_flag가 set되었을 경우
	if (rank_flag) {
		p = 0; //p값을 0으로 초기화
		rank = 1; //rank를 1로 초기화
		ofSetColor(ofColor::black);
		sprintf(str3, "  rank             moving times");
		myFont.drawString(str3, 5, 20);
		tree_print(root); //rank대로 출력해줌
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
	// y를 누르면 mv값을 rb tree안에 넣고 regame_flag를 1로 set
	if (key == 'y') {
		rb_insert(mv);
		regame_flag = true;
	}
	// n을 누르면 regame_flag를 1로 set
	if (key == 'n') {
		regame_flag = true;
	}
	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}
	//상하좌우 값을 눌렀을 경우 flag를 1로 set해줌
	// 상하좌우 값을 동시에 누르지 못하게하고 게임이 끝나지 않았을 경우만 누를 수 있음
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
	//g를 눌렀을 때 게임을 재시작하기 위해 값들을 초기화 해준다
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
	//상하좌우의 flag값들을 0으로 set
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

			ofFile file(fileName); //파일 열어줌

			//파일의 존재유무에 따른 출력
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
			freeMemory(); //할당된 메모리 해제
			memfree(root); //rb tree초기화
			init_rb(); //rb tree 다시 만들기
			ofFile fp1(fileName); //파일 열어줌
			string s;  //파일 한 줄을 저장할 문자열 선언
			getline(fp1, s);
			//미로의 가로길이를 idx에 저장
			idx = s.length() - 1;
			ofFile fp2(fileName); //파일 열기
			//미로의 높이를 cnt에 저장
			while (getline(fp2,s))
			{
				cnt++;
			}
			ofFile fp3(fileName); //파일 열어줌
			//미로의 row, col만큼 input 배열을 메모리 할당해준다
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
			maze_col = idx; //전역변수인 maze_col에 idx값 넘겨줌
			maze_row = cnt; //전역변수인 maze_row에 cnt값 넘겨줌
			construct_maze();  // 함수 호출
			init_game(); //함수호출
		}	
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::init_game() {
	mv = 0;  //움직인 횟수 초기화
	//ball의 x,y좌표 1,1로 초기화
	ball_x = 1;
	ball_y = 1;
	regame_flag = false;
	//상하좌우 flag 초기화
	mvleft_flag = false;
	mvright_flag = false;
	mvup_flag = false;
	mvdown_flag = false;
	finish_flag = false; //게임 끝난거 reset
	rank_flag = false; //rank_flag reset
}
void ofApp::construct_maze()
{
	//input함수의 상하좌우 값을 모두 0으로 초기화 해준다
	for (int i = 1; i < maze_row; i += 2) {
		for (int j = 1; j < maze_col; j += 2) {
			input[i][j].u = 0;
			input[i][j].d = 0;
			input[i][j].l = 0;
			input[i][j].r = 0;
		}
	}
	//미로의 상하좌우 방향에 벽이 있으면 0 벽이 없으면 1로 설정
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
//rb tree를 초기화 해주는 함수
void ofApp::init_rb() {
	nill = (node*)malloc(sizeof(struct node));
	nill->color = BLACK;
	root = nill;
}
// rb tree에 할당된 모든 노드들의 메모리를 해제한다
void ofApp::memfree(struct node* x) {
	if (x == nill) return;
	memfree(x->left);
	memfree(x->right);
	free(x);
	return;
}
void ofApp::freeMemory() {
	// malloc한 memory를 free해주는 함수
	delete input;
}
//tree의 key값을 찾아주는 함수
struct ofApp::node *ofApp::tree_search(int key) {
	struct node *x; 
	x = root; //x에 root 저장
	//x의 키값을 찾을때 까지 while문을 돌려준다
	while (x != nill && x->key != key) {
		if (key < x->key) x = x->left;
		else x = x->right;
	}
	return x;
}
struct ofApp::node *ofApp::rank_search(int rank) {
	struct node *x;
	x = root; //x에 root 저장
	//x의 키값을 찾을때 까지 while문을 돌려준다
	while (x != nill && x->rank != rank) {
		if (rank < x->rank) x = x->left;
		else x = x->right;
	}
	return x;
}
//x의 successor을 구해주는 함수
struct ofApp::node *ofApp::tree_mim(struct node* x) {
	while (x->left != nill)
		x = x->left;
	return x;
}
//left rotate 연산해주는 함수
void ofApp::left_rotate(struct node *x) {
	struct node *y; //y설정
	y = x->right; //y의 값을 x의 오른쪽 자식으로 
	x->right = y->left; //x->right를 y의 자식값으로
	//y의 왼쪽 자식의 값이 null이 아니라면 y의 왼쪽자식의 부모노드를 x로 할당
	if (y->left != nill) {
		y->left->parent = x;
	}
	y->parent = x->parent; //y의 부모노드를 x의 부모노드로 할당한다
	//x가 root라면 y를 루트로 설정한다
	if (x->parent == nill) {
		root = y;
	}
	//x가 x의 부모노드의 왼쪽 노드였다면 x의 부모의 왼쪽 자식이 y가 된다
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	//x가 x의 부모노드의 오른쪽 노드였다면 x의 부모의 오른쪽 자식이 y가 된다
	else
		x->parent->right = y;
	y->left = x; //x는 y의 왼쪽 자식
	x->parent = y;	//y가 x의 부모 노드가 되게한다
}
void ofApp::right_rotate(struct node *x) {
	struct node *y; //y 설정
	y = x->left;	//y의 값을 x의 왼쪽 자식으로
	x->left = y->right;	//x->left를 y의 자식값으로
	//y의 오른쪽 자식의 값이 null이 아니라면 y의 오른쪽자식의 부모노드를 x로 할당
	if (y->right!= nill) {
		y->right->parent = x;
	}
	y->parent = x->parent; //y의 부모노드를 x의 부모노드로 할다한다
	//x가 root라면 y를 루트로 설정한다
	if (x->parent == nill) {
		root = y;
	}
	//x가 x의 부모노드의 왼쪽 노드였다면 x의 부모의 왼쪽 자식이 y가 된다
	else if (x == x->parent->left) {
		x->parent->left = y;
	}
	//x가 x의 부모노드의 오른쪽 노드였다면 x의 부모의 오른쪽 자식이 y가 된다
	else
		x->parent->right = y;
	y->right = x; //x는 y의 오른쪽 자식
	x->parent = y;	//y가 x의 부모 노드가 되게한다
}
//rb tree안에 key값을 넣어주는 함수
void ofApp::rb_insert(int key) {
	struct node *z, *x, *y;
	z = (node*)malloc(sizeof(struct node)); //메모리 할당
	z->key = key; //key값 넘겨줌
	z->color = RED;  //넣어주는 노드의 색은 항상 red
	//오른쪽, 왼쪽 자리 모두 nill
	z->left = nill;
	z->right = nill;
	x = root;
	y = nill;
	//x의 값을 넣어줄 자리의 부모를 y값으로 설정 
	while (x != nill) {
		y = x;
		if (z->key <= x->key) x = x->left;
		else x = x->right;
	}
	if (y == nill) root = z;  //y의 값이 null이면 z가 root 노드가 된다
	//y의 왼쪽 자식일지 오른쪽 자식일지 결정한다.
	else if (z->key <= y->key) y->left = z;
	else y->right = z;
	z->parent = y;
	rb_insertfix(z); //조건에 위반되는 경우에 insertfix함수를 호출한다.
}
void ofApp::rb_insertfix(struct node *z) {
	while (z->parent->color == RED) { //z의 부모노드가 red가 아닐때 까지 while문 수행 red-red면 조건 위반
		if (z->parent == z->parent->parent->left) { //z의 할아버지 노드의 왼쪽자식이 z의 부모노드인 경우
			if (z->parent->parent->right->color == RED) { //z의 할아버지의 오른쪽 자식 색이 빨강일 경우
				z->parent->color = BLACK; //z의 부모노드 색은 검은색
				z->parent->parent->right->color = BLACK; //z의 할아버지의 오른쪽 자식을 검은색으로 바꾼다
				z->parent->parent->color = RED; //z의 할아버지 색을 빨강으로 바꾼다
				z = z->parent->parent; //z를 z의 할아버지 노드로 바꾼다
			}
			else {//z의 할아버지 노드의 오른쪽 자식의 색이 검은색일 때
				if (z == z->parent->right) { //z가 z의 부모 오른쪽 자식인 경우 z노드를 기준으로 leftrotate연산을 한다
					z = z->parent;
					left_rotate(z);
				}

				z->parent->color = BLACK; //z의 부모노드의 색을 검은색으로 바꾼다 
				z->parent->parent->color = RED; //z의 할아버지노드의 색을 빨강으로 바꾼다
				right_rotate(z->parent->parent); //z의 할아버지노드를 기준으로 rightrotate연산을 해준다
			}
		}
		else {//z의 할아버지 노드의 오른쪽 자식이 z의 부모노드인 경우
			if (z->parent->parent->left->color == RED) {//z의 할아버지의 왼쪽 자식이 빨강인 경우
				z->parent->color = BLACK;	//z의 부모노드의 색을 검은색으로 바꾼다
				z->parent->parent->left->color = BLACK; //z의 할아버지 왼쪽 자식의 노드색을 검은색으로 바꾼다
				z->parent->parent->color = RED;	//z의 할아버지 색을 red로 바꾼다
				z = z->parent->parent; //z를 z의 할아버지 노드로 바꾼다
			}
			else {//z의 할아버지의 왼쪽 자식이 검은색인 경우
				if (z == z->parent->left) {//z가 z의 부모 왼쪽 자식인 경우 z노드를 기준으로 rightrotate연산을 한다
					z = z->parent;
					right_rotate(z);
				}

				z->parent->color = BLACK;	//z의 부모색을 검은색으로 바꾼다
				z->parent->parent->color = RED;	//z의 할아버지 색을 빨강으로 바꾼다	
				left_rotate(z->parent->parent);	//z의 할아버지 기준으로 leftrotate연산을 해준다
			}
		}
	}
	root->color = BLACK;	//root의 색을 검은색으로 만들어준다
}
void ofApp::rb_delete(struct node *z) {
	struct node *y, *x; //y와 x값 설정
	int yOriginalColor; //y의 원래색 값
	y = z; //y에 z를 저장
	yOriginalColor = y->color; //y의 원래 색 저장
	
	//z가 자식이 없거나 오른쪽 자식만 있는 경우
	if (z->left == nill) {
		x = z->right;  //x에 z의 오른쪽 자식 저장
		rb_transplant(z, z->right);
	}
	//z가 왼쪽 자식만 있는 경우
	else if (z->right == nill) {
		x = z->left; //x에 z의 왼쪽 자식을 저장
		rb_transplant(z, z->left);
	}
	//z의 자식이 둘다 있는 경우
	else {
		y = tree_mim(z->right);  //z의 successor을 찾아서 y에 저장 bst와 동일
		yOriginalColor = y->color; //y의 원래 색 저장
		x = y->right; //x는 y의 오른쪽 자식 저장
		
		//y의 부모노드가 z랑 같다면 x의 부모노드를 y로 설정
		if (y->parent == z) {
			x->parent = y;
		}
		//y의 부모노드가 z랑 다르다면
		else {
			//y의 오른쪽 자식의 하위트리로 transplant 함
			rb_transplant(y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}
		
		rb_transplant(z, y); //y를 z로 transplant 함
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	//y의 색이 검은색이였다면 deletefix 호출
	if (yOriginalColor == BLACK) {
		rb_deletefix(x);
	}
}
void ofApp::rb_deletefix(struct node *x) {
	struct node *w;
	while (x != root && x->color == BLACK) {
		//x가 왼쪽자식 노드인 경우
		if (x == x->parent->left) {
			w = x->parent->right;
			//w가 red인 경우
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				left_rotate(x->parent);
				w = x->parent->right;
			}
			//w의 두자녀가 모두 black인 경우
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->parent;
			}
			else {
				//w는 검은색이고 w의 오른쪽자식이 검은 색이고 왼쪽이 빨강인 경우
				if (w->right->color == BLACK) {
					w->color = RED;
					w->left->color = BLACK;
					right_rotate(w);
					w = x->parent->right;
				}
				//w는 검은색이고 오른쪽 자식은 빨강인 경우
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				left_rotate(x->parent);
				x = root;
			}
		}
		//x가 오른쪽자식 노드인 경우
		else {
			w = x->parent->left;

			//w의 색이 빨강인 경우
			if (w->color == RED) {
				w->color = BLACK;
				x->parent->color = RED;
				right_rotate(x->parent);
				w = x->parent->left;
			}
			//w의 자식이 둘다 black인 경우
			if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				x = x->parent;
			}
			else {
				//w는 검은색이고 w의 오른쪽자식이 검은 색이고 왼쪽이 빨강인 경우
				if (w->left->color == BLACK) {
					w->color = RED;
					w->right->color = BLACK;
					left_rotate(w);
					w = x->parent->left;
				}
				//w는 검은색이고 오른쪽 자식은 빨강인 경우
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				right_rotate(x->parent);
				x = root;
			}
		}

	}
	//x의 색을 black으로 설정
	x->color = BLACK;
}
void ofApp::rb_transplant(struct node *u, struct node *v) {
	if (u->parent == nill) root = v;  //u가 루트인 경우, v를 root값으로 설정
	else if (u == u->parent->left) u->parent->left = v; //u가 왼쪽자식인 경우 u의 부모의 왼쪽 자식을 v로 설정
	else u->parent->right = v; //u가 오른쪽자식인 경우 u의 부모의 오른쪽 자식을 v로 설정
	v->parent = u->parent;
}
// tree를 key값을 오름차순으로 화면에 그려주는 함수
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
