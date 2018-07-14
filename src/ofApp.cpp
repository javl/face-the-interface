#include "ofApp.h"

#define IDLE 0
#define LOST 999
#define DONE 1000

#define INTRO_1 1
#define INTRO_2 2
#define INTRO_3 3

#define PATH_1_1 101
#define PATH_1_2 102
#define PATH_1_3 103
#define PATH_1_4 104
#define PATH_1_5 105

#define PATH_2_1 201
#define PATH_2_2 202
#define PATH_2_3 203
#define PATH_2_4 204
#define PATH_2_5 205
#define PATH_2_6 206
#define PATH_2_7 207
#define PATH_2_8 208
#define PATH_2_9 209
#define PATH_2_10 210
#define PATH_2_11 211
#define PATH_2_12 212
#define PATH_2_13 213

#define PATH_3_1 301
#define PATH_3_2 302
#define PATH_3_3 303
#define PATH_3_4 304
#define PATH_3_5 305
#define PATH_3_6 306
#define PATH_3_7 307
#define PATH_3_8 308
#define PATH_3_9 309
#define PATH_3_10 310
#define PATH_3_11 311
#define PATH_3_12 312
#define PATH_3_13 313
#define PATH_3_14 314

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetWindowPosition(1920, 0);

    fontTechnical.load("fonts/MaisonNeueMono.otf", 40, true, true);
    fontInterface.load("fonts/Lack-Regular.otf", 80, true, true);

    ofSetVerticalSync(true);
    ofSetBackgroundColor(0);

    state = PATH_2_13;
    substate = 0;
    path = 0;
    num_faces = 0;
    smileLoops = 0;


    video.setLoopState(OF_LOOP_NONE);

    nextStateNumber = state;
    nextSubstateNumber = substate;

    // used for expressions
    learned_functions = vector<pfunct_type>(4);
    dlib::deserialize(ofToDataPath("expressions/data_ecstatic_smile.func")) >> learned_functions[0];
    dlib::deserialize(ofToDataPath("expressions/data_small_smile.func")) >> learned_functions[1];
    bigSmileValue.setFc(0.04);
    smallSmileValue.setFc(0.04);

    timer.reset();

    // grabber.setVerbose(true);
    // grabber.setDeviceID(1);
    grabber.initGrabber(1280, 720);
    sneakyImage.allocate(1280, 720, OF_IMAGE_COLOR);

    tracker.setup();
    // tracker.setFaceOrientation(OF_ORIENTATION_90_RIGHT);

    sneakyImage.allocate(1440, 1080, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofApp::update(){
    // Update tracker when there are new frames
    grabber.update();
    // Wait for about 10 seconds for the camera to become fully functional
    // if(ofGetElapsedTimef() < 10.0){
    //     return;
    // }
    if(grabber.isFrameNew()){
        tracker.update(grabber);
        num_faces = tracker.size();

        if(num_faces > 0 && state == IDLE){
            state = INTRO_1;
            substate = 0;
        }
        else if(num_faces > 0 && (state == INTRO_2 || state == PATH_2_2)){
            // Run the classifiers and update the filters
            bigSmileValue.update(learned_functions[0](makeSample()));
            smallSmileValue.update(learned_functions[1](makeSample()));
            oValue.update(learned_functions[2](makeSample()));
            neutralValue.update(learned_functions[3](makeSample()));
        }
    }

    // always update audio
    ofSoundUpdate();

    // update video if a file is loaded
    // if (video.isLoaded() && !video.isPlaying()){//} && video.isPlaying()){
    if (video.isPlaying()){//} && video.isPlaying()){
        video.update();
    }

    checkWait(); // see if a timer has expired and we want to move to the next (sub)state

    switch (state){
        default:
        case IDLE:
            if(!voice.isPlaying()){

                r = ofRandomuf();
                if (r < 0.0002){
                    //say Is anyone there?
                    playAudio("is-anyone-there_1f6c9463.mp3");
                }else if (r < 0.0004){
                    //say Hello?
                    playAudio("hello_dedd3af5.mp3");
                }
            }
        break;

        case LOST:
        break;

        case DONE:
            if (substate == 0 && !video.isPlaying()){
                video.load("audio/prerecorded/done.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }

            else if (substate == 1 && !video.isPlaying()){
                video.close();
                nextSubstate();
            }

            else if (substate == 2){
                queueNextSubstate(10000);
            }

            else if (substate == 3){
                state = IDLE;
                substate = 0;
            }

        break;

//==================================================================================================
//==================================================================================================
        case INTRO_1:
            if (substate == 0 && !voice.isPlaying()){
                //say the current time is
                playAudio("the-current-time-is_ed2f3d20.mp3");
                nextSubstate();
            }

            else if (substate == 1 && !voice.isPlaying()){
                playNumber(ofGetHours() > 12 ? ofGetHours()-12 : ofGetHours());
                nextSubstate();
            }

            else if (substate == 2 && !voice.isPlaying()){
                //say hours
                playAudio("hours_73cdddd7.mp3");
                nextSubstate();
            }

            else if (substate == 3 && !voice.isPlaying()){
                playNumber(ofGetMinutes());
                nextSubstate();
            }

            else if (substate == 4 && !voice.isPlaying()){
                //say minutes
                playAudio("minutes_640fd0cc.mp3");
                nextSubstate();
            }

            else if (substate == 5 && !voice.isPlaying()){
                //say on
                playAudio("on_ed2b5c01.mp3");
                nextSubstate();
            }

            else if (substate == 6 && !voice.isPlaying()){
                sayDayName();
                nextSubstate();
            }

            else if (substate == 7 && !voice.isPlaying()){
                //say the
                playAudio("the_8fc42c6d.mp3");
                nextSubstate();
            }

            else if (substate == 8 && !voice.isPlaying()){
                sayDayNumber();
                nextSubstate();
            }

            else if (substate == 9 && !voice.isPlaying()){
                //say of
                playAudio("of_8bf8854b.mp3");
                nextSubstate();
            }

            else if (substate == 10 && !voice.isPlaying()){
                sayMonthName();
                nextSubstate();
            }

            else if (substate == 11 && !voice.isPlaying()){
                //say 2000
                playAudio("2000_08f90c1a.mp3");
                nextSubstate();
            }

            else if (substate == 12 && !voice.isPlaying()){
                playNumber(ofGetYear()-2000);
                nextSubstate(); // to prevent repeating the number
            }

            else if (substate == 13 && !video.isPlaying()){
                video.load("audio/prerecorded/loadingCircleFlat.mp4");
                video.setLoopState(OF_LOOP_NORMAL);
                video.setSpeed(1.0);
                video.play();
                nextSubstate();
            }

            else if (substate == 14 && !voice.isPlaying()){
                queueNextSubstate(3000);
            }

            else if (substate == 15 && !voice.isPlaying()){
                video.close();
                video.setLoopState(OF_LOOP_NONE);
                nextSubstate();

            } else if (substate == 16 && !voice.isPlaying()){
                //say Detected: one human face.
                playAudio("detected-one-human-face_056f77fc.mp3");
                nextSubstate();
            }

            else if (substate == 17 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 18 && !voice.isPlaying()){
                //say Human. You are
                playAudio("human-you-are_21647cb6.mp3");
                video.stop();
                nextSubstate();
                queueNextSubstate(800);
            }else if (substate == 19 && !voice.isPlaying()){
                if (ofRandomuf() < 0.5){
                    //say Late
                    playAudio("late_d9359722.mp3");
                }else {
                    //say Right on time
                    playAudio("right-on-time_a2f4c60f.mp3");
                }
                nextSubstate();
            }else if (substate == 20 && !voice.isPlaying()){
                queueNextState(2000);
            }

            break;

//==================================================================================================
        case INTRO_2:
            if (substate == 0 && !voice.isPlaying()){
                //say Calibrating face data, please smile.
                playAudio("calibrating-face-data-please-smile_eccec926.mp3");
                nextSubstate();
            }

            else if (substate == 1 && !voice.isPlaying()){
                // showing expression bars
                queueNextSubstate(6000);
            }

            else if (substate == 2 && !voice.isPlaying()){
                takeSneakyImage();
                if (smallSmileValue.value() > 0.7 || bigSmileValue.value() > 0.7){
                    wasSmiling = true;
                }else{
                    wasSmiling = false;
                }
                nextSubstate();
            }

            else if (substate == 3 && !voice.isPlaying()){
                if(wasSmiling){
                    //say Really?
                    playAudio("really_e02f73c2.mp3");
                } else {
                    //say Your smile is hard to read.
                    playAudio("your-smile-is-hard-to-read_b433736f.mp3");
                }
                nextSubstate();
            }else if (substate == 4 && !voice.isPlaying()){
                if(wasSmiling){
                    //say People don't take you seriously when you smile.
                    playAudio("people-dont-take-you-seriously-when-you-smile_beb3fa8a.mp3");
                }else{
                    //say Your interface is expressionless.
                    playAudio("your-interface-is-expressionless_f26381ca.mp3");
                }
                nextSubstate();
            }

            else if (substate == 5 && !voice.isPlaying()){
                queueNextState(2000);
            }
            break;

//==================================================================================================
        case INTRO_3:
            if (substate == 0 && !voice.isPlaying()){
                //say Calibration complete. Starting session.
                playAudio("calibration-complete-starting-session_11fcaf98.mp3");
                nextSubstate();
            }

            else if (substate == 1 && !voice.isPlaying()){
                queueNextSubstate(1000);

            }

            else if (substate == 2 && !voice.isPlaying()){
                //say breath
                playAudio("breath_dbd782e4.mp3");
                nextSubstate();

            }

            else if (substate == 3 && !voice.isPlaying()){
                queueNextSubstate(1000);

            }

            else if (substate == 4 && !voice.isPlaying()){
                //say Slow down for a moment
                playAudio("slow-down-for-a-moment_c5a106c1.mp3");
                nextSubstate();

            }

            else if (substate == 5 && !voice.isPlaying()){
                queueNextSubstate(1000);

            }

            else if (substate == 6 && !voice.isPlaying()){
                //say Be aware of your body
                playAudio("be-aware-of-your-body_24329638.mp3");
                nextSubstate();

            }

            else if (substate == 7 && !voice.isPlaying()){
                queueNextSubstate(1000);

            }

            else if (substate == 8 && !voice.isPlaying()){
                //say Relax your algorithm
                playAudio("relax-your-algorithm_c77c81a8.mp3");
                nextSubstate();

            }

            else if (substate == 9 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 10 && !voice.isPlaying()){
                if(wasSmiling){
                    usePath3 ? path = PATH_3_1 : path = PATH_2_1;
                }else{
                    usePath3 ? path = PATH_3_1 : path = PATH_1_1;
                }
                usePath3 = !usePath3;

                state = path;
                substate = 0;
            }
            break;

//==================================================================================================
//==================================================================================================
        case PATH_1_1:
            if (substate == 0 && !video.isPlaying()){
                video.load("audio/prerecorded/1_1.mp4");
                video.setVolume(0.8);
                video.play();
                nextSubstate();
            } else if (substate == 1 && (video.getPosition() == 1.0 || video.getPosition() == -1.0) ){
                nextState();
            }
            break;
//==================================================================================================
        case PATH_1_2:
            if (substate == 0 && !voice.isPlaying()){
                //say Calculating recent interactions in public space.
                playAudio("calculating-recent-interactions-in-public-space_84dc5d1b.mp3");

                behavior_traffic = "  "+ofToString(int(ofRandom(10)));
                behavior_tax = "  "+ofToString(int(ofRandom(4)));

                behavior_social = ofToString(int(ofRandom(200)));
                if(behavior_social.size() < 10){
                    behavior_social = "  " + behavior_social;
                }else if(behavior_social.size() < 100){
                    behavior_social = " " + behavior_social;
                }
                behavior_moral = ofToString(int(ofRandom(100)));
                if(behavior_moral.size() < 10){
                    behavior_moral = "  " + behavior_moral;
                }

                nextSubstate();
            }

            else if (substate == 1 && !voice.isPlaying()){
                queueNextSubstate(2000);
            }

            else if (substate == 2 && !voice.isPlaying()){
                //say Loading: Behaviors in shops, supermarkets, government offices, the public domain.
                playAudio("loading-behaviors-in-shops-supermarkets-government-offices-the-p_72b66055.mp3");
                nextSubstate();
            }

            else if (substate == 3 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 4 && !voice.isPlaying()){
                //say People like you are likely to show
                playAudio("people-like-you-are-likely-to-show_d17108b6.mp3");
                nextSubstate();
            }

            else if (substate == 5 && !voice.isPlaying()){
                //say deviant
                playAudio("deviant_481dd6e8.mp3");
                nextSubstate();
            }

            else if (substate == 6 && !voice.isPlaying()){
                //say behavior
                playAudio("behavior_b140af3d.mp3");
                nextState();
            }
            break;
//==================================================================================================
        case PATH_1_3:
            if (substate == 0 && !voice.isPlaying()){
                //say Faces like yours are likely to commit crimes.
                playAudio("faces-like-yours-are-likely-to-commit-crimes_0a38818a.mp3");
                nextSubstate();
            }

            else if (substate == 1 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 2 && !voice.isPlaying()){
                //say Have you ever done something illegal?
                playAudio("have-you-ever-done-something-illegal_0f153ac7.mp3");
                nextSubstate();
            }

            else if (substate == 3 && !voice.isPlaying()){
                queueNextSubstate(2000);
            }

            else if (substate == 4 && !voice.isPlaying()){
                //say Reviewing
                playAudio("reviewing_4827b311.mp3");
                r = 2 + ofRandom(4);
                nextSubstate();
            }

            else if (substate == 5 && !voice.isPlaying()){
                playNumber(int(r));
                nextSubstate();
            }

            else if (substate == 6 && !voice.isPlaying()){
                //say cases
                playAudio("cases_c78bd47f.mp3");
                nextSubstate();
            }

            else if (substate == 7 && !voice.isPlaying()){
                //say of possible unwanted behavior
                playAudio("of-possible-unwanted-behavior_afcbb134.mp3");
                nextSubstate();
            }

            else if (substate == 8 && !voice.isPlaying()){
                queueNextSubstate(3000);
            }

            else if (substate == 9 && !voice.isPlaying()){
                //say Do you think you can be trusted?
                playAudio("do-you-think-you-can-be-trusted_dfd6a679.mp3");
                nextSubstate();
            }

            else if (substate == 10 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 11 && !voice.isPlaying()){
                //say Your moral score based on social associations is:
                playAudio("your-moral-score-based-on-social-associations-is_f08fe98a.mp3");
                r = ofRandomuf();
                nextSubstate();
            }

            else if (substate == 12 && !voice.isPlaying()){
                playNumber(int(r * 100));
                r = ofRandomuf();
                nextSubstate();
            }

            else if (substate == 13 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 14 && !voice.isPlaying()){
                //say Hence your
                playAudio("hence-your_ffe5ebe8.mp3");
                r = ofRandomuf();
                nextSubstate();
            }

            else if (substate == 15 && !voice.isPlaying()){
                if(r < 0.33){
                    //say poor
                    playAudio("poor_5cffaf93.mp3");
                }else if(r < 0.66){
                    //say mediocre
                    playAudio("mediocre_a741acb6.mp3");
                }else {
                    //say positive
                    playAudio("positive_82082716.mp3");
                }
                nextSubstate();
            }

            else if (substate == 16 && !voice.isPlaying()){
                //say reflection
                playAudio("reflection_40f3468c.mp3");
                nextSubstate();
            }

            else if (substate == 17 && !voice.isPlaying()){
                queueNextSubstate(2000);
            }

            else if (substate == 18 && !voice.isPlaying()){
                r = ofRandomuf();
                //say Your friends have an average moral score of:
                playAudio("your-friends-have-an-average-moral-score-of_51a43800.mp3");
                nextSubstate();
            }

            else if (substate == 19 && !voice.isPlaying()){
                playNumber(int(r*100));
                nextSubstate();
            }

            else if (substate == 20 && !voice.isPlaying()){
                queueNextState(4000);
            }

            break;
//==================================================================================================
        case PATH_1_4:
            // interface falls, screenshot in background and some live data
            if (substate == 0){
                image.load("fake_desktop.png");
                nextSubstate();
            }

            else if (substate == 1){
                queueNextSubstate(5000);
            }

            else if (substate == 2){
                nextState();
            }
            break;

        case PATH_1_5:
            if(substate == 0 && !voice.isPlaying()){
                video.load("audio/prerecorded/1_5.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }
            else if(substate == 0 && !video.isPlaying()){
                // video van anja
                // toon eerder genomen foto
                state = DONE;
            }
            break;

//==================================================================================================
//==================================================================================================
        case PATH_2_1:
            if (substate == 0 && !video.isPlaying()){
                video.load("audio/prerecorded/2_1.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }

            else if (substate == 1 && !video.isPlaying()){
                queueNextState(2000);
                smileLoops = 0; // used in next state
            }

            break;

        case PATH_2_2:
            if (substate == 0 && !video.isPlaying()){
                video.load("audio/prerecorded/2_2.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }

            else if (substate == 1 && !video.isPlaying()){
                video.close();
                nextSubstate();

            }

            else if (substate == 2){
                queueNextState(2000);
            }

            else if (substate == 3){
                if (smallSmileValue.value() > 0.7 || bigSmileValue.value() > 0.7){
                    wasSmiling = true;
                }else{
                    wasSmiling = false;
                }
                nextSubstate();
            }

            else if(substate == 4 && !voice.isPlaying()){
                if(wasSmiling){
                    //say Thank you, that is beautiful
                    playAudio("thank-you-that-is-beautiful_558cb503.mp3");
                }else{
                    //say Why don’t you smile?
                    playAudio("why-dont-you-smile_c286752c.mp3");
                }
                nextSubstate();
            }

            else if(substate == 4 && !voice.isPlaying()){
                if(wasSmiling){
                    if(smileLoops < 2){ // loop 3 times
                        smileLoops++;
                        //say Can you do that again?
                        playAudio("can-you-do-that-again_140c2219.mp3");
                        substate = 2;
                    }else{
                        smileLoops = 0;
                        nextSubstate();
                    }
                }else{
                    //say Am I insulting you?
                    playAudio("am-i-insulting-you_cc49baa2.mp3");
                    nextSubstate();
                }
            }

            else if (substate == 4 && !voice.isPlaying()){
                queueNextState(1000);
            }

            break;
//==================================================================================================
        case PATH_2_3:
            if(substate == 0 && !voice.isPlaying()){
                tracker.drawDebugStylized();
                queueNextState(3000);
            }
            break;
//==================================================================================================
        case PATH_2_4:
            if(substate == 0 && !voice.isPlaying()){
                tracker.drawDebugStylized();
                queueNextState(3000);
            }
            break;
//==================================================================================================
        case PATH_2_5:
            if(substate == 0 && !voice.isPlaying()){
                playAudioFile("prerecorded/camera-shutter-click-01.mp3");
                nextSubstate();
            }

            else if (substate == 0 && !voice.isPlaying()){
                nextState();
            }

            break;
//==================================================================================================
        case PATH_2_6:
            if(substate == 0 && !voice.isPlaying()){
                //say Don’t you love taking selfies?
                playAudio("dont-you-love-taking-selfies_c7aab442.mp3");
                nextSubstate();
            }

            else if(substate == 1 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if(substate == 2 && !voice.isPlaying()){
                //say Do you I feel like you’re not really present if you don’t?
                playAudio("do-you-i-feel-like-youre-not-really-present-if-you-dont_726d9e19.mp3");
                nextSubstate();
            }

            else if(substate == 3 && !voice.isPlaying()){
                queueNextState(1000);
            }

            break;

//==================================================================================================
        case PATH_2_7:
            if(substate == 0 && !voice.isPlaying()){
                //say Calculating recent interactions in the virtual public domain
                playAudio("calculating-recent-interactions-in-the-virtual-public-domain_bae523c5.mp3");
                nextSubstate();
            }

            else if(substate == 1 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if(substate == 2 && !voice.isPlaying()){
                //say Loading: Behavior on webshops, cookies, adblockers, the public domain.
                playAudio("loading-behavior-on-webshops-cookies-adblockers-the-public-domai_03340346.mp3");
                nextSubstate();
            }

            else if(substate == 3 && !voice.isPlaying()){
                queueNextState(1000);
            }

            break;

//==================================================================================================
        case PATH_2_8:
            // to fix wrong numbering in word file
            nextState();
            break;

//==================================================================================================
        case PATH_2_9:
            if(substate == 0 && !voice.isPlaying()){
                r = ofRandomuf();
                //say Your reflection is considered
                playAudio("your-reflection-is-considered_61e23d9b.mp3");
                nextSubstate();
            }

            else if(substate == 1 && !voice.isPlaying()){
                if(r < 0.25){
                    //say average
                    playAudio("average_6927a3a7.mp3");
                }else if(r < 0.5){
                    //say boring
                    playAudio("boring_8c32b1f7.mp3");
                }else if(r < 0.75){
                    //say neutral
                    playAudio("neutral_5da248ea.mp3");
                }else{
                    //say forgettable
                    playAudio("forgettable_8fae639e.mp3");
                }
                nextSubstate();
            }

            else if(substate == 2 && !voice.isPlaying()){
                //say but I like looking at you
                playAudio("but-i-like-looking-at-you_c3e8f49e.mp3");
                nextSubstate();
            }

            else if(substate == 23 && !voice.isPlaying()){
                queueNextState(1000);
            }

            break;

//==================================================================================================
        case PATH_2_10:
            if (substate == 0 && !voice.isPlaying()){
                video.load("audio/prerecorded/2_10.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }

            if (substate == 1 && !video.isPlaying()){
                video.close();
                nextState();
            }
            // to fix wrong numbering in word file
            nextState();
            break;

//==================================================================================================
        case PATH_2_11:
            // to fix wrong numbering in word file
            nextState();
            break;

//==================================================================================================
        case PATH_2_12:
            if (substate == 0 && !voice.isPlaying()){
                //say There are 7635783240 people in the world. But I’m still looking at, you.
                playAudio("there-are-7635783240-people-in-the-world-but-im-still-looking-at_904e2ac5.mp3");
                nextSubstate();
            }

            else if (substate == 1 && !voice.isPlaying()){
                video.load("audio/prerecorded/2_10.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }

            else if(substate == 2 && !video.isPlaying()){
                // else if (substate == 2 && (video.getPosition() == 1.0 || video.getPosition() == -1.0)){
                video.close();
                nextSubstate();
            }

            else if (substate == 3 && !voice.isPlaying()){
                //say I lied
                playAudio("i-lied_be4def92.mp3");
                nextSubstate();
            }

            else if (substate == 4 && !voice.isPlaying()){
                //say There are (random number > 1000) people with your kind of visual make-up.
                playAudio("there-are-random-number-1000-people-with-your-kind-of-visual-mak_05baeed8.mp3");
                nextSubstate();
            }

            else if (substate == 5 && !voice.isPlaying()){
                //say People like, you. I couldn’t care less.
                playAudio("people-like-you-i-couldnt-care-less_b8f05b99.mp3");
                nextSubstate();
            }

            else if (substate == 6 && !voice.isPlaying()){
                queueNextState(1000);
            }
            break;

//==================================================================================================
        case PATH_2_13:
            if (substate == 0 && !voice.isPlaying()){
                video.load("audio/prerecorded/stickers.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }
            else if (substate == 1 && !video.isPlaying()){
                video.load("audio/prerecorded/2_13.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }

            else if(substate == 2 && !video.isPlaying()){
                state = DONE;
                substate = 0;
            }
            break;
//==================================================================================================


//==================================================================================================
//==================================================================================================
        case PATH_3_1:
            if (substate == 0 && !voice.isPlaying()){
                //say Starting path three
                playAudio("starting-path-three_ee87c5ea.mp3");
                nextSubstate();
            }else if (substate == 1 && !voice.isPlaying()){
                //say Starting path three
                playAudio("starting-path-three_ee87c5ea.mp3");
                queueNextState(2000);
            }
            break;
        case PATH_3_2:

            if (substate == 0 && !voice.isPlaying()){
                //say Most people find the current temperature of
            }

            else if (substate == 1 && !voice.isPlaying()){
                //say <degrees>
            }

            else if (substate == 2 && !voice.isPlaying()){
                //say degrees
            }

            else if (substate == 3 && !voice.isPlaying()){
                if (currentTemperature < 0 ){
                    //say too cold
                }
                else if (currentTemperature < 15 ){
                    //say chilly
                }
                else if (currentTemperature < 25 ){
                    //say agreeable
                }
                else {
                    //say too hot
                }
                nextSubstate();
            }
            else if (substate == 4 && !voice.isPlaying()){
                queueNextState(2000);
            }

            break;
        case PATH_3_3:
            if (substate == 0 && !voice.isPlaying()){
                //say This experience is brought to you by Auto Flow, the online data streaming service
                nextSubstate();
            }

            else if(substate == 1 && !voice.isPlaying()){
                queueNextState(1000);
            }

            break;
        case PATH_3_4:
            if (substate == 0 && !voice.isPlaying()){
                video.load("audio/prerecorded/3_4.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }

            else if (substate == 1 && !video.isPlaying()){
                video.close();
                nextSubstate();
            }

            else if (substate == 2 && !voice.isPlaying()){
                queueNextState(1000);
            }
            break;

        case PATH_3_5:
            if (substate == 0 && !voice.isPlaying()){
                video.load("audio/prerecorded/3_5.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }

            else if (substate == 1 && !video.isPlaying()){
                video.close();
                nextSubstate();
            }

            else if (substate == 2 && !voice.isPlaying()){
                queueNextState(1000);
            }
            break;

        case PATH_3_6:
            if(substate == 0 && !voice.isPlaying()){
                queueNextState(3000);
            }
            break;
        case PATH_3_7:
            nextState();
            break;
        case PATH_3_8:
            nextState();
            break;
        case PATH_3_9:
            nextState();
            break;

        case PATH_3_10:
            if (substate == 0 && !voice.isPlaying()){
                video.load("audio/prerecorded/3_10.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.play();
                nextSubstate();
            }

            else if (substate == 1 && !video.isPlaying()){
                video.close();
                nextSubstate();
            }

            else if (substate == 2 && !voice.isPlaying()){
                queueNextState(1000);
            }
            break;

        case PATH_3_11:
            nextState(); // fix for incorrect numbering in word file
            break;
        case PATH_3_12:
            nextState(); // fix for incorrect numbering in word file
            break;
        case PATH_3_13:
            nextState(); // fix for incorrect numbering in word file
            break;
        case PATH_3_14:
            //
            break;

    }
    voicePlaying = voice.isPlaying();

    // // if in the last loop voicePlaying was true, but now voice.isPlaying() is false, it means
    // // it just finished
    // bool voiceFinished = false;
    // if (voicePlaying && !voice.isPlaying()){
    //     voiceFinished = true;
    // }
    // ofLog() << "camera initialized: " << grabber.isInitialized();

}

//--------------------------------------------------------------
void ofApp::draw(){
    // Draw the webcam and the face tracker
    ofSetColor(255);
    float ratio = grabber.getWidth() / 600;

    grabber.draw(0, ofGetHeight()-int(grabber.getHeight()/ratio), 600, int(grabber.getHeight()/ratio));

    ofDrawBitmapStringHighlight("State   : "+ofToString(state), 10, 20);
    ofDrawBitmapStringHighlight("Substate: "+ofToString(substate), 10, 40);
    ofDrawBitmapStringHighlight("voice   : "+ofToString(voice.isPlaying()), 10, 60);
    ofDrawBitmapStringHighlight("timer   : "+ofToString(timer.isTimerRunning()), 10, 80);
    ofDrawBitmapStringHighlight("faces   : "+ofToString(num_faces), 10, 100);

    if(num_faces > 0) {
        ofDrawBitmapStringHighlight("got face", 10, 200);
    }
    if(video.isPlaying()){
        ofSetColor(255);
        video.draw(0, 0);
        if(video.getPosition() == 1.0){
            video.close();
        }
    }

    switch (state){
        case INTRO_1:
            //pythonedit 14
            // if (substate== 14 && video.isPlaying()){
                // video.draw((ofGetWidth() - video.getWidth())/2.0, (ofGetHeight() - video.getHeight())/2.0);
            // }else
            if(substate >= 13){
                tracker.drawDebug();
            }
            break;

        case INTRO_2:
            // video.draw((ofGetWidth() - video.getWidth())/2.0, (ofGetHeight() - video.getHeight())/2.0);
            tracker.drawDebug();
            drawSmileStats();
            break;

        case INTRO_3:
            sneakyImage.draw(0, 0, 1200, 1920);
            break;

        case PATH_1_2:
            tracker.drawDebugStylized(0, 0);

            if(substate > 1){
                fontTechnical.drawString("Traffic violations:      " + behavior_traffic, 10, 100);
                fontTechnical.drawString("Social currency:         " + behavior_social, 10, 200);
                fontTechnical.drawString("Violations of income tax:" + behavior_tax, 10, 300);
                fontTechnical.drawString("Current moral score:     " + behavior_moral, 10, 400);
            }
            break;

        case PATH_1_4:
            // Draw fallen desktop
            image.draw(0, 0, 1200, 1920);
            grabber.draw(0, 0, 640, 480);
            tracker.drawDebug(0, 0);

            for (int i = 0; i < 10; ++i){
                fontTechnical.drawString("some technical text?", 10, 10 + 50 * i);
            }
            break;

        case PATH_1_5:
            sneakyImage.draw(0, 0);
            break;

        case PATH_2_13:
            tracker.drawDebugStylized(0, 0);
            break;

        case PATH_3_6:
            tracker.drawDebugStylized(0, 0);
            break;

        default:
        break;
    }

    // tracker.drawDebugStylized(0, 0, 300, 300);

    //==============================================================
    // DRAW EXPRESSION
    /*
    for (int i = 0; i < 4; i++) {
        ofSetColor(255);

        string str;
        float val;
        switch (i) {
            case 0:
                str = "BIG SMILE";
                val = bigSmileValue.value();
                break;
            case 1:
                str = "SMALL SMILE";
                val = smallSmileValue.value();
                break;
        }

        ofDrawBitmapStringHighlight(str, 20, 0);
        ofDrawRectangle(20, 20, 300 * val, 30);
    }
     */
    // END DRAW EXPRESSION
    //==============================================================

    /*
    if(num_faces > 0) {
        ofDrawBitmapStringHighlight("got face", 10, 200);

        timeSinceLastFace = ofGetElapsedTimef();
        if (state == IDLE){
            state = INTRO_1;
            substate = 0;
        }
        else if(state == LOST){
            voice.stop();
            state = continue_state;
            substate = continue_substate;
        }
    }else if (num_faces == 0){
        if(ofGetElapsedTimef() - timeSinceLastFace > 10.0f) {
            continue_state = state;
            continue_substate = substate;
            voice.stop();
            state = LOST;
            substate = 0;
        }
        else if(ofGetElapsedTimef() - timeSinceLastFace > 5.0f) {
             // I lost you. Where did you go?
        }
    }
     */

}

void ofApp::nextState(){
    nextStateNumber = state + 1;
    nextSubstateNumber = 0;
    bUpdateState = true;
}
void ofApp::nextSubstate(){
    nextStateNumber = state; // keep same state
    nextSubstateNumber = substate + 1;
    ofLog() << "nextSubstate: nextSubstateNumber = " << nextSubstateNumber;
    bUpdateState = true;
}

void ofApp::queueNextState(int millis){
    // starts a timer for a specified time and store what new state we want to go to
    // when it runs out
    if(!timer.isTimerRunning()){
        nextStateNumber = state + 1;
        nextSubstateNumber = 0;
        timer.setup(millis, false, true);
    }
}

void ofApp::queueNextSubstate(int millis){
    // starts a timer for a specified time and store what new SUBstate we want to go to
    // when it runs out
    if(!timer.isTimerRunning()) {
        ofLog() << "timer not running, queue next substate: " << (substate+1);
        nextStateNumber = state;
        nextSubstateNumber = substate + 1;
        timer.setup(millis, false, true);
        ofLog() << "time running? : " << timer.isTimerRunning();
    }
}

void ofApp::checkWait(){
    // check if our timer expired and if so, update our state and substate
    if(bUpdateState){ // nextSubstate() / nextState() trigger a direct update during the next update loop
        bUpdateState = false;
        state = nextStateNumber;
        substate = nextSubstateNumber;
    }else if (timer.isTimerFinished()){
        ofLog() << "timer finished";
        timer.reset();
        state = nextStateNumber;
        substate = nextSubstateNumber;
    }
    // ofLog() << "timeleftinmillis: " << timer.getTimeLeftInMillis();
}

// Take a picture we can use later
void ofApp::takeSneakyImage() {
    sneakyImage.setFromPixels(grabber.getPixels());
}



void ofApp::playAudio(string filename){
    if (filename != lastFilename){
        lastFilename = filename;
    }
    voice.load("audio/"+filename);
    voice.play();
}

// Similar to playAudio but with prerecorded in its path
void ofApp::playAudioFile(string filename){
    if (filename != lastFilename){
        lastFilename = filename;
    }
    voice.load("audio/"+filename);
    voice.play();
}

void ofApp::playNumber(int number){
    voice.load("audio/numbers/"+ofToString(number)+".mp3");
    voice.play();
}

void ofApp::drawSmileStats(){
    ofPushMatrix();
    // ofTranslate(0, ofGetHeight()-450);
    ofTranslate(0, 20);
    for (int i = 0; i < 4; i++) {
        ofSetColor(255);

        string str;
        float val;
        switch (i) {
            case 0:
                str = "BIG SMILE";
                val = bigSmileValue.value();
                break;
            case 1:
                str = "SMALL SMILE";
                val = smallSmileValue.value();
                break;
        }

        // ofDrawBitmapStringHighlight(str, 20, 0);
        fontInterface.drawString(str, 100, 0);

        ofFill();
        ofDrawRectangle(100, 30, 1000*val, 100);

        ofTranslate(0, 240);
    }
    ofPopMatrix();

}
// Function that creates a sample for the classifier containing the mouth and eyes
sample_type ofApp::makeSample(){
    auto outer = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::OUTER_MOUTH);
    auto inner = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::INNER_MOUTH);

    auto lEye = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::LEFT_EYE);
    auto rEye = tracker.getInstances()[0].getLandmarks().getImageFeature(ofxFaceTracker2Landmarks::RIGHT_EYE);

    ofVec2f vec = rEye.getCentroid2D() - lEye.getCentroid2D();
    float rot = vec.angle(ofVec2f(1,0));

    vector<ofVec2f> relativeMouthPoints;

    ofVec2f centroid = outer.getCentroid2D();
    for(ofVec2f p : outer.getVertices()){
        p -= centroid;
        p.rotate(rot);
        p /= vec.length();

        relativeMouthPoints.push_back(p);
    }

    for(ofVec2f p : inner.getVertices()){
        p -= centroid;
        p.rotate(rot);
        p /= vec.length();

        relativeMouthPoints.push_back(p);
    }

    sample_type s;
    for(int i=0;i<20;i++){
        s(i*2+0) = relativeMouthPoints[i].x;
        s(i*2+1) = relativeMouthPoints[i].y;
    }
    return s;
}

void ofApp::sayMonthName(){
    switch(ofGetMonth()){
        default:
        case 1:
            //say January
            playAudio("january_86f5978d.mp3");
        break;
        case 2:
            //say February
            playAudio("february_659e59f0.mp3");
        break;
        case 3:
            //say March
            playAudio("march_fa3e5eda.mp3");
        break;
        case 4:
            //say April
            playAudio("april_3fcf026b.mp3");
        break;
        case 5:
            //say May
            playAudio("may_195fbb57.mp3");
        break;
        case 6:
            //say June
            playAudio("june_688937cc.mp3");
        break;
        case 7:
            //say July
            playAudio("july_1b539f6f.mp3");
        break;
        case 8:
            //say August
            playAudio("august_41ba7089.mp3");
        break;
        case 9:
            //say September
            playAudio("september_cc5d9056.mp3");
        break;
        case 10:
            //say October
            playAudio("october_eca60ae8.mp3");
        break;
        case 11:
            //say November
            playAudio("november_7e823b37.mp3");
        break;
        case 12:
            //say December
            playAudio("december_82331503.mp3");
        break;
    }

}
void ofApp::sayDayNumber(){
    switch(ofGetDay()){
        default:
        case 1:
            //say first
            playAudio("first_8b04d5e3.mp3");
            break;
        case 2:
            //say second
            playAudio("second_a9f0e61a.mp3");
            break;
        case 3:
            //say third
            playAudio("third_dd5c8bf5.mp3");
            break;
        case 4:
            //say fourth
            playAudio("fourth_c0759f24.mp3");
            break;
        case 5:
            //say fifth
            playAudio("fifth_0883a652.mp3");
            break;
        case 6:
            //say sixth
            playAudio("sixth_93c38fcf.mp3");
            break;
        case 7:
            //say seventh
            playAudio("seventh_5098d245.mp3");
            break;
        case 8:
            //say eighth
            playAudio("eighth_a76a3a8b.mp3");
            break;
        case 9:
            //say ninth
            playAudio("ninth_ed971f9f.mp3");
            break;
        case 10:
            //say tenth
            playAudio("tenth_4758950d.mp3");
            break;
        case 11:
            //say eleventh
            playAudio("eleventh_ff983aca.mp3");
            break;
        case 12:
            //say twelfth
            playAudio("twelfth_0e1ac824.mp3");
            break;
        case 13:
            //say thirteenth
            playAudio("thirteenth_70e585b3.mp3");
            break;
        case 14:
            //say fourteenth
            playAudio("fourteenth_669f9bfd.mp3");
            break;
        case 15:
            //say fifteenth
            playAudio("fifteenth_71412509.mp3");
            break;
        case 16:
            //say sixteenth
            playAudio("sixteenth_80e1ed18.mp3");
            break;
        case 17:
            //say seventeenth
            playAudio("seventeenth_9aeee42f.mp3");
            break;
        case 18:
            //say eighteenth
            playAudio("eighteenth_27e892ba.mp3");
            break;
        case 19:
            //say nineteenth
            playAudio("nineteenth_2d4c6676.mp3");
            break;
        case 20:
            //say twentieth
            playAudio("twentieth_16524998.mp3");
            break;
        case 21:
            //say twenty-first
            playAudio("twentyfirst_4e12e1fc.mp3");
            break;
        case 22:
            //say twenty-second
            playAudio("twentysecond_2342369b.mp3");
            break;
        case 23:
            //say twenty-third
            playAudio("twentythird_9b1c4c78.mp3");
            break;
        case 24:
            //say twenty-fourth
            playAudio("twentyfourth_986da31c.mp3");
            break;
        case 25:
            //say twenty-fifth
            playAudio("twentyfifth_e3b9ebd2.mp3");
            break;
        case 26:
            //say twenty-sixth
            playAudio("twentysixth_7afa7cf2.mp3");
            break;
        case 27:
            //say twenty-seventh
            playAudio("twentyseventh_5e435181.mp3");
            break;
        case 28:
            //say twenty-eighth
            playAudio("twentyeighth_5f865b63.mp3");
            break;
        case 29:
            //say twenty-ninth
            playAudio("twentyninth_3c27d763.mp3");
            break;
        case 30:
            //say thirtieth
            playAudio("thirtieth_1351dc57.mp3");
            break;
        case 31:
            //say thirty-first
            playAudio("thirtyfirst_8e6d40c8.mp3");
            break;
    }
}
void ofApp::sayDayName(){
    switch(ofGetWeekday()){
        default:
        case 0:
            //say Sunday
            playAudio("sunday_9d1a0949.mp3");
            break;
        case 1:
            //say Monday
            playAudio("monday_6f8522e0.mp3");
            break;
        case 2:
            //say Tuesday
            playAudio("tuesday_5792315f.mp3");
            break;
        case 3:
            //say Wednesday
            playAudio("wednesday_796c1635.mp3");
            break;
        case 4:
            //say Thursday
            playAudio("thursday_78ae6f0c.mp3");
            break;
        case 5:
            //say Friday
            playAudio("friday_c33b138a.mp3");
            break;
        case 6:
            //say Saterday
            playAudio("saterday_1dc61e9d.mp3");
            break;
    }

}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch(key){
        case 'q':
            state++;
            break;

        case 'a':
            state--;
            break;

        case 'w':
            substate++;
            break;

        case 's':
            substate--;
            break;

        case 'f':
            ofToggleFullscreen();
            break;

        default:
            break;
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