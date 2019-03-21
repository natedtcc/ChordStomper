/*
  SynthProgram.cpp - Library for Synthesizer Programs
  Nate Nasteff 2019
*/  

#include <Arduino.h>
#include "SynthProgram.h"

SynthProgram::SynthProgram(int *notes1, int *notes2, int *notes3, int *notes4, int index, int prog, String title){
	  chord1 = notes1;
      chord2 = notes2;
      chord3 = notes3;
      chord4 = notes4;
      name = title;
      programCC = prog;
      thisIndex = index;
}
String SynthProgram::getName() {
      return name;
    }
int SynthProgram::getProg() {
      return programCC;
    }