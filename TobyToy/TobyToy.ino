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
const int steer_deadband = 10; // amount control pushes beyond to affect motion
const int power_deadband = 5; 
int throttle_min;
int throttle_max;
int steer; // steer mapped to -255,255
int power; // drive power mapped to -255,255
const int MAXPOWER = 20; // max drive power available

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
  monitorSerial.begin(115200);  // Serial to laptop via USB TTL cable begin
  monitorSerial.println("TobyToy started");

  steer_min = 960;
  steer_max = 2040;
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
  ch1 = pulseIn(2, HIGH, 25000); // steering. Read the pulse width of. 25000
  delay(5);
  ch2 = pulseIn(3, HIGH, 25000); // throttle.set at 25000

  steering = ch1 != 0 ? ch1 : steering; // if we get a non-zero value, update, else leave it
  throttle = ch2 != 0 ? ch2 : throttle;


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
//    testSteer(steer, power);
  
  // SEND COMMANDS TO MOTOR CONTROLLERS
//  Left.motor(Rear, 0); //Left Rear
//  Left.motor(Front, -10); //Left Front
//  Right.motor(Rear, 0); //Right Rear
//  Right.motor(Front, 10); //Right Front
//  delay(5);
  
};


void testSteer(int steer, int power){
  monitorSerial.println(steer);
}



// main entry to driving the robot
// looks for sensible values
// applied deadband logic
// sends values onto lower-level commands
void driveRobot(int power, int steer){
//    monitorSerial.print("driveRobot. power: ");
//    monitorSerial.print(power);
//    monitorSerial.print(" | ");
//    monitorSerial.println(steer);
  


  if(power < power_deadband && power > -power_deadband){
    monitorSerial.println("No power, STOPPED");
    allSTOP();
  }else{
      if(steer < steer_deadband && steer > -steer_deadband){
        driveStraight(power);
      }else{
        mixSteering(power, steer);
      }
  }

}



void allSTOP(){
  power = 0;
  Left.motor(Rear, power); //Left Rear
  Left.motor(Front, power); //Left Front
  Right.motor(Rear, power); //Right Rear
  Right.motor(Front, power); //Right Front
}


// simplest drive motion: straight line
void driveStraight(int power){
  monitorSerial.print("Drive Straight: ");
  monitorSerial.println(power);
  Left.motor(Rear, power); //Left Rear
  Left.motor(Front, power); //Left Front
  Right.motor(Rear, power); //Right Rear
  Right.motor(Front, power); //Right Front
}


// non-straight driving
// mix the mechanum steering
void mixSteering(int power, int steer){
  float reducer = 0;
  int lf = 0; // left front
  int rf = 0; // right front
  int lr = 0; // left rear
  int rr = 0; // right rear
  String movement = "na";
  reducer = (abs(map(steer, 0, 255, 0, 130))*1.0)/100; // come up with a percentage based on amount of steer

    // right: -255 to -200, crab | -199 to 0 skid
    // left: 255 to 200, crab | 199 to 0 skid
    
    // SKID LEFT
    if(steer > 0 && steer < 200){ // skid left
      movement = "skid_left";
      lf = power;
      lr = power;
      rf = power * reducer;
      rr = power * reducer;
    }

    // CRAB LEFT
    if(steer > 200){ // skid left
      movement = "crab_left";
    }

    // SKID RIGHT
    if(steer < 0 && steer > -200){ // skid left
      movement = "skid_right";
    }

    // CRAB RIGHT
    if(steer < -200){ // skid left
      movement = "crab_right";
    }


    monitorSerial.print("mixSteering. power: ");
    monitorSerial.print(power);
    monitorSerial.print(" | ");
    monitorSerial.print(steer);
    monitorSerial.print(" | ");
    monitorSerial.print(reducer);
    monitorSerial.print(" | ");
    monitorSerial.println(movement);
}





