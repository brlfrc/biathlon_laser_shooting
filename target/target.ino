// Program for the target
// Here the debug it's just for the radio

#define Debug true

// radio library
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//servo library
#include <Servo.h>

// radio pinout
int CE= 9;
int CSN= 8;
const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
RF24 radio(CE, CSN);
int number_ammo = 0; // this is the data sent in the TX.
bool newData = false;

// target pinout
int target_pin[5]= {A0, A1, A2, A3, A4};
bool target_covered[5];
int threshold_target[5];
int target_hit= 0;

// servo pinout
int const close_angle= -45; //counterclockwise: Positive
int const offset_angle = 90;
int const n_servo= 3;
int servo_attaches[n_servo]= {5,6,3}; // they are PWM pins
Servo myservo[n_servo];



void setup() {
  if (Debug){
    Serial.begin(9600);
    Serial.println("SimpleRx Starting");
  }

  //radio setup
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();

  // target setup
  for (int i=0; i<5; i++){
    pinMode(target_pin[i], INPUT_PULLUP); //check it
    target_covered[i] = false;
  }
    
  // servo setup
  for (int i=0; i<n_servo; i++)
    myservo[i].attach(servo_attaches[i]);
  reset_servo();

  // threshold calibration
  for (int i=0; i<5; i++)
    threshold_target[i]= calibration_fun(target_pin[i]);
}
  
void loop() {
  
  if (radio.available()){
    radio.read(&number_ammo, sizeof(number_ammo));
    newData = true;
  }

  for (int i=0; i<5; i++){
    if (target_covered[i] == false){
      if(analogRead(target_pin[i])> threshold_target[i]){
         close_target (i+1, target_covered);
         target_covered[i]= true;
         target_hit=target_hit+1;
      }
    }
  }
  
  if (target_hit == 5 || number_ammo== 10){
    delay(2000);
    reset_servo();
    target_hit=0;
    number_ammo= 10;
    for (int i=0; i<5; i++){
      target_covered[i] = false;
      threshold_target[i]= calibration_fun(target_pin[i]);
    }
    delay(5000);
  }

  if(Debug == true){
    Serial.print("Radio Debug: New data received? ");
    if (newData)
      Serial.print("Y and value of number of ammo: ");
    else
      Serial.print("N and value of number of ammo: ");
    Serial.println(number_ammo);
  }

  newData = false;
  delay(10);

}

// Servo function

void reset_servo(){
  for (int i=0; i<3; i++)
    myservo[i].write(offset_angle);
}

void close_target (int target, bool* target_covered){
  if (target == 1 || target == 2){
    close_servo_target (0, target, target_covered);
    return;
  }
  if (target == 3 || target == 4){
    close_servo_target (1, target, target_covered);
    return;
  }
  myservo[2].write(close_angle);
  return;
}

void close_servo_target (int servo, int target, bool* target_covered){
  if (target%2== 0){
    if (target_covered[target-2]==true){
      myservo[servo].write(offset_angle+2*close_angle);
      return;
    }
    myservo[servo].write(offset_angle-close_angle);
    return;
  }
  if (target_covered[target]==true){ // this target is target+1-1(array)
    myservo[servo].write(offset_angle-2*close_angle);
    return;
  }
  myservo[servo].write(offset_angle+close_angle);
  return;
}


// Calibration function

int calibration_fun(int target){
  double sum = 0.0;

  for (int i =0; i<20; i++){
    sum += analogRead(target);
    delay(100);
  }
  
  double mean=sum / 20;
    
  //return int(mean +d*mean*mean*mean+a*mean*mean+b*mean+c);
  return mean+40;
}
