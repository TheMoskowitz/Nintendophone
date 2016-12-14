// final Nintendophone code
     
// Arduino pin connections for each of the joystick wires
// (note: the color comments are for my use as they remind
// which wire goes where)
int rightPin = 6; //red
int leftPin = 5; //pink
int topPin = 13; //green
int bottomPin = 2; //yellow

// Button that changes key
int keyPin = 8; //yellow button

// Button that lowers the octave
int octavedownPin = 9; //blue button with red power wire

// Button that raises the octave
int octaveupPin = 10; //blue button with yellow power wire

// Button that changes the scale
int scalePin = 11; //black button with (stupidly) black wire

// Switch that flips it into a drumset
int drumPin = 12;

// These keep the current values of each joystick direction
int valRight;
int valLeft;
int valTop;
int valBottom;

// These keep the current values of each button (and drum switch)
int valKey;
int valOctaveup;
int valOctavedown;
int valScale;
int valDrum;

// keyCounter holds the current Key
// The boolean, for the Key button and all other
// buttons is for debouncing
int keyCounter = 0;
boolean keyPressed = false;

// octaveCounter holds the current octave
int octaveCounter = 4;
boolean octaveupPressed = false;
boolean octavedownPressed = false;

// scaleNumber holds the current scale
int scaleNumber = 0;
boolean scalePressed = false;

// These are the chromatic shifts between each
// note in the current scale.
// It starts in a blues scale so the first shift is up
// three chromatic notes (i.e. a minor third), etc...
int shiftOne = 3;
int shiftTwo = 5;
int shiftThree = 6;
int shiftFour = 7;
int shiftFive = 10;
int shiftSix = 12;
int shiftSeven = 24;

// check if all buttons are pressed for the special feature
// super mario theme
boolean allPressed = false;


// boolean isPlaying = false;

#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX

byte note = 52; // The initial MIDI note value to be played
byte resetMIDI = 4; // Tied to VS1053 Reset line
byte ledPin = 13; // MIDI traffic inidicator
int  instrument = 0;
byte lastNote = 0;

int drum = 44; // Initial drum value to be played if in drumset mode
byte lastDrum = 0;

void setup() {
  
  Serial.begin(57600);

  //Setup soft serial for MIDI control
  mySerial.begin(31250);

  //Reset the VS1053
  pinMode(resetMIDI, OUTPUT);
  digitalWrite(resetMIDI, LOW);
  delay(100);
  digitalWrite(resetMIDI, HIGH);
  delay(100);
  talkMIDI(0xB0, 0x07, 120); //0xB0 is channel message, set channel volume to near max (127)
  
  Serial.begin(9600); // for testing joystick
  pinMode(rightPin, INPUT_PULLUP);
  pinMode(leftPin, INPUT_PULLUP);
  pinMode(topPin, INPUT_PULLUP);
  pinMode(bottomPin, INPUT_PULLUP);
  pinMode(keyPin, INPUT_PULLUP);
  pinMode(octavedownPin, INPUT_PULLUP);
  pinMode(octaveupPin, INPUT_PULLUP);
  pinMode(scalePin, INPUT_PULLUP);
  pinMode(drumPin, INPUT_PULLUP);
  
}

void loop() {
  
  // Check what's being pressed
  valRight = digitalRead(rightPin);
  valLeft = digitalRead(leftPin);
  valTop = digitalRead(topPin);
  valBottom = digitalRead(bottomPin);
  valKey = digitalRead(keyPin);
  valOctavedown = digitalRead(octavedownPin);
  valOctaveup = digitalRead(octaveupPin);
  valScale = digitalRead(scalePin);
  valDrum = digitalRead(drumPin);
  
  
if (valDrum == HIGH) {
  // If normal mode is on (HIGH means the switch to
  // drumset mode hasn't been flipped)
  
  talkMIDI(0xB0, 0, 0x79); //bank select melodic
  
  talkMIDI(0xC0, 86, 0); //set instrument
  
  // Check Key Button
  if (valKey == LOW) {
    if (keyPressed == false) {
      keyPressed = true;
      // Raise the initial note by one chromatic step
      note = note + 1;
      // Raise the Key by one chromatic step
      keyCounter = keyCounter + 1;
      if (keyCounter == 12) {
        note = note - 12;
        keyCounter = 0;
      }
    }
    else {
    // It's the same key press so don't do anything
    }
    // Serial.println(note);
  } 
  else {
    keyPressed = false;
  }
  
  // Check Octave Up
  if (valOctaveup == LOW) {
    if (octaveupPressed == false) {
      octaveupPressed = true;
      if (octaveCounter != 9) {
      note = note + 12;
      octaveCounter = octaveCounter + 1;
      }
      else {
      // You've hit the upper octave limit so don't
      // go any higher
      }
    }
    else {
      // It's the same key press so don't do anything
    }
    // Serial.println(octaveCounter);
  } 
  else {
    octaveupPressed = false;
  }
  
   // Check Octave Down
  if (valOctavedown == LOW) {
    if (octavedownPressed == false) {
      octavedownPressed = true;
      if (octaveCounter != 2) {
      note = note - 12;
      octaveCounter = octaveCounter - 1;
      }
      else {
        // You've reached the lower octave limit so
        // it won't go down anymore
      }
    }
    else {
      // It's the same key press so don't do anything
    }
    // Serial.println(octaveCounter);
  } 
  else {
    octavedownPressed = false;
  }
  
    // Check Scale
  if (valScale == LOW) {
    if (scalePressed == false) {
      scalePressed = true;
      // There are only four scales to choose from in 
      // this incarnation but you can of course add 
      // as many as you want.
      if (scaleNumber == 3) {
      // Blues Scale
      shiftOne = 3;
      shiftTwo = 5;
      shiftThree = 6;
      shiftFour = 7;
      shiftFive = 10;
      shiftSix = 12;
      shiftSeven = 24;
      scaleNumber = 0;
      }
      else if (scaleNumber == 0) {
      // Major Scale
      shiftOne = 2;
      shiftTwo = 4;
      shiftThree = 5;
      shiftFour = 7;
      shiftFive = 9;
      shiftSix = 11;
      shiftSeven = 12;
      scaleNumber = 1;  
      }
      else if (scaleNumber == 1) {
      // Minor Scale
      shiftOne = 2;
      shiftTwo = 3;
      shiftThree = 5;
      shiftFour = 7;
      shiftFive = 9;
      shiftSix = 10;
      shiftSeven = 12;
      scaleNumber = 2;  
      }
      else if (scaleNumber == 2) {
      // Phrygian Scale (kind of a Spanish sound)
      shiftOne = 1;
      shiftTwo = 4;
      shiftThree = 5;
      shiftFour = 7;
      shiftFive = 8;
      shiftSix = 10;
      shiftSeven = 12;
      scaleNumber = 3;  
      }
    }
    else {
      // it's the same key press so don't do anything
    }
    // Serial.println(scaleNumber);
  } 
  else {
    scalePressed = false;
  }
  
    //Check for Special Feature
    //Check if all keys were pressed
  if ((valScale == LOW) && (valKey == LOW) && (valOctaveup == LOW) && (valOctavedown == LOW)) {
    if (allPressed == false) {
    allPressed = true;
    //Play Mario Theme Song
    // Serial.println("Mario song");
    playMario();
    }
    else {
      // Same button press so don't do anything
    }
  }
  else {
    allPressed = false;
  }

  // Here we'll actually check what joystick is doing and play
  // the relevant note
  // Read Joystick  
  if ((valTop == LOW) && (valLeft == HIGH) && (valRight == HIGH)) {
    // A value is LOW if the joystick is moving in that direction.
    // If it is moving in two directions at once (i.e. UP-RIGHT), both
    // direction values will be low
    // Serial.println("Up");
    if ((lastNote != note)) {
        noteOff(0, lastNote, 0);
        noteOn(0, note, 127);
        lastNote = note;
     }
    else {   
      // This is also for debouncing. Same joystick movement, don't do anything. 
     }
    

  }
  else if ((valTop == LOW) && (valRight == LOW)) {
    // Serial.println("Top-Right");
    if ((lastNote != note + shiftOne)) {
        noteOff(0, lastNote, 0);
        noteOn(0, note + shiftOne, 127);
        lastNote = note + shiftOne;
     }
    else {    
    // Same joystick move, don't do anything
     }
 
  }
  else if ((valRight == LOW) && (valTop == HIGH) && (valBottom == HIGH)) {
    // Serial.println("Right");
    if ((lastNote != note + shiftTwo)) {
        noteOff(0, lastNote, 0);
        noteOn(0, note + shiftTwo, 127);
        lastNote = note + shiftTwo;
     }
    else {  
      // Same joystick move, don't do anything  
     }
    
  }
  else if ((valRight == LOW) && (valBottom == LOW)) {
    // Serial.println("Down-Right");
    if ((lastNote != note + shiftThree)) {
        noteOff(0, lastNote, 0);
        noteOn(0, note + shiftThree, 127);
        lastNote = note + shiftThree;
     }
    else {  
      // Same joystick move, don't do anything  
     }
    
    
  }
  else if ((valBottom == LOW) && (valRight == HIGH) && (valLeft == HIGH)) {
    // Serial.println("Down");
    if ((lastNote != note + shiftFour)) {
        noteOff(0, lastNote, 0);
        noteOn(0, note + shiftFour, 127);
        lastNote = note + shiftFour;
     }
    else { 
      // Same joystick move, don't do anything   
     }
    
  }
  else if ((valBottom == LOW) && (valLeft == LOW)) {
    // Serial.println("Down-Left");
    if ((lastNote != note + shiftFive)) {
        noteOff(0, lastNote, 0);
        noteOn(0, note + shiftFive, 127);
        lastNote = note + shiftFive;
     }
    else {   
      // Same joystick move, don't do anything 
     }
    
  }
  else if ((valLeft == LOW) && (valTop == HIGH) && (valBottom == HIGH)) {
    // Serial.println("Left");
    if ((lastNote != note + shiftSix)) {
        noteOff(0, lastNote, 0);
        noteOn(0, note + shiftSix, 127);
        lastNote = note + shiftSix;
     }
    else {   
      // Same joystick move, don't do anything 
     }
    
  }
  else if ((valTop == LOW) && (valLeft == LOW)) {
    // Serial.println("Up-Left");
    if ((lastNote != note + shiftSeven)) {
        noteOff(0, lastNote, 0);
        noteOn(0, note + shiftSeven, 127);
        lastNote = note + shiftSeven;
     }
    else {    
      // Same joystick move, don't do anything
     }
    
  }
  else  {
   // Serial.println("Fail");
   // delay(1000);
   //noteOff(0, lastNote, 0);
   if (lastNote != 0) {
     noteOff(0, lastNote, 0);
     lastNote = 0;
   }
   else {
   }
   
  }
  
}

else if (valDrum == LOW) {

  // Set it to drumset mode
  // In drumset mode, only the joystick works,
  // the buttons don't do anything
  
  talkMIDI(0xB0, 0, 0x78); //bank select percussion
  
  talkMIDI(0xC0, 0, 0); //set instrument

     // Read Joystick  
  if ((valTop == LOW) && (valLeft == HIGH) && (valRight == HIGH)) {
    // Serial.println("Splash cymbal");
    
    // play a Splash Cymbal
    if ((lastDrum != 55)) {
        noteOff(0, lastDrum, 0);
        noteOn(0, 55, 127);
        lastDrum = 55;
     }
    else {    
      // Same joystick move, don't do anything
     }
    

  }
  else if ((valTop == LOW) && (valRight == LOW)) {
    // Serial.println("Hi-Hat");
    // play a Hi-Hat
    if ((lastDrum != 44)) {
        noteOff(0, lastDrum, 0);
        noteOn(0, 44, 127);
        lastDrum = 44;
     }
    else { 
      // Same joystick move, don't do anything   
     }
 
  }
  else if ((valRight == LOW) && (valTop == HIGH) && (valBottom == HIGH)) {
    // Serial.println("Snare Drum");
    // play a Snare Drum
    if ((lastDrum != 38)) {
        noteOff(0, lastDrum, 0);
        noteOn(0, 38, 127);
        lastDrum = 38;
     }
    else {    
      // Same joystick move, don't do anything
     }
    
  }
  else if ((valRight == LOW) && (valBottom == LOW)) {
    // Serial.println("Bass Drum");
    // play a Bass Drum
    if ((lastDrum != 35)) {
        noteOff(0, lastDrum, 0);
        noteOn(0, 35, 127);
        lastDrum = 35;
     }
    else {   
      // Same joystick move, don't do anything 
     }
    
    
  }
  else if ((valBottom == LOW) && (valRight == HIGH) && (valLeft == HIGH)) {
    // Serial.println("Low Floor Tom");
    // play a Floor Tom
    if ((lastDrum != 41)) {
        noteOff(0, lastDrum, 0);
        noteOn(0, 41, 127);
        lastDrum = 41;
     }
    else {    
      // Same joystick move, don't do anything
     }
    
  }
  else if ((valBottom == LOW) && (valLeft == LOW)) {
    // Serial.println("Low Tom");
    // play a Low Tom
    if ((lastDrum != 45)) {
        noteOff(0, lastDrum, 0);
        noteOn(0, 45, 127);
        lastDrum = 45;
     }
    else {  
      // Same joystick move, don't do anything  
     }
    
  }
  else if ((valLeft == LOW) && (valTop == HIGH) && (valBottom == HIGH)) {
    // Serial.println("High Tom");
    // play a High Tom
    if ((lastDrum != 50)) {
        noteOff(0, lastDrum, 0);
        noteOn(0, 50, 127);
        lastDrum = 50;
     }
    else {  
      // Same joystick move, don't do anything  
     }
    
  }
  else if ((valTop == LOW) && (valLeft == LOW)) {
    // Serial.println("Ride Cymbal");
    // play a Ride Cymbal
    if ((lastDrum != 51)) {
        noteOff(0, lastDrum, 0);
        noteOn(0, 51, 127);
        lastDrum = 51;
     }
    else {    
      // Same joystick move, don't do anything
     }
    
  }
  else  {
   // Serial.println("Fail");
   // delay(1000);
   //noteOff(0, lastNote, 0);
   if (lastDrum != 0) {
     noteOff(0, lastDrum, 0);
     lastDrum = 0;
   }
   else {
   }
   
 }

}

} // End of void loop()

//Send a MIDI note-on message.  Like pressing a piano key
//channel ranges from 0-15
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}

//Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}

//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
  digitalWrite(ledPin, HIGH);
  mySerial.write(cmd);
  mySerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    mySerial.write(data2);

  digitalWrite(ledPin, LOW);
}     

// Hardcoded Mario theme
void playMario() {
  noteOff(0, lastNote, 0);
  //Part 1
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    delay(300);
    noteOn(0, 67, 127);
    delay(150);
    noteOff(0, 67, 0);
    delay(300);
    noteOn(0, 64, 127);
    delay(150);
    noteOff(0, 64, 0);
    delay(300);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 71, 127);
    delay(150);
    noteOff(0, 71, 0);
    delay(150);
    noteOn(0, 70, 127);
    delay(150);
    noteOff(0, 70, 0);
    noteOn(0, 69, 127);
    delay(300);
    noteOff(0, 69, 0);
    noteOn(0, 67, 127);
    delay(200);
    noteOff(0, 67, 0);
    noteOn(0, 76, 127);
    delay(200);
    noteOff(0, 76, 0);
    noteOn(0, 79, 127);
    delay(200);
    noteOff(0, 79, 0);
    noteOn(0, 81, 127);
    delay(300);
    noteOff(0, 81, 0);
    noteOn(0, 77, 127);
    delay(150);
    noteOff(0, 77, 0);
    noteOn(0, 79, 127);
    delay(150);
    noteOff(0, 79, 0);
    
    delay(150);
    noteOn(0, 76, 127);
    delay(300);
    noteOff(0, 76, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    noteOn(0, 74, 127);
    delay(150);
    noteOff(0, 74, 0);
    noteOn(0, 71, 127);
    delay(150);
    noteOff(0, 71, 0);
    delay(300);
    
  //Repeat Part 1
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    delay(300);
    noteOn(0, 67, 127);
    delay(150);
    noteOff(0, 67, 0);
    delay(300);
    noteOn(0, 64, 127);
    delay(150);
    noteOff(0, 64, 0);
    delay(300);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 71, 127);
    delay(150);
    noteOff(0, 71, 0);
    delay(150);
    noteOn(0, 70, 127);
    delay(150);
    noteOff(0, 70, 0);
    noteOn(0, 69, 127);
    delay(300);
    noteOff(0, 69, 0);
    noteOn(0, 67, 127);
    delay(200);
    noteOff(0, 67, 0);
    noteOn(0, 76, 127);
    delay(200);
    noteOff(0, 76, 0);
    noteOn(0, 79, 127);
    delay(200);
    noteOff(0, 79, 0);
    noteOn(0, 81, 127);
    delay(300);
    noteOff(0, 81, 0);
    noteOn(0, 77, 127);
    delay(150);
    noteOff(0, 77, 0);
    noteOn(0, 79, 127);
    delay(150);
    noteOff(0, 79, 0);
    
    delay(150);
    noteOn(0, 76, 127);
    delay(300);
    noteOff(0, 76, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    noteOn(0, 74, 127);
    delay(150);
    noteOff(0, 74, 0);
    noteOn(0, 71, 127);
    delay(150);
    noteOff(0, 71, 0);
    delay(300);
    
  //Part 2
    delay(300);
    noteOn(0, 79, 127);
    delay(150);
    noteOff(0, 79, 0);
    noteOn(0, 78, 127);
    delay(150);
    noteOff(0, 78, 0);
    noteOn(0, 77, 127);
    delay(150);
    noteOff(0, 77, 0);
    noteOn(0, 75, 127);
    delay(300);
    noteOff(0, 75, 0);
    noteOn(0, 76, 127);
    delay(150);
    noteOff(0, 76, 0);
    
    delay(150);
    noteOn(0, 68, 127);
    delay(150);
    noteOff(0, 68, 0);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    delay(150);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    noteOn(0, 74, 127);
    delay(150);
    noteOff(0, 74, 0);
    
    delay(300);
    noteOn(0, 79, 127);
    delay(150);
    noteOff(0, 79, 0);
    noteOn(0, 78, 127);
    delay(150);
    noteOff(0, 78, 0);
    noteOn(0, 77, 127);
    delay(150);
    noteOff(0, 77, 0);
    noteOn(0, 75, 127);
    delay(300);
    noteOff(0, 75, 0);
    noteOn(0, 76, 127);
    delay(150);
    noteOff(0, 76, 0);
    
    delay(150);
    noteOn(0, 84, 127);
    delay(150);
    noteOff(0, 84, 0);
    delay(150);
    noteOn(0, 84, 127);
    delay(150);
    noteOff(0, 84, 0);
    noteOn(0, 84, 127);
    delay(600);
    noteOff(0, 84, 0);
    
    delay(300);
    noteOn(0, 79, 127);
    delay(150);
    noteOff(0, 79, 0);
    noteOn(0, 78, 127);
    delay(150);
    noteOff(0, 78, 0);
    noteOn(0, 77, 127);
    delay(150);
    noteOff(0, 77, 0);
    noteOn(0, 75, 127);
    delay(300);
    noteOff(0, 75, 0);
    noteOn(0, 76, 127);
    delay(150);
    noteOff(0, 76, 0);
    
    delay(150);
    noteOn(0, 68, 127);
    delay(150);
    noteOff(0, 68, 0);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    delay(150);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    noteOn(0, 74, 127);
    delay(150);
    noteOff(0, 74, 0);
    
    delay(300);
    noteOn(0, 75, 127);
    delay(300);
    noteOff(0, 75, 0);
    delay(150);
    noteOn(0, 74, 127);
    delay(300);
    noteOff(0, 74, 0);
    delay(150);
    
    noteOn(0, 72, 127);
    delay(600);
    noteOff(0, 72, 0);
    delay(600);
    
  // Repeat Part 2
  // I'd recommend cutting this
    delay(300);
    noteOn(0, 79, 127);
    delay(150);
    noteOff(0, 79, 0);
    noteOn(0, 78, 127);
    delay(150);
    noteOff(0, 78, 0);
    noteOn(0, 77, 127);
    delay(150);
    noteOff(0, 77, 0);
    noteOn(0, 75, 127);
    delay(300);
    noteOff(0, 75, 0);
    noteOn(0, 76, 127);
    delay(150);
    noteOff(0, 76, 0);
    
    delay(150);
    noteOn(0, 68, 127);
    delay(150);
    noteOff(0, 68, 0);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    delay(150);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    noteOn(0, 74, 127);
    delay(150);
    noteOff(0, 74, 0);
    
    delay(300);
    noteOn(0, 79, 127);
    delay(150);
    noteOff(0, 79, 0);
    noteOn(0, 78, 127);
    delay(150);
    noteOff(0, 78, 0);
    noteOn(0, 77, 127);
    delay(150);
    noteOff(0, 77, 0);
    noteOn(0, 75, 127);
    delay(300);
    noteOff(0, 75, 0);
    noteOn(0, 76, 127);
    delay(150);
    noteOff(0, 76, 0);
    
    delay(150);
    noteOn(0, 84, 127);
    delay(150);
    noteOff(0, 84, 0);
    delay(150);
    noteOn(0, 84, 127);
    delay(150);
    noteOff(0, 84, 0);
    noteOn(0, 84, 127);
    delay(600);
    noteOff(0, 84, 0);
    
    delay(300);
    noteOn(0, 79, 127);
    delay(150);
    noteOff(0, 79, 0);
    noteOn(0, 78, 127);
    delay(150);
    noteOff(0, 78, 0);
    noteOn(0, 77, 127);
    delay(150);
    noteOff(0, 77, 0);
    noteOn(0, 75, 127);
    delay(300);
    noteOff(0, 75, 0);
    noteOn(0, 76, 127);
    delay(150);
    noteOff(0, 76, 0);
    
    delay(150);
    noteOn(0, 68, 127);
    delay(150);
    noteOff(0, 68, 0);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    delay(150);
    noteOn(0, 69, 127);
    delay(150);
    noteOff(0, 69, 0);
    noteOn(0, 72, 127);
    delay(150);
    noteOff(0, 72, 0);
    noteOn(0, 74, 127);
    delay(150);
    noteOff(0, 74, 0);
    
    delay(300);
    noteOn(0, 75, 127);
    delay(300);
    noteOff(0, 75, 0);
    delay(150);
    noteOn(0, 74, 127);
    delay(300);
    noteOff(0, 74, 0);
    delay(150);
    
    noteOn(0, 72, 127);
    delay(600);
    noteOff(0, 72, 0);
    delay(600);
    
    keyCounter = 0;
    keyPressed = false;
    octaveupPressed = false;
    octavedownPressed = false;
    octaveCounter = 4;
    scalePressed = false;
    scaleNumber = 0;
    shiftOne = 3;
    shiftTwo = 5;
    shiftThree = 6;
    shiftFour = 7;
    shiftFive = 10;
    shiftSix = 12;
    shiftSeven = 24;
}
