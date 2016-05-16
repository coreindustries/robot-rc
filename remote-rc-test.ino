     
/*
 RC PulseIn Serial Read out
 By: Nick Poole
 SparkFun Electronics
 Date: 5
 License: CC-BY SA 3.0 - Creative commons share-alike 3.0
 use this code however you'd like, just keep this license and
 attribute. Let me know if you make hugely, awesome, great changes.
 */
 
int ch1; // steering
int ch2; // throttle
int steering; // normalized ch1
int throttle; // normalized ch2
int steer_min; // min value seen
int steer_max; // max value seen
int throttle_min;
int throttle_max;
int steer; // steer mapped to -255,255
int power; // drive power mapped to -255,255

void setup() {
  pinMode(5, INPUT); // Set our input pins as such
  pinMode(6, INPUT);
  Serial.begin(115200); // Pour a bowl of Serial

  steer_min = 960;
  steer_max = 2040;
  throttle_min = 910;
  throttle_max = 2000;
}

void loop() {

  ch1 = pulseIn(5, HIGH, 25000); // Read the pulse width of 
  ch2 = pulseIn(6, HIGH, 25000); // each channel

  steering = ch1 != 0 ? ch1 : steering;
  throttle = ch2 != 0 ? ch2 : throttle;

  // RAW VALUES
  Serial.print("Steering:"); // Print the value of 
  Serial.print(steering);        // each channel
  Serial.print(" | ");
  Serial.print("Throttle:");
  Serial.println(throttle);


  // MAPPED VALUES
  // learning min, max
//  steer_min = steering < steer_min ? steering : steer_min;
//  steer_max = steering > steer_max ? steering : steer_max;
//  throttle_min = throttle < throttle_min ? throttle : throttle_min;
//  throttle_max = throttle > throttle_max ? throttle : throttle_max;
  
  steer = map(steering, steer_min,steer_max, -255, 255); //center over zero
  power = map(throttle, throttle_min, throttle_max, -255, 255);
  steer = constrain(steer, -255, 255); //only pass values whose absolutes are
  power = constrain(power, -255, 255); //only pass values whose absolutes are
  Serial.print("*Steer:"); // Print the value of 
  Serial.print(steer);        // each channel
  Serial.print(" | ");
  Serial.print("Power:");
  Serial.println(power);

  delay(100); // I put this here just to make the terminal 
              // window happier
}