
void DO_KEYPAD(){

  for (int f=0;f<32;f++) {
    if (trigger_on[f]==1){
      trigger_on[f]=0;
      //Serial.println(f);
      nkey=real_key[f];
      //// pad keys select rotary when shiftR1 is pressed
      if (nkey<16 && shiftR1){
        shiftR1=false;
        drawShift();
        selected_rot=nkey;
        if (nkey==10) ztranspose=0;
        refreshTEXTZONE=true;
      //// 16 pad keys
      } else if (nkey<16){
      
        pad_touched=nkey;      
        switch (modeZ) {
          case tPiano: // 16 keys=16 notes
            synthESP32_mTrigger(selected_sound,nkey+(12*octave));
            if (recording){
              bitWrite(pattern[selected_sound],sstep,1);
              melodic[selected_sound][sstep]=nkey+(12*octave);
            }
            refreshPATTERN=true;
            break;          
          case tPad: // play pads
            if (recording){
              bitWrite(pattern[nkey],sstep,1); //!bitRead(pattern[nkey],sstep));
              melodic[selected_sound][nkey]=ROTvalue[selected_sound][3];
            } else {

                synthESP32_trigger(nkey);

              last_sound=nkey;
            }
            refreshPATTERN=true;
            break;          
          case tSel:  // Select sound
            selected_sound=nkey;
            if (selected_sound!=oldselected_sound){          
              oldselected_sound=selected_sound;
              setSound(nkey);  
            }
            last_sound=selected_sound;
            modeZ=tWrite;
            refreshPATTERN=true;
            refreshTEXTZONE=true;
            break;
          case tWrite:
            bitWrite(pattern[selected_sound],nkey,!bitRead(pattern[selected_sound],nkey));
            refreshPATTERN=true;
            melodic[selected_sound][nkey]=ROTvalue[selected_sound][3];
            break;
          case tMelod:
            bitWrite(isMelodic,nkey,!bitRead(isMelodic,nkey));
            refreshPATTERN=true;            
            synthESP32_setPitch(nkey,ROTvalue[nkey][3]);
            selected_sound=nkey;
            break;
          case tMute:
            bitWrite(mutes,nkey,!bitRead(mutes,nkey));
            refreshPATTERN=true;            
            selected_sound=nkey;
            if (selected_sound!=oldselected_sound){
              oldselected_sound=selected_sound;
            } 
            break;
          case tSolo:
            bitWrite(solos,nkey,!bitRead(solos,nkey));
            refreshPATTERN=true;            
            selected_sound=nkey;
            if (selected_sound!=oldselected_sound){
              oldselected_sound=selected_sound;
            } 
            break;
          case tClear: // clear
            pattern[nkey]=0;
            selected_sound=nkey;
            if (selected_sound!=oldselected_sound){
              oldselected_sound=selected_sound;
            } 
            break;
          case tLoadP:
            load_pattern(nkey);
            selected_pattern=nkey;
            break;
          case tSaveP:
            save_pattern(nkey);
            selected_pattern=nkey;
            modeZ=tPad; // pongo el modeZ en 0 para que no se quede en grabar
            break;
          case tLoadS:
            load_sound(nkey);
            selected_sndSet=nkey;
            break;
          case tSaveS:
            save_sound(nkey);
            selected_sndSet=nkey;
            modeZ=tPad; // pongo el modeZ en 0 para que no se quede en grabar
            break;
          case tLoaPS:
            load_pattern(nkey);
            selected_pattern=nkey;
            load_sound(nkey);
            selected_sndSet=nkey;
            break;
          case tSavPS:
            save_pattern(nkey);
            selected_pattern=nkey;
            save_sound(nkey);
            selected_sndSet=nkey;                        
            modeZ=tPad; // pongo el modeZ en 0 para que no se quede en grabar
            break;
          case tRndS:
            setRandomVoice(nkey);
            setSound(nkey);
            selected_sound=nkey;
            if (selected_sound!=oldselected_sound){
              oldselected_sound=selected_sound;
            }          
            break;
          case tRndP:
            setRandomPattern(nkey);
            setSound(nkey);
            selected_sound=nkey;
            if (selected_sound!=oldselected_sound){
              oldselected_sound=selected_sound;
            }          
            break;
          case tRndNo:
            setRandomNotes(nkey);
            setSound(nkey);
            selected_sound=nkey;
            if (selected_sound!=oldselected_sound){
              oldselected_sound=selected_sound;
            }          
            break;
          case tFirst:
            refreshPATTERN=true;
            firstStep=nkey;
            break;          
          case tLast:
            refreshPATTERN=true;
            newLastStep=nkey;
            break;  
          case tSong:
            song[pattern_song_counter]=nkey;
            break;       
        }
        refreshPADTOUCHED=true;
        refreshMODES=true;
      //// 8 funtion keys
      } else if (nkey<24){
        refreshPATTERN=true;
        clearPATTERN=true;
        refreshMODES=true;
        if (!shiftR1){
          switch (nkey) {

            // playpad/select/write
            case 16:
              // 0 - play pad sound
              // 1 - select sound
              // 2 - edit track, write
              if (modeZ>tWrite){
                modeZ=tPad;
              } else {
                modeZ++;
              }
              if (modeZ>tWrite) modeZ=tPad;
              //refreshMODES=true;  
              break;
            // random sound/pattern/Notes      
            case 17:
              if (modeZ==tRndP){
                modeZ=tRndS;
              } else if (modeZ==tRndS){
                modeZ=tRndNo;
              } else {
                modeZ=tRndP;
              }
              //refreshMODES=true;
              break;
            // load       
            case 18:
              if (modeZ>=tLoadP && modeZ<tLoaPS ) {
                modeZ=modeZ+1;
              } else {
                modeZ=tLoadP;
              }       
              break;              
            // save      
            case 19:
              if (modeZ>=tSaveP && modeZ<tSavPS ) {
                modeZ=modeZ+1;
              } else {
                modeZ=tSaveP;
              }       
              break; 
            // solos/mutes/melodic
            case 20:
              if (modeZ==tMute) {
                modeZ=tSolo;
              } else if (modeZ==tSolo){
                modeZ=tMelod;
              } else if (modeZ==tMelod){
                modeZ=tClear;
              } else if (modeZ==tClear){
                modeZ=tFirst;               
              } else if (modeZ==tFirst){
                modeZ=tLast; 
              } else {
                modeZ=tMute;
              }    
              break; 
            // piano
            case 21:
              modeZ=tPiano;
              break;
            // Play
            case 22:
              if (playing){
                uClock.stop();
                sstep=firstStep;
                recording=false;
                refreshSTEP=true;  
              } else {
                if (sync_state==2){ // if this machine is slave dont start playing now
                  pre_playing=true;
                } else {
                  pattern_song_counter=-1;
                  if (songing) load_flag=true;
                  uClock.start();
                  sstep=firstStep;
                  refreshSTEP=true;  
                }
              }
              playing=!playing; 
              break;
            // Song
            case 23:
              if (modeZ==tSong) {
                last_pattern_song=pattern_song_counter;
                pattern_song_counter=-1;
                modeZ=0;
              } else {
                modeZ=tSong;
                selected_rot=13;
                refreshTEXTZONE=true;
              }
              break;                      
          }    
        // shift 
                          
        } else {
          shiftR1=false;
          drawShift();
          switch (nkey){
            // reset mutes & solos and firstStep=0 and lastStep=15       
            case 20:
              mutes=0;
              solos=0; 
              firstStep=0;
              lastStep=15;
              break;
            // Play
            case 22:
              if (playing){
                recording=!recording;
              } else {
                uClock.start();
                recording=true;
                playing=true;
                sstep=firstStep;
                refreshSTEP=true;  
              }
              //refreshMODES=true;  
              break;
            case 23:
              songing=!songing;
              break;                    
          }         
        }
      } else {
        refreshTEXTZONE=true;
        switch (nkey) {

            // playpad/select/write
            case 24:  
              shiftR1=!shiftR1;
              refreshTEXTZONE=true;          
              break;          
            case 25:
              updateVirtualRotary(-1);
              break;
            case 26:
              updateVirtualRotary(-5);
              break;
            case 27:
              updateVirtualRotary(-10);
              break;
            case 28:
              updateVirtualRotary(+10);
              break;
            case 29:
              updateVirtualRotary(+5);
              break;
            case 30:
              updateVirtualRotary(+1);
              break;
            case 31:
              shiftR1=!shiftR1;
              refreshTEXTZONE=true;
              break; 
        }
      }
    }
  } 
}

void updateVirtualRotary(int change){

  refreshTEXTZONE=true;
  
  if (selected_rot==11){
    counter1=master_filter; 
  } else if (selected_rot==12){
    counter1=octave;      
  } else if (selected_rot==8){
    counter1=bpm;
  } else if (selected_rot==9){
    counter1=master_vol;
  } else if (selected_rot==10){
    ztranspose=0;
    counter1=ztranspose;
   } else if (selected_rot==13){
    counter1=pattern_song_counter;               
   } else if (selected_rot==14){
    counter1=sync_state; 
  } else if (selected_rot==15){
    counter1=i2s_out_device;   
  } else {
    counter1=ROTvalue[selected_sound][selected_rot];
  }
  old_counter1=counter1;

  counter1=counter1+change;
  
  if (counter1>max_values[selected_rot]) counter1=max_values[selected_rot];
  if (counter1<min_values[selected_rot]) counter1=min_values[selected_rot];
  if (counter1!=old_counter1){
    
    if (selected_rot==11){
      master_filter=counter1;
      synthESP32_setMFilter(master_filter); //  filter is 0-255 and i got 0-127. inside function i 'll multiply by 2
    } else if (selected_rot==12){
      octave=counter1;
    } else if (selected_rot==8){
      bpm=counter1;
      uClock.setTempo(bpm);
    } else if (selected_rot==9){
      master_vol=counter1;
      synthESP32_setMVol(master_vol);
    } else if (selected_rot==10){
      ztranspose=counter1;
      // cambiar pitch en melodic
      for (byte f=0; f<16;f++){
        melodic[selected_sound][f]=melodic[selected_sound][f]+ztranspose;
      }
    } else if (selected_rot==13){
      pattern_song_counter=counter1;                 
    } else if (selected_rot==14){
      sync_state=counter1; 
    } else if (selected_rot==15){
      i2s_out_device=counter1;       
    } else {
      ROTvalue[selected_sound][selected_rot]=counter1;
      setSound(selected_sound);
      int nvalue=ROTvalue[selected_sound][selected_rot];
      // arreglar pan
      if (selected_rot==6) {
        nvalue=map(nvalue,-99,99,0,127);
      } 
      // play sound if
      if (selected_rot<5){
        synthESP32_trigger(selected_sound);
      }
    }
    
  }
}
