
#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

#include "xylo.h"

#define STARTNOTE 60
#define ENDNOTE 76 //Equal to the highest note
#define KEY_UP_TIME 40


#define LED 13
MIDI_CREATE_DEFAULT_INSTANCE();

//int played = 0; //Does nothing?
unsigned long startTime = 0;

//Before rewiring:
//Low C# was 13

int pinnumbers[] = {22, 50, 23, 38, 24, 25, 34, 26, 35, 27, 36, 28, 29, 37, 30, 51, 31}; //Ports for C, C#, D, ..., high D#, high E
unsigned long pintimes[] = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}; //Time since turned on
bool toPlay[17] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}; //Stores whether we want to play the solenoids next cycle; updates as we run
int nPins = 17;

void handleNoteOn(byte channel, byte pitch, byte velocity){
  //This function queues up notes to be played
  if(channel != 1) return; //Only play channel 1
  if(velocity == 0) return; //Ignore velocity 0
  
  int noteIndex = pitch;

  //Rescale notes
  while(noteIndex < STARTNOTE){
    noteIndex += 12;
  }
  while(noteIndex > ENDNOTE){
    noteIndex -= 12;
  }

  toPlay[noteIndex - STARTNOTE] = true;

  digitalWrite(pinnumbers[noteIndex-STARTNOTE], HIGH);
  pintimes[noteIndex-STARTNOTE] = millis();
/*
  //Add extra time delay any time we get new messages so chords don't get split
  startTime += KEY_UP_TIME/5;
  
  unsigned long curTime = millis();
  if(startTime > curTime){
    startTime = curTime;
  }*/
}

void playNotes(){
  
  //Plays any queued notes, then clears the queue
  for(int x = 0; x < nPins; x++){
    if(toPlay[x]){
      Serial.println("a");
      int keyPin = pinnumbers[x]; // map the note to the pin
      digitalWrite(keyPin, HIGH);
    }
  }
  //digitalWrite(LED, HIGH);
  delay(KEY_UP_TIME);
  for(int x = 0; x < nPins; x++){
    if(toPlay[x]){
      int keyPin = pinnumbers[x]; // map the note to the pin
      digitalWrite(keyPin, LOW);
    }
  }

  //Reset toPlay array to all false
  for(int x = 0; x < nPins; x++){
    toPlay[x] = false;
  }
}

void setup()
{
  xylo_init();
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  for(int x = 0; x < nPins; x++){
    pinMode(pinnumbers[x], OUTPUT);
    digitalWrite(pinnumbers[x], LOW);
  }
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.begin(MIDI_CHANNEL_OMNI);          // Launch MIDI and listen to channel 1
  MIDI.turnThruOn();
  startTime = millis();
}

void loop()
{
  //Check for and process new MIDI messages, then if it's time to play notes, play notes
  MIDI.read();

  for(int x = 0; x < nPins; x++){
    if(millis() - pintimes[x] > KEY_UP_TIME){
      int keyPin = pinnumbers[x]; // map the note to the pin
      digitalWrite(keyPin, LOW);
    }
  }

  /*unsigned long curTime = millis();
  if(curTime - startTime > KEY_UP_TIME){
    startTime = curTime;
    playNotes();
  }*/
}













