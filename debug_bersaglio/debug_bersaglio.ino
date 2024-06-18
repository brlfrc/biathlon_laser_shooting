// Program for the debug of the target, reading values and threshold
/*
 *  First mode: just read the values and check the calibration
 *  
 *  Second mode: force the closure of the target and check if the fuctions are working 
 * 
 */

#define Mode_1 false
#define Mode_2 true

//servo library
#include <Servo.h>
 
// target pinout
int target_pin[5]= {A0, A1, A2, A3, A4};
bool target_covered[5];
int threshold_target[5];
int target_hit = 0;
int target_close = 0;

// servo pinout
int const close_angle= -45; //counterclockwise: Positive
int const offset_angle = 90;
int const n_servo= 3;
int servo_attaches[n_servo]= {5, 6, 3}; // they are PWM pins
Servo myservo[n_servo];



void setup() {
  
  Serial.begin(9600);  

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

  if (Mode_1){
    Serial.println("\033[1mCalibration: \033[0m");
    for (int i=0; i<5; i++){
      Serial.print("Target: ");
      Serial.print(i+1);
      Serial.print("\t");
      Serial.println(threshold_target[i]);
    }
  }
}
  
void loop() {
  
  if (Mode_1){
    Serial.print("Digit the traget: ");
    while(target_hit < 5){
      if (Serial.available() > 0) {
        target_close = Serial.read()-49; // i don't know, but when i digit 1 the machine read 49

        Serial.println(target_close+1);

        
        if (target_covered[target_close] == false){
          close_target (target_close+1, target_covered);
          target_covered[target_close]= true;
          target_hit=target_hit+1;
        }
        
        Serial.println("Target covered:");
        for (int i=0; i<5; i++){
          Serial.print(target_covered[i]);
          Serial.print("\t");   
        }
        Serial.println();
        Serial.print("Digit the traget: ");
      }
    }
    reset_servo();
    target_hit=0;
    for (int i=0; i<5; i++)
        target_covered[i] = false;
  }

  
  if (Mode_2){
    for (int i=0; i<5; i++){
      if (target_covered[i] == false){
        if(read_value(target_pin[i])> threshold_target[i]){
           close_target (i+1, target_covered);
           target_covered[i]= true;
           target_hit=target_hit+1;
        }
      }
    }
    
    Serial.println("\033[1mVALUE READ: \033[0m");
    for (int i=0; i<5; i++){
      Serial.print("Target: ");
      Serial.print(i+1);
      Serial.print("\t value: ");
      Serial.println(read_value(target_pin[i]));
      Serial.print("\t threshold:");
      Serial.println(threshold_target[i]);
    }
        
    
    if (target_hit == 5){
      delay(2000);
      reset_servo();
      target_hit=0;
      for (int i=0; i<5; i++){
        target_covered[i] = false;
        threshold_target[i]= calibration_fun(target_pin[i]);
      }
    }
     delay(500);
  }
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
  myservo[2].write(30);
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

  for (int i =0; i<50; i++){
    sum += analogRead(target);
    delay(5);
  }
  
  double mean=sum / 50;
    
  return mean*(0.08/500*(800- mean)+1.02);
}


// read a value register for a point making a mean of 5 value, to reduce the noise activation

int read_value(int target_pin){
  double sum = 0.0;

  for (int i =0; i<5; i++){
    sum += analogRead(target_pin);
  }
      
  return sum / 5;
}
