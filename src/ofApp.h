#pragma once

#include "ofMain.h"
#include "ofxDlib.h"
#include "ofxFaceTracker2.h"
#include "ofxBiquadFilter.h"
#include "ofxTimer.h"
#include "ofxSvg.h"
#include "ofxXmlSettings.h"

typedef dlib::matrix<double,40,1> sample_type;
typedef dlib::radial_basis_kernel<sample_type> kernel_type;

typedef dlib::decision_function<kernel_type> dec_funct_type;
typedef dlib::normalized_function<dec_funct_type> funct_type;

typedef dlib::probabilistic_decision_function<kernel_type> probabilistic_funct_type;
typedef dlib::normalized_function<probabilistic_funct_type> pfunct_type;


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


		void drawSmileStats();
		void checkWait();
		void nextState();
		void nextSubstate();
		void queueNextState(int millis);
		void queueNextSubstate(int millis);

		void sayMonthName();
		void sayDayNumber();
		void sayDayName();
		bool voiceDone();

		// string urlResponse(ofHttpResponse &httpResponse);

		bool sayReady;

		// debug stuff
		bool cursor;

		int num_faces;
		int prev_num_faces;
		float last_face_time;
		float r; // used to store random float to make choices
		float r2; // used to store random float to make choices

		// NOT SURE IF ELEMENTS BELOW ARE USED
		// for expression recognition
		sample_type makeSample();
		ofxBiquadFilter1f neutralValue;
		ofxBiquadFilter1f smallSmileValue;
		ofxBiquadFilter1f bigSmileValue;
		ofxBiquadFilter1f oValue;

		vector<pfunct_type> learned_functions;

		ofxSVG stocks;
		int stocksX;
		int stocksY;

		int currentTemp;
		bool tempFromInternet;
		string city;
		void urlResponse(ofHttpResponse & response);

		ofTrueTypeFont fontTechnical;
		ofTrueTypeFont fontInterface;

		bool wasSmiling;
		int smileLoops;
		bool usePath3;

		int path; // which path; 1, 2 or 3
		int state;
		int substate;
		int continue_state; // used when face is lost and found again
		int continue_substate;
		int nextStateNumber;
		int nextSubstateNumber;
		bool bUpdateState;

		bool voicePlaying;
		bool voiceFinished;

		long timeSinceLastFace;

		ofSoundPlayer voice;
		ofVideoPlayer video;
		ofImage image;

		string lastFilename;
		void playAudio(string filename);
		void playAudioFile(string filename);
		void playNumber(int number);


		ofxTimer timer;

		string behavior_traffic;
		string behavior_social;
		string behavior_tax;
		string behavior_moral;
		string behavior_downloads;
		string behavior_privacy_act;


		// Video and face tracker
		ofVideoGrabber grabber;
		ofxFaceTracker2 tracker;
		ofImage sneakyImage; // used for taking a picture
		float sneaky_ratio;
		float sneaky_w;
		float sneaky_h;
		float dataSendStart; // for the 'fallen' screen

		ofxXmlSettings temperatureXML;

		void takeSneakyImage();
		void drawStocks();
		int getCurrentTemperature();


};
