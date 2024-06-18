// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


// radio pinout
int CE = 9;
int CSN = 8;
const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};


RF24 radio(CE, CSN);

char dataToSend[10] = "Message 8";
char txNum = '0';

int number_ammo = 10; // this is the data sent in the TX.



unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 500; // send once per second


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
int selection_pin = 13;
int selection_zero;


void setup() {

  Serial.begin(9600);

  Serial.println("SimpleTx Starting");

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
  
}

//====================

void loop() {
  currentMillis = millis();
  if (currentMillis - prevMillis >= txIntervalMillis) {
    send();
    prevMillis = millis();
  }
}

//====================

void send() {

  bool rslt;
  rslt = radio.write( &number_ammo, sizeof(number_ammo) );
  // Always use sizeof() as it gives the size as the number of bytes.
  // For example if dataToSend was an int sizeof() would correctly return 2
  Serial.print("Data Sent ");
  Serial.print(number_ammo);
  if (rslt) {
    Serial.println("  Acknowledge received");
    updateMessage();
  }
  else {
    Serial.println("  Tx failed");
  }
}

//================

void updateMessage() {
  // so you can see that new data is being sent
  number_ammo= number_ammo+1;
}
