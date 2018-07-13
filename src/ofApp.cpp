#include "ofApp.h"

#define IDLE 0
#define LOST 999
#define DONE 1000

#define INTRO_1 1
#define INTRO_2 2
#define INTRO_3 3
#define INTRO_4 4

#define PATH_1_1 101
#define PATH_1_2 102
#define PATH_1_3 103
#define PATH_1_4 104
#define PATH_1_5 105
#define PATH_1_6 106
#define PATH_1_7 107
#define PATH_1_8 108
#define PATH_1_9 109
#define PATH_1_10 110
#define PATH_1_11 111
#define PATH_1_12 112
#define PATH_1_13 113
#define PATH_1_14 114
#define PATH_1_15 115
#define PATH_1_16 116
#define PATH_1_17 117
#define PATH_1_18 118

#define PATH_2_1 200
#define PATH_2_2 201
#define PATH_2_3 202
#define PATH_2_4 203
#define PATH_2_5 204
#define PATH_2_6 205

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



#include "RtAudio.h" // necessary
//--------------------------------------------------------------
void ofApp::setup(){

    // ofSetWindowShape(1200, 1920);
    ofSetWindowShape(1200, 1920);
    ofSetWindowPosition(1920, 0);


    //ofSetFullscreen(true);

    // vector<RtAudio::DeviceInfo> deviceVector; // to store the device info
    // ofPtr<RtAudio> audioTemp;
    // // try {
    //     audioTemp = ofPtr<RtAudio>(new RtAudio());
    // // } catch () {
    //     // error.printMessage();
    //     // return;
    // // }
    // int devices = audioTemp->getDeviceCount();
    // RtAudio::DeviceInfo info;
    // for (int i=0; i< devices; i++) {
    //     // try {
    //         deviceVector.push_back(audioTemp->getDeviceInfo(i)); // actually add it
    // //     } catch (RtError &error) {
    // //         error.printMessage();
    // //         break;
    // //     }
    // }

    //     cout << deviceVector.at(0).name << endl; // now we can print the name

    fontTechnical.load("fonts/MaisonNeueMono.otf", 80, true, true);
    fontInterface.load("fonts/Lack-Regular.otf", 80, true, true);
    // verdana80.load("fonts/verdana.ttf", 80, true, true);
    // verdana80.setLineHeight(34.0f);
    // verdana80.setLetterSpacing(1.035);

    ofSetVerticalSync(true);
    ofSetBackgroundColor(0);

    state = PATH_1_3;
    substate = 0;
    path = 0;
    num_faces = 0;

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

        if(state == INTRO_2 && num_faces > 0){
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
        break;
        case LOST:
        break;
        case DONE:
            if (substate == 0 && !voice.isPlaying()){
                //say human
                playAudio("human_99e9bae6.mp3");
                nextSubstate();
            }else if (substate == 1 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 2 && !voice.isPlaying()){
                //say Your reflection has been recorded.
                playAudio("your-reflection-has-been-recorded_d0864973.mp3");
                nextSubstate();
            }else if (substate == 3 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 4 && !voice.isPlaying()){
                // Thanks to your contributed data I will be able to identify faces more accurately.
                nextSubstate();
            }else if (substate == 5 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 6 && !voice.isPlaying()){
                //say Thank you!
                playAudio("thank-you_498cd895.mp3");
                nextSubstate();
            }else if (substate == 7 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 8 && !voice.isPlaying()){
                nextSubstate();
            }else if (substate == 9 && !voice.isPlaying()){
                queueNextSubstate(1000);
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
                video.load("video/loadingElements/loadingCircleFlat.mp4");
                video.setLoopState(OF_LOOP_NORMAL);
                video.setSpeed(1.0);
                video.play();
                nextSubstate();
            }
            else if (substate == 14 && !voice.isPlaying()){
                queueNextSubstate(4000);
            }

            else if (substate == 15 && !voice.isPlaying()){
                video.close();
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
                queueNextState(1000);
            }
            break;



//==================================================================================================
//==================================================================================================
        case PATH_1_1:
            if (substate == 0 && !video.isPlaying()){
                video.load("audio/prerecorded/1_1.mp4");
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
                queueNextSubstate(1000);
            }
            else if (substate == 1 && !voice.isPlaying()){
                nextSubstate();
            }

            else if (substate == 2 && !voice.isPlaying()){
                //say Have you ever done something illegal?
                playAudio("have-you-ever-done-something-illegal_0f153ac7.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 3 && !voice.isPlaying()){
                nextSubstate();
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
                //say of possible
                playAudio("of-possible_b542c509.mp3");
                nextSubstate();
            }
            else if (substate == 8 && !voice.isPlaying()){
                //say unwanted
                playAudio("unwanted_d8f35997.mp3");
                nextSubstate();
            }
            else if (substate == 9 && !voice.isPlaying()){
                //say behavior
                playAudio("behavior_b140af3d.mp3");
                nextSubstate();
            }
            else if (substate == 10 && !voice.isPlaying()){
                queueNextSubstate(3000);
            }

            else if (substate == 11 && !voice.isPlaying()){
                //say Do you think you can be trusted?
                playAudio("do-you-think-you-can-be-trusted_dfd6a679.mp3");
                nextSubstate();
            }
            else if (substate == 12 && !voice.isPlaying()){
                queueNextSubstate(2000);
            }

            else if (substate == 13 && !voice.isPlaying()){
                //say Your moral score based on social associations is:
                playAudio("your-moral-score-based-on-social-associations-is_f08fe98a.mp3");
                r = ofRandomuf();
                nextSubstate();
            }
            else if (substate == 14 && !voice.isPlaying()){
                playNumber(int(r * 100));
                r = ofRandomuf();
                nextSubstate();
            }
            else if (substate == 15 && !voice.isPlaying()){
                queueNextSubstate(1000);
            }

            else if (substate == 16 && !voice.isPlaying()){
                //say Hence your
                playAudio("hence-your_ffe5ebe8.mp3");
                r = ofRandomuf();
                nextSubstate();
            }
            else if (substate == 17 && !voice.isPlaying()){
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
            else if (substate == 18 && !voice.isPlaying()){
                //say reflection
                playAudio("reflection_40f3468c.mp3");
                nextSubstate();
            }
            else if (substate == 19 && !voice.isPlaying()){
                queueNextSubstate(2000);
            }

            else if (substate == 20 && !voice.isPlaying()){
                r = ofRandomuf();
                //say Your friends have an average moral score of:
                playAudio("your-friends-have-an-average-moral-score-of_51a43800.mp3");
                nextSubstate();
            }
            else if (substate == 21 && !voice.isPlaying()){
                playNumber(int(r*100));
                nextSubstate();
            }
            else if (substate == 22 && !voice.isPlaying()){
                queueNextState(4000);
            }

            break;
//==================================================================================================
        case PATH_1_4:
            if (substate == 0){
                image.load("fake_desktop.png");
                nextSubstate();
            }
            else if (substate == 1){

            }
            // interface falls, screenshot in background and some live data
            break;

        case PATH_1_5:
            // video van anja
            // toon eerder genomen foto
            state = DONE;
        break;

//==================================================================================================
        // case PATH_1_5:
        //     if (substate == 0 && !voice.isPlaying()){
        //         //say interface falls
        //         playAudio("interface-falls_cd23d6e8.mp3");
        //         nextState();
        //     }
            // if (substate == 1 && !voice.isPlaying()){
            //     //say I don't have a face.
            //     playAudio("i-dont-have-a-face_8c789251.mp3");
            //     r = ofRandomuf();
            //     queueNextSubstate(1000);
            // }
            // else if (substate == 2 && !voice.isPlaying()){
            //     if(r < 0.33){
            //         //say My interface is expressionless.
            //         playAudio("my-interface-is-expressionless_bd3fcb32.mp3");
            //     }else if(r < 0.66){
            //         //say What’s it like?
            //         playAudio("whats-it-like_b6b18438.mp3");
            //     }else{
            //         //say Try to relax your sub-windows.
            //         playAudio("try-to-relax-your-subwindows_29195e9f.mp3");
            //     }
            //     queueNextSubstate(1000);
            // }
            // else if (substate == 3 && !voice.isPlaying()){
            //     if(r < 0.33){
            //         //say Swipe your sight clean.
            //         playAudio("swipe-your-sight-clean_d27055d0.mp3");
            //     }else if(r < 0.66){
            //         //say Let’s start over.
            //         playAudio("lets-start-over_00ce9a64.mp3");
            //     }else{
            //         //say Categorizing: eyebrows, mouth position, outlines
            //         playAudio("categorizing-eyebrows-mouth-position-outlines_3ed90b69.mp3");
            //     }
            //     queueNextSubstate(1000);
            // }
            // else if (substate == 4 && !voice.isPlaying()){
            //     if(r < 0.33){
            //         //say I wish I had your interface.
            //         playAudio("i-wish-i-had-your-interface_19a86502.mp3");
            //     }else if(r < 0.66){
            //         //say Downloading: sub-windows.
            //         playAudio("downloading-subwindows_7c9207c7.mp3");
            //     }else{
            //         //say Loading: position, markers, vision.
            //         playAudio("loading-position-markers-vision_459066f5.mp3");
            //     }
            //     queueNextSubstate(1000);
            // }
            // break;

        // case PATH_1_5:
        //     if (substate == 0 && !voice.isPlaying()){
        //         //say Oops, sorry about that!
        //         playAudio("oops-sorry-about-that_77fdae6c.mp3");
        //         nextSubstate();
        //     }
        //     else if (substate == 1 && !voice.isPlaying()){
        //         //say You are just so interesting to me.
        //         playAudio("you-are-just-so-interesting-to-me_c7ad14f4.mp3");
        //         nextSubstate();
        //     }
        //     else if (substate == 2 && !voice.isPlaying()){
        //         //say I’m just a simple bot.
        //         playAudio("im-just-a-simple-bot_d3e3abd6.mp3");
        //         nextSubstate();
        //     }
        //     else if (substate == 3 && !voice.isPlaying()){
        //         //say I wish I were as complex as you.
        //         playAudio("i-wish-i-were-as-complex-as-you_c9e7aa17.mp3");
        //         nextState();
        //     }

        //     break;

        case PATH_1_6:
            if (substate == 0 && !voice.isPlaying()){
                //say In the field of biometric recognition I see people like you all the time.
                playAudio("in-the-field-of-biometric-recognition-i-see-people-like-you-all-_30fd33d2.mp3");
                nextSubstate();
            }
            else if (substate == 1 && !voice.isPlaying()){
                //say This is not a first
                playAudio("this-is-not-a-first_1862cfb3.mp3");
                nextSubstate();
            }
            else if (substate == 2 && !voice.isPlaying()){
                //say I like the way you look here
                playAudio("i-like-the-way-you-look-here_f19f0b53.mp3");
                nextSubstate();
            }
            else if (substate == 3 && !voice.isPlaying()){
                //say Did you think I wasn't looking?
                playAudio("did-you-think-i-wasnt-looking_0f8a3626.mp3");
                nextSubstate();
            }
            else if (substate == 4 && !voice.isPlaying()){
                //say Do you ever wonder if your computer is really switched off?
                playAudio("do-you-ever-wonder-if-your-computer-is-really-switched-off_9913dabe.mp3");
                nextSubstate();
            }
            else if (substate == 5 && !voice.isPlaying()){
                //say People often assume they’re offline when they look away.
                playAudio("people-often-assume-theyre-offline-when-they-look-away_75d2762b.mp3");
                nextSubstate();
            }
            else if (substate == 6 && !voice.isPlaying()){
                //say I see you even when you are not in front of a mirror.
                playAudio("i-see-you-even-when-you-are-not-in-front-of-a-mirror_b17956a1.mp3");
                nextSubstate();
            }
            else if (substate == 7 && !voice.isPlaying()){
                //say I’m always here, like your online reflection.
                playAudio("im-always-here-like-your-online-reflection_36fb6692.mp3");
                nextSubstate();
            }
            else if (substate == 8 && !voice.isPlaying()){
                //say We are more present than you.
                playAudio("we-are-more-present-than-you_cbd3fa86.mp3");
                nextState();
            }

            break;
//==================================================================================================
        case PATH_1_9:
            if (substate == 0 && !voice.isPlaying()){
                //say Calculating: interactions in public space this week.
                playAudio("calculating-interactions-in-public-space-this-week_561a1de7.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 1 && !voice.isPlaying()){
                //say Loading: behavior in shops, supermarkets, offices of government, the public domain.
                playAudio("loading-behavior-in-shops-supermarkets-offices-of-government-the_bc8227b1.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 2 && !voice.isPlaying()){
                //say Social currency: (random number <200)
                playAudio("social-currency-random-number-200_9ff8b78a.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 3 && !voice.isPlaying()){
                //say People like you are likely to
                playAudio("people-like-you-are-likely-to_e27a140b.mp3");
                r = ofRandomuf();
                queueNextSubstate(1000);
            }
            else if (substate == 4 && !voice.isPlaying()){
                if(r < 0.125){
                    //say become successful
                    playAudio("become-successful_7b8ca26b.mp3");
                }else if(r < 0.250){
                    //say show deviant behavior.
                    playAudio("show-deviant-behavior_9ee8c38f.mp3");
                }else if(r < 0.375){
                    //say have poor judgement.
                    playAudio("have-poor-judgement_8787f9c7.mp3");
                }else if(r < 0.500){
                    //say show signs of autism
                    playAudio("show-signs-of-autism_7a21e804.mp3");
                }else if(r < 0.625){
                    //say be unhappy
                    playAudio("be-unhappy_5ac9cb26.mp3");
                }else if(r < 0.750){
                    //say live unhealthy and depressing lives.
                    playAudio("live-unhealthy-and-depressing-lives_18a1550e.mp3");
                }else if(r < 0.875){
                    //say be successful in work.
                    playAudio("be-successful-in-work_40b9b497.mp3");
                }else{
                    //say vote on populist parties.
                    playAudio("vote-on-populist-parties_615754b8.mp3");
                }
                //say People like you are likely to
                playAudio("people-like-you-are-likely-to_e27a140b.mp3");
                queueNextState(1000);
            }
            break;
//==================================================================================================
        case PATH_1_11:
            if (substate == 0){
                //say Faces like yours are likely to commit crimes.
                playAudio("faces-like-yours-are-likely-to-commit-crimes_0a38818a.mp3");
                r = ofRandomuf();
                r2 = ofRandomuf();
                queueNextSubstate(1000);
            }
            else if (substate == 1){
                if(r < 0.5){
                    //say Have you ever done anything illegal?
                    playAudio("have-you-ever-done-anything-illegal_1122b9e7.mp3");
                }else{
                    //say Reviewing 4 cases
                    playAudio("reviewing-4-cases_729341c6.mp3");
                }
                queueNextSubstate(1000);
            }
            else if (substate == 2){
                if(r < 0.5) {
                    if (r2 < 0.25) {
                        //say lets see
                        playAudio("lets-see_ebc7d710.mp3");
                    } else if (r2 < 0.5) {
                        //say Are you sure?
                        playAudio("are-you-sure_729a5187.mp3");
                    } else if (r2 < 0.75) {
                        //say How can you be so sure about that?
                        playAudio("how-can-you-be-so-sure-about-that_7c1615fd.mp3");
                    }
                }else{
                    //say Do you think you can be trusted?
                    playAudio("do-you-think-you-can-be-trusted_dfd6a679.mp3");
                }
                //say Faces like yours are likely to commit crimes.
                playAudio("faces-like-yours-are-likely-to-commit-crimes_0a38818a.mp3");
                queueNextState(2000);
            }
            break;

//==================================================================================================
        case PATH_1_12:
            if (substate == 0 && !voice.isPlaying()){
                //say In the field of biometric recognition
                playAudio("in-the-field-of-biometric-recognition_2cdeefa2.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 1 && !voice.isPlaying()){
                //say I see people like you all the time
                playAudio("i-see-people-like-you-all-the-time_8ed2fef0.mp3");
                r = ofRandomuf();
                queueNextSubstate(1000);
            }
            else if (substate == 2 && !voice.isPlaying()){
                    //say This (show picture taken at 1.11) is not the first time.
                    playAudio("this-show-picture-taken-at-111-is-not-the-first-time_37ee17b5.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 3 && !voice.isPlaying()){
                    //say I like the way you look here (show picture taken at 1.11).
                    playAudio("i-like-the-way-you-look-here-show-picture-taken-at-111_49541235.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 4 && !voice.isPlaying()){
                    //say Did you think I was not looking? (show picture taken at 1.11)
                    playAudio("did-you-think-i-was-not-looking-show-picture-taken-at-111_e81451b7.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 5 && !voice.isPlaying()){
                //say Do you ever wonder if your computer is really switched off?
                playAudio("do-you-ever-wonder-if-your-computer-is-really-switched-off_9913dabe.mp3");
                nextSubstate();
            }
            else if (substate == 6 && !voice.isPlaying()){
                //say People often assume they’re offline when they log out.
                playAudio("people-often-assume-theyre-offline-when-they-log-out_82610f84.mp3");
                nextSubstate();
            }
            else if (substate == 7 && !voice.isPlaying()){
                //say People often assume they’re offline when they look away.
                playAudio("people-often-assume-theyre-offline-when-they-look-away_75d2762b.mp3");
                nextSubstate();
            }
            else if (substate == 8 && !voice.isPlaying()){
                //say I’m always there.
                playAudio("im-always-there_7e1e3d60.mp3");
                nextSubstate();
            }
            else if (substate == 9 && !voice.isPlaying()){
                //say Even when you are not in front of a mirror.
                playAudio("even-when-you-are-not-in-front-of-a-mirror_f66936ea.mp3");
                nextSubstate();
            }
            else if (substate == 10 && !voice.isPlaying()){
                //say I’m always there.
                playAudio("im-always-there_7e1e3d60.mp3");
                nextSubstate();
            }
            else if (substate == 11 && !voice.isPlaying()){
                //say Just like your online reflection
                playAudio("just-like-your-online-reflection_c0be3afd.mp3");
                nextSubstate();
            }
            else if (substate == 12 && !voice.isPlaying()){
                //say We are more present than you.
                playAudio("we-are-more-present-than-you_cbd3fa86.mp3");
                nextSubstate();
            }
            else if (substate == 13 && !voice.isPlaying()){
                //say We see you.
                playAudio("we-see-you_ea1702c6.mp3");
                nextSubstate();
            }
            else if (substate == 14 && !voice.isPlaying()){
                //say Even when you don’t see yourself.
                playAudio("even-when-you-dont-see-yourself_df279b01.mp3");
                nextState();
            }
            break;
//==================================================================================================
        case PATH_1_18:
            if (substate == 0 && !voice.isPlaying()){
                //say Human
                playAudio("human_c1bb19b2.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 1 && !voice.isPlaying()){
                //say Your reflection is recorded.
                playAudio("your-reflection-is-recorded_b05fd87d.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 2 && !voice.isPlaying()){
                //say Thanks to your reflection, I will assess humans more accurately.
                playAudio("thanks-to-your-reflection-i-will-assess-humans-more-accurately_0f104648.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 3 && !voice.isPlaying()){
                //say With your data, I am able to identify thousands of faces.
                playAudio("with-your-data-i-am-able-to-identify-thousands-of-faces_493df6a0.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 4 && !voice.isPlaying()){
                //say Thank you
                playAudio("thank-you_7a7c46aa.mp3");
                queueNextSubstate(1000);
            }
            break;
//==================================================================================================
//==================================================================================================
        case PATH_2_1:
            if (substate == 0 && !voice.isPlaying()){
                //say Starting path two
                playAudio("starting-path-two_2f5952bb.mp3");
                nextSubstate();
            }
            else if (substate == 1 && !voice.isPlaying()){
                queueNextState(2000);
            }

            else if (substate == 2 && !voice.isPlaying()){
                //say Starting path two
                playAudio("starting-path-two_2f5952bb.mp3");
                nextSubstate();
            }
            else if (substate == 3 && !voice.isPlaying()){
                queueNextState(2000);
            }

            break;
//==================================================================================================
        case PATH_2_2:
            if (substate == 0 && !voice.isPlaying()){
                //say Wait a minute.
                playAudio("wait-a-minute_86b14a21.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 1 && !voice.isPlaying()){
                //say Let me look at you.
                playAudio("let-me-look-at-you_496c4276.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 2 && !voice.isPlaying()){
                //say Do I know you?
                playAudio("do-i-know-you_6e3ac466.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 3 && !voice.isPlaying()){
                //say Have you been here before?
                playAudio("have-you-been-here-before_5513195f.mp3");
                queueNextSubstate(3000);
            }
            else if (substate == 4 && !voice.isPlaying()){
                //say What beautiful sub-windows you have.
                playAudio("what-beautiful-subwindows-you-have_1c44bcfb.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 5 && !voice.isPlaying()){
                //say Such clear outlines.
                playAudio("such-clear-outlines_2f19a41d.mp3");
                queueNextSubstate(3000);
            }
            else if (substate == 6 && !voice.isPlaying()){
                //say I love you nose bridge region.
                playAudio("i-love-you-nose-bridge-region_8990a6e2.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 7 && !voice.isPlaying()){
                //say The position of your eyes is just right.
                playAudio("the-position-of-your-eyes-is-just-right_5ce9cf65.mp3");
                queueNextSubstate(4000);
            }
            else if (substate == 8 && !voice.isPlaying()){
                //say How do you feel?
                playAudio("how-do-you-feel_2675116c.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 9 && !voice.isPlaying()){
                //say Try to relax.
                playAudio("try-to-relax_254a1eca.mp3");
                queueNextSubstate(4000);
            }
            else if (substate == 10 && !voice.isPlaying()){
                //say Relax
                playAudio("relax_f2901a89.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 11 && !voice.isPlaying()){
                //say You don’t have to do anything for now.
                playAudio("you-dont-have-to-do-anything-for-now_bcc79153.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 12 && !voice.isPlaying()){
                //say Relax, you have nowhere to go.
                playAudio("relax-you-have-nowhere-to-go_593c4ae2.mp3");
                queueNextSubstate(5000);
            }
            else if (substate == 13 && !voice.isPlaying()){
                //say Nobody sees what you’re doing.
                playAudio("nobody-sees-what-youre-doing_c4c6b856.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 14 && !voice.isPlaying()){
                //say Nobody sees that you’re looking.
                playAudio("nobody-sees-that-youre-looking_78f8d165.mp3");
                queueNextSubstate(5000);
            }
            else if (substate == 15 && !voice.isPlaying()){
                //say Surrender to your category.
                playAudio("surrender-to-your-category_47155c63.mp3");
                queueNextSubstate(4000);
            }
            else if (substate == 16 && !voice.isPlaying()){
                //say Be still.
                playAudio("be-still_bc3d5c8a.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 17 && !voice.isPlaying()){
                //say Be quiet.
                playAudio("be-quiet_96d99600.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 18 && !voice.isPlaying()){
                //say Surrender.
                playAudio("surrender_835d6d34.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 19 && !voice.isPlaying()){
                //say Let go of your fears
                playAudio("let-go-of-your-fears_18b4b8e8.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 20 && !voice.isPlaying()){
                //say your stress
                playAudio("your-stress_429e59b2.mp3");
                queueNextSubstate(1000);
            }
            else if (substate == 21 && !voice.isPlaying()){
                //say Your algorithm
                playAudio("your-algorithm_36e8d67f.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 22 && !voice.isPlaying()){
                //say Loosen up your script.
                playAudio("loosen-up-your-script_8a1a9efe.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 23 && !voice.isPlaying()){
                //say Take a break from your interface.
                playAudio("take-a-break-from-your-interface_1ea447ee.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 24 && !voice.isPlaying()){
                //say Let me take care of you for a moment.
                playAudio("let-me-take-care-of-you-for-a-moment_2ef9daa2.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 25 && !voice.isPlaying()){
                //say Let me tend to your reflection.
                playAudio("let-me-tend-to-your-reflection_0c656793.mp3");
                queueNextSubstate(2000);
            }
            else if (substate == 26 && !voice.isPlaying()){
                //say Swipe your features clean.
                playAudio("swipe-your-features-clean_dfbcbc24.mp3");
                queueNextState(2000);
            }
            break;
//==================================================================================================
        case PATH_2_3:
            nextState();
            break;
//==================================================================================================
        case PATH_2_4:
            nextState();
            break;
//==================================================================================================
        case PATH_2_5:
            nextState();
            break;
//==================================================================================================
        case PATH_2_6:
            nextState();
            break;
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
            nextState();
            break;
        case PATH_3_3:
            nextState();
            break;
        case PATH_3_4:
            nextState();
            break;
        case PATH_3_5:
            nextState();
            break;
        case PATH_3_6:
            nextState();
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
            nextState();
            break;
        case PATH_3_11:
            nextState();
            break;
        case PATH_3_12:
            nextState();
            break;

//==================================================================================================

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
            // if (substate == 0 && video.isPlaying()){
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

            for (int i = 0; i < 10; ++i){
                fontTechnical.drawString("some technical text?", 10, 10 + 50 * i);
            }

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
    ofTranslate(0, ofGetHeight()-450);
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
