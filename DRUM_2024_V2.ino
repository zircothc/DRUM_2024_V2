////////////////////////////
// DRUM 2024 V2           //
////////////////////////////
// ZCARLOS 2024
// Based on DRUM 2024 V1
// Synth engine Based on DZL's library "the_synth"
// This version is for GUITION JC4827W543 (C)apacitive board - ESP32 S3


//////////////////////////// MIDI USB
// #include <Adafruit_TinyUSB.h>
// #include <MIDI.h>
// Adafruit_USBD_MIDI usb_midi;
// MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// includes
#include <Arduino.h>
////////////////////////////// SPIFFS
#include <FS.h>
#include <SPIFFS.h>
//..........................................................................
// ////////////////////////////// LCD and TOUCH

#include <Arduino_GFX_Library.h>
#define GFX_DEV_DEVICE ESP32_4827A043_QSPI
#define GFX_BL 1
Arduino_DataBus *bus = new Arduino_ESP32QSPI(45 /* cs */, 47 /* sck */, 21 /* d0 */, 48 /* d1 */, 40 /* d2 */, 39 /* d3 */);
//Arduino_DataBus *bus = new Arduino_ESP32SPI(18/* TFT_DC */, 35/* TFT_CS */, 12 /* SCK */, 16 /* MOSI */, GFX_NOT_DEFINED /* MISO */, HSPI /* spi_num */);

Arduino_GFX *gfx = new Arduino_NV3041A(bus, GFX_NOT_DEFINED /* RST */, 0 /* rotation */, true /* IPS */);
#ifdef ESP32
#undef F
#define F(s) (s)
#endif

#include <bb_captouch.h>
//#include <Wire.h>
//#define LCD DISPLAY_CYD_543
#define TOUCH_SDA 8
#define TOUCH_SCL 4
#define TOUCH_INT 3
#define TOUCH_RST 38
BBCapTouch bbct;

//............................................................................
// Button properties
#define Bwidth 59
#define Bheight 59
#define Bleft 1
#define Btop 1
#define Bspace 1
#define Bcolumns 8
#define Brows 4
#define BTOTAL Bcolumns*Brows

int BPOS[32][2]; // filled in setup with button [x] and [y] calculated coordinates
int BCOLOR[32]; // to store current button color
const int debounce_time=100; // time to wait, miliseconds
long start_debounce;
int last_touched=-1;
bool show_last_touched=false;
long start_showlt;
// Touchscreen coordinates: (x, y) and pressure (z)
int cox, coy, coz;

////////////////////////////// SYNTH
#define SAMPLE_RATE 22050
#include "driver/i2s.h"
#include "synthESP32LowPassFilter_E.h" // filter
#include "tablesESP32_E.h"

byte i2s_out_device=0; // 0=external, 1=internal, 2=both

////////////////////////////// TIMER SEQ 
#include <uClock.h>


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////// SCREEN

const String trot[18] = { "WAV", "ENV", "LEN", "PIT", "MOD", "VOL", "PAN", "FIL", "BPM","MVO","TRP","MFI","OCT","PSN","SYN","OUT","GP2","GD2"};
const String tmodeZ[20] = { "Pad", "Sel", "Write", "Mute", "Solo", "Clear","LoadP","LoadS","LoaPS","SaveP","SaveS","SavPS","RndS","RndP","First","Last","Melod","RndNo","Piano","Song"};

const String tbuttons_shift[16] = {"WAVE", "ENVELOP", "LENGTH", "PITCH", "MODULAT", "VOLUME", "PAN", "FILTER", "BPM","MVOLUME","TRANSPO","MFILTER","OCTAVE","PATNUMB","SYNC","OUT"};
const String tbuttons1[8]       = {"PAD", "RND P", "LOAD P", "SAVE P", "MUTE", "PIANO", "PLAY", "SONG"};
const String tbuttons2[8]       = {"SHIFT","-1", "-5", "-10", "+10", "+5", "+1",  "SHIFT"};


// modeZ types

//  0 - play pad sound
//  1 - select sound
//  2 - edit track, write
//  3 - mute
//  4 - solo
//  5 - clear

//  6 - load pattern
//  7 - load pattern
//  8 - load pattern and sound

//  9 - save pattern
// 10 - save sound
// 11 - save sound and pattern
 
// 12 - random sound
// 13 - random pattern
// 14 - first step
// 15 - last step
// 16 - set melodic (read pitch from pattern melodic instead of rotary value)
// 17 - random note
// 18 - piano
// 10 - song (edit song)

#define tPad    0
#define tSel    1
#define tWrite  2
#define tMute   3
#define tSolo   4
#define tClear  5
#define tLoadP  6
#define tLoadS  7
#define tLoaPS  8
#define tSaveP  9
#define tSaveS 10
#define tSavPS 11
#define tRndS  12
#define tRndP  13
#define tFirst 14
#define tLast  15
#define tMelod 16
#define tRndNo 17
#define tPiano 18
#define tSong  19

////////////////////////////// SYNTH

// SINE     0
// TRIANGLE 1
// SQUARE   2
// SAW      3
// RAMP     4
// NOISE    5
// PHASOR   6
// WN1      7
// WN2      8
// WN3      9
// WN4      10
// WN5      11
// WN6      12
// WN7      13
//
// ENVELOPE0 0
// ENVELOPE1 1
// ENVELOPE2 2
// ENVELOPE3 3


#define SAMPLE_RATE 22050

// i2s
// External I2S DAC
#define I2S_BCK_PIN 7
#define I2S_WS_PIN 6 
#define I2S_DATA_OUT_PIN 15
// Internal I2S DAC mono  
#define I2S_BCK_PIN2 42
#define I2S_WS_PIN2 2
#define I2S_DATA_OUT_PIN2 41

#define DMA_BUF_LEN     32          
#define DMA_NUM_BUF     2

static uint16_t out_buf[DMA_BUF_LEN * 2];

// Filters master (L&R) and 16 sounds
LowPassFilter lpfR;
LowPassFilter lpfL;

LowPassFilter lpf0;
LowPassFilter lpf1;
LowPassFilter lpf2;
LowPassFilter lpf3;
LowPassFilter lpf4;
LowPassFilter lpf5;
LowPassFilter lpf6;
LowPassFilter lpf7;
LowPassFilter lpf8;
LowPassFilter lpf9;
LowPassFilter lpf10;
LowPassFilter lpf11;
LowPassFilter lpf12;
LowPassFilter lpf13;
LowPassFilter lpf14;
LowPassFilter lpf15;

const int cutoff=255;
const int reso=511;

int vmeter=0;

static unsigned int PCW[16] = {0, 0, 0, 0,0 ,0 ,0 ,0,0, 0, 0, 0,0 ,0 ,0 ,0};      //-Wave phase accumolators
static unsigned int FTW[16] = {1000, 200, 300, 400, 1000, 200, 300, 400, 1000, 200, 300, 400, 1000, 200, 300, 400};           //-Wave frequency tuning words
static unsigned char AMP[16] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};           //-Wave amplitudes [0-255]
static unsigned int PITCH[16] = {500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500};          //-Voice pitch
static int MOD[16] = {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};                      //-Voice envelope modulation [0-1023 512=no mod. <64 pitch down >64 pitch up]

static int VOL_R[16] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}; 
static int VOL_L[16] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};   
static int PAN[16] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10}; 
static int FILTER[16]= {127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127};                  //-mozzi FILTERS  0-255

static unsigned int wavs[16];                                  //-Wave table selector [address of wave in memory]
static unsigned int envs[16];                                  //-Envelope selector [address of envelope in memory]
static unsigned int EPCW[16] = {0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000}; //-Envelope phase accumulator
static unsigned int EFTW[16] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};               //-Envelope speed tuning word
static unsigned int divider = 16;                             //-Sample rate decimator for envelope

static int mvol=10;
int master_vol=17;
int master_filter=0;
int octave=5;

////////////////////////////// LEDS

// Set more friendly led numbers
// 0 to 15 as 16 step, 16 sounds,... and 16 to 23 as selected modes
// 24 to 31 -,--,--- ,shift,shift, +++,++,+
const byte real_led[32]={ 
0,1,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,
24,25,26,27,28,29,30,31};

int counter_refresh=0;
const byte limit_counter_refresh=7;
boolean start_counter_refresh=false;



////////////////////////////// TIMER SEQ 
int bpm=120;
int stepInterval;
int microStep=0;
byte selected_pattern=0;
byte sstep=0;
byte selected_len=16;
boolean updateBPM=false;
uint16_t mutes=0;
uint16_t solos=0;
uint16_t pattern[16];
byte song[255];
int pattern_song_counter=-1;
byte last_pattern_song=255;
uint8_t  melodic[16][16];
//uint16_t isMelodic=B11111111<<8 | B00000000; // last 8 sounds are set to melodic
uint16_t isMelodic=B00000000<<8 | B00000000; // none set to melodic
byte firstStep=0;
byte lastStep=15;
byte newLastStep=15; 
byte selected_sound=0;
byte oldselected_sound=0;
byte selected_sndSet=0;
int ztranspose=0;
boolean load_flag=false;

byte sync_state=0; // 0 no sync, 1, master, 2 slave
boolean sync_flag=false;

////////////////////////////// sound parameters and global parameters

byte ENC_PORT1=0;
int counter1=0;
int old_counter1=0;
boolean readencoders=false;
byte shiftR1=false;
byte RV;

// 8 sound parameters + bpm + master vol + transpose + master filter + octave + Pattern song selector + Sync mode + OUT device
const int max_values[18]={13,3,127,127,127,31, 99,127,400,31, 1,127,10,255,2,2,511,255}; 
const int min_values[18]={ 0,0,  0,  0,  0, 0,-99,  0,  0, 0,-1,  0, 0,  0,0,0,  0,0};

int ROTvalue[16][8]={ // init sound values
  { 0,2, 32, 35,29,7,-31,0}, // THIS IS VOICE #0
  { 5,1, 50,127,31,7,-31,0}, // THIS IS VOICE #1
  { 9,2, 25, 41,95,7,  0,0}, // ...
  { 0,0, 60, 64,30,7,  0,0},
  {13,2, 47, 44,82,7,  0,0},
  { 8,2, 42, 29,31,7,  0,0},
  { 0,0, 30, 54,64,7,  0,0},
  { 3,0, 25, 44,64,7,  0,0},
  
  { 0,2, 32, 25,29,7, 66,0},
  { 5,2, 50,107,31,7, 66,0},
  { 9,2, 25, 31,95,7,  0,0},
  { 0,0, 60, 54,30,7,  0,0},
  {13,2, 47, 54,82,7,  0,0},
  { 8,2, 42, 19,31,7,  0,0},
  { 0,0, 40, 44,64,7, 30,0},
  { 3,0, 80, 34,64,7,-30,0}
};

byte selected_rot=0;

////////////////////////////// KEYS


byte trigger_on[32];
byte nkey;

// Set more friendly key numbers
// 0 to 15 as 16 step, 16 sounds,... and 16 to 23 as selected modes
const byte real_key[32]={ 
0,1,2,3,4,5,6,7,
8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,
24,25,26,27,28,29,30,31};


byte pad_touched=0;

////////////////////////////// MISC
byte modeZ=0;
byte last_modeZ=0;
byte last_sound=0;

boolean playing   = false;
boolean pre_playing=false;
boolean songing   = false; // switch to make load auto patterns++
boolean recording = false;
//boolean shifting  = false;

boolean clearPATTERN=false;
boolean clearSTEP=false;
boolean refreshPATTERN=true;
boolean refreshSTEP=false;
boolean refreshSTATUS=false;
boolean refreshMODES=true;
boolean refreshTEXTZONE=true;
boolean refreshPADTOUCHED=false;
boolean stepzero = false; // tool to reset the first step and last step the seq searchs for start/end point of the loop
#define MIDI_CLOCK 0xF8
#define MIDI_START 0xFA
#define MIDI_STOP  0xFC

//////////////////////////////////////////////



//..............................................................................................................................
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_touch(){

  TOUCHINFO ti;
  if (bbct.getSamples(&ti)) {
    cox=ti.x[0];
    coy=ti.y[0];
    for (byte f=0;f<BTOTAL;f++){
      if ( (cox > BPOS[f][0]) && (cox < (BPOS[f][0]+Bwidth)) && (coy > BPOS[f][1]) && (coy < (BPOS[f][1]+Bheight)) ) {
          if (f==last_touched){
            if (start_debounce+debounce_time>millis() ){
              return;
            } 
          } else {
            start_debounce=0;
          }
          //gfx->fillRect(BPOS[f][0], BPOS[f][1], Bwidth, Bheight, GREEN);

          last_touched=f;
          show_last_touched=true;
          trigger_on[f]=1;
          start_debounce=millis();
          start_showlt=start_debounce;
          // add double time to debounce if key is between 16 and 23
          if (f>15 && f<24) start_debounce=start_debounce+debounce_time;
      }
    }

  } else { // ya no se esta tocando
    //start_debounce=0;
  }

}
//..............................................................................................................................
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void setSound(byte f){
  synthESP32_setWave(f,ROTvalue[f][0]);  
  synthESP32_setEnvelope(f,ROTvalue[f][1]);
  synthESP32_setLength(f,ROTvalue[f][2]);
  synthESP32_setPitch(f,ROTvalue[f][3]);
  synthESP32_setMod(f,ROTvalue[f][4]); 
  synthESP32_setVol(f,ROTvalue[f][5]);  
  synthESP32_setPan(f,ROTvalue[f][6]);  
  synthESP32_setFilter(f,ROTvalue[f][7]);  
}
void setRandomVoice(byte f){
  ROTvalue[f][0]=random(0, 14);
  ROTvalue[f][1]=random(0, 4);
  ROTvalue[f][2]=random(1, 60);
  ROTvalue[f][3]=random(0, 127);
  ROTvalue[f][4]=random(10,100);
  ROTvalue[f][5]=random(10,15);
  ROTvalue[f][6]=random(-99,99);
  ROTvalue[f][7]=random(0,60);
}
void setRandomPattern(byte f){
  byte veces=random(0,2);
  for (byte b = 0; b < 16; b++) {
    byte mybit=random(0,2);
    if (veces) {
      if (mybit) mybit=random(0,2); // Si es 1 hago otro random para que haya menos unos
    }
    bitWrite(pattern[f],b,mybit);
  } 
  setRandomPitch(f);
}

void setRandomPitch(byte f){
  // Tomo como referencia para el rango el valor del pot pitch actual
  uint8_t actual=ROTvalue[selected_sound][3];
  uint8_t limite;
  uint8_t prelimite=24;
  for (byte b = 0; b < 16; b++) {
     limite=actual+random(0,prelimite);
     melodic[f][b]=random(actual-(prelimite>>1),limite);
  }  
}

void setRandomNotes(byte f){
  setRandomPitch(f);
}
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void onSync24Callback(uint32_t tick){

  
  // Lanzar cambio de step
  if (!(tick % (6))) tic(tick);
  // Limpiar patrón
  if ((tick % (6))==4) clearPATTERN=true;

  
}

void tic(uint32_t tick){
  
//  if (playing){
    //if (microStep==0){

  if (sstep==firstStep){
    sync_flag=true;
  }

    
      clearSTEP=true;
      for (int f = 0; f < 16; f++) { 
        if (!bitRead(mutes, f)) {
          if (solos == 0 || (solos > 0 && bitRead(solos, f))) {
            if (bitRead(pattern[f], sstep)) { // note on
              if (bitRead(isMelodic,f)){
                //synthESP32_setPitch(f,melodic[f][sstep]);

                  synthESP32_mTrigger(f,melodic[f][sstep]);                  

              } else {
                // reestablecer el pitch
                synthESP32_setPitch(f,ROTvalue[f][3]);                
                synthESP32_trigger(f);
              }
            } 
          }
        }
      }
  
      sstep++;
      // Comprobar step final
      if (sstep==(lastStep+1) || sstep==(newLastStep+1) || (stepzero) || sstep==16) {
        lastStep=newLastStep;
        stepzero=false;
        sstep=firstStep;
        //Serial.println("Toc");
        if (songing){
          load_flag=true; // inside loop I will load next pattern
        }
      }
      refreshSTEP=true;
      refreshPATTERN=true; 
//    }


    

}
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void load_pattern(byte pat){
  
  // Leer pattren
  String nombreArchivoP = "/PAT" + String(pat);
  File archivoP = SPIFFS.open(nombreArchivoP, FILE_READ);   
  if (!archivoP) {
    Serial.println("Error al abrir el archivo para leer");
    return;
  }
  int fila = 0;
  while (archivoP.available()) {
    pattern[fila] = (archivoP.readStringUntil('\n')).toInt();
    //Serial.println(pattern[fila]);
    fila++;
  }
  archivoP.close();
  //Serial.println("-");


  // Leer melodic
  String nombreArchivoM = "/MED" + String(pat);
  File archivoM = SPIFFS.open(nombreArchivoM, FILE_READ);
  if (!archivoM) {
    Serial.println("Error al abrir el archivo para leer");
    return;
  }
  fila = 0;
  int columna = 0;  
  while (archivoM.available()) {
    melodic[fila][columna] = (archivoM.readStringUntil('\n')).toInt();
    columna++;
    if (columna == 16) {
      fila++;
      columna = 0;
    }
  }
  archivoM.close(); 
  refreshTEXTZONE=true;
  
}

void load_sound(byte pat){
  
  // Leer sound
  String nombreArchivoS = "/SND" + String(pat);
  File archivoS = SPIFFS.open(nombreArchivoS, FILE_READ);
  if (!archivoS) {
    Serial.println("Error al abrir el archivo para leer");
    return;
  }
  int fila = 0;
  int columna = 0;  
  while (archivoS.available()) {
    ROTvalue[fila][columna] = (archivoS.readStringUntil('\n')).toInt();
    columna++;
    if (columna == 8) {
      // Set voices
      setSound(fila);
      fila++;
      columna = 0;
    }
  }
  archivoS.close(); 
  refreshTEXTZONE=true;
}

void save_pattern(byte pat){
  
  // Guardar pattern
  String nombreArchivoP = "/PAT" + String(pat);
  File archivoP = SPIFFS.open(nombreArchivoP, FILE_WRITE);   
  if (!archivoP) {
    Serial.println("Error al abrir el archivo para escribir");
    return;
  }
  for (int i = 0; i < 16; i++) {
    archivoP.println(pattern[i]);
    //Serial.println(pattern[i]);
  }
  archivoP.close();
  //Serial.println("-");  

  // Guardar melodic
  String nombreArchivoM = "/MED" + String(pat);
  File archivoM = SPIFFS.open(nombreArchivoM, FILE_WRITE); 
  if (!archivoM) {
    Serial.println("Error al abrir el archivo para escribir");
    return;
  }
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      archivoM.println(melodic[i][j]);
    }
  }
  archivoM.close();
  refreshTEXTZONE=true;
  
}
void save_sound(byte pat){
  
  // Guardar sound
  String nombreArchivoS = "/SND" + String(pat);
  File archivoS = SPIFFS.open(nombreArchivoS, FILE_WRITE); 
  if (!archivoS) {
    Serial.println("Error al abrir el archivo para escribir");
    return;
  }
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 8; j++) {
      archivoS.println(ROTvalue[i][j]);
    }
  }
  archivoS.close();
  refreshTEXTZONE=true;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////  S E T U P  //////////////////////////////
void setup() {
  //btStop();
  // Serial
  Serial.begin(115200);


//........................................................................................................................
// LCD and TOUCH
  // Init Display
  if (!gfx->begin())
  // if (!gfx->begin(80000000)) /* specify data bus speed */
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(BLACK);

  #ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
  #endif


  fillBPOS();
  drawScreen1();
  bbct.init(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_INT);
  int iType = bbct.sensorType();

//........................................................................................................................

  //delay(2000);


  // Serial 2
  //Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2);    //Hardware Serial of ESP32
  
  // MISI USB
  // MIDI.begin(MIDI_CHANNEL_OMNI);
  // MIDI.setHandleNoteOn(handleNoteOn);  
  // MIDI.setHandleControlChange(handleCC);
  // // check device mounted
  // if ( !TinyUSBDevice.mounted() ){
  //   Serial.println("Error USB");
  // }
  
  // Synth
  synthESP32_begin();
  // Set 16 voices
  for (byte f=0;f<16;f++){
    setSound(f);
  }
  // Set master vol
  synthESP32_setMVol(master_vol);
  // Set master filter
  synthESP32_setMFilter(master_filter);  



  
  // SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Error al montar el sistema de archivos SPIFFS");
    return;
  } 
  
  // Seq


   // fill melodic with 60
  for  (byte a=0;a<16;a++){
    for  (byte b=0;b<16;b++){
     melodic[a][b]=60;
    }    
  }
  // set 8-15 as melodic;
  isMelodic=B11111111<<8 | B00000000;
  // Setup our clock system
  uClock.init();
  //uClock.setOnStep(onStepCallback);
  uClock.setOnSync24(onSync24Callback);
  uClock.setTempo(bpm);

   // demo pattern
  //pattern[0]=B00010001<<8 | B10010001;
  //pattern[1]=B11000101<<8 | B10101011;
  //  pattern[6]=B10110101<<8 | B10001111;
  //  pattern[3]=B00000110<<8 | B00000000;
  //  pattern[7]=B00000000<<8 | B01001000;
   // start playing demo pattern
   //uClock.start();


   sstep=firstStep;
   refreshSTEP=true; 
   //playing=true;

}

//////////////////////////////  L O O P  //////////////////////////////

void loop() {

  // flag to do things outside sequencer timer isr
  if (load_flag){
    load_flag=false;

    pattern_song_counter++;
    if (pattern_song_counter==255) pattern_song_counter=0;
    if (pattern_song_counter>last_pattern_song) pattern_song_counter=0; 
        
    load_pattern(song[pattern_song_counter]);
    selected_pattern=song[pattern_song_counter];
    load_sound(song[pattern_song_counter]);
    selected_sndSet=song[pattern_song_counter];
           
    refreshTEXTZONE=true;
  }

  // if (sync_flag){ 
  //     sync_flag=false;
  //     if (sync_state==1){   // if this machine is master
  //       Serial1.write(11);
  //     }
  // }
  // if (sync_state==2){ // if this machine is slave
  //   if (Serial1.available() > 0){
  //     byte var=Serial1.read();
  //     if (var==11){
  //       if (pre_playing){
  //         sstep=firstStep;
  //         pre_playing=false;
  //         uClock.start();
  //         refreshSTEP=true;          
  //       }
  //     }
  //   }
  // }

  // Read MIDI ports
  //MIDI.read();

  read_touch();
  DO_KEYPAD();
  REFRESH_KEYS();
  REFRESH_TEXT();
  showLastTouched();
  clearLastTouched();
  //REFRESH_VUMETER();


}


