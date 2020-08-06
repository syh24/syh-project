/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

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
*/

	#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#include <fstream>
#define RED 0
#define BLACK 1

class ofApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key); // Traps escape key if exit disabled
		void keyReleased(int key); //키가 떼어졌을 때 
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		bool readFile(); //파일을 읽어주는 함수
		void freeMemory(); // malloc한 변수를 메모리 해제해주는 함수
		void init_game();  //게임의 값을 초기화 해줌
		int ball_x; //ball의 x좌표
		int ball_y; //ball의 y좌표
		int HEIGHT;//미로의 높이
		int WIDTH;//미로의 너비
		//char** input;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
		typedef struct maze{
			bool u;
			bool d;
			bool r;
			bool l;
			char p;
		}maze;
		maze **input; //미로를 나타내는 2차원 배열

		struct node {
			int rank;
			int key;
			int color;
			struct node *parent;
			struct node *left;
			struct node *right;
		};
		struct node *root; //rb tree의 루트 노드
		struct node *nill; //rb tree의 nil

		void left_rotate(struct node *x); //left_rotate연산을 하는 함수
		void right_rotate(struct node *x);	//right_rotate 연산을 하는 함수
		void rb_insert(int key);  //rb tree에서 노드의 key 값을 넣어주는 함수
		void rb_insertfix(struct node *z); //rb tree의 조건을 위반해서 새로운 조정이 필요할 때
		struct node* rank_search(int rank); // rank를 찾는 함수
		struct node* tree_search(int key); //rb tree에서 key값을 찾아주는 함수
		struct node* tree_mim(struct node *x); //x의 successor를 찾아주는 함수
		void rb_transplant(struct node *u, struct node *v);
		void rb_delete(struct node *z);  //rb tree에서 key값을 삭제해주는 함수
		void rb_deletefix(struct node *x); //rb tree의 조건을 위반해서 새로운 조정이 필요할 때
		void tree_print(struct node* x); // tree의 값들을 ranking 화면에 띄어준다
		void memfree(struct node* x); // rb tree의 모든 노드들의 메모리를 해제해준다
		void init_rb(); //rb tree를 초기화 해주는 함수
		int mv; // ball이 몇번 움직였는지 체크하는 변수
		int rank; // 등수 표기
		int p;	 //화면에 랭킹을 표기할 때 위치를 설정하게 해주는 변수
		void construct_maze();
		int maze_col;//미로칸의 열의 인덱스를 가리킨다.
		int maze_row;//미로칸의 행의 인덱스를 가리킨다.
		int draw_flag = 0;
		int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
		int mvup_flag; //위로 움직일수 있는지 판단, 움직일수 있으면 1 없으면 0
		int mvdown_flag;//아래로 움직일수 있는지 판단, 움직일수 있으면 1 없으면 0
		int mvright_flag;//오른쪽으로 움직일수 있는지 판단, 움직일수 있으면 1 없으면 0
		int mvleft_flag;//왼쪽으로 움직일수 있는지 판단, 움직일수 있으면 1 없으면 0
		int finish_flag; //game이 끝났으면 1, 아니면 0
		int regame_flag; //game을 다시할건지 물어보는 flag
		int rank_flag; //rank를 보여줄거면 1 아니면 0
		// Menu
		ofxWinMenu * menu; // Menu object
		void appMenuFunction(string title, bool bChecked); // Menu return function

		// Used by example app
		ofTrueTypeFont myFont;
        ofImage myImage;
		float windowWidth, windowHeight;
		HWND hWnd; // Application window
		HWND hWndForeground; // current foreground window

		// Example menu variables
		bool bShowInfo;
		bool bFullscreen;
		bool bTopmost;
		bool isgame;
		// Example functions
 		void doFullScreen(bool bFull);
		void doTopmost(bool bTop);	
};
