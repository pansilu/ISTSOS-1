//includes
#include <DallasTemperature.h>
#include <Wire.h>
#include <dht.h> 
#include <BH1750.h> 
#include "RTClib.h"

// definitins
#define EXTERNAL_TEMP_PIN 11  // External temperature pin
#define DHT11_IN_PIN 13       // internal temperature
#define BUZZER 12             // buzzer pin
#define SM_PIN 8              //  for SM sensor
#define BMP085_ADDRESS 0x77   // bmp sensor Address  
#define BATT 0                // get battry meter value

// Dullas Temperature Mesurement
OneWire oneWire(EXTERNAL_TEMP_PIN);
DallasTemperature externalTemp(&oneWire);
DeviceAddress insideThermometer;

// light meter
BH1750 lightMeter;

// Clock module
RTC_DS1307 rtc;      
DateTime now;   // now time 
String datetime_;

// log file datetime 
String logfile="log.txt";

// dht 11 internal temperature
dht internal_temperature_meter;

// global variables
double ext_temperature=0; // external temperature 
double int_temperature=0; // internal temperature
double int_humidity=0;    // internal humidity
double ext_humidity=0;    // external humidity
double soilemoisture_value=0;// soile mosture 
double pressure_value=0;     // pressure value;
double altitude_value=0;    // altitude value
double lux_value=0;         // inetensity value
double battery_value=0;     // battry value
 
void setup() {
  Serial.begin(9600);   // serial monitor for showing 
  while (!Serial){}     // wait for Serial Monitor On
  Serial1.begin(9600);  // serial  for GPRS 
  while (!Serial1){}    // wait for GPRS Monitor

  initialize();

  // read Datetime once
  RTCDateTime();
  // read sensor values onece
  readSensorValues();
}

void loop() {
  
}

// read current time value
void RTCDateTime()
{
    now = rtc.now(); 
    datetime_=String(now.year(),DEC);
    datetime_.concat('-');
    datetime_.concat(String(now.month(), DEC));
    datetime_.concat('-');
    datetime_.concat(String(now.day(), DEC));
    datetime_.concat('-');
    datetime_.concat(String(now.hour(), DEC));
    datetime_.concat(':');
    datetime_.concat(String(now.minute(), DEC));
    datetime_.concat(':');
    datetime_.concat(String(now.second(), DEC));

    logfile=String(now.year(),DEC);
    logfile.concat('-');
    logfile.concat(String(now.month(),DEC));
    logfile.concat(".txt");
    
}

void readSensorValues(){
  
    // read External temperature
    ext_temperature = readExternalTemperature();
    printValues("Ext Temperature:",ext_temperature);

    // read Internal temperature
    int_temperature=readInternalTemperature();
    printValues("Int Temperature:",int_temperature);

    // read Internal humidiy
    int_humidity=readInternalHumidity();
    printValues("Int Humidity:",int_humidity);

    // by pass internal humidity to external humidity
    ext_humidity = int_humidity;
    printValues("Ext Humidity:",ext_humidity);

    // soile mosture value
    soilemoisture_value=readSoileMoisture();
    printValues("Soile Moisture:",soilemoisture_value);

    // pressure value
    pressure_value=readPressure();
    printValues("Pressure:",pressure_value);

    // altitude value
    altitude_value = readAltitude();
    printValues("Altitude:",altitude_value);

    // lux value
    lux_value= readItensity();
    printValues("Intensity:",lux_value);

    battery_value=readBatteryVoltage();
    printValues("Battry Value:",battery_value);
    
    // current time and date
    printValues("Time : ",datetime_);
    
    // station is up
    soundIndicator(1);
      
}

// print and show values
void printValues(String name_index,double value){
    Serial.print(name_index);
    Serial.println(value);
}

void printValues(String name_index,String value){
    Serial.print(name_index);
    Serial.println(value);
}

void printError(char *f){
  Serial.println(f); 
}

//==============================

// read external temperature from dullas
double readExternalTemperature(){
  externalTemp.requestTemperatures();
  return externalTemp.getTempC(insideThermometer);
}

//read interna temoerature
double readInternalTemperature(){
  internal_temperature_meter.read11(DHT11_IN_PIN);
  return internal_temperature_meter.temperature;
  
}

double readInternalHumidity(){
  internal_temperature_meter.read11(DHT11_IN_PIN);
  return internal_temperature_meter.humidity;
}

// read soile moisture
double readSoileMoisture(){
  soilemoisture_value = analogRead(SM_PIN);
  if(soilemoisture_value < 1023){
    return soilemoisture_value;
  }else{
    return 0;
  }  
}

// read Altitude
double readAltitude(){
    return (float)44330 * (1 - pow(((float) pressure_value/101325), 0.190295));
}

// read pressure value
double readPressure(){
  return bmp085GetPressure(bmp085ReadUP()); // kpa
}

// read lux value
double readItensity(){
    return lightMeter.readLightLevel();
}

// read battry values
double readBatteryVoltage(){
    return (analogRead(BATT) * 0.0145);
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

    // calibrate BMP180 sensor
    bmp085Calibration();

    // start light meter
    lightMeter.begin();  
    // delay 

    //   clock module initialization
    if (! rtc.begin()) {
      printError("RTC Not Connected ... !");
      while (1);
    }
    
    if (! rtc.isrunning()) {
      printError("RTC Not Running \nSet Time ...!");
      rtc.adjust(DateTime(__DATE__, __TIME__));
      Serial.print("Date : ");
    }
  
    rtc.adjust(DateTime(__DATE__, __TIME__));
    if (! rtc.isrunning()) {
      printError("RTC Not Running ...!");
      setup();
    }
  
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
