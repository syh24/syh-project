#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	TF.open("input.txt", ofFile::ReadOnly);
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	ofSetLineWidth(4);
}

//--------------------------------------------------------------
void ofApp::update(){

}


//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(127, 23, 31);
	ofDrawRectangle(0, 0, 1024, 40);
	ofDrawRectangle(0, 728, 1024, 40);


	if (draw_flag) {
		for (int i = 0; i < n1; i++)
		{
			ofSetColor(ofColor::brown);
			ofSetLineWidth(5);
			ofDrawLine(k[i][0], k[i][1], k[i][2], k[i][3]);
		}
		for (int i = 0; i < n2; i++)
		{
			if (s[i][2]) {
				ofSetColor(ofColor::red);
				ofDrawCircle(s[i][0], s[i][1], 10);
			}
			else if(s[i][2] == 0)
			{
				ofSetColor(ofColor::black);
				ofDrawCircle(s[i][0], s[i][1], 10);
			}
		}
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'l' || key == 'L') {
		ofFileDialogResult f = ofSystemLoadDialog();;
		f.filePath = "/bin/data/";
		if (f.bSuccess)
		{
			cout << "we found the target file.\n";
			processOpenFileSelection(f);
		}
		else {
			cout << "We don't found the target file.\n";
			return;
		}
	}
	if (key == 'd' || key == 'D')
	{
		draw_flag = 1;
		cout << "Selected Dot Coordinate is ";
		cout << "(";
		cout << s[0][0];
		cout << ", ";
		cout << s[0][1];
		cout << ")" << endl;

	}
	if (key == 'q' || key == 'Q')
	{
		delete k;
		cout << "for line segment has been freed." << endl;
		delete s;
		cout << "for dot has been freed." << endl;
		return;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == OF_KEY_RIGHT)
	{
		s[water_flag][2] = 0;
		water_flag++;
		if (water_flag == n2)  water_flag = 0;
	    s[water_flag][2] = 1;
		cout << "Selected Dot Coordinate is ";
		cout << "(";
		cout << s[water_flag][0];
		cout << ", ";
		cout << s[water_flag][1];
		cout << ")" << endl;
	}
	else if (key == OF_KEY_LEFT)
	{
		s[water_flag][2] = 0;
		water_flag--;
		if (water_flag < 0)  water_flag = n2-1;
		s[water_flag][2] = 1;
		cout << "Selected Dot Coordinate is ";
		cout << "(";
		cout << s[water_flag][0];
		cout << ", ";
		cout << s[water_flag][1];
		cout << ")" << endl;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
void ofApp::processOpenFileSelection(ofFileDialogResult openFileResult)
{

		TF >> n1;
		cout << "The number of line is : " << n1 << endl;
		k = new int*[n1];
		for (int i = 0; i < n1; i++)
		{
			k[i] = new int[4];
		}
		for (int i = 0; i < n1; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				TF >> k[i][j];
			}
		}
		TF >> n2;
		cout << "The number of dots is: " << n2 << endl;
		s = new int*[n2];
		for (int i = 0; i < n2; i++)
		{
			s[i] = new int[3];
		}
		for (int i = 0; i < n2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				TF >> s[i][j];
			}
		}
		for (int i = 0; i < n2; i++) {
			s[i][2] = 0;
		}
		s[0][2] = 1;
}
