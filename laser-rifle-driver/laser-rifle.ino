// program for the gun

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// radio pinout
int CE = 9;
int CSN = 8;
const byte address[6] = "00011";
RF24 radio(CE, CSN);

// laser and trigger pinout
int laser_duration= 250; //ms
int laser_pin = 1;
int trigger_pin = 2;

// led ammo
int led_ammo[5]={3,4,5,6,7};

void setup() {
  //radio setup
  radio.begin();
  radio.openWritingPipe(address);
  radio.stopListening();

  // trigger and laser setup
  pinMode(laser_pin, OUTPUT);
  pinMode(trigger_pin, INPUT_PULLUP);

  // led ammo setup
  for (int i=0; i<4; i++)
    pinMode(led_ammo[i], OUTPUT);
}

void loop() {
  int number_ammo=5;
  
  int value_zero = digitalRead(trigger_pin);
  for (int i = 0; i < number_ammo; i++)
    digitalWrite(led_ammo[i], HIGH);
    
  while (number_ammo>0){
    set_led_amm(number_ammo, led_ammo);
    radio.write(&number_ammo, sizeof(number_ammo));
    if (digitalRead(trigger_pin)!=value_zero)
      number_ammo=bang_bang(laser_pin, laser_duration, number_ammo);
    delay(10);
  }
  
  for (int i=0; i<100; i++){
    radio.write(&number_ammo, sizeof(number_ammo));
    delay(10);
  }
  
  delay(10000);
}

void set_led_amm(int number_ammo, int* led_ammo){
  for (int i = number_ammo; i < 5; i++)
    digitalWrite(led_ammo[i], LOW);
}

int bang_bang (int laser_pin, int laser_duration, int number_ammo){
  digitalWrite(laser_pin, HIGH);
  delay (laser_duration);
  digitalWrite(laser_pin, LOW);
  return number_ammo-1;
}
