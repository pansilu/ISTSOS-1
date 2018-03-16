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
String GUID_CODE = String(GUID_SLPIOT);


// Dullas Temperature Mesurement
OneWire oneWire(EXTERNAL_TEMP_PIN);
DallasTemperature externalTemp(&oneWire);
DeviceAddress insideThermometer;

// light meter
BH1750 lightMeter;

// Clock module     
unsigned long lastSendTime;   // last send Time
unsigned long lastRTCUpdatedTime;

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
// rain gauge variables
double rain_gauge=0;
volatile unsigned long rain_count=0;
volatile unsigned long rain_last=0; 
double battery_value=0;     // battry value
int loopCount=0;


const char istserver[] = IST_SERVER;
const char isturi[] = POSTREQ;
const char slpserver[] = SERVER;
const char slpuri[] = REQ_STR;
 
void setup() {
  Serial.begin(9600);   // serial monitor for showing 
  while (!Serial){}     // wait for Serial Monitor On
  Serial2.begin(9600);  // serial  for GPRS 
  while (!Serial1){}    // wait for GPRS Monitor
  
  //Run Unit tests
  #ifdef UNIT_CPP
    unitRun();
  #endif
  initialize();

  // initial sending data,
  readSensorValues();
  saveAndSendData();
  lastSendTime = getUnixTime();
}

void loop() {
  // read sensor values onece
  readSensorValues();
  if((getUnixTime() - lastSendTime) >= TIME_RATE * 60){
    Serial.println();
    saveAndSendData();
    lastSendTime = getUnixTime();
  }  

  if(RTC_UPDATE_BY_NTP){
    if((getUnixTime() - lastRTCUpdatedTime) >= RTC_UPDATE_TIME_RATE){
      DateTime tsp = ntpUpdate();
      if(tsp.year()>2017){
        setNTPTime();
      }
      lastRTCUpdatedTime = getUnixTime();
    }
  }
  Serial.print("#");
}

void saveAndSendData(){
  getAvarageSensorValues();
  
  //send logs
  sendLogData();

  String slpiot_request = getSlpiotRequest();
  String istsos_request = getIstsosRequest();

  Serial.println(slpiot_request);
  Serial.println(istsos_request);

  // send IST
  #ifdef ISTSOS
  if(sendRequstMessage(istserver,isturi,istsos_request,1)== SEND_SUCCESS){
    writeFileSD(F("DT_LOG/ISTSOS/"),getFileNameDate(),istsos_request);
  }else{
    writeFileSD(F("MEM_LOG/ISTSOS/"),getFileNameTime() ,istsos_request);
  }
  #endif

  #ifdef SLPIOT
  
  if(sendRequstMessage(slpserver,slpuri,slpiot_request,0)== SEND_SUCCESS){
    writeFileSD(F("DT_LOG/SLPIOT/"),getFileNameDate(),slpiot_request);
  }else{
    writeFileSD(F("MEM_LOG/SLPIOT/"),getFileNameTime(),slpiot_request);
  }
  #endif
    
  clearSensorVariables();
  
}


/*
 * Get Request String
 */

 String getSlpiotRequest(){
   return getRequestString(&ext_humidity,
            &ext_temperature,
            &int_temperature,
            &lux_value,
            &wind_speed,
            &wind_direction,
            &rain_gauge,
            &pressure_value,
            &soilemoisture_value,
            &altitude_value,
            &battery_value,
            SLPIOT_REQUEST,
            getLocalTime(),
            GUID_CODE);
 }

 String getIstsosRequest(){
   return getRequestString(&ext_humidity,
          &ext_temperature,
            &int_temperature,
            &lux_value,
            &wind_speed,
            &wind_direction,
            &rain_gauge,
            &pressure_value,
            &soilemoisture_value,
            &altitude_value,
            &battery_value,
            ISTSOS_REQUEST,
            getGrinichTime(),
            PROCEDURE);
 }

/****************************************************
 * SENSOR Reading functions
 * **************************************************
 */
 
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
    clearLCD();
    // Watch Dog ON
    Watchdog.enable(WATCHDOG_TIME_OUT);
    // read External temperature
    if(EXT_TEMP_ENABLE){
      ext_temperature += readExternalTemperature();
      printValuesOnPanel(F("TM"),ext_temperature/loopCount,String((char)223)+"C");
    }
    Watchdog.reset();

    // read Internal temperature
    if(INT_TEMP_ENABLE){
      int_temperature += readInternalTemperature();
      printValuesOnPanel(F("HT"),int_temperature/loopCount,String((char)223)+"C");
    }
    Watchdog.reset();
    
    // read Internal humidiy
    if(INT_HUM_ENABLE){
      int_humidity += readInternalHumidity();
      printValuesOnPanel(F("HM"),int_humidity/loopCount,"%");
    }
    Watchdog.reset();
    
    // read external humidity
    if(EXT_HUM_ENABLE){
      ext_humidity += readExternalHumidity();
    }
    Watchdog.reset();
    
    // soile mosture value
    if(SM_ENABLE){
      soilemoisture_value += readSoileMoisture();
      printValuesOnPanel(F("SM"),soilemoisture_value/loopCount,"%");
    }
    Watchdog.reset();
    
    // pressure value
    if(PRESSURE_ENABLE){
      pressure_value += readPressure();
      printValuesOnPanel(F("PR"),pressure_value/(loopCount),"kpa");
    }

    Watchdog.reset();
    
    // altitude value
    if(ALTITUDE_ENABLE){
      altitude_value += readAltitude();
      printValuesOnPanel(F("AL"),altitude_value/(loopCount),"m");
    }
    Watchdog.reset();

    // lux value
    if(LUX_ENABLE){
      lux_value += readItensity();
      printValuesOnPanel(F("IN"),lux_value/(loopCount*1000),"klx");
    }
    Watchdog.reset();

    // wind direction
    if(WD_ENABLE){
      wind_direction = readWinDirection();
      printValuesOnPanel(F("WD"),wind_direction,String((char)223));
    }
    Watchdog.reset();

    // wind speed
    if(WS_ENABLE){
      wind_speed += readWindSpeed();
      printValuesOnPanel(F("WS"),wind_speed/(loopCount),"m/s");
    }
    Watchdog.reset();
    
    // rain guarge
    if(RG_ENABLE){
      rain_gauge += readRainGuarge();
      printValuesOnPanel(F("RG"),rain_gauge/(loopCount),"mm");
    }
    Watchdog.reset();

    // get battery voltage
    if(BT_ENABLE){
      battery_value = readBatteryVoltage();
      printValuesOnPanel(F("BT"),battery_value,"V");
    }
    Watchdog.reset();

    // Fan operator
    funcFan();
    Watchdog.reset();
    // station is up
    soundIndicator(0,1);

    Watchdog.disable();
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
  rain_gauge=0;
  rain_count=0;

  loopCount=0;
}

// read external temperature from dullas
double readExternalTemperature(){
  externalTemp.requestTemperatures();
  if(externalTemp.getTempCByIndex(0) <-120)
  {
    if(!isBME280Working()){
      printSystemLog(F("I2C ERROR"),F("BME 280"),BME_I2C_ERROR);
      return 0;
    }

    if(bme280.getHumidity() == 0 ){
      printSystemLog(F("I2C ERROR"),F("BME 280"),BME_I2C_ERROR);
      return 0;
    }
    return bme280.getTemperature(); 
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
    printSystemLog(F("I2C ERROR"),F("BME 280"),BME_I2C_ERROR);
    return 0;
  }

  if(bme280.getHumidity() == 0 ){
    printSystemLog(F("I2C ERROR"),F("BME 280"),BME_I2C_ERROR);
    return 0;
  }
  return bme280.getHumidity();
}

// read soile moisture
double readSoileMoisture(){
  double sm = analogRead(SM_PIN);
  if(sm < 1018){
    sm *= (- 0.0482);
    sm += 49.017;
    return sm;
  }else if(sm < 1018){
    return 100;
  }else{
    return 0;
  }  
}

// read Altitude
double readAltitude(){
    if(!isBME280Working()){
      printSystemLog(F("I2C ERROR"),F("BME 280"),BME_I2C_ERROR);
      return 0;
    }
    
    if(bme280.getPressure() > 118000 ){
      printSystemLog(F("I2C ERROR"),F("BME 280"),BME_I2C_ERROR);
      return 0;
    }
    return bme280.calcAltitude(bme280.getPressure());
}

// read pressure value
double readPressure(){
  if(!isBME280Working()){
      printSystemLog(F("I2C ERROR"),F("BME 280"),BME_I2C_ERROR);
      return 0;
    }
    
    if(bme280.getPressure() > 118000 ){
      printSystemLog(F("I2C ERROR"),F("BME 280"),BME_I2C_ERROR);
      return 0;
    }
    return bme280.getPressure()*0.001; // kpa
}

// read lux value
double readItensity(){
    return lightMeter.readLightLevel();
}

// read battry values
double readBatteryVoltage(){
    return (analogRead(BATT)*16.6f/1023);
    
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

/*
 * SYSTEM INITIALIZATION 
 */
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

    // GPRS
    ServiceBegin();
    delay(1000);

    if(rtc.lostPower()){
        setNTPTime();
    }
       
    // Dullas temperature 
    if(EXT_TEMP_ENABLE){
	    printString(F("INITIALIZING"),F("DS18B20"));
      externalTemp.begin();
      externalTemp.getAddress(insideThermometer, 0);
      externalTemp.setResolution(insideThermometer, 12);
	    printSystemLog(F(SUCCESSFULL),F("DS18B20"));
      delay(1000);
    }
    
    // BME 280 calibration
    if(EXT_HUM_ENABLE || PRESSURE_ENABLE || ALTITUDE_ENABLE){
	    printString(F("INITIALIZING"),F("BME280"));	
      if(!bme280.init()){
        printSystemLog(F(SUCCESS_ERROR),F("BME280"),BME_NOT_INIT);
        is_bme280_working=0;
      }
      else
        printSystemLog(F(SUCCESSFULL),F("BME280"));
        is_bme280_working=1;
      delay(1000);
    }

    // start light meter
    if(LUX_ENABLE){
	    printString(F("INITIALIZING"),F("BH 1750"));
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

    clearSensorVariables();   // initialize all sensor variables 
    printSystemLog(F(SUCCESSFULL),F("SYSTEM INIT"),INIT_DONE);   
    delay(2000);
}



// fan on in the range of temperature high
void funcFan(){
    if((int_temperature/loopCount)>TEMP_UP){
          digitalWrite(FAN_PIN,LOW);
    }

    if((int_temperature/loopCount)<TEMP_DOWN){
        digitalWrite(FAN_PIN,HIGH);  
    }
}

// check the bme 280 initialized at the first place.
uint8_t isBME280Working(){
  return is_bme280_working==1;
}

