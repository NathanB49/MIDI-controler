/*
  Programme pour piloter le Botier MIDI 
  https://soundcloud.com/benoit-dumont-1
*/

/*  Les potentiomètres sont adresser comme ci-dessous
 * 
 *    
 *    ______________________________________________
 *  |                                               |
 *  | 00  | 01  |  02 |  03 |  04 |  05 |  06 |  07 |  <-- Ligne A = A0
 *  |                                               |
 *  | 08  | 09  |  10 |  11 |  12 |  13 |  14 |  15 |  <-- Ligne B = A1
 *  |                                               |
 *  | 16  | 17  |  18 |  19 |  20 |  21 |  22 |  23 |  <-- Ligne C = A2
 *  |                                               |
 *  |                                               |
 *  | 24  | 25  |  26 |  27 |  28 |  29 |  30 |  31 |  <-- Ligne D = A3
 *  |                                               |
 *  |                                               |
 *    ______________________________________________   
 */

// Bibliothèque gérant le protocol MIDI
#include "MIDIUSB.h"

// Sorties pour piloté l'adressage des multiplexeurs analogiques
#define S0 14
#define S1 16
#define S2 10

// Filtrage
#define FITRAGE_MODE
#define ECART_TYPE 5

#define REFRESH_RATE_MS 1

// Parce que les potentiomètre angulaire sont chinois, la moitié sont HS
bool potentiometre_ok[32] = { 
                              0,  1,  1,  1,  0,  0,  0,  0,
                              0,  0,  0,  0,  1,  0,  1,  1, 
                              1,  0,  0,  0,  1,  0,  0,  0, 
                              1,  1,  1,  1,  1,  1,  1,  1    
                            };
int val;
int last_val[32];

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  for(int i=0; i<32; i++){
    if(potentiometre_ok[i]==1){
      val = getValue(i);
#ifdef FITRAGE_MODE
      if((val > (last_val[i]+ECART_TYPE)) || (val < (last_val[i]-ECART_TYPE))){
        last_val[i] = val;  
        midiEventPacket_t note = {0x0B, 0xB0, i, val/8};
        MidiUSB.sendMIDI(note);
        MidiUSB.flush();
      }
#else
      midiEventPacket_t note = {0x0B, 0xB0, i, val/8};
      MidiUSB.sendMIDI(note);
      MidiUSB.flush();
#endif
      delay(REFRESH_RATE_MS);
    }
  }
}

int getValue(int addr){

  // Addressage du 74HC4051
  digitalWrite(S0, addr & 1);
  digitalWrite(S1, (addr >> 1) & 1);
  digitalWrite(S2, (addr >> 2) & 1);

  // Laisser un peu de temps au 74HC4051 de changer son adresse
  delay(1);
  
  if(addr < 8){
    return analogRead(A0);
  }else if(addr < 16){
    return analogRead(A1);
  }else if(addr < 24){
    return analogRead(A2);
  }else if(addr < 32){
    return analogRead(A3);
  }

}
