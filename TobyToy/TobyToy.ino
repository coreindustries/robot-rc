// Sketch for controlling Toby/mecanum robot from a RC radio controller

#include <Sabertooth.h>

// RC receiver vars
int ch1; // steering
int ch2; // throttle
int steering; // normalized ch1
int throttle; // normalized ch2
int steer_min; // min value seen
int steer_max; // max value seen
int steer_deadband; // amount control pushes beyond to affect motion
int power_deadband; 
int throttle_min;
int throttle_max;
int steer; // steer mapped to -255,255
int power; // drive power mapped to -255,255

// Sabertooth vars
const int Rear = 1;
const int Front = 2;
Sabertooth Left = Sabertooth(128);
Sabertooth Right = Sabertooth(129);

void setup()
{
  // receiver
  pinMode(5, INPUT); // Set our input pins as such
  pinMode(6, INPUT);
  Serial.begin(115200); // Pour a bowl of Serial

  steer_min = 960;
  steer_max = 2040;
  steer_deadband = 10;
  throttle_min = 910;
  throttle_max = 2000;
  power_deadband = 30;

  // sabertooth
  SabertoothTXPinSerial.begin(9600);
  Sabertooth::autobaud(SabertoothTXPinSerial); 

  // STOP ALL MOTORS AT STARTUP
  Left.motor(Rear, 0);
  Left.motor(Front, 0);
  Right.motor(Rear, 0);
  Right.motor(Front, 0);
}

void loop()
{
  //
  // LISTEN FOR SIGNALS FROM CONTROLLER
  ch1 = pulseIn(5, HIGH, 25000); // Read the pulse width of 
  ch2 = pulseIn(6, HIGH, 25000); // each channel

  steering = ch1 != 0 ? ch1 : steering;
  throttle = ch2 != 0 ? ch2 : throttle;
//  throttle = ch2;

  // RAW VALUES
//  Serial.print("Steering:"); // Print the value of 
//  Serial.print(steering);        // each channel
//  Serial.print(" | ");
//  Serial.print("Throttle:");
//  Serial.println(throttle);

  steer = map(steering, steer_min,steer_max, -255, 255); //center over zero
  power = map(throttle, throttle_min, throttle_max, -255, 255);
  steer = constrain(steer, -255, 255); //only pass values whose absolutes are
  power = constrain(power, -255, 255); //only pass values whose absolutes are
//  Serial.print("*Steer:"); // Print the value of 
//  Serial.print(steer);        // each channel
//  Serial.print(" | ");
//  Serial.print("Power:");
//  Serial.println(power);


  driveRobot(power, steer);
  
  // SEND COMMANDS TO MOTOR CONTROLLERS
//  Left.motor(Rear, 0); //Left Rear
//  Left.motor(Front, 0); //Left Front
//  Right.motor(Rear, 0); //Right Rear
//  Right.motor(Front, 0); //Right Front
//  delay(100);
  
};


// main entry to driving the robot
// looks for sensible values
// applied deadband logic
// sends values onto lower-level commands
void driveRobot(int power, int steer){
  if(steer < steer_deadband && steer > -steer_deadband){
//    Serial.println("No steering, drive straight");
  }

  if(power < power_deadband && power > -power_deadband){
//    Serial.println("No power, STOPPED");
  }else{
    driveStraight(power);
  }

};


// simplest drive motion: straight line
void driveStraight(int power){
//  Serial.print("Drive Straight: ");
//  Serial.println(power);
  Left.motor(Rear, power); //Left Rear
  Left.motor(Front, power); //Left Front
  Right.motor(Rear, power); //Right Rear
  Right.motor(Front, power); //Right Front
}


