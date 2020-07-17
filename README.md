 
 MIDI ChordStomper - Nate Nasteff 2019
 
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
