// https://github.com/jacobrosenthal/arduino-cli
// https://github.com/arduino/Arduino/blob/ide-1.5.x/build/shared/manpage.adoc


// Sketch for controlling Toby/mecanum robot from a RC radio controller

#include <Sabertooth.h>
#include <SoftwareSerial.h>

// RC receiver vars
volatile int ch1; // steering
volatile int ch2; // throttle
int steering; // normalized ch1
int throttle; // normalized ch2
int steer_min; // min value seen
int steer_max; // max value seen
int steer_deadband; // amount control pushes beyond to affect motion
const int power_deadband = 5; 
int throttle_min;
int throttle_max;
int steer; // steer mapped to -255,255
int power; // drive power mapped to -255,255
const int MAXPOWER = 30; // max drive power available

// software serial #1: RX = digital pin 10, TX = digital pin 11
SoftwareSerial monitorSerial(10, 11);

// Sabertooth vars
const int Rear = 1;
const int Front = 2;
Sabertooth Left = Sabertooth(128);
Sabertooth Right = Sabertooth(129);

void setup()
{
  interrupts();
  
  // receiver
  pinMode(2, INPUT);          // Set our input pins as such
  pinMode(3, INPUT);
//  Serial.begin(115200);       // Serial to Sabertooth
  monitorSerial.begin(9600);  // Serial to laptop via USB TTL cable begin
  monitorSerial.println("TobyToy started");

  steer_min = 960;
  steer_max = 2040;
  steer_deadband = 10;
  throttle_min = 910;
  throttle_max = 2000;
//  power_deadband = 10;
//  MAXPOWER = 50; // 255 is the absolute max

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
  ch1 = pulseIn(2, HIGH, 25000); // Read the pulse width of. 25000
  delay(5);
  ch2 = pulseIn(3, HIGH, 25000); // each channel. 25000

  steering = ch1 != 0 ? ch1 : steering;
  throttle = ch2 != 0 ? ch2 : throttle;
//  throttle = ch2;

  // RAW VALUES
//  monitorSerial.print("Steering:"); // Print the value of 
//  monitorSerial.print(steering);        // each channel
//  monitorSerial.print(" | ");
//  monitorSerial.print("Throttle:");
//  monitorSerial.println(throttle);

  steer = map(steering, steer_min,steer_max, -255, 255); //center over zero
//  power = map(throttle, throttle_min, throttle_max, -255, 255);
  power = map(throttle, throttle_min, throttle_max, -MAXPOWER, MAXPOWER);
  steer = constrain(steer, -255, 255); //only pass values whose absolutes are
  power = constrain(power, -255, 255); //only pass values whose absolutes are

  
//  monitorSerial.print("Steer:"); // Print the value of 
//  monitorSerial.print(steer);        // each channel
//  monitorSerial.print(" | ");
//  monitorSerial.print("Power:");
//  monitorSerial.println(power);


  driveRobot(power, steer);
  
  // SEND COMMANDS TO MOTOR CONTROLLERS
//  Left.motor(Rear, 0); //Left Rear
//  Left.motor(Front, -10); //Left Front
//  Right.motor(Rear, 0); //Right Rear
//  Right.motor(Front, 10); //Right Front
  delay(50);
  
};


// main entry to driving the robot
// looks for sensible values
// applied deadband logic
// sends values onto lower-level commands
void driveRobot(int power, int steer){
  // apply MAX power safety
  if(power > 0 && power > MAXPOWER){
    power = MAXPOWER;
  }
  if(power < 0 && power < -MAXPOWER){
    power = -MAXPOWER;
  }
  
  if(steer < steer_deadband && steer > -steer_deadband){
    monitorSerial.println("No steering, drive straight");
  }

  if(power < power_deadband && power > -power_deadband){
    monitorSerial.println("No power, STOPPED");
    driveStraight(0);
  }else{
    driveStraight(power);
  }

};


// simplest drive motion: straight line
void driveStraight(int power){
  monitorSerial.print("Drive Straight: ");
  monitorSerial.println(power);
  Left.motor(Rear, power); //Left Rear
  Left.motor(Front, power); //Left Front
  Right.motor(Rear, power); //Right Rear
  Right.motor(Front, power); //Right Front
}


