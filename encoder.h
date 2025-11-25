// encoder.h
#ifndef ENCODER_H
#define ENCODER_H
volatile bool lastA = 0;
volatile bool lastB = 0;
void readJogEncoder(String axis);
void encoderOut();
void encoderISR();

#endif
