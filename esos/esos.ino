//includes
#include <DallasTemperature.h>
#include <Wire.h>

// definitins
#define EXTERNAL_TEMP_PIN 11  // External temperature pin
#define BUZZER 12             // buzzer pin

// Dullas Temperature Mesurement
OneWire oneWire(EXTERNAL_TEMP_PIN);
DallasTemperature externalTemp(&oneWire);
DeviceAddress insideThermometer;

// global variables
double ext_temperature=0; // external temperature 

void setup() {
  Serial.begin(9600);   // serial monitor for showing 
  while (!Serial){}     // wait for Serial Monitor On
  Serial1.begin(9600);  // serial  for GPRS 
  while (!Serial1){}    // wait for GPRS Monitor

  initialize();

  // read sensor values onece
  readSensorValues();
}

void loop() {
  
}

void readSensorValues(){
      

      // read External temperature
      ext_temperature = readExternalTemperature();
      printValues("Ext Temperature:",ext_temperature);


      
      // station is up
      soundIndicator(1);
      
}

void printValues(String name_index,double value){
    Serial.print(name_index);
    Serial.println(value);
}

// read external temperature from dullas
double readExternalTemperature(){
  externalTemp.requestTemperatures();
  return externalTemp.getTempC(insideThermometer);
}

// initialize componants
void initialize(){
    // one wire intialization
    Wire.begin();
    
    // tone startup // 2 beeps
    soundIndicator(2);
    // Dullas temperature 
    externalTemp.begin();
    // get device count
    Serial.print("Dullas Count: ");
    Serial.print(externalTemp.getDeviceCount(), DEC);
    if (externalTemp.isParasitePowerMode()) // Parasite power
      Serial.println("Parasite power is : ON");
    else 
      Serial.println("Parasite power is : OFF");
      
    if (!externalTemp.getAddress(insideThermometer, 0))   // get address of the external temperature device
      Serial.println("Unable to find address for Device 0");
    
    externalTemp.setResolution(insideThermometer, 9);     // set the resolution

    // delay 
    delay(1000);
}

// tone genarator
void soundIndicator(int count){ // 1KHz 100ms out 1
  for(int i=0;i<count;i++){
    tone(BUZZER,1000);
    delay(100);
    noTone(BUZZER);
    delay(10);
  }
}
