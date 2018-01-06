//includes
#include "Settings.h"
#include <DallasTemperature.h>
#include <OneWire.h>
#include <dht.h>
#include <BH1750.h> 
#include <RTClib.h>
#include <SD.h>
#include <Seeed_BME280.h>
#include <Wire.h>
#include "log.h"
#include "Clocks.h"
#include "Service.h"
//#include "unit.h"

// Factors
const int MIN_WIND_FACTOR=476;
const int MAX_WIND_FACTOR=780;

// Procedure 
String GUID_CODE = String("07ec7356-9b13-48");


// Dullas Temperature Mesurement
OneWire oneWire(EXTERNAL_TEMP_PIN);
DallasTemperature externalTemp(&oneWire);
DeviceAddress insideThermometer;

// light meter
BH1750 lightMeter;

// Clock module     
unsigned long lastSendTime;   // last send Time
String curruntDatetimeStr;

// dht 11 internal temperature
dht internal_temperature_meter;

// BME 280
BME280 bme280;
uint8_t is_bme280_working=0;

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
  
  //Run Unit tests
  #ifdef UNIT_CPP
    unitRun();
  #endif

  initialize();
  // initial sending data,
  readSensorValues();
  getAvarageSensorValues();
  sendData();
}

void loop() {
  // read sensor values onece
  readSensorValues();
  if((getUnixTime() - lastSendTime) > TIME_RATE * 60){
    getAvarageSensorValues();
    sendData();
  }  
}

void sendData(){
  printStr("Sending Data");
  uint8_t temp;
  
  
  
  if(ENABLE_ISTSOS){
    curruntDatetimeStr = getGrinichTime();
    String sr =  String(PROCEDURE);
    temp= executeRequest(&ext_humidity,
          &ext_temperature,
            &int_temperature,
            &lux_value,
            &wind_speed,
            &wind_direction,
            &rain_gauge,
            &pressure_value,
            &soilemoisture_value,
            &water_level,
            &altitude_value,
            &battery_value,
            POST_REQUEST,
            curruntDatetimeStr,
            sr);
  }
  delay(5000);
  if(ENABLE_SLPIOT){
    curruntDatetimeStr = getLocalTime();
    temp= executeRequest(&ext_humidity,
          &ext_temperature,
            &int_temperature,
            &lux_value,
            &wind_speed,
            &wind_direction,
            &rain_gauge,
            &pressure_value,
            &soilemoisture_value,
            &water_level,
            &altitude_value,
            &battery_value,
            GET_REQUEST,
            curruntDatetimeStr,
            GUID_CODE);
  }
  
  clearSensorVariables();
  lastSendTime = getUnixTime();

}

void getAvarageSensorValues(){
  ext_temperature /= loopCount;
  int_temperature /= loopCount;
  int_humidity /= loopCount;
  ext_humidity /= loopCount;
  soilemoisture_value /= loopCount;
  pressure_value /= loopCount;
  pressure_value *= 1000;
  altitude_value /= loopCount;
  lux_value /= loopCount;
  wind_speed /= loopCount;
  rain_gauge /= loopCount;
  battery_value /= loopCount;
}

void readSensorValues(){
    loopCount++;
    // read External temperature
    if(EXT_TEMP_ENABLE){
      ext_temperature += readExternalTemperature();
      printValues(F("EX_T"),getLocalTime(),ext_temperature/loopCount);
    }

    // read Internal temperature
    if(INT_TEMP_ENABLE){
      int_temperature += readInternalTemperature();
      printValues(F("IN_T"),getLocalTime(),int_temperature/loopCount);
    }

    // read Internal humidiy
    if(INT_HUM_ENABLE){
      int_humidity += readInternalHumidity();
      printValues(F("IN_H"),getLocalTime(),int_humidity/loopCount);
    }

    // read external humidity
    if(EXT_HUM_ENABLE){
      ext_humidity += readExternalHumidity();
      printValues(F("EX_H"),getLocalTime(),ext_humidity/loopCount);
    }

    // soile mosture value
    if(SM_ENABLE){
      soilemoisture_value += readSoileMoisture();
      printValues(F("SM"),getLocalTime(),soilemoisture_value/loopCount);
    }

    // pressure value
    if(PRESSURE_ENABLE){
      pressure_value += readPressure();
      printValues(F("P"),getLocalTime(),pressure_value/loopCount);
    }

    // altitude value
    if(ALTITUDE_ENABLE){
      altitude_value += readAltitude();
      printValues(F("AL"),getLocalTime(),altitude_value/loopCount);
    }

    // lux value
    if(LUX_ENABLE){
      lux_value += readItensity();
      printValues(F("IN"),getLocalTime(),lux_value/loopCount);
    }

    // wind direction
    if(WD_ENABLE){
      wind_direction = readWinDirection();
      printValues(F("WD"),getLocalTime(),wind_direction);
    }

    // wind speed
    if(WS_ENABLE){
      wind_speed += readWindSpeed();
      printValues(F("WS"),getLocalTime(),wind_speed/loopCount);
    }
    
    // rain guarge
    if(RG_ENABLE){
      rain_gauge += readRainGuarge();
      printValues(F("RG"),getLocalTime(),(rain_gauge/loopCount));
    }

    // get battery voltage
    if(BT_ENABLE){
      battery_value = readBatteryVoltage();
      printValues(F("BT"),getLocalTime(),battery_value);
    }

    // Fan operator
    funcFan();
    // station is up
    soundIndicator(0,1);

    
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

  loopCount=0;
}

// read external temperature from dullas
double readExternalTemperature(){
  externalTemp.requestTemperatures();
  if(externalTemp.getTempCByIndex(0) <-120)
  {
    printErrorCode("DS18B20_ERROR",getLocalTime(),DS18B20_ERROR);
    return 0;  
  }
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
  if(!isBME280Working()){
    printErrorCode("BME_I2C_ERROR",getLocalTime(),BME_I2C_ERROR);
    return 0;
  }

  if(bme280.getHumidity() == 0 ){
    printErrorCode("BME_I2C_ERROR",getLocalTime(),BME_I2C_ERROR);
    return 0;
  }
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
    if(!isBME280Working()){
      printErrorCode("BME_I2C_ERROR",getLocalTime(),BME_I2C_ERROR);
      return 0;
    }
    
    if(bme280.getPressure() > 118000 ){
      printErrorCode("BME_I2C_ERROR",getLocalTime(),BME_I2C_ERROR);\
      return 0;
    }
    return bme280.calcAltitude(bme280.getPressure());
}

// read pressure value
double readPressure(){
  if(!isBME280Working()){
      printErrorCode("BME_I2C_ERROR",getLocalTime(),BME_I2C_ERROR);
      return 0;
    }
    
    if(bme280.getPressure() > 118000 ){
      printErrorCode("BME_I2C_ERROR",getLocalTime(),BME_I2C_ERROR);
      return 0;
    }
  return bme280.getPressure()*0.001; // kpa
}

// read lux value
double readItensity(){
    if(lightMeter.readLightLevel()>54000){
      
      printErrorCode("LIGHT_I2C_ERROR",getLocalTime(),LIGHT_I2C_ERROR);
      return 0;
    }
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
    // LCD 
    initLCD();
    
    // RTC Initialize
    initRTC();
    delay(300);
    // SD init
    initSD();
    delay(300);
    // Dullas temperature 
    if(EXT_TEMP_ENABLE){
      externalTemp.begin();
      externalTemp.getAddress(insideThermometer, 0);
      externalTemp.setResolution(insideThermometer, 12);
    }
    
    // BME 280 calibration
    if(EXT_HUM_ENABLE || PRESSURE_ENABLE || ALTITUDE_ENABLE){
      if(!bme280.init()){
        printErrorCode("BME_NOT_INIT",getLocalTime(),BME_NOT_INIT);
        is_bme280_working=0;
      }
      else
        printStr(F("BMP OK"),getLocalTime(),INIT_DONE);
        is_bme280_working=1;
    }

    // start light meter
    if(LUX_ENABLE){
      lightMeter.begin(); 
    } 

    // Rain guarge
    if(RG_ENABLE){
      pinMode(RAIN_GAUGE_PIN,INPUT);
      digitalWrite(RAIN_GAUGE_PIN,HIGH);  // Turn on the internal Pull Up Resistor
      attachInterrupt(RAIN_GAUGE_INT,rainGageClick,FALLING);
    }

    // turn on interrupts
    interrupts();

    // check and initialize fan
    pinMode(FAN_PIN,OUTPUT);
    digitalWrite(FAN_PIN,HIGH);

    // GPRS
    ServiceBegin();

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

// check the bme 280 initialized at the first place.
uint8_t isBME280Working(){
  return is_bme280_working==1;
}

