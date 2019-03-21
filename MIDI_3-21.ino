

/* MIDI ChordStomper - Nate Nasteff 2019

  This code is tested on a Teensy 3.2.
  https://www.pjrc.com/store/teensy32.html

  The idea behind this project is to make a MIDI controller that has
  four footpedals which can be assigned to chords (or individual notes), which
  can then be sent through the device to a synthesizer or other MIDI device. I'm
  doing this so I can play my keyboard(s) hands-free in my band (I play bass).

  The MIDI I/O is controlled by a simple MIDI I/O circuit and utilizes the
  Arduino MIDI Library. Seding chords through MIDI out is done by referencing
  chords built in a SynthProgram object, or on-the-fly through the implemented
  assignLatch() menu. The assignLatch() function parses MIDI IN data to determine
  which keys to latch to which buttons.

  The current build uses a 16x2 LCD with the LiquicCrystal lib to display the 
  program bank (ie, 01: Hammond B3 Organ, 02: Karma Police) as well as a menu 
  for the chord latch feature. In the future this display might be upgraded.

  When it's complete, this device will hopefully utilize four footpedals, as well
  as three buttons (or footswitches), two for up/down menu navigation, and one for
  accessing the 'latch' feature.

  Further down the road I would like to have more control over MIDI CC and
  parameter assignments, utilizing an array of potentiometers to control them
  accordingly. I would also like to add a Micro-SD card slot to be able to
  load and parse .csv files containing variables for SynthProgram structs, as
  well as saving newly made presets.

*/

#include <Bounce2.h>
#include <MIDI.h>
#include <LiquidCrystal.h>
#include <LinkedList.h>

LiquidCrystal myLcd(12, 11, 10, 9, 8, 7);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

// MIDI Params

String progName = "";
const int CC_TEMPO = 12;
int VAL_TEMPO;
const int CC_ALL_NOTE_OFF = 123;
const int VAL_ALL_NOTE_OFF = 00;

// Button Pins

const int BP_1 = 13;
const int BP_2 = 14;
const int BP_3 = 15;
const int BP_4 = 16;
const int BP_5 = 17;
const int BP_6 = 18;
const int BP_7 = 19;

// Various

int progNum;
int latchVel;
int latchProg;
int channel = 1;
int menuIndex = 0;
int latchIndex = 0;
bool assign = false;
bool globLatched = false;
bool isLatched1 = false;
bool isLatched2 = false;
bool isLatched3 = false;
bool isLatched4 = false;

// Test chords

int mychord1[4] = {60, 64, 67, 70};
int mychord2[5] = {65, 69, 72, 75, 0};
int mychord3[10] = {85, 86, 87, 88, 89, 90, 91, 92, 93, 94};
int mychord4[5] = {102, 0, 0, 0, 0};

// Chords for latch function

int latchChord1[10];
int latchChord2[10];
int latchChord3[10];
int latchChord4[10];

// Synth Program Structure - used with LinkedList ADT

typedef struct {
  int * chord1;
  int * chord2;
  int * chord3;
  int * chord4;
  int prog;
  String name;
} SynthProgram;

// Current / Global Setting Program - will be updated by the menu/latch

SynthProgram globalProg;

// Testing structures via LCD print

String structTest(SynthProgram p) {
//  String s;
//  s += p.chord1 + " " + p.name + " " + p.prog;
//  return s;
}

// ADT Implementation

LinkedList<SynthProgram> programBank;
SynthProgram prog1 = {mychord1, mychord2, mychord1, mychord2, 22, "Test1"};
SynthProgram prog2 = {mychord1, mychord2, mychord1, mychord2, 23, "Test2"};
SynthProgram prog3 = {mychord1, mychord2, mychord1, mychord2, 26, "Test3"};
SynthProgram prog4 = {mychord1, mychord2, mychord1, mychord2, 32, "Test4"};
SynthProgram prog5 = {mychord1, mychord2, mychord1, mychord2, 44, "Test5"};
SynthProgram prog6 = {mychord1, mychord2, mychord1, mychord2, 55, "Test6"};
SynthProgram prog7 = {mychord1, mychord2, mychord1, mychord2, 29, "Test7"};
SynthProgram prog8 = {mychord1, mychord2, mychord1, mychord2, 77, "Test8"};
SynthProgram prog9 = {mychord1, mychord2, mychord1, mychord2, 31, "Test9"};
SynthProgram prog10 = {mychord1, mychord2, mychord1, mychord2, 12, "Test10"};
SynthProgram prog11 = {mychord1, mychord2, mychord1, mychord2, 66, "Test11"};
SynthProgram prog12 = {mychord1, mychord2, mychord1, mychord2, 76, "Test12"};
SynthProgram prog13 = {mychord1, mychord2, mychord1, mychord2, 25, "M1 Organ"};
SynthProgram prog14 = {mychord1, mychord2, mychord1, mychord2, 48, "Test14"};
SynthProgram prog15 = {mychord1, mychord2, mychord1, mychord2, 62, "Test15"};
SynthProgram prog16 = {mychord1, mychord2, mychord1, mychord2, 77, "Test16"};
SynthProgram prog17 = {mychord1, mychord2, mychord1, mychord2, 28, "Test17"};
SynthProgram prog18 = {mychord1, mychord2, mychord1, mychord2, 26, "Test18"};
SynthProgram prog19 = {mychord1, mychord2, mychord1, mychord2, 80, "Test19"};
SynthProgram prog20 = {mychord1, mychord2, mychord1, mychord2, 66, "Test20"};


// Custom Chars for LCD

byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000,
};
byte latched[8] = {
  B00000,
  B00100,
  B00100,
  B00100,
  B00111,
  B00000,
  B00000,
};
byte slash[8] = {
  B00000,
  B10000,
  B01000,
  B00100,
  B00010,
  B00001,
  B00000,
};
byte jjDown[8] = {
  B00000,
  B00000,
  B00100,
  B01110,
  B10101,
  B00100,
  B01010,
  B01010
};
byte jjUp[8] = {
  B00000,
  B00000,
  B10101,
  B01110,
  B00100,
  B00100,
  B01010,
  B10001
};
byte trademark[8] = {
  B01110,
  B00100,
  B00100,
  B00000,
  B01010,
  B10101,
  B10101,
  B00000,
};


// Instances of Bounce() for buttons

Bounce BUTTON_1 = Bounce();
Bounce BUTTON_2 = Bounce();
Bounce BUTTON_3 = Bounce();
Bounce BUTTON_4 = Bounce();
Bounce BUTTON_5 = Bounce();
Bounce BUTTON_6 = Bounce();
Bounce BUTTON_7 = Bounce();

// Send a chord

void sendChord(int *notes, int vel) {
  if (sizeof(notes) == 0) {
    MIDI.sendNoteOn(notes[0], vel, channel);
  }
  else {
    for (int i = 0; i <= sizeof(notes); i++) {
      if (notes[i] != 0) {
        MIDI.sendNoteOn(notes[i], vel, channel);
      }
    }
  }
}

// Kills previously sent chord

void killChord(int *notes, int vel) {
  for (int i = 0; i <= sizeof(notes); i++) {
    MIDI.sendNoteOff(notes[i], vel, channel);
  }
}

// Latch assign function

void assignLatch() {
  int note, velocity;
  int buttonselect = 0;
  latchIndex = 0;

  while (assign) {

    BUTTON_1.update();
    BUTTON_2.update();
    BUTTON_3.update();
    BUTTON_4.update();
    BUTTON_5.update();
    BUTTON_6.update();
    BUTTON_7.update();

    // If MIDI-IN is detected...

    if (MIDI.read()) {
      byte type = MIDI.getType();
      switch (type) {

        // When NoteOn is received...

        case midi::NoteOn:
          note = MIDI.getData1();
          velocity = MIDI.getData2();
          channel = MIDI.getChannel();

          // Button 1
          if (velocity > 0 && note < 127 && note > 0 && buttonselect == 1) {
            latchChord1[latchIndex] = note;
            latchIndex++;
            myLcd.setCursor(0, 1);
            myLcd.print("    Latched!    ");
          }

          // Button 2
          if (velocity > 0 && note < 127 && note > 0 && buttonselect == 2) {
            latchChord2[latchIndex] = note;
            latchIndex++;
            myLcd.setCursor(0, 1);
            myLcd.print("    Latched!    ");
          }

          // Button 3
          if (velocity > 0 && note < 127 && note > 0 && buttonselect == 3) {
            latchChord3[latchIndex] = note;
            latchIndex++;
            myLcd.setCursor(0, 1);
            myLcd.print("    Latched!    ");
          }

          // Button 4
          if (velocity > 0 && note < 127 && note > 0 && buttonselect == 4) {
            latchChord4[latchIndex] = note;
            latchIndex++;
            myLcd.setCursor(0, 1);
            myLcd.print("    Latched!   ");
          }
          break;

        // When NoteOff is received...
        // Reset latch / button vars
        case midi::NoteOff:
          note = MIDI.getData1();
          velocity = MIDI.getData2();
          channel = MIDI.getChannel();
          latchIndex = 0;
          buttonselect = 0;
          break;
      }
    }
    if (BUTTON_1.fell() == true) {
      if (buttonselect != 0) {
        setScreen(0, 1, "Play something!!", false);
      }

      else {
        myLcd.setCursor(0, 1);
        myLcd.print(" Play a chord..");
        buttonselect = 1;
        isLatched1 = true;
        globLatched = true;
        for (int i = 0; i <= 9; i++) {
          latchChord1[i] = 0;
        }
      }
    }
    else if (BUTTON_2.fell() == true) {
      if (buttonselect != 0) {
        setScreen(0, 1, "Play something!!", false);
      }

      else {
        myLcd.setCursor(0, 1);
        myLcd.print(" Play a chord..");
        buttonselect = 2;
        isLatched2 = true;
        globLatched = true;
        for (int i = 0; i <= 9; i++) {
          latchChord2[i] = 0;
        }
      }
    }
    else if (BUTTON_3.rose() == true) {
      if (buttonselect != 0) {
        setScreen(0, 1, "Play something!!", false);
      }

      else {
        myLcd.setCursor(0, 1);
        myLcd.print(" Play a chord..");
        buttonselect = 3;
        globLatched = true;
        isLatched3 = true;
        for (int i = 0; i <= 9; i++) {
          latchChord3[i] = 0;
        }
      }
    }
    else if (BUTTON_4.fell() == true) {
      if (buttonselect != 0) {
        setScreen(0, 1, "Play something!!", false);
      }

      else {
        myLcd.setCursor(0, 1);
        myLcd.print(" Play a chord..");
        buttonselect = 4;
        globLatched = true;
        isLatched4 = true;
        for (int i = 0; i <= 9; i++) {
          latchChord4[i] = 0;
        }
      }
    }

    if (BUTTON_6.fell() == true) {
      if (buttonselect != 0) {
        setScreen(0, 1, "Play something!!", false);
      }
      else {
        assign = false;
        menuUpdate();
        break;
      }
    }
  }
}

// Cancel Latch

void unLatch() {
  isLatched1 = false;
  isLatched2 = false;
  isLatched3 = false;
  isLatched4 = false;
  globLatched = false;
}

// Save latch to program bank

SynthProgram saveLatch() {
  //TODO: Implement saveLatch() - NEEDS TESTING
  SynthProgram s;
  s = globalProg;
  programBank.add(s);
}

// Set Tempo Out

void setTempo(int tempo) {
  //TODO: Implement setTempo()
  MIDI.sendControlChange(CC_TEMPO, tempo, channel);
}

int tapTempo() {
  //TODO: Implement Tap-Tempo
  int tempo = 0;
  return tempo;
}

// Build strings for menu presets (fills blank LCDs)

String menuStringBuilder(){
  String s;
  if (menuIndex < 9){
    s = "0" + (String)(menuIndex + 1) + ": " + globalProg.name;
  }
  else {
    s = (String)(menuIndex + 1) + ": " + globalProg.name;
  }
  while (s.length()<16){
    s += " ";
  }
  return s;
}

// Update menu display
// This assigns the chords from the SynthProgram ADT to globalProg
// whenever a button is pressed, except when it latch is enabled.

void menuUpdate() {
  if (!globLatched) {
    globalProg = programBank.get(menuIndex);
    MIDI.sendProgramChange(globalProg.prog, channel);
  }
    setScreen(0, 0, "  Patch Select  ", false);
    setScreen(0, 1, menuStringBuilder(), false);
  

  if (globLatched) {
    myLcd.setCursor(15, 0);
    myLcd.write(byte(1));
  }
}

// Screen Write Function

void setScreen(int x, int y, String text, bool clear) {
  if (clear) {
    myLcd.clear();
  }
  myLcd.setCursor(x, y);
  myLcd.print(text);
}

// Startup Animation

void startupAnimation() {
  myLcd.setCursor(0, 0);
  myLcd.print(" CHORD  STOMPER ");
  myLcd.setCursor(0, 1);
  myLcd.print("   v0.5");
  myLcd.setCursor(10, 1);
  myLcd.write(byte(0));
  myLcd.setCursor(12, 1);
  myLcd.write(byte(4));
  myLcd.setCursor(14, 1);
  myLcd.print("/");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(3));
  myLcd.setCursor(14, 1);
  myLcd.print("-");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(4));
  myLcd.setCursor(14, 1);
  myLcd.write(byte(2));
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(3));
  myLcd.setCursor(14, 1);
  myLcd.print("|");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(4));
  myLcd.setCursor(14, 1);
  myLcd.print("/");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(3));
  myLcd.setCursor(14, 1);
  myLcd.print("-");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(4));
  myLcd.setCursor(14, 1);
  myLcd.write(byte(2));
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(3));
  myLcd.setCursor(14, 1);
  myLcd.print("|");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(4));
  myLcd.setCursor(14, 1);
  myLcd.print("/");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(3));
  myLcd.setCursor(14, 1);
  myLcd.print("-");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(4));
  myLcd.setCursor(14, 1);
  myLcd.write(byte(2));
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(3));
  myLcd.setCursor(14, 1);
  myLcd.print("|");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(4));
  myLcd.setCursor(14, 1);
  myLcd.print("/");
  delay(400);
  myLcd.setCursor(12, 1);
  myLcd.write(byte(3));
  myLcd.setCursor(14, 1);
  myLcd.print("-");
}

// Program init

void setup() {

  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Add synth programs to programBank LinkedList

  programBank.add(prog1);
  programBank.add(prog2);
  programBank.add(prog3);
  programBank.add(prog4);
  programBank.add(prog5);
  programBank.add(prog6);
  programBank.add(prog7);
  programBank.add(prog8);
  programBank.add(prog9);
  programBank.add(prog10);
  programBank.add(prog11);
  programBank.add(prog12);
  programBank.add(prog13);
  programBank.add(prog14);
  programBank.add(prog15);
  programBank.add(prog16);
  programBank.add(prog17);
  programBank.add(prog18);
  programBank.add(prog19);
  programBank.add(prog20);

  // Set Pin Modes / Pullup
  pinMode(BP_1, INPUT_PULLUP);
  pinMode(BP_2, INPUT_PULLUP);
  pinMode(BP_3, INPUT_PULLUP);
  pinMode(BP_4, INPUT_PULLUP);
  pinMode(BP_5, INPUT_PULLUP);
  pinMode(BP_6, INPUT_PULLUP);
  pinMode(BP_7, INPUT_PULLUP);

  // Attach pins to de-bouncer
  BUTTON_1.attach(BP_1);
  BUTTON_2.attach(BP_2);
  BUTTON_3.attach(BP_3);
  BUTTON_4.attach(BP_4);
  BUTTON_5.attach(BP_5);
  BUTTON_6.attach(BP_6);
  BUTTON_7.attach(BP_7);

  // Assign debounce interval
  BUTTON_1.interval(1);
  BUTTON_2.interval(5);
  BUTTON_3.interval(5);
  BUTTON_4.interval(5);
  BUTTON_5.interval(5);
  BUTTON_6.interval(5);
  BUTTON_7.interval(5);

  // LCD Startup
  myLcd.begin(16, 2);

  // ADT TESTING
  //progz.add(proga);
  //progz.add(progc);
  //progf = progz.get(0);
  //myLcd.print(structTest(progf));
  //delay(5000);
  //progf = progz.get(1);
  //myLcd.print(structTest(progf));
  //delay(5000);

  myLcd.createChar(0, smiley);
  myLcd.createChar(1, latched);
  myLcd.createChar(2, slash);
  myLcd.createChar(3, jjDown);
  myLcd.createChar(4, jjUp);
  myLcd.createChar(5, trademark);

  // Startup Animation
  startupAnimation();

  // Start LCD Menu interface
  menuUpdate();
}

// Main Loop - Chords are sent from button presses
//           Latch menu will be accessed from button 7

void loop() {

  BUTTON_1.update();
  BUTTON_2.update();
  BUTTON_3.update();
  BUTTON_4.update();
  BUTTON_5.update();
  BUTTON_6.update();
  BUTTON_7.update();

  // Chord Trigger Buttons (currently 1-3)

  if (BUTTON_1.fell() == true) {
    if (isLatched1 == true) {
      sendChord(latchChord1, 50);
    }
    else {
      sendChord(globalProg.chord1, 50);
    }
  }

  if (BUTTON_1.rose() == true) {
    if (isLatched1 == true) {
      killChord(latchChord1, 50);
    }
    else {
      killChord(globalProg.chord1, 50);
    }
  }

  if (BUTTON_2.fell() == true) {
    if (isLatched2 == true) {
      sendChord(latchChord2, 50);
    }
    else {
      sendChord(globalProg.chord2, 50);
    }
  }

  if (BUTTON_2.rose() == true) {
    if (isLatched2 == true) {
      killChord(latchChord2, 50);
    }
    else {
      killChord(globalProg.chord2, 50);
    }
  }

  if (BUTTON_3.fell() == true) {
    if (isLatched3 == true) {
      sendChord(latchChord3, 50);
    }
    else {
      sendChord(globalProg.chord3, 50);
    }
  }

  if (BUTTON_3.rose() == true) {
    if (isLatched3 == true) {
      killChord(latchChord3, 50);
    }
    else {
      killChord(globalProg.chord3, 50);
    }
  }

  // These next two buttons move the menu up and down..

  if (BUTTON_4.fell() == true && menuIndex < programBank.size()-1) {
    menuIndex++;
    unLatch();
    menuUpdate();
  }

  if (BUTTON_5.fell() == true && menuIndex > 0) {
    menuIndex--;
    unLatch();
    menuUpdate();
  }

  // Latch menu testing

  if (BUTTON_6.fell() == true) {
    myLcd.clear();
    myLcd.setCursor(3, 0);
    myLcd.print("Latch Menu");
    myLcd.setCursor(0, 1);
    myLcd.print("Push a button..");
    assign = true;
    MIDI.sendControlChange(CC_ALL_NOTE_OFF, VAL_ALL_NOTE_OFF, channel);
    assignLatch();
  }

  
  //  if (BUTTON_4.fell() == true) {
  //    if (isLatched4 == true) {
  //      sendChord(latchChord4, 50);
  //    }
  //    else {
  //      sendChord(globalProg.chord4, 50);
  //    }
  //  }
  //
  //  if (BUTTON_4.rose() == true) {
  //    if (isLatched4 == true) {
  //      killChord(latchChord4, 50);
  //    }
  //    else {
  //      killChord(globalProg.chord4, 50);
  //    }
  //  }
  
}
