
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "Adafruit_MMA8451.h"
#include "Adafruit_Sensor.h"
#include "adc.h"


Adafruit_MMA8451 mma = Adafruit_MMA8451();
MAX30105 particleSensor;

// Variables for Heartrate
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

// Variables for GSR
int sensorPin = A6;
int sensorValue = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  ADC0_Ch6_Init();
  /*
  Serial.println("Initializing MAX30102");
  if(!particleSensor.begin(Wire, I2C_SPEED_FAST)){
    Serial.println("MAX30102 Failed to Initialize");
    while(1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  Serial.println("MAX30102 Initialized");

  Serial.println("Initializing MMA8451");
  if(!mma.begin()){
    Serial.println("MMA8451 not found");
  }
  mma.setRange(MMA8451_RANGE_2_G);
  Serial.println("MMA8451 Initialized");
  */
}

void loop() {

  // Read GSR
  //sensorValue = analogRead(sensorPin);
  sensorValue = ADC_Ch6_Samples(10);

  // Calculate Heart Rate
  /*
  long irValue = particleSensor.getIR();
  if(checkForBeat(irValue) == true){
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if(beatsPerMinute < 255 && beatsPerMinute > 20){
      rates[rateSpot++] = (byte)beatsPerMinute;
      rateSpot %= RATE_SIZE;
      beatAvg = 0;
      for(byte x = 0; x < RATE_SIZE; x++){
        beatAvg += rates[x];
      }
      beatAvg /= RATE_SIZE;
    }
  }

  // Calculate Position
  mma.read();
  sensors_event_t event;
  mma.getEvent(&event);
  uint8_t orientation = mma.getOrientation();

  // Print Heart Rate
  Serial.print("IR = ");
  Serial.print(irValue);
  Serial.print(", Avg BPM=");
  Serial.println(beatAvg);

  // Print Accelerometer
  Serial.print("X: \t"); Serial.print(event.acceleration.x); Serial.print("\t");
  Serial.print("Y: \t"); Serial.print(event.acceleration.y); Serial.print("\t");
  Serial.print("Z: \t"); Serial.print(event.acceleration.z); Serial.print("\t");
  Serial.println("m/s^2 ");
  
  switch(orientation){
    case MMA8451_PL_PUF: 
      Serial.println("Portrait Up Front");
      break;
    case MMA8451_PL_PUB: 
      Serial.println("Portrait Up Back");
      break;    
    case MMA8451_PL_PDF: 
      Serial.println("Portrait Down Front");
      break;
    case MMA8451_PL_PDB: 
      Serial.println("Portrait Down Back");
      break;
    case MMA8451_PL_LRF: 
      Serial.println("Landscape Right Front");
      break;
    case MMA8451_PL_LRB: 
      Serial.println("Landscape Right Back");
      break;
    case MMA8451_PL_LLF: 
      Serial.println("Landscape Left Front");
      break;
    case MMA8451_PL_LLB: 
      Serial.println("Landscape Left Back");
      break;
  }
  */
  Serial.println(sensorValue);
  
}
