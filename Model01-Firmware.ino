// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "tjk 2020-03-05"
#endif


// The Kaleidoscope core
#include "Kaleidoscope.h"

// Support for storing the keymap in EEPROM
#include "Kaleidoscope-EEPROM-Settings.h"
#include "Kaleidoscope-EEPROM-Keymap.h"

// Support for communicating with the host via a simple Serial protocol
#include "Kaleidoscope-FocusSerial.h"

// Support for macros
#include "Kaleidoscope-Macros.h"

// Support for controlling the keyboard's LEDs
#include "Kaleidoscope-LEDControl.h"

// Support for the "Boot greeting" effect, which pulses the 'LED' button for 10s
// when the keyboard is connected to a computer (or that computer is powered on)
#include "Kaleidoscope-LEDEffect-BootGreeting.h"

// Support for LED modes that pulse the keyboard's LED in a rainbow pattern
#include "Kaleidoscope-LEDEffect-Rainbow.h"

// Support for Keyboardio's internal keyboard testing mode
#include "Kaleidoscope-HardwareTestMode.h"

// Support for host power management (suspend & wakeup)
#include "Kaleidoscope-HostPowerManagement.h"

// Support for magic combos (key chords that trigger an action)
#include "Kaleidoscope-MagicCombo.h"

// Support for USB quirks, like changing the key state report protocol
#include "Kaleidoscope-USB-Quirks.h"

// tjk additions
#include <Kaleidoscope-IdleLEDs.h>


enum { MACRO_VERSION_INFO }; // macros
enum { PRIMARY, FUNCTION }; // layers
#define PRIMARY_KEYMAP_QWERTY


// *INDENT-OFF*
KEYMAPS(

  [PRIMARY] = KEYMAP_STACKED
  (Key_Escape,      Key_1, Key_2, Key_3, Key_4, Key_5, Key_Equals,
   Key_Tab,         Key_Q, Key_W, Key_E, Key_R, Key_T, Key_LeftBracket,
   Key_LeftShift,   Key_A, Key_S, Key_D, Key_F, Key_G,
   Key_Backtick,    Key_Z, Key_X, Key_C, Key_V, Key_B, Key_Spacebar,
   Key_LeftControl, Key_LeftGui, Key_LeftAlt, Key_Enter,
   ShiftToLayer(FUNCTION),

   Key_LEDEffectNext, Key_6, Key_7, Key_8,     Key_9,      Key_0,         Key_Minus,
   Key_RightBracket,  Key_Y, Key_U, Key_I,     Key_O,      Key_P,         Key_Quote,
                      Key_H, Key_J, Key_K,     Key_L,      Key_Semicolon, Key_RightShift,
   Key_Backspace,     Key_N, Key_M, Key_Comma, Key_Period, Key_Slash,     Key_Backslash,
   Key_Delete, Key_Enter, Key_Spacebar, Key_RightControl,
   ShiftToLayer(FUNCTION)),

  [FUNCTION] =  KEYMAP_STACKED
  (Key_Sysreq,    Key_F1,     Key_F2,   Key_F3,        Key_F4,                Key_F5, ___,
   ___,           ___,        ___,      ___,           ___,                   ___,    Key_PageUp,
   ___,           ___,        ___,      ___,           ___,                   ___,
   Consumer_Menu, ___,        Key_Home, Key_End,       M(MACRO_VERSION_INFO), ___,    Key_PageDown,
   ___,           ___,        ___,      Key_Backspace,
   ___,

   Consumer_ScanPreviousTrack, Key_F6,                 Key_F7,        Key_F8,      Key_F9,         Key_F10,          Key_F11,
   Consumer_PlaySlashPause,    Consumer_ScanNextTrack, ___,           Key_Insert,  ___,            Key_PrintScreen,  Key_F12,
                               Key_LeftArrow,          Key_DownArrow, Key_UpArrow, Key_RightArrow, ___,              ___,
   ___,                        ___,                    Consumer_Mute, Key_Home,    Key_End,        ___,              ___,
   ___, ___, ___, ___,
   ___)
) // KEYMAPS(
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


/** toggleLedsOnSuspendResume toggles the LEDs off when the host goes to sleep,
 * and turns them back on when it wakes up.
 */
void toggleLedsOnSuspendResume(kaleidoscope::plugin::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::plugin::HostPowerManagement::Suspend:
    LEDControl.disable();
    break;
  case kaleidoscope::plugin::HostPowerManagement::Resume:
    LEDControl.enable();
    break;
  case kaleidoscope::plugin::HostPowerManagement::Sleep:
    break;
  }
}

/** hostPowerManagementEventHandler dispatches power management events (suspend,
 * resume, and sleep) to other functions that perform action based on these
 * events.
 */
void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

/** This 'enum' is a list of all the magic combos used by the Model 01's
 * firmware The names aren't particularly important. What is important is that
 * each is unique.
 *
 * These are the names of your magic combos. They will be used by the
 * `USE_MAGIC_COMBOS` call below.
 */
enum {
  // Toggle between Boot (6-key rollover; for BIOSes and early boot) and NKRO
  // mode.
  COMBO_TOGGLE_NKRO_MODE,
  // Enter test mode
  COMBO_ENTER_TEST_MODE
};

/** Wrappers, to be used by MagicCombo. **/

/**
 * This simply toggles the keyboard protocol via USBQuirks, and wraps it within
 * a function with an unused argument, to match what MagicCombo expects.
 */
static void toggleKeyboardProtocol(uint8_t combo_index) {
  USBQuirks.toggleKeyboardProtocol();
}

/**
 *  This enters the hardware test mode
 */
static void enterHardwareTestMode(uint8_t combo_index) {
  HardwareTestMode.runTests();
}


/** Magic combo list, a list of key combo and action pairs the firmware should
 * recognise.
 */
USE_MAGIC_COMBOS({.action = toggleKeyboardProtocol,
                  // Left Fn + Esc + Shift
                  .keys = { R3C6, R2C6, R3C7 }
}, {
  .action = enterHardwareTestMode,
  // Left Fn + Prog + LED
  .keys = { R3C6, R0C0, R0C6 }
});

// First, tell Kaleidoscope which plugins you want to use.
// The order can be important. For example, LED effects are
// added in the order they're listed here.
KALEIDOSCOPE_INIT_PLUGINS(
  // The EEPROMSettings & EEPROMKeymap plugins make it possible to have an
  // editable keymap in EEPROM.
  EEPROMSettings,
  EEPROMKeymap,

  // Focus allows bi-directional communication with the host, and is the
  // interface through which the keymap in EEPROM can be edited.
  Focus,

  // FocusSettingsCommand adds a few Focus commands, intended to aid in
  // changing some settings of the keyboard, such as the default layer (via the
  // `settings.defaultLayer` command)
  FocusSettingsCommand,

  // FocusEEPROMCommand adds a set of Focus commands, which are very helpful in
  // both debugging, and in backing up one's EEPROM contents.
  FocusEEPROMCommand,

  // The boot greeting effect pulses the LED button for 10 seconds after the
  // keyboard is first connected
  BootGreetingEffect,

  // The hardware test mode, which can be invoked by tapping Prog, LED and the
  // left Fn button at the same time.
  HardwareTestMode,

  // LEDControl provides support for other LED modes
  LEDControl,

  IdleLEDs,

  // We start with the LED effect that turns off all the LEDs.
  LEDOff,

  // The rainbow wave effect lights up your keyboard with all the colors of a rainbow
  // and slowly moves the rainbow across your keyboard
  LEDRainbowWaveEffect,

  // The macros plugin adds support for macros
  Macros,

  // The HostPowerManagement plugin allows us to turn LEDs off when then host
  // goes to sleep, and resume them when it wakes up.
  HostPowerManagement,

  // The MagicCombo plugin lets you use key combinations to trigger custom
  // actions - a bit like Macros, but triggered by pressing multiple keys at the
  // same time.
  MagicCombo,

  // The USBQuirks plugin lets you do some things with USB that we aren't
  // comfortable - or able - to do automatically, but can be useful
  // nevertheless. Such as toggling the key report protocol between Boot (used
  // by BIOSes) and Report (NKRO).
  USBQuirks
);

/** The 'setup' function is one of the two standard Arduino sketch functions.
 * It's called when your keyboard first powers up. This is where you set up
 * Kaleidoscope and any plugins.
 */
void setup() {
  // First, call Kaleidoscope's internal setup function
  Kaleidoscope.setup();

  // We set the brightness of the rainbow effects to 150 (on a scale of 0-255)
  // This draws more than 500mA, but looks much nicer than a dimmer effect
  LEDRainbowEffect.brightness(120);
  LEDRainbowWaveEffect.brightness(120);

  // Set the action key the test mode should listen for to Left Fn
  HardwareTestMode.setActionKey(R3C6);

  // We want to make sure that the firmware starts with LED effects off
  // This avoids over-taxing devices that don't have a lot of power to share
  // with USB devices
  LEDOff.activate();

  // To make the keymap editable without flashing new firmware, we store
  // additional layers in EEPROM. For now, we reserve space for five layers. If
  // one wants to use these layers, just set the default layer to one in EEPROM,
  // by using the `settings.defaultLayer` Focus command, or by using the
  // `keymap.onlyCustom` command to use EEPROM layers only.
  EEPROMKeymap.setup(5);

  IdleLEDs.setIdleTimeoutSeconds(900);
}

void loop() {
  Kaleidoscope.loop();
}
