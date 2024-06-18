// program for the gun

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// radio pinout
int CE = 9;
int CSN = 8;
const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};

RF24 radio(CE, CSN);

// laser and trigger pinout
int laser_duration= 250; //ms
int laser_pin = 2;
int trigger_pin = 3  ;
int value_zero;

// led ammo
int led_ammo[5]={A0, 5, 6, 7, 10};

// noise
int noise_pin = 4;

// mode selection
int selection_pin = A1;
int selection_zero;

void setup() {
  //radio setup
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3, 5); // delay, count
  radio.openWritingPipe(slaveAddress);

  // trigger, noise and laser setup
  pinMode(noise_pin, OUTPUT);
  pinMode(laser_pin, OUTPUT);
  pinMode(trigger_pin, INPUT_PULLUP);
  value_zero = digitalRead(trigger_pin);

  // mode selection
  pinMode(selection_pin, INPUT_PULLUP);
  selection_zero= digitalRead(selection_pin);

  // led ammo setup
  for (int i=0; i<5; i++)
    pinMode(led_ammo[i], OUTPUT);
    
  Serial.begin(9600);

}


void loop() {
  int number_ammo=5;
  int current_selection = digitalRead(selection_pin);
  

  // Race condition
  if (current_selection != selection_zero){
    for (int i = 0; i < number_ammo; i++)
      digitalWrite(led_ammo[i], HIGH);
      
    while (number_ammo>0){
      if (digitalRead(trigger_pin)!=value_zero){
          number_ammo = bang_bang(laser_pin, laser_duration, number_ammo);
          set_led_amm(number_ammo, led_ammo);
          delay(40);
      }
      radio.write(&number_ammo, sizeof(number_ammo));
      delay(10);
    }
    number_ammo=10;
    for (int i=0; i<100; i++){
      radio.write(&number_ammo, sizeof(number_ammo));
      delay(10);
    }
    delay(10000);
  }

  // Easy condition
  else{
    for (int i = 0; i < number_ammo; i++)
      digitalWrite(led_ammo[i], HIGH);
      
    while (number_ammo>0){
      if (digitalRead(trigger_pin)!=value_zero){
            number_ammo = bang_bang(laser_pin, laser_duration, number_ammo);
            set_led_amm(number_ammo, led_ammo);
            delay(40);
        }
        radio.write(&number_ammo, sizeof(number_ammo));
        delay(10);
    }
    number_ammo=10;
    for (int i=0; i<100; i++){
      radio.write(&number_ammo, sizeof(number_ammo));
      delay(10);
    }
    delay(10);
  }
}


int bang_bang (int laser_pin, int laser_duration, int number_ammo){
  digitalWrite(laser_pin, HIGH);
  digitalWrite(noise_pin, HIGH);
  delay (laser_duration);
  digitalWrite(noise_pin, LOW);
  digitalWrite(laser_pin, LOW);
  return number_ammo-1;
}


void set_led_amm(int number_ammo, int* led_ammo){
  for (int i = number_ammo; i < 5; i++)
    digitalWrite(led_ammo[i], LOW);
}
