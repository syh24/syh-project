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
		void keyReleased(int key); //Ű�� �������� �� 
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		bool readFile(); //������ �о��ִ� �Լ�
		void freeMemory(); // malloc�� ������ �޸� �������ִ� �Լ�
		void init_game();  //������ ���� �ʱ�ȭ ����
		int ball_x; //ball�� x��ǥ
		int ball_y; //ball�� y��ǥ
		int HEIGHT;//�̷��� ����
		int WIDTH;//�̷��� �ʺ�
		//char** input;//�ؽ�Ʈ ������ ��� ������ ��� ������ �迭�̴�.
		typedef struct maze{
			bool u;
			bool d;
			bool r;
			bool l;
			char p;
		}maze;
		maze **input; //�̷θ� ��Ÿ���� 2���� �迭

		struct node {
			int rank;
			int key;
			int color;
			struct node *parent;
			struct node *left;
			struct node *right;
		};
		struct node *root; //rb tree�� ��Ʈ ���
		struct node *nill; //rb tree�� nil

		void left_rotate(struct node *x); //left_rotate������ �ϴ� �Լ�
		void right_rotate(struct node *x);	//right_rotate ������ �ϴ� �Լ�
		void rb_insert(int key);  //rb tree���� ����� key ���� �־��ִ� �Լ�
		void rb_insertfix(struct node *z); //rb tree�� ������ �����ؼ� ���ο� ������ �ʿ��� ��
		struct node* rank_search(int rank); // rank�� ã�� �Լ�
		struct node* tree_search(int key); //rb tree���� key���� ã���ִ� �Լ�
		struct node* tree_mim(struct node *x); //x�� successor�� ã���ִ� �Լ�
		void rb_transplant(struct node *u, struct node *v);
		void rb_delete(struct node *z);  //rb tree���� key���� �������ִ� �Լ�
		void rb_deletefix(struct node *x); //rb tree�� ������ �����ؼ� ���ο� ������ �ʿ��� ��
		void tree_print(struct node* x); // tree�� ������ ranking ȭ�鿡 ����ش�
		void memfree(struct node* x); // rb tree�� ��� ������ �޸𸮸� �������ش�
		void init_rb(); //rb tree�� �ʱ�ȭ ���ִ� �Լ�
		int mv; // ball�� ��� ���������� üũ�ϴ� ����
		int rank; // ��� ǥ��
		int p;	 //ȭ�鿡 ��ŷ�� ǥ���� �� ��ġ�� �����ϰ� ���ִ� ����
		void construct_maze();
		int maze_col;//�̷�ĭ�� ���� �ε����� ����Ų��.
		int maze_row;//�̷�ĭ�� ���� �ε����� ����Ų��.
		int draw_flag = 0;
		int isOpen; //������ ���ȴ����� �Ǵ��ϴ� ����. 0�̸� �ȿ��Ȱ� 1�̸� ���ȴ�.
		int mvup_flag; //���� �����ϼ� �ִ��� �Ǵ�, �����ϼ� ������ 1 ������ 0
		int mvdown_flag;//�Ʒ��� �����ϼ� �ִ��� �Ǵ�, �����ϼ� ������ 1 ������ 0
		int mvright_flag;//���������� �����ϼ� �ִ��� �Ǵ�, �����ϼ� ������ 1 ������ 0
		int mvleft_flag;//�������� �����ϼ� �ִ��� �Ǵ�, �����ϼ� ������ 1 ������ 0
		int finish_flag; //game�� �������� 1, �ƴϸ� 0
		int regame_flag; //game�� �ٽ��Ұ��� ����� flag
		int rank_flag; //rank�� �����ٰŸ� 1 �ƴϸ� 0
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
