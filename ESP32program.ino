/*
  *This is a code for the battlebot Uppercut. Made by the contributors of this repo: https://github.com/lazy-mannn/TUES-Battle-Bots/tree/main
  *When recreating check the repo for how to connect everything.
  *DON'T DO THIS AT HOME WITHOUT THE ADVISORY OF AN ADULT SPECIALIST. you might blow up a capacitor or two!
  *Report any issues if seen!
  *This code isn't optimized!
*/



#include <Wire.h>
#include <SPI.h> 
#include <Adafruit_MCP4725.h>
#include <PS4Controller.h>


#define SDA_1 27    //pins for the dac responsible controlling steer
#define SCL_1 26

#define SDA_2 33    //pins for the dac responsible controlling speed
#define SCL_2 32

int weapon = 22;    //pin for the weapon
int gear;           //For limiting speed

TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);

Adafruit_MCP4725 speed;       //initialize mcp4725 responsible for speed
Adafruit_MCP4725 direction;   //initialize mcp4725 responsible for directons/steer

int forward, back, steer;     //Variables for the controls
int forward_value, back_value, steer_value;   //The variables that the DACs give
//for the DS4
int player = 1;
int battery = 0;

void setup()
{
  Serial.begin(115200);
  

  I2Cone.begin(SDA_1, SCL_1, 400000); 
  I2Ctwo.begin(SDA_2, SCL_2, 400000);

  bool status = direction.begin(0x60, &I2Cone);   //You may use different address for the DAC
  bool status1 = speed.begin(0x61, &I2Ctwo);      //You may use different address for the DAC

  pinMode(weapon, OUTPUT);    //For the weapon pin.

  PS4.begin();    //Don't change the ESP32 bluetooth mac address. Flash the ESP32 mac address via sixaxis pairing tool. https://sixaxispairtool.en.lo4d.com/windowslo4You
  Serial.println("Ready.");
  gear = 0;
}

void loop()
{
  //Speed gear
  if (PS4.Up()) gear = 2;   //Higher speed
  if (PS4.Down()) gear = 0; //Lower speed
  //For moving back
  back = 0;
  back =  (PS4.L2Value());
  back_value = (2048-back*(6+gear));
  //Use for debuging:
    //Serial.println(PS4.L2Value());
    //Serial.println(back_value);
  
  //For moving forward
  forward = 0;
  forward = (PS4.R2Value());
  forward_value = (2048+forward*(6+gear));
  //Use for debuging:
    //Serial.println(PS4.R2Value());
    //Serial.println(forward_value);

  //For going back
  if((PS4.L2Value())>0 && (PS4.R2Value())==0)
  speed.setVoltage(back_value, false);

  //For going forward
  if((PS4.R2Value())>0 && (PS4.L2Value())==0)
  speed.setVoltage(forward_value, false);
  
  //For staying idle
  if((PS4.R2Value())==0 && (PS4.L2Value())==0)
  speed.setVoltage(2048, false);

  //For going forward
  if((PS4.R2Value())>0 && (PS4.L2Value())>0)
  speed.setVoltage(forward_value, false);

  //For steering
  steer = 0;
  steer = (PS4.LStickX());
  if(steer < -10 || steer >10)    //A deadzone for the joystick. You can modify it as you like.
  {
    steer_value = 2048+steer*16;
    direction.setVoltage(steer_value, false);
    //Use for debuging
      //Serial.println(PS4.LStickX());
      //Serial.println(steer);
      //Serial.println(steer_value);
  }
  else
  {
    direction.setVoltage(2048, false);    //Staying idle
  }


  //The code for the weapon. You ca comment it if you won't use it.
  if(PS4.Square())
  digitalWrite(weapon, HIGH);
  else
  digitalWrite(weapon, LOW);
  


  delay(20);

}
