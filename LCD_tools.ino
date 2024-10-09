// #define BLACK RGB565_BLACK
// #define NAVY RGB565_NAVY
// #define DARKGREEN RGB565_DARKGREEN
// #define DARKCYAN RGB565_DARKCYAN
// #define MAROON RGB565_MAROON
// #define PURPLE RGB565_PURPLE
// #define OLIVE RGB565_OLIVE
// #define LIGHTGREY RGB565_LIGHTGREY
// #define DARKGREY RGB565_DARKGREY
// #define BLUE RGB565_BLUE
// #define GREEN RGB565_GREEN
// #define CYAN RGB565_CYAN
// #define RED RGB565_RED
// #define MAGENTA RGB565_MAGENTA
// #define YELLOW RGB565_YELLOW
// #define WHITE RGB565_WHITE
// #define ORANGE RGB565_ORANGE
// #define GREENYELLOW RGB565_GREENYELLOW
// #define PALERED RGB565_PALERED
#define MICOLOR RGB565(255, 247, 0)
#define MICOLOR2 RGB565(10, 46, 11)
#define MICOLOR3 RGB565(60, 60, 60)

void showLastTouched() {
  if (!show_last_touched) {
    return;
  }
  gfx->drawRoundRect(BPOS[real_led[last_touched]][0] + 2, BPOS[real_led[last_touched]][1] + 2, Bwidth - 4, Bheight - 4, 4, BCOLOR[last_touched]);
  gfx->drawRoundRect(BPOS[real_led[last_touched]][0] + 3, BPOS[real_led[last_touched]][1] + 3, Bwidth - 6, Bheight - 6, 4, BCOLOR[last_touched]);
  //gfx->drawRoundRect(BPOS[real_led[last_touched]][0]+4, BPOS[real_led[last_touched]][1]+4, Bwidth-8, Bheight-8,4, BCOLOR[last_touched]);
  //gfx->drawRoundRect(BPOS[real_led[last_touched]][0]+5, BPOS[real_led[last_touched]][1]+5, Bwidth-10, Bheight-10,4, BCOLOR[last_touched]);
}
void clearLastTouched() {
  if (!show_last_touched) {
    return;
  }
  if (start_showlt + (debounce_time * 2) > millis()) {
    return;
  }
  for (byte f = 0; f < BTOTAL; f++) {
    gfx->drawRoundRect(BPOS[real_led[f]][0] + 2, BPOS[real_led[f]][1] + 2, Bwidth - 4, Bheight - 4, 4, BLACK);
    gfx->drawRoundRect(BPOS[real_led[f]][0] + 3, BPOS[real_led[f]][1] + 3, Bwidth - 6, Bheight - 6, 4, BLACK);
    //gfx->drawRoundRect(BPOS[real_led[f]][0]+4, BPOS[real_led[f]][1]+4, Bwidth-8, Bheight-8,4, BLACK);
    //gfx->drawRoundRect(BPOS[real_led[f]][0]+5, BPOS[real_led[f]][1]+5, Bwidth-10, Bheight-10,4, BLACK);
  }
  show_last_touched = false;
}

void drawBT(byte bt, int color, String texto = "") {
  //gfx->fillRect(BPOS[real_led[bt]][0]+2, BPOS[real_led[bt]][1]+2, Bwidth-4, Bheight-4, color);
  BCOLOR[bt] = color;
  gfx->fillRoundRect(BPOS[real_led[bt]][0] + 6, BPOS[real_led[bt]][1] + 6, Bwidth - 12, Bheight - 12, 4, color);
  // textos
  gfx->setTextSize(1);
  gfx->setTextColor(BLACK, color);


  // Button texts

  if (bt < 16) {
    gfx->setCursor(BPOS[real_led[bt]][0] + 8, BPOS[real_led[bt]][1] + 12);
    if (shiftR1) {
      if (texto == "") {
        gfx->println(F(tbuttons_shift[bt]));
      } else {
        gfx->println(F(texto));
      }
    } else {
      gfx->println(F(bt));
    }
    // if write mode show note number
    if (modeZ == tWrite) {
      gfx->setTextColor(WHITE, color);
      gfx->setCursor(BPOS[real_led[bt]][0] + 8, BPOS[real_led[bt]][1] + 36);
      if (bitRead(pattern[selected_sound], bt)) {  // note on
        gfx->println(F(melodic[selected_sound][bt]));
      }
      gfx->setTextColor(BLACK, color);
    }

  } else if (bt < 24) {
    gfx->setCursor(BPOS[real_led[bt]][0] + 8, BPOS[real_led[bt]][1] + 12);
    if (texto == "") {
      gfx->println(F(tbuttons1[bt - 16]));
    } else {
      gfx->println(F(texto));
    }
  } else {  //  +, - shift
    gfx->setCursor(BPOS[real_led[bt]][0] + 8, BPOS[real_led[bt]][1] + 12);
    if (texto == "") {
      gfx->println(F(tbuttons2[bt - 24]));
    } else {
      gfx->println(F(texto));
    }
  }
}

void REFRESH_KEYS() {

  boolean refresh = false;

  if (refreshMODES) {
    refreshMODES = false;

    // Borrar modos
    for (byte f = 16; f < 24; f++) {
      drawBT(f, DARKGREY);
    }
    switch (modeZ) {
      case tPad:
        drawBT(16, GREEN);
        break;
      case tSel:
        drawBT(16, BLUE, "SEL");
        break;
      case tWrite:
        drawBT(16, RED, "WRITE");
        break;
      case tMute:
        drawBT(20, BLUE);
        break;
      case tSolo:
        drawBT(20, RED, "SOLO");
        break;
      case tMelod:
        drawBT(20, YELLOW, "MELODIC");
        break;
      case tClear:
        drawBT(20, ORANGE, "CLEAR");
        break;
      case tFirst:
        drawBT(20, CYAN, "FIRST");
        break;
      case tLast:
        drawBT(20, GREENYELLOW, "LAST");
        break;
      case tLoadP:
        drawBT(18, BLUE);
        break;
      case tLoadS:
        drawBT(18, CYAN, "LOAD S");
        break;
      case tLoaPS:
        drawBT(18, WHITE, "LOAD PS");
        break;
      case tSaveP:
        drawBT(19, RED);
        break;
      case tSaveS:
        drawBT(19, YELLOW, "SAVE S");
        break;
      case tSavPS:
        drawBT(19, WHITE, "SAVE PS");
        break;

      case tRndS:
        drawBT(17, MAGENTA, "RND S");
        break;
      case tRndP:
        drawBT(17, YELLOW, "RND P");
        break;
      case tRndNo:
        drawBT(17, CYAN, "RND N");
        break;
      case tPiano:
        drawBT(21, MAGENTA);
        break;
      case tSong:
        drawBT(23, WHITE);
        break;
    }
    // play/stop
    if (playing) {
      drawBT(22, GREEN);
    }
    if (recording) {
      drawBT(22, RED, "REC");
    }
    // song
    if (songing) {
      drawBT(23, CYAN);
    }
  }

  if (refreshPATTERN) {
    refreshPATTERN = false;

    switch (modeZ) {
      case tWrite:

        for (byte f = 0; f < 16; f++) {
          if bitRead (pattern[selected_sound], f) {
            drawBT(f, RED);
          } else {
            drawBT(f, LIGHTGREY);
          }
        }
        break;
      case tMute:

        for (byte f = 0; f < 16; f++) {
          if bitRead (mutes, f) {
            drawBT(f, ORANGE);
          } else {
            drawBT(f, LIGHTGREY);
          }
        }
        break;
      case tSolo:

        for (byte f = 0; f < 16; f++) {
          if bitRead (solos, f) {
            drawBT(f, GREEN);
          } else {
            drawBT(f, LIGHTGREY);
          }
        }
        break;
      case tMelod:

        for (byte f = 0; f < 16; f++) {
          if bitRead (isMelodic, f) {
            drawBT(f, PALERED);
          } else {
            drawBT(f, LIGHTGREY);
          }
        }
        break;
      case tFirst:
        for (byte f = 0; f < 16; f++) {
          if (firstStep == f) {
            drawBT(f, GREEN);
          } else {
            drawBT(f, LIGHTGREY);
          }
        }
        break;
      case tLast:
        for (byte f = 0; f < 16; f++) {
          if (newLastStep == f) {
            drawBT(f, RED);
          } else {
            drawBT(f, LIGHTGREY);
          }
        }
        break;
    }
  }

  if (refreshSTEP) {
    refreshSTEP = false;
    // Si SEQ activo muestro los pads que suenan
    if (playing) {
      for (byte f = 0; f < 16; f++) {
        if bitRead (pattern[f], sstep) {
          drawBT(f, BLUE);
        }
      }
    }
    // Mostrar step actual
    drawBT(sstep, YELLOW);
  }


  if (clearPATTERN) {
    clearPATTERN = false;
    if (modeZ != 2) {
      for (byte f = 0; f < 16; f++) {
        drawBT(f, LIGHTGREY);
      }
    }

    // refresco lo que hay en el step en el siguiente bucle
    refreshPATTERN = true;
  }
}


void fillBPOS() {
  byte counter = 0;
  int y_accumulated = Btop;
  for (byte r = 0; r < Brows; r++) {
    int x_accumulated = Bleft;
    for (byte c = 0; c < Bcolumns; c++) {
      // x - left
      BPOS[counter][0] = x_accumulated + Bspace;
      x_accumulated = x_accumulated + Bspace + Bwidth;
      BPOS[counter][1] = y_accumulated + Bspace;
      counter++;
    }
    x_accumulated = 0;
    y_accumulated = y_accumulated + Bspace + Bheight;
    if (counter == 24) y_accumulated = y_accumulated + 30;
  }
}

void drawShift() {
  int color = DARKGREY;
  if (shiftR1) {
    color = ORANGE;
  }
  drawBT(24, color);
  drawBT(31, color);
  // Actualizar texto botones 0-15
  for (byte f = 0; f < 16; f++) {
    drawBT(f, LIGHTGREY);
  }
  refreshPATTERN=true;
}
void drawScreen1() {
  for (byte f = 0; f < BTOTAL; f++) {
    gfx->fillRect(BPOS[f][0], BPOS[f][1], Bwidth, Bheight, BLACK);
    //if (f<16)  gfx->drawRoundRect(BPOS[f][0]+1, BPOS[f][1]+1, Bwidth-2, Bheight-2,6, DARKGREY);
    if (f < 16) {
      drawBT(f, LIGHTGREY);
    } else {
      drawBT(f, DARKGREY);
    }
  }
  // botones +,-,shift
  for (byte f = 24; f < 32; f++) {
    drawBT(f, MICOLOR3);
  }
}



void REFRESH_TEXT() {
  if (!refreshTEXTZONE) return;

  refreshTEXTZONE = false;

  int pos_y = 189;
  int offset = 20;
  int offset2 = 8;

  // Clear zone
  gfx->fillRect(BPOS[0][0] + 6, pos_y - 6, (Bwidth * 2) - 10, 27, MICOLOR2);
  gfx->fillRect(BPOS[2][0] + 6, pos_y - 6, (Bwidth * 2) - 10, 27, MICOLOR2);
  gfx->fillRect(BPOS[4][0] + 6, pos_y - 6, (Bwidth * 2) - 10, 27, MICOLOR2);
  gfx->fillRect(BPOS[6][0] + 6, pos_y - 6, (Bwidth * 2) - 10, 27, MICOLOR2);



  gfx->setTextColor(WHITE, MICOLOR2);

  gfx->setTextSize(2);

  gfx->setCursor(BPOS[0][0] + (offset * 2), pos_y);
  gfx->print(F("S "));
  gfx->setCursor(BPOS[1][0] + offset2, pos_y);
  gfx->print(F(selected_sound));
  gfx->setCursor(BPOS[2][0] + offset, pos_y);
  gfx->print(F(trot[selected_rot]));
  gfx->setCursor(BPOS[3][0] + offset2, pos_y);
  if (selected_rot == 11) {
    gfx->print(F(master_filter));
  } else if (selected_rot == 12) {
    gfx->print(F(octave));
  } else if (selected_rot == 8) {
    gfx->print(F(bpm));
  } else if (selected_rot == 9) {
    gfx->print(F(master_vol));
  } else if (selected_rot == 10) {
    gfx->print(F(ztranspose));
  } else if (selected_rot == 13) {
    gfx->print(F(pattern_song_counter));
  } else if (selected_rot == 14) {
    gfx->print(F(sync_state));
  } else if (selected_rot == 15) {
    gfx->print(F(i2s_out_device));
  } else {
    gfx->print(F(ROTvalue[selected_sound][selected_rot]));
  }
  gfx->setCursor(BPOS[4][0] + (offset * 2), pos_y);
  gfx->print(F("P "));
  gfx->setCursor(BPOS[5][0] + offset2, pos_y);
  gfx->print(F(selected_pattern));
  gfx->setCursor(BPOS[6][0] + (offset * 2), pos_y);
  gfx->print(F("S "));
  gfx->setCursor(BPOS[7][0] + offset2, pos_y);
  gfx->print(F(selected_sndSet));

  drawShift();
}
