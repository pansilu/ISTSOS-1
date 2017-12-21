//includes
#include "Settings.h"
#include <DallasTemperature.h>
#include <OneWire.h>
#include <dht.h>
#include <BH1750.h> 
#include <RTClib.h>
#include <SD.h>
#include <Seeed_BME280.h>
#include <sim800.h>
#include <Wire.h>
#include "log.h"
#include "Clocks.h"
#include "unit.h"

// Factors
const int MIN_WIND_FACTOR=476;
const int MAX_WIND_FACTOR=780;

// Dullas Temperature Mesurement
OneWire oneWire(EXTERNAL_TEMP_PIN);
DallasTemperature externalTemp(&oneWire);
DeviceAddress insideThermometer;

// light meter
BH1750 lightMeter;

// Clock module     
DateTime now;   // now time

// SIM800
Sim800 server = Sim800(Serial1, APN, USERNAME, PASSWORD);

// dht 11 internal temperature
dht internal_temperature_meter;

// BME 280
BME280 bme280;

// global variables
double ext_temperature=0; // external temperature 
double int_temperature=0; // internal temperature
double int_humidity=0;    // internal humidity
double ext_humidity=0;    // external humidity
double soilemoisture_value=0;// soile mosture 
double pressure_value=0;     // pressure value;
double altitude_value=0;    // altitude value
double lux_value=0;         // inetensity value
double wind_direction=0;    // win direction value
// wind speed 
double wind_speed=0;        // wind speed value
float sensor_voltage=0;
double water_level=0;       // water level
// rain gauge variables
double rain_gauge=0;
volatile unsigned long rain_count=0;
volatile unsigned long rain_last=0; 
double battery_value=0;     // battry value
int loopCount=0;
 
void setup() {
  Serial.begin(9600);   // serial monitor for showing 
  while (!Serial){}     // wait for Serial Monitor On
  Serial1.begin(9600);  // serial  for GPRS 
  while (!Serial1){}    // wait for GPRS Monitor
  
  initialize();

}

void loop() {
  //Run Unit tests
  unitRun();
  
  // read sensor values onece
  readSensorValues(); 

}

void readSensorValues(){
  
    // read External temperature
    ext_temperature += readExternalTemperature();
    printValues(F("EX_T"),ext_temperature);

    // read Internal temperature
    int_temperature += readInternalTemperature();
    printValues(F("IN_T"),int_temperature);

    // read Internal humidiy
    int_humidity += readInternalHumidity();
    printValues(F("IN_H"),int_humidity);

    // read external humidity
    ext_humidity += readExternalHumidity();
    printValues(F("EX_H"),ext_humidity);

    // soile mosture value
    soilemoisture_value += readSoileMoisture();
    printValues(F("SM"),soilemoisture_value);

    // pressure value
    pressure_value += readPressure();
    printValues(F("P"),pressure_value);

    // altitude value
    altitude_value += readAltitude();
    printValues(F("AL"),altitude_value);

    // lux value
    lux_value += readItensity();
    printValues(F("IN"),lux_value);

    // wind direction
    wind_direction = readWinDirection();
    printValues(F("WD"),wind_direction);

    // wind speed
    wind_speed += readWindSpeed();
    printValues(F("WS"),wind_speed);
    
    // rain guarge
    rain_gauge += readRainGuarge();
    printValues(F("RG"),rain_gauge);

    // get battery voltage
    battery_value = readBatteryVoltage();
    printValues(F("BT"),battery_value);
    
    // current time and date
    printValues(F("LCT"),getLocalTime());

    // Fan operator
    funcFan();
    // station is up
    soundIndicator(0,1);

    loopCount++;
}

//clear variables setup to sensor Data
void clearSensorVariables(){
  ext_temperature=0;
  int_temperature=0;
  ext_humidity=0;
  int_humidity=0;
  soilemoisture_value=0;
  pressure_value=0;
  altitude_value=0;
  lux_value=0;
  wind_direction=0;
  wind_speed=0;
  sensor_voltage=0;
  water_level=0;
  rain_gauge=0;
  rain_count=0;
}

// read external temperature from dullas
double readExternalTemperature(){
  externalTemp.requestTemperatures();
  return externalTemp.getTempCByIndex(0);
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

double readExternalHumidity(){
  return bme280.getHumidity();
}

// read soile moisture
double readSoileMoisture(){
  soilemoisture_value = analogRead(SM_PIN);
  if(soilemoisture_value < 1023){
    soilemoisture_value /= 957.35;
    soilemoisture_value = log(soilemoisture_value);
    soilemoisture_value = soilemoisture_value /(-0.029);
    return soilemoisture_value;
  }else{
    return 0;
  }  
}

// read Altitude
double readAltitude(){
    return bme280.calcAltitude(bme280.getPressure());
}

// read pressure value
double readPressure(){
  return bme280.getPressure(); // kpa
}

// read lux value
double readItensity(){
    return lightMeter.readLightLevel();
}

// read battry values
double readBatteryVoltage(){
    return ((analogRead(BATT)*15/1023));
}

// read wind direction
double readWinDirection(){

  sensor_voltage = analogRead(WIN_DIRECTION_PIN) * 0.004882814;  // convert to actual voltage
  if(sensor_voltage <= WIND_DIRECTION_VOLTAGE_MIN && sensor_voltage >= WIND_DIRECTION_VOLTAGE_MAX)
    return 0;
  else{
    return abs(((sensor_voltage-WIND_DIRECTION_VOLTAGE_MIN)* 360 / (WIND_DIRECTION_VOLTAGE_MAX-WIND_DIRECTION_VOLTAGE_MIN)));  // convert it to leaniar relationship
  } 
  
}

// read Wind Speed
double readWindSpeed(){

  sensor_voltage = analogRead(WIN_SPEED_PIN);  // convert to actual voltage
  if(sensor_voltage <= WIND_VOLTAGE_MIN)
    return 0;
  else{
    return abs(((sensor_voltage-WIND_VOLTAGE_MIN)* WIND_FACTOR / (WIND_VOLTAGE_MAX - WIND_VOLTAGE_MIN) ));  // convert it to leaniar relationship
  }
  
}

// read rain guarge
double readRainGuarge(){
  return rain_count * RAIN_FACTOR;
}

void rainGageClick()
{
    long thisTime=micros()-rain_last;
    rain_last=micros();
    if(thisTime>500)
    {
      rain_count++;
    }
}

// initialize componants
void initialize(){
    // one wire intialization
    Wire.begin();
    
    // RTC Initialize
    initRTC();
    
    // Dullas temperature 
    externalTemp.begin();
    externalTemp.begin();
    externalTemp.getAddress(insideThermometer, 0);
    externalTemp.setResolution(insideThermometer, 12);

    // BME 280 calibration
    if(!bme280.init())
      printErrorCode("BME_NOT_INIT",BME_NOT_INIT);

    // start light meter
    lightMeter.begin();  

    // Rain guarge
    pinMode(RAIN_GAUGE_PIN,INPUT);
    digitalWrite(RAIN_GAUGE_PIN,HIGH);  // Turn on the internal Pull Up Resistor
    attachInterrupt(RAIN_GAUGE_INT,rainGageClick,FALLING);

    // turn on interrupts
    interrupts();

    // check and initialize fan
    pinMode(FAN_PIN,OUTPUT);
    digitalWrite(FAN_PIN,HIGH);

    // SD init
    initSD();
    
    // LCD 
    initLCD();

    // GPRS

    clearSensorVariables();   // initialize all sensor variables 
    printStr(F("INIT_DONE"),getLocalTime(),INIT_DONE);    
    delay(2000);
}



// fan on in the range of temperature high
void funcFan(){
    if(int_temperature>TEMP_UP){
          digitalWrite(FAN_PIN,LOW);
    }

    if(int_temperature<TEMP_DOWN){
        digitalWrite(FAN_PIN,HIGH);  
    }
}

