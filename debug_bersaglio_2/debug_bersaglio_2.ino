/*
 *  PROGRAM TO READ VALUE AND STUDY THE THRESHOLD
 *  
 *  Use with python program 
 * 
 */

 
// target pinout
int target_pin[5]= {A0, A1, A2, A3, A4};
int threshold_target[5];



void setup() {
  
  Serial.begin(9600);  

  // target setup
  for (int i=0; i<5; i++){
    pinMode(target_pin[i], INPUT_PULLUP); //check it
  }

    
  // threshold calibration
  for (int i=0; i<5; i++){
    Serial.println("Calibration threshold:" + String(i+1));
    threshold_target[i]= calibration_fun(target_pin[i]);
  }


  Serial.println("Threshold: ");
  for (int i=0; i<5; i++)
    Serial.println(threshold_target[i]);
}
  
void loop() {
      
    Serial.println("VALUE READ:");
    for (int i=0; i<5; i++){
      Serial.print(read_value(target_pin[i]));
      Serial.print("\t");
    }
    Serial.println();
}

// Calibration function

int calibration_fun(int target){
  double sum = 0.0;
  int value=0;
  
  for (int i =0; i<50; i++){
    value = analogRead(target);
    sum += value;
    Serial.println(value);
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
