#include "ofApp.h"

#define IDLE 0
#define DONE 999
#define DISCLAIMER 1000

#define INTRO_1 1
#define INTRO_2 2
#define INTRO_3 3

#define PATH_1_1 101
#define PATH_1_2 102
#define PATH_1_3 103
#define PATH_1_4 104
#define PATH_1_5 105
#define PATH_1_6 106

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

#define VIDEO_VOLUME 0.5

//--------------------------------------------------------------
void ofApp::setup(){
    
    city = "den haag";
    currentTemp = 28;
    tempFromInternet = false;
    //    if( temperatureXML.loadFile("temperatureSettings.xml") ){
    //        city             = temperatureXML.getValue("temperature:city", city);
    //        currentTemp      = temperatureXML.getValue("temperature:value", currentTemp);
    //        int fromInternet = temperatureXML.getValue("temperature:useInternet", 1);
    //        fromInternet == 1 ? tempFromInternet = true : tempFromInternet = false;
    //    }
    
    ofHideCursor();
    state = IDLE;
    substate = 0;
    
    //debug
//     state = PATH_2_6;
    
    path = 0;
    num_faces = 0;
    smileLoops = 0;
    sayReady = true; // make sure viola says something during the first loop of IDLE
    
    // stocks.load("stocks_shape.svg");
    // stocksX = ofGetWidth();
    // stocksY = 6;
    
    ofSetWindowPosition(1920, 0);
    
    fontTechnical.load("fonts/MaisonNeueMono.otf", 26, true, true);
    fontInterface.load("fonts/Lack-Regular.otf", 80, true, true);
    
    ofSetVerticalSync(true);
    ofSetBackgroundColor(0);
    
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

    if(grabber.isFrameNew()){
//        grabber.getPixels().mirror(false, true);

        tracker.update(grabber);
        num_faces = tracker.size();
        
        if (num_faces > 0){
            last_face_time = ofGetElapsedTimef();
        }
        
        if(num_faces > 0 && state == IDLE){
            state = INTRO_1;
            substate = 0;
        }
        // update the smile info
        else if(num_faces > 0 && (state == INTRO_2 || state == PATH_2_1)){
            // Run the classifiers and update the filters
            bigSmileValue.update(learned_functions[0](makeSample()));
            smallSmileValue.update(learned_functions[1](makeSample()));
            oValue.update(learned_functions[2](makeSample()));
            neutralValue.update(learned_functions[3](makeSample()));
        }
        else if(num_faces == 0 && (state == INTRO_2 || state == PATH_2_1)){
            // Run the classifiers and update the filters
            bigSmileValue.update(0.0);
            smallSmileValue.update(0.0);
            oValue.update(0.0);
            neutralValue.update(0.0);
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
                if (sayReady){
                    sayReady = false;
                    r = 0.0001;
                }
                
                if (r < 0.0002){
                    //say Is daar iemand?
                    playAudio("is-daar-iemand_ea60e8db.mp3");
                }
                else if (r < 0.0004){
                    //say Hallo?
                    playAudio("hallo_1177acec.mp3");
                }
            }
            break;
            
        case DONE:
            if (substate == 0){
                playAudioFile("prerecorded/disclaimer.mp3");
                nextSubstate();
            }
            
            else if (substate == 1 && voiceDone()){
                nextSubstate();
            }
            
            else if(substate == 2 && voiceDone()){
                // stay in this mode until we haven't seen a face in for seconds
                if (num_faces == 0 && (ofGetElapsedTimef()-last_face_time) > 15.0){
                    state = IDLE;
                    substate = 0;
                    sayReady = true; // make sure viola says something during the first loop of IDLE
                }
            }
            
            break;
            
            
            //==================================================================================================
            //==================================================================================================
        case INTRO_1:
            if (substate == 0 && !voice.isPlaying()){
                //say Het is
                playAudio("het-is_bc0c17bb.mp3");
                nextSubstate();
            }
            
            else if (substate == 1 && voiceDone()){
                playNumber(ofGetHours() > 12 ? ofGetHours()-12 : ofGetHours());
                nextSubstate();
            }
            
            else if (substate == 2 && voiceDone()){
                //say uur
                playAudio("uur_7991ef6f.mp3");
                nextSubstate();
            }
            
            else if (substate == 3 && voiceDone()){
                playNumber(ofGetMinutes());
                nextSubstate();
            }
            
            else if (substate == 4 && voiceDone()){
                //say op
                playAudio("op_11d8c28a.mp3");
                nextSubstate();
            }
            
            else if (substate == 5 && voiceDone()){
                sayDayName();
                nextSubstate();
            }
            
            else if (substate == 6 && voiceDone()){
                nextSubstate();
            }
            
            else if (substate == 7 && voiceDone()){
                //                sayDayNumber();
                playNumber(ofGetDay());
                nextSubstate();
            }
            
            else if (substate == 8 && voiceDone()){
                nextSubstate();
            }
            
            else if (substate == 9 && voiceDone()){
                sayMonthName();
                nextSubstate();
            }
            
            else if (substate == 10 && voiceDone()){
                //say 2000
                playAudio("2000_08f90c1a.mp3");
                nextSubstate();
            }
            
            else if (substate == 11 && voiceDone()){
                playNumber(ofGetYear()-2000);
                nextSubstate(); // to prevent repeating the number
            }
            
            else if (substate == 12 && voiceDone()){
                video.load("audio/prerecorded/loadingCircle.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 13 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 14 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 15){
                //say Eén menselijk gezicht herkend
                playAudio("en-menselijk-gezicht-herkend_ae2a8087.mp3");
                nextSubstate();
            }
            
            else if (substate == 16 && voiceDone()){
                queueNextState(2000);
            }
            
            break;
            
            //==================================================================================================
        case INTRO_2:
            if (substate == 0 && voiceDone()){
                //say Even lachen, alsjeblieft
                playAudio("even-lachen-alsjeblieft_5f2196ba.mp3");
                nextSubstate();
            }
            
            else if (substate == 1 && voiceDone()){
                // showing expression bars
                queueNextSubstate(6000);
            }
            
            else if (substate == 2 && voiceDone()){
                takeSneakyImage();
                if (smallSmileValue.value() > 0.4 || bigSmileValue.value() > 0.4){
                    wasSmiling = true;
                }else{
                    wasSmiling = false;
                }
                nextSubstate();
            }
            
            else if (substate == 3 && voiceDone()){
                video.load("audio/prerecorded/s_2.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 4 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 5 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }

            else if (substate == 6 && voiceDone()){
                if(wasSmiling){
                    r = ofRandomuf();
                    if(r < 0.5){
                        //say Mensen nemen je niet serieus als je lacht.
                        playAudio("mensen-nemen-je-niet-serieus-als-je-lacht_c2328ca4.mp3");
                    }else{
                        //say Goed gedaan.
                        playAudio("goed-gedaan_8c6462b3.mp3");
                    }
                }else{
                    r = ofRandomuf();
                    if(r < 0.5){
                        //say Jouw interface zegt me heel weinig.
                        playAudio("jouw-interface-zegt-me-heel-weinig_ca0389c2.mp3");
                    }else{
                        //say Jouw gezicht is moeilijk te lezen.
                        playAudio("jouw-gezicht-is-moeilijk-te-lezen_7a28b0b5.mp3");
                    }
                }
                nextSubstate();
            }

            else if (substate == 7 && voiceDone()){
                queueNextState(1000);
            }
            
            break;
            
            //==================================================================================================
        case INTRO_3:
            
            if (substate == 0){
                video.load("audio/prerecorded/s_3.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 1 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 2 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 3 && voiceDone()){
                queueNextSubstate(1000);
            }
            
            else if (substate == 4){
                if(wasSmiling){
                    //                    usePath3 ? path = PATH_3_1 : path = PATH_2_1;
                    path = PATH_2_1;
                }else{
                    //                    usePath3 ? path = PATH_3_1 : path = PATH_1_1;
                    path = PATH_1_1;
                }
                //                usePath3 = !usePath3;
                
                //debug
                // path = PATH_3_1;
                state = path;
                substate = 0;
            }
            
            break;
            
            //==================================================================================================
            //==================================================================================================
        case PATH_1_1:

            if (substate == 0){
                video.load("audio/prerecorded/1_1.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 1 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 2 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }

            else if (substate == 3 && voiceDone()){
                queueNextState(2500);
            }
            
            break;
            //==================================================================================================
        case PATH_1_2:
            if (substate == 0 && voiceDone()){
                //say Actuele berekening van interacties in de openbare ruimte
                playAudio("actuele-berekening-van-interacties-in-de-openbare-ruimte_ec995b71.mp3");
                
                behavior_traffic = "  "+ofToString(int(ofRandom(10)));
                behavior_tax = "  "+ofToString(int(ofRandom(4)));
                
                behavior_social = ofToString(int(ofRandom(200)));
                if(behavior_social.size() < 10){
                    behavior_social = "  " + behavior_social;
                }
                else if(behavior_social.size() < 100){
                    behavior_social = " " + behavior_social;
                }
                
                r = int(ofRandomuf() * 100);
                behavior_moral = ofToString(r);
                if(behavior_moral.size() < 10){
                    behavior_moral = "  " + behavior_moral;
                }
                
                nextSubstate();
            }
            
            else if (substate == 1 && voiceDone()){
                queueNextSubstate(2000);
            }
            
            else if (substate == 2 && voiceDone()){
                //say Gegevens ophalen van: gedrag in
                playAudio("gegevens-ophalen-van-gedrag-in_2e4d3643.mp3");
                nextSubstate();
            }
            
            else if (substate == 3 && voiceDone()){
                //say winkels, supermarkten, gemeentelijke gebouwen, het publieke domein.
                playAudio("winkels-supermarkten-gemeentelijke-gebouwen-het-publieke-domein_ded40d0b.mp3");
                nextSubstate();
            }
            
            else if (substate == 4 && voiceDone()){
                queueNextSubstate(1000);
            }
            
            if (substate == 5){
                video.load("audio/prerecorded/1_2_2.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 6 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 7 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 8 && voiceDone()){
                queueNextState(2000);
            }
            break;
            //==================================================================================================
        case PATH_1_3:
            if (substate == 0 && voiceDone()){
                //say Kijk naar je spiegelbeeld
                playAudio("kijk-naar-je-spiegelbeeld_3093ce51.mp3");
                nextSubstate();
            }
            
            if (substate == 1 && voiceDone()){
                queueNextSubstate(1000);
            }
            
            else if (substate == 2 && voiceDone()){
                //say Mensen met zo'n gezicht zijn relatief vaak
                playAudio("mensen-met-zon-gezicht-zijn-relatief-vaak_88d9189f.mp3");
                nextSubstate();
            }
            
            else if (substate == 3 && voiceDone()){
                //say crimineel
                playAudio("crimineel_03311953.mp3");
                nextSubstate();
            }
            
            else if (substate == 4 && voiceDone()){
                queueNextSubstate(2000);
            }
            
            else if (substate == 5 && voiceDone()){
                //say Heb jij weleens iets illegaals gedaan?
                playAudio("heb-jij-weleens-iets-illegaals-gedaan_e0547ae4.mp3");
                nextSubstate();
            }
            
            else if (substate == 6 && voiceDone()){
                queueNextSubstate(2000);
            }
            
            
            else if (substate == 7 && voiceDone()){
                //say Denk jij dat je te vertrouwen bent?
                playAudio("denk-jij-dat-je-te-vertrouwen-bent_575a3a14.mp3");
                nextSubstate();
            }
            
            else if (substate == 8 && voiceDone()){
                queueNextSubstate(1000);
            }
            
            else if (substate == 9 && voiceDone()){
                //say En jouw vrienden?
                playAudio("en-jouw-vrienden_b3cebf6e.mp3");
                queueNextSubstate(1000);
            }
            
            else if (substate == 10 && voiceDone()){
                video.load("audio/prerecorded/counterDots.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 11 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 12 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 13 && voiceDone()){
                //say Op basis van je vriendenkring is je morele score:
                playAudio("op-basis-van-je-vriendenkring-is-je-morele-score_bc4f2cd9.mp3");
                nextSubstate();
            }
            
            else if (substate == 14 && voiceDone()){
                playNumber(r);
                r = ofRandomuf();
                nextSubstate();
            }
            
            else if (substate == 15 && voiceDone()){
                queueNextSubstate(1000);
            }
            
            else if (substate == 16 && voiceDone()){
                //say Dat verklaart je
                playAudio("dat-verklaart-je_98ccc844.mp3");
                r = ofRandomuf();
                nextSubstate();
            }
            
            else if (substate == 17 && voiceDone()){
                if(r < 33){
                    //say slechte
                    playAudio("slechte_db238127.mp3");
                }else if(r < 66){
                    //say matige
                    playAudio("matige_866113d7.mp3");
                }else {
                    //say mooie
                    playAudio("mooie_267377fa.mp3");
                }
                nextSubstate();
            }
            
            else if (substate == 18 && voiceDone()){
                //say reflectie
                playAudio("reflectie_36c528aa.mp3");
                nextSubstate();
            }
            
            else if (substate == 19 && voiceDone()){
                dataSendStart = ofGetElapsedTimef();
                queueNextState(1000);
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
                queueNextState(5000);
            }
            
            break;
            
        case PATH_1_5:
            if (substate == 0 && voiceDone()){
                takeSneakyImage();
                playAudioFile("prerecorded/1_5.mp3");
                nextSubstate();
            }
            
            else if (substate == 1 && voiceDone()){
                nextState();
            }

            break;
            
        case PATH_1_6:
            
            if (substate == 0){
                video.load("audio/prerecorded/1_6.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 1 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 2 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 3){
                state = DONE;
                substate = 0;
            }
            break;
            
            //==================================================================================================
            //==================================================================================================
        case PATH_2_1:
            if (substate == 0){
                video.load("audio/prerecorded/2_1.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 1 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 2 && video.getIsMovieDone()){
                video.close();
                smileLoops = 0; // used in next state
                nextSubstate();
            }
            
            else if (substate == 3){
                queueNextSubstate(3000);
            }
            
            else if (substate == 4){
                if (smallSmileValue.value() > 0.4 || bigSmileValue.value() > 0.4){
                    wasSmiling = true;
                }else{
                    wasSmiling = false;
                }
                nextSubstate();
            }
            
            else if (substate == 5 && voiceDone()){
                if(wasSmiling){
                    if (smileLoops == 0){
                        //say Dank je wel, dat is prachtig
                        playAudio("dank-je-wel-dat-is-prachtig_3f4a220b.mp3");
                        nextSubstate();
                    }else{
                        //say Dank je wel, dat is overtuigend
                        playAudio("dank-je-wel-dat-is-overtuigend_3a822c71.mp3");
                        nextSubstate();
                    }
                } else {
                    if(smileLoops < 2){ // loop 3 times
                        smileLoops++;
                        //say probeer het nog eens
                        playAudio("probeer-het-nog-eens_43820d34.mp3");
                        substate = 3;
                    }else{
                        smileLoops = 0;
                        //say laat maar zitten
                        playAudio("laat-maar-zitten_172e6fb4.mp3");
                        nextSubstate();
                    }
                }
            }
            
            else if (substate == 6 && voiceDone()){
                queueNextState(1000);
            }
            
            break;
            //==================================================================================================
        case PATH_2_2:
            nextState();
            break;
            //==================================================================================================
        case PATH_2_3:
            if (substate == 0 && voiceDone()){
                queueNextSubstate(3000);
            }
            
            else if (substate == 1 && voiceDone()){
                takeSneakyImage();
                playAudioFile("prerecorded/camera-shutter-click-01.mp3");
                nextSubstate();
            }
            
            else if (substate == 2 && voiceDone()){
                nextState();
            }
            break;
            //==================================================================================================
        case PATH_2_4:
            if (substate == 0 && voiceDone()){
                takeSneakyImage();
                playAudioFile("prerecorded/2_4.mp3");
                nextSubstate();
            }
            
            else if (substate == 1 && voiceDone()){
                nextSubstate();
            }

            else if (substate == 2 && voiceDone()){
                queueNextState(1000);
            }
            
            break;
            //==================================================================================================
        case PATH_2_5:
            if (substate == 0 && voiceDone()){
                // ("er-zijn-7635783240-gezichten-op-de-wereld-maar-ik-moet-naar-jou-_fbf6805c.mp3");
                //say er zijn
                playAudio("er-zijn_eac7b6e8.mp3");
                nextSubstate();
            }
            else if (substate == 1 && voiceDone()){
                //say 7635783241
                playAudio("7635783241_7b4db3c1.mp3");
                nextSubstate();
            }
            
            else if (substate == 2 && voiceDone()){
                //say gezichten op de wereld
                playAudio("gezichten-op-de-wereld_305c9202.mp3");
                nextSubstate();
            }
            
            else if (substate == 3 && voiceDone()){
                //say maar ik moet naar jou kijken
                playAudio("maar-ik-moet-naar-jou-kijken_675c33ae.mp3");
                nextSubstate();
            }
            
            else if (substate == 4 && voiceDone()){
                queueNextState(2000);
            }
            break;
            
            //==================================================================================================
        case PATH_2_6:
            if (substate == 0 && voiceDone()){
                r = ofRandomuf();
                //say Mensen vinden jouw spiegelbeeld:
                playAudio("mensen-vinden-jouw-spiegelbeeld_25003d6c.mp3");
                nextSubstate();
            }
            
            else if (substate == 1 && voiceDone()){
                if(r < 0.33){
                    //say alledaags
                    playAudio("alledaags_9a41c63c.mp3");
                }else if(r < 0.66){
                    //say Saai.
                    playAudio("saai_787aed00.mp3");
                }else{
                    //say onbeduidend
                    playAudio("onbeduidend_c1a23d81.mp3");
                }
                nextSubstate();
            }
            
            else if (substate == 2 && voiceDone()){
                queueNextState(2000);
            }
            break;
            
            
            //==================================================================================================
        case PATH_2_7:
            if (substate == 0){
                // stickers
                video.load("audio/prerecorded/2_7.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 1 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 2 && video.getIsMovieDone()){
                video.close();
                state = DONE;
                substate = 0;
            }
            break;
            
            //==================================================================================================
            
            
            //==================================================================================================
            //==================================================================================================
        case PATH_3_1:
            if (substate == 0){
                currentTemp = getCurrentTemperature();
                nextSubstate();
            }
            else if (substate == 1 && voiceDone()){
                //say Current temperature at location is
                playAudio("current-temperature-at-location-is_f044b0f4.mp3");
                nextSubstate();
            }
            else if (substate == 2 && voiceDone()){
                playNumber(int(currentTemp));
                nextSubstate();
            }
            else if (substate == 3 && voiceDone()){
                //say degrees
                playAudio("degrees_18daee01.mp3");
                nextSubstate();
            }
            
            else if (substate == 4 && voiceDone()){
                //say celcius
                playAudio("celcius_ac035f30.mp3");
                nextSubstate();
            }
            
            else if (substate == 5 && voiceDone()){
                nextState();
            }
            
            break;
            
        case PATH_3_2:
            if (substate == 0){
                currentTemp = getCurrentTemperature();
                nextSubstate();
            }
            
            else if (substate == 1){
                //say Most faces find
                playAudio("most-faces-find_dccc6db2.mp3");
                nextSubstate();
            }
            
            else if (substate == 2 && voiceDone()){
                playNumber(int(currentTemp));
                nextSubstate();
            }
            else if (substate == 3 && voiceDone()){
                //say degrees
                playAudio("degrees_18daee01.mp3");
                nextSubstate();
            }
            
            else if (substate == 4 && voiceDone()){
                //say celcius
                playAudio("celcius_ac035f30.mp3");
                nextSubstate();
            }
            
            else if (substate == 5 && voiceDone()){
                if (currentTemp < 0 ){
                    //say too cold
                    playAudio("too-cold_67b4d969.mp3");
                }
                else if (currentTemp < 15 ){
                    //say chilly
                    playAudio("chilly_ba70ae92.mp3");
                }
                else if (currentTemp < 25 ){
                    //say agreeable
                    playAudio("agreeable_cd0a6398.mp3");
                }
                else {
                    //say too hot
                    playAudio("too-hot_62486f03.mp3");
                }
                nextSubstate();
            }
            else if (substate == 6 && voiceDone()){
                queueNextState(2000);
            }
            break;
            
        case PATH_3_3:
            if (substate == 0){
                //say This experience is brought to you by Auto Flow, the online data streaming service
                playAudio("this-experience-is-brought-to-you-by-auto-flow-the-online-data-s_dbf30ad8.mp3");
                nextSubstate();
            }
            
            else if (substate == 1 && voiceDone()){
                video.load("audio/prerecorded/logos/logoAutoflow.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 2 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 3 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 4 && voiceDone()){
                queueNextState(1000);
            }
            
            break;
            
        case PATH_3_4:
            if (substate == 0){
                video.load("audio/prerecorded/3_4.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 1 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 2 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 3){
                queueNextState(1000);
            }
            break;
            
        case PATH_3_5:
            if (substate == 0){
                video.load("audio/prerecorded/3_5.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 1 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 2 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 3){
                queueNextState(1000);
            }
            break;
            
        case PATH_3_6:
            if (substate == 0){
                queueNextState(3000);
            }
            break;
            
        case PATH_3_7:
            if (substate == 0 && voiceDone()){
                
                behavior_downloads = "  "+ofToString(int(ofRandom(10)));
                behavior_social = ofToString(int(ofRandom(200)));
                if(behavior_social.size() < 10){
                    behavior_social = "  " + behavior_social;
                }
                else if(behavior_social.size() < 100){
                    behavior_social = " " + behavior_social;
                }
                behavior_privacy_act = "  "+ofToString(int(ofRandom(4)));
                
                behavior_moral = ofToString(int(ofRandom(100)));
                if(behavior_moral.size() < 10){
                    behavior_moral = "  " + behavior_moral;
                }
                
                //say Calculating recent interactions in the virtual public domain
                playAudio("calculating-recent-interactions-in-the-virtual-public-domain_bae523c5.mp3");
                nextSubstate();
            }
            
            else if (substate == 1 && voiceDone()){
                queueNextSubstate(1000);
            }
            
            else if (substate == 2 && voiceDone()){
                //say Loading: behaviour on webshops, cookies, adblockers, the public domain.
                playAudio("loading-behaviour-on-webshops-cookies-adblockers-the-public-doma_3a92bbb0.mp3");
                nextSubstate();
            }
            
            else if (substate == 3 && voiceDone()){
                queueNextSubstate(1000);
            }
            
            else if (substate == 4 && voiceDone()){
                //say Faces like yours are likely to
                playAudio("faces-like-yours-are-likely-to_3af01064.mp3");
                r = ofRandomuf();
                nextSubstate();
            }
            
            else if (substate == 5 && voiceDone()){
                if(r < 0.25){
                    //say spend more than they can afford
                    playAudio("spend-more-than-they-can-afford_08f20a17.mp3");
                }else if(r < 0.5){
                    //say be material istic
                    playAudio("be-material-istic_1a920f6f.mp3");
                }else if(r < 0.75){
                    //say commit crime
                    playAudio("commit-crime_f7b9de44.mp3");
                }else{
                    //say obey
                    playAudio("obey_b678b9b0.mp3");
                }
                nextSubstate();
            }
            
            else if (substate == 6 && voiceDone()){
                queueNextState(1000);
            }
            break;
            
        case PATH_3_8:
            nextState();
            break;
            
        case PATH_3_9:
            if (substate == 0){
                video.load("audio/prerecorded/3_9.mp4");
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 1 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 2 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 3){
                queueNextState(1000);
            }
            break;
            
        case PATH_3_10:
            if (substate == 0){
                r = ofRandomuf();
                if(r < 0.5){
                    video.load("audio/prerecorded/3_10_A.mp4");
                }else{
                    video.load("audio/prerecorded/3_10_B.mp4");
                }
                video.setLoopState(OF_LOOP_NONE);
                video.setVolume(VIDEO_VOLUME);
                video.play();
                nextSubstate();
            }
            
            else if (substate == 1 && video.isLoaded()){
                nextSubstate();
            }
            
            else if (substate == 2 && video.getIsMovieDone()){
                video.close();
                nextSubstate();
            }
            
            else if (substate == 3){
                queueNextSubstate(1000);
            }
            
            else if (substate == 4 && !voice.isPlaying()){
                state = DONE;
                substate = 0;
            }
            break;
    }
    
    voicePlaying = voice.isPlaying();
    
}
//--------------------------------------------------------------
void ofApp::draw(){
    // Draw the webcam and the face tracker
    ofSetColor(255);
    float ratio = grabber.getWidth() / 600;
    // grabber.draw(0, ofGetHeight()-int(grabber.getHeight()/ratio), 600, int(grabber.getHeight()/ratio));
    // tracker.drawDebugStylized();
    // tracker.drawDebug(ofGetWidth()/2, ofGetHeight()-int(grabber.getHeight()/ratio), 600, int(grabber.getHeight()/ratio));
    
    if(video.isPlaying()){
        ofSetColor(255);
        video.draw(0, 0);
    }
    tracker.drawDebug();
    switch (state){
        case INTRO_1:
            if (substate >= 16){
                // tracker.drawDebug();
            }
            break;
            
        case INTRO_2:
            if (substate < 3){
                // video.draw((ofGetWidth() - video.getWidth())/2.0, (ofGetHeight() - video.getHeight())/2.0);
                // tracker.drawDebug();
                drawSmileStats();
            }
            break;
        case PATH_1_1:
            if (substate >= 3){
                tracker.drawDebugStylized(0, 0);
            }
            break;

        case PATH_1_2:
            tracker.drawDebugStylized(0, 0);
            
            if (substate > 1){
                fontTechnical.drawString("Verkeersovertredingen: " + behavior_traffic, 10, 100);
//                fontTechnical.drawString("Sociaal-maatschappelijk tegoed: " + behavior_social, 10, 200);
                fontTechnical.drawString("Belastingfraude: " + behavior_tax, 10, 200);
                fontTechnical.drawString("Huidige morele score: " + behavior_moral, 10, 300);
            }
            break;
            
        case PATH_1_4:
            // ofLog() << mouseX << ", " << mouseY;
            // Draw fallen desktop
            image.draw(0, 0, 1200, 1920);
            grabber.draw(200, 1300, 640, 480);
            tracker.drawDebug(200, 1300, 640, 480);
            
            ofLog() << mouseX << ", " << mouseY;
            // for (int i = 0; i < 10; ++i){
            // fontTechnical.drawString("some technical text?", 450, 520 + 50 * i);
            // }
            fontTechnical.drawString("Beelden per seconde: " + ofToString(tracker.getThreadFps()), 450, 480);
            fontTechnical.drawString("Leeftijd: onbepaald", 450, 480 + 36 * 1);
            fontTechnical.drawString("Lichaamstemperatuur: 37.4", 450, 480 + 36 * 2);
            if(tracker.size() > 0){
                fontTechnical.drawString(
                                         "Positie: " + ofToString(tracker.getInstances()[0].getBoundingBox().getTopLeft().x) +
                                         ", " +
                                         ofToString(tracker.getInstances()[0].getBoundingBox().getTopLeft().y), 450, 480 + 36 * 3);
            }else{
                fontTechnical.drawString(
                                         "Positie: niet beschikbaar", 450, 480 + 36 * 3);
            }
            fontTechnical.drawString("Data verzonden: " + ofToString(min(100, 26 + int((ofGetElapsedTimef()-dataSendStart)))) + "%", 450, 480 + 36 * 4);
            fontTechnical.drawString("Schijfruimte vrij: 2.4Tb", 450, 480 + 36 * 5);
            fontTechnical.drawString("CPU: " + ofToString(63+int(ofRandom(1)))+"%", 450, 480 + 36 * 6);
            break;
            
        case PATH_1_5:
            sneakyImage.draw(0, (ofGetHeight()/2.0) - (sneaky_h/2.0), sneaky_w, sneaky_h);
            break;
            
        case PATH_2_1:
            if(substate == 3){
                drawSmileStats();
            }
            break;
            
        case PATH_2_3:
            tracker.drawDebugStylized();
            break;
            
        case PATH_2_13:
            tracker.drawDebugStylized();
            break;
            
        case PATH_3_7:
            
            if (substate > 1 && substate < 4){
                fontTechnical.drawString("Illegal downloads:      " + behavior_downloads, 10, 100);
                fontTechnical.drawString("Social currency:         " + behavior_social, 10, 200);
                fontTechnical.drawString("Violations of privacy act:" + behavior_privacy_act, 10, 300);
                fontTechnical.drawString("Current moral score:     " + behavior_moral, 10, 400);
            }
            break;
            
        case PATH_3_6:
            tracker.drawDebugStylized();
            // drawStocks();
            break;
            
            // case PATH_3_1:
            // case PATH_3_2:
            // case PATH_3_3:
            // case PATH_3_4:
            // case PATH_3_5:
            // case PATH_3_7:
            // case PATH_3_8:
            // case PATH_3_9:
            // case PATH_3_10:
            // case PATH_3_11:
            // case PATH_3_12:
            // case PATH_3_13:
            // case PATH_3_14:
            //     drawStocks();
            //     break;
            
        default:
            break;
    }
    
    //debug
//     ofDrawBitmapStringHighlight("State   : "+ofToString(state), 10, 20);
//     ofDrawBitmapStringHighlight("Substate: "+ofToString(substate), 10, 40);
//     ofDrawBitmapStringHighlight("voice   : "+ofToString(voice.isPlaying()), 10, 60);
//     ofDrawBitmapStringHighlight("timer   : "+ofToString(timer.isTimerRunning()), 10, 80);
//     ofDrawBitmapStringHighlight("faces   : "+ofToString(num_faces), 10, 100);
    
    
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
}

// Take a picture we can use later
void ofApp::takeSneakyImage() {
    sneakyImage.setFromPixels(grabber.getPixels());
    sneaky_ratio = sneakyImage.getWidth() / ofGetWidth();
    sneaky_w = ofGetWidth(); //full screen width
    sneaky_h = sneakyImage.getHeight() / sneaky_ratio;
}

void ofApp::drawStocks(){
    ofPushMatrix();
    ofPushStyle();
    ofSetColor(255);
    ofDrawRectangle(0, 0, ofGetWidth(), 115);
    ofTranslate(stocksX, stocksY);
    stocksX-=5;
    if(stocksX < stocks.getWidth() * -1){
        stocksX = ofGetWidth();
    }
    stocks.draw();
    ofPopStyle();
    ofPopMatrix();
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

bool ofApp::voiceDone(){
    // return (voice.isLoaded() && !voice.isPlaying());
    return (!voice.isLoaded() || (voice.isLoaded() && !voice.isPlaying()));
}

void ofApp::drawSmileStats(){
    ofPushMatrix();
    ofTranslate(0, 100);
    for (int i = 0; i < 2; i++) {
        ofSetColor(255);
        
        string str;
        float val;
        switch (i) {
            case 0:
                str = "GLIMLACH";
                val = smallSmileValue.value();
                break;
            case 1:
                str = "GROTE LACH";
                val = bigSmileValue.value();
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
            //say januari
            playAudio("januari_154d4536.mp3");
            break;
        case 2:
            //say februari
            playAudio("februari_c513efb0.mp3");
            break;
        case 3:
            //say maart
            playAudio("maart_7b7897d0.mp3");
            break;
        case 4:
            //say april
            playAudio("april_37d153a0.mp3");
            break;
        case 5:
            //say mei
            playAudio("mei_57e80e4a.mp3");
            break;
        case 6:
            //say juni
            playAudio("juni_582d74b3.mp3");
            break;
        case 7:
            //say juli
            playAudio("juli_4c37dbfa.mp3");
            break;
        case 8:
            //say augustus
            playAudio("augustus_664d4d15.mp3");
            break;
        case 9:
            //say september
            playAudio("september_110cea74.mp3");
            break;
        case 10:
            //say oktober
            playAudio("oktober_44ea3380.mp3");
            break;
        case 11:
            //say november
            playAudio("november_4d04cedd.mp3");
            break;
        case 12:
            //say december
            playAudio("december_f962bed5.mp3");
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
            //say Zondag
            playAudio("zondag_90011854.mp3");
            break;
        case 1:
            //say Maandag
            playAudio("maandag_6d080a77.mp3");
            break;
        case 2:
            //say Dinsdag
            playAudio("dinsdag_eead4419.mp3");
            break;
        case 3:
            //say Woensdag
            playAudio("woensdag_67800576.mp3");
            break;
        case 4:
            //say Donderdag
            playAudio("donderdag_8b006d98.mp3");
            break;
        case 5:
            //say Vrijdag
            playAudio("vrijdag_668efa06.mp3");
            break;
        case 6:
            //say Zaterdag
            playAudio("zaterdag_56f5d6a0.mp3");
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
            
        case 'c':
            cursor = !cursor;
            if (cursor){
                ofShowCursor();
            }else{
                ofHideCursor();
            }
            break;
            
        default:
            break;
    }
}

int ofApp::getCurrentTemperature(){
    if(!tempFromInternet){
        return currentTemp;
    }
    
    ofXml tempXml;
    int temp;
    
    // Do not reuse this API key for any other projects
    ofHttpResponse resp = ofLoadURL("https://api.openweathermap.org/data/2.5/weather?q="+city+"&mode=xml&units=metric&appid=0834fca6801ebf806a7495e1cc28536b");
    tempXml.load(resp.data.getText());
    //if we can't access the URL/data
    if(!tempXml.load(resp.data)){
        temp = int(ofRandom(10, 25));
        ofLog() << "Couldn't load file, get random temperature: " << temp;
    }else{
        string val = tempXml.getChild("current").getChild("temperature").getAttribute("value").getValue();
        // If we don't have temperature data for some reason
        if (val == "" ){
            ofLog() << "no val found in XML, get random temperature: " << temp;
            temp = int(ofRandom(10, 25));
        }else{
            temp = ofToInt(val);
            ofLog() << "live temp: " << temp;
        }
    }
    return temp;
}
// string ofApp::urlResponse(ofHttpResponse &httpResponse){
//     if(httpResponse.status==200 ){  // i.e is it ok
//         return httpResponse.data.getText();
//     }else{
//         ofLog() << "error returned: " << httpResponse.status;
//         return "";
//     }
// }
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
    if(button == 2){
        ofExit();
    }
    
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
