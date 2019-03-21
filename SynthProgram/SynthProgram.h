/*
  SynthProgram.h - Library for Synthesizer Programs
  Nate Nasteff 2019
*/

#ifndef SynthProgram_h
#define SynthProgram_h

#include <Arduino.h>

class SynthProgram
{
  public:
    SynthProgram(int *notes1, int *notes2, int *notes3, int *notes4, int index, int prog, String title);
    String getName();
    int getProg();
    int note;
    int * chord1;
    int * chord2;
    int * chord3;
    int * chord4;
    int thisIndex;
    int programCC;
    String name;

};

#endif