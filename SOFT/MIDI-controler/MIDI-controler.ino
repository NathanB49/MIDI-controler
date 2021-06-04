/*
 * Good vibes here --> https://soundcloud.com/benoit-dumont-1
 * 
 * Potentiometers are adressed as shown below (front view)
 *    ______________________________________________
 *  |                                               |
 *  | 08  | 07  |  06 |  05 |  04 |  03 |  02 |  01 |  
 *  |                                               |
 *  | 16  | 15  |  14 |  13 |  12 |  11 |  10 |  09 |  
 *  |                                               |
 *  | 24  | 23  |  22 |  21 |  20 |  19 |  18 |  17 |  
 *  |                                               |
 *  |                                               |
 *  | 32  | 31  |  30 |  29 |  28 |  27 |  26 |  25 | 
 *  |                                               |
 *  |                                               |
 *    ______________________________________________   
 */

// USB library for MIDI protocol
#include "MIDIUSB.h"

#define DELTA_KNOB  5
#define DELTA_FADER 15
#define DEAD_BAND   15

// Digital output for analog switches
#define S0 14
#define S1 16
#define S2 10

// LEDs
#define RXLED 17
#define TXLED 30

// Store potentiometer value, value[0] and last_value[0] are not used
int value[33], last_value[33];

/*
 * truth_table[analog pin][analog switch position]
 * 
 * Parameter :
 * analog pin = 0..4 --> A0..A3
 * analog switch position = 0..7
 * 
 * Return :
 * Potentiometer number
 */
int truth_table[4][8] = { {2,   9,  10, 1,  12, 3,  11, 4},
                          {25,  18, 17, 26, 19, 27, 20, 28},
                          {15,  8,  7,  16, 5,  14, 6,  13},
                          {24,  32, 31, 23, 30, 21, 29, 22}  };

void setup() {
  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);

  pinMode(RXLED, OUTPUT);
  pinMode(TXLED, OUTPUT);

  // for debug purpose
  //Serial.begin(115200);
  
}

void loop() {

  refreshValue();

  //Send Knob value
  for(int i=1; i<25; i++){
    if( (value[i] > (last_value[i]+DELTA_KNOB)) || 
        (value[i] < (last_value[i]-DELTA_KNOB))){

      //print_value(i);
      
      sendNote(value[i], i);
      last_value[i] = value[i];
    }
  }

  //Send Fader value
  for(int i=25; i<33; i++){
    if( (value[i] > (last_value[i]+DELTA_FADER)) || 
        (value[i] < (last_value[i]-DELTA_FADER))){

      //print_value(i);

      sendNote(value[i], i);
      last_value[i] = value[i];
    }
  }

}

void refreshValue(){
  
  for(int analog_switches = 0; analog_switches<8; analog_switches++){
    
    // Analog switches
    bool S0_o = analog_switches & 1;
    bool S1_o = analog_switches & 2;
    bool S2_o = analog_switches & 4;
    digitalWrite(S0, S0_o);
    digitalWrite(S1, S1_o);
    digitalWrite(S2, S2_o);

    // Get and store value
    value[truth_table[0][analog_switches]] = dead_band(A0);
    value[truth_table[1][analog_switches]] = dead_band(A1);
    value[truth_table[2][analog_switches]] = dead_band(A2);
    value[truth_table[3][analog_switches]] = dead_band(A3);
  }

}

int dead_band(int analog_pin){
  
  int analog_value = analogRead(analog_pin);

  if(analog_value < DEAD_BAND)
    return 0;
  else if(analog_value > (1023-DEAD_BAND))
    return 1023;
  else
    return analog_value;

}

void sendNote(int note, int id){
  
  midiEventPacket_t midiEventPacket = {0x0B, 0xB0, id, note/8};
  MidiUSB.sendMIDI(midiEventPacket);
  MidiUSB.flush();
  
}

// for debug purpose
void print_value(int i){
      Serial.print("last_value[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.print(last_value[i]);
      
      Serial.print(" --> value[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(value[i]);
}
