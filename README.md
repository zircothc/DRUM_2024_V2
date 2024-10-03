# DRUM_2024_V2

ESP32 DRUM SYNTH MACHINE (JC4827W543 C BOARD)

This is my DRUM SYNTH LOFI MACHINE for Guition ESP32 JC4827W543 (Capacitive touch model)

All features from DRUM_2024_V1 are ported to this system.

https://github.com/zircothc/DRUM_2004_V1

I have changed buttons, LEDs, rotary encoder and OLED screen for a TFT screen with touch panel
 

About the board:
It has a I2S DAC with amp ready to attach a speaker.
In the GUI you can select:

0, external I2S DAC (default)
1, internal i2S DAC
2, Both

# Hardware:

- Guition JC4827W543 C (ESP32 S3, LCD ILI9488, touch panel GT911)
- PCM5102A I2s DAC (for better sound output) or a speaker (with that tiny conector)

# Software:

IDE:
Arduino `2.3.2`

Boards:
Expressif Systems `2.0.17`

Board: ESP32S3 Dev module

Necessary Libraries:

- Sequencer Timer - [uClock](https://github.com/midilab/uClock)
- TFT - [Arduino_GFX_Library](https://github.com/moononournation/Arduino_GFX)
- Touch panel - [bb_captouch](https://github.com/bitbank2/bb_captouch)

# Notes:

Can't find schematics of the board but info about pinout is avaliable throught other projects.
External PINs used to wired PCM5102 module:
IO6 - I2S_WS_PIN - LCK
IO7 - I2S_BCK_PIN - BCK
IO15 - I2S_DATA_OUT_PIN - DIN

Cheat sheet style PDF V1 version is fully compatible. Added function to select audio out.

I2S PCM5102A module:
- Join solder pads near SCK pin on PCM5102A module.
- Solder pads on the back of PCM5102A module ([more info](https://github.com/pschatzmann/ESP32-A2DP/wiki/External-DAC#pcm5102-dac))
  
  - H1L: FLT - Low
  
  - H2L: DEMP - Low
  
  - H3L: XSMT - High
  
  - H4L: FMT - Low
 
    
