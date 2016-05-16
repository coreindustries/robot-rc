// Shared Line Sample for Packet Serial
// Copyright (c) 2012 Dimension Engineering LLC
// See license.txt for license details.

#include <Sabertooth.h>

const int Rear = 1;
const int Front = 2;
Sabertooth Left = Sabertooth(128);
Sabertooth Right = Sabertooth(129);

void setup()
{
  SabertoothTXPinSerial.begin(9600);
  Sabertooth::autobaud(SabertoothTXPinSerial); // Autobaud is for the whole serial line -- you don't need to do
                                               // it for each individual motor driver. This is the version of
                                               // the autobaud command that is not tied to a particular
                                               // Sabertooth object.
                                               // See the examples in 1.Basics for information on whether you
                                               // need this line at all.
  // STOP ALL AT STARTUP
  Left.motor(Rear, 0);
  Left.motor(Front, 0);
  Right.motor(Rear, 0);
  Right.motor(Front, 0);
}

void loop()
{
  Left.motor(Rear, 20); //Left Rear
  Left.motor(Front, -10); //Left Front
  Right.motor(Rear, 20); //Right Rear
  Right.motor(Front, -10); //Right Front
  delay(500);
  
}

