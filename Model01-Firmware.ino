// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "tjk 20171028"
#endif


// The Kaleidoscope core
#include "Kaleidoscope.h"

// Allows modifiers to serve a different purpose when tapped
#include <Kaleidoscope-SpaceCadet.h>

// Support for macros
#include "Kaleidoscope-Macros.h"

// Support for controlling the keyboard's LEDs
#include "Kaleidoscope-LEDControl.h"

// Support for an "LED off mode"
#include "LED-Off.h"

// Support for the "Boot greeting" effect, which pulses the 'LED' button for 10s
// when the keyboard is connected to a computer (or that computer is powered on)
#include "Kaleidoscope-LEDEffect-BootGreeting.h"

// Support for LED modes that set all LEDs to a single color
#include "Kaleidoscope-LEDEffect-SolidColor.h"

// Support for an LED mode that makes all the LEDs 'breathe'
#include "Kaleidoscope-LEDEffect-Breathe.h"

// Support for LED modes that pulse the keyboard's LED in a rainbow pattern
#include "Kaleidoscope-LEDEffect-Rainbow.h"


enum { MACRO_VERSION_INFO }; // macros

enum { QWERTY, FUNCTION }; // layers


// *INDENT-OFF*
const Key keymaps[][ROWS][COLS] PROGMEM = {

  [QWERTY] = KEYMAP_STACKED
  (Key_Backtick, Key_1, Key_2, Key_3, Key_4, Key_5, Key_Home,
   Key_Tab,      Key_Q, Key_W, Key_E, Key_R, Key_T, Key_Enter,
   Key_PageUp,   Key_A, Key_S, Key_D, Key_F, Key_G,
   Key_PageDown, Key_Z, Key_X, Key_C, Key_V, Key_B, Key_Delete,
   Key_LeftControl, Key_LeftGui, Key_LeftShift, Key_LeftAlt,
   ShiftToLayer(FUNCTION),

   Key_End,        Key_6, Key_7, Key_8,     Key_9,      Key_0,         Key_Minus,
   Key_Enter,      Key_Y, Key_U, Key_I,     Key_O,      Key_P,         Key_Equals,
                   Key_H, Key_J, Key_K,     Key_L,      Key_Semicolon, Key_Quote,
   Key_Backspace,  Key_N, Key_M, Key_Comma, Key_Period, Key_Slash,     Key_Backslash,
   Key_RightAlt, Key_RightShift, Key_Spacebar, Key_RightControl,
   ShiftToLayer(FUNCTION)),

  [FUNCTION] =  KEYMAP_STACKED
  (Key_Sysreq, Key_F1,            Key_F2, Key_F3, Key_F4,                Key_F5, Key_LEDEffectNext,
   ___,        ___,               ___,    ___,    ___,                   ___,    ___,
   ___,        ___,               ___,    ___,    ___,                   ___,
   ___,        ___,               ___,    ___,    M(MACRO_VERSION_INFO), ___,    Key_Backspace,
   ___,        Key_PcApplication, ___,    ___,
   ___,

   Consumer_ScanPreviousTrack, Key_F6,                 Key_F7,        Key_F8,                   Key_LeftBracket,          Key_RightBracket, Key_F11,
   Consumer_PlaySlashPause,    Consumer_ScanNextTrack, ___,           Key_Insert,               ___,                      Key_PrintScreen,  Key_F12,
                               Key_LeftArrow,          Key_DownArrow, Key_UpArrow,              Key_RightArrow,           ___,              ___,
   Key_Delete,                 ___,                    Key_Mute,      Consumer_VolumeDecrement, Consumer_VolumeIncrement, ___,              ___,
   ___, ___, ___, ___,
   ___)
};
// *INDENT-ON*


static void versionInfoMacro(uint8_t keyState) {
  if (keyToggledOn(keyState)) {
    Macros.type(PSTR("Keyboardio Model 01 - Kaleidoscope "));
    Macros.type(PSTR(BUILD_INFORMATION));
  }
}

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  switch (macroIndex) {

  case MACRO_VERSION_INFO:
    versionInfoMacro(keyState);
    break;

  }
  return MACRO_NONE;
}


// These 'solid' color effect definitions define a rainbow of
// LED color modes calibrated to draw 500mA or less on the
// Keyboardio Model 01.
static kaleidoscope::LEDSolidColor solidRed(160, 0, 0);
static kaleidoscope::LEDSolidColor solidOrange(140, 70, 0);
static kaleidoscope::LEDSolidColor solidYellow(130, 100, 0);
static kaleidoscope::LEDSolidColor solidGreen(0, 160, 0);
static kaleidoscope::LEDSolidColor solidBlue(0, 70, 130);
static kaleidoscope::LEDSolidColor solidIndigo(0, 0, 170);
static kaleidoscope::LEDSolidColor solidViolet(130, 0, 120);


void setup() {
  Kaleidoscope.setup();

  Kaleidoscope.use(
    // The boot greeting effect pulses the LED button for 10 seconds after the keyboard is first connected
    &BootGreetingEffect,

    // LEDControl provides support for other LED modes
    &LEDControl,

    // We start with the LED effect that turns off all the LEDs.
    &LEDOff,

    // The rainbow wave effect lights up your keyboard with all the colors of a rainbow
    // and slowly moves the rainbow across your keyboard
    &LEDRainbowWaveEffect,

    // The breathe effect slowly pulses all of the LEDs on your keyboard
    &LEDBreatheEffect,

    // These static effects turn your keyboard's LEDs a variety of colors
    &solidRed, &solidOrange, &solidYellow, &solidGreen, &solidBlue, &solidIndigo, &solidViolet,

    // The macros plugin adds support for macros
    &Macros,

    // SpaceCadet allows modifiers to be tapped to serve as another key
    &SpaceCadet
  );

  // Set the keymap with a timeout per-key
  // Setting is {KeyThatWasPressed, AlternativeKeyToSend, TimeoutInMS}
  // Note: must end with the SPACECADET_MAP_END delimiter
  static kaleidoscope::SpaceCadet::KeyBinding spacecadetmap[] = {
      {Key_LeftGui, Key_Escape, 500}
    , {Key_LeftAlt, Key_LeftCurlyBracket, 500}
    , {Key_RightAlt, Key_RightCurlyBracket, 500}
    , {Key_LeftControl, Key_LeftBracket, 500}
    , {Key_RightControl, Key_RightBracket, 500}
    , SPACECADET_MAP_END
  };
  // Set the map.
  SpaceCadet.map = spacecadetmap;

  // We set the brightness of the rainbow effects to 150 (on a scale of 0-255)
  // This draws more than 500mA, but looks much nicer than a dimmer effect
  LEDRainbowEffect.brightness(120);
  LEDRainbowWaveEffect.brightness(120);

  // We want to make sure that the firmware starts with LED effects off
  // This avoids over-taxing devices that don't have a lot of power to share
  // with USB devices
  LEDOff.activate();
}

void loop() {
  Kaleidoscope.loop();
}
