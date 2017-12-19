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
String grinichDateTime;;
byte l_hour=0,l_minute=0; // to taken time differece of TIME_RATE defined time rate
int returnCount =0;

// SIM800
Sim800 istsos = Sim800(Serial1, APN, USERNAME, PASSWORD);

// GSM power up pin
String sp;

// water level calc
String temp;

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

  // read sensor values onece
  readSensorValues(); 

  
}

// send Data To server
void sendData(){
 
  int check_gprs =sendAsGPRS();
  if(check_gprs != 0){      // id GPRS data false to send it tryis 5 times; then print the communication Error
    for(int i=0;i<5;i++){
        check_gprs=sendAsGPRS();
        if(check_gprs ==0)
          break;
    }
    printError("GPRS IS NOT CONNECTED TO SERVER");
  }
  l_hour=now.hour();
  l_minute=now.minute();
  returnCount ++;
}

int sendGPRSDataAsGET(){
  sendGPRSData();
  return ShowSerialData('K');
}

int sendAsGPRS(){

  
  if(SERVER_SETUP==0){
    sendGPRSDataAsGET();
  }
}


// // update RTC time from ntp time server
// void updateRTC(){
//   uint32_t* timepointer=ntpUpdateTime();

//   // init datetime componants

//  int yyyy =  timepointer[0];
//  int MM   =  timepointer[1];
//  int dd   =  timepointer[2];
//  int hh   =  timepointer[3];
//  int mm   =  timepointer[4];
//  int ss   =  timepointer[5];
 
//  DateTime ds(yyyy,MM,dd,hh,mm,ss);
//  ds =ds + TimeSpan(0, 5, 30, 00);
//  rtc.adjust(ds);

//  delay(3000);
//  // Read RTC
//  RTCDateTime();

//  printValues("Gr Time : ",grinichDateTime);
//  printValues("Lc Time : ",localDateTime);
 
// }

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


    printStr(F("Initialization Completed"));    
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


// end of extention


// =================   GPRS SETUP FOR GET STATEMENT ==============

int setupGPRS(){
  int check_gprs;  // check each module is OK;
  Serial.println("Check The GPRS module : ");
  
  Serial1.print("AT\r"); 
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    soundIndicator(3,1);
    return -1;
  
  }
    
  // check pin reset happened : unlocked
  Serial1.print("AT+CPIN?\r");
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    soundIndicator(3,2);
    return -1;
  
  }
    
  // check sim registered
  Serial1.print("AT+CREG?\r"); 
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    soundIndicator(3,3);
    return -1;
  
  }
    
  //check GPRS attached :
  
  Serial1.print("AT+CGATT?\r"); 
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    soundIndicator(3,4);
    return -1;
  
  }  
  // Reset the IP session if any
  Serial1.print("AT+CIPSHUT\r");
  delay(2000);
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    soundIndicator(3,5);
    return -1;
  
  }
    
 //Check if the IP stack is initialized
  Serial1.print("AT+CIPSTATUS\r");
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    soundIndicator(3,6);
    return -1;
  
  }
    
 // To keep things simple, I’m setting up a single connection mode
  Serial1.print("AT+CIPMUX=0\r"); 
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    soundIndicator(3,7);
    return -1;
  
  }
    
  // Start the task, based on the SIM card you are using, you need to know the APN, username and password for your service provider
  Serial1.print("AT+CSTT= \"mobitel\", \"\", \"\"\r"); 
  delay(1000);
  if(check_gprs == -1){
    soundIndicator(3,8);
    return -1;
  
  }
    
  // Now bring up the wireless. Please note, the response to this might take some time
  Serial1.print("AT+CIICR\r");
  delay(2000);
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    soundIndicator(3,9);
    return -1;
  
  }
    
  //Get the local IP address. Some people say that this step is not required, but if I do not issue this, it was not working for my case. So I made this mandatory, no harm.
  Serial1.print(" AT+CIFSR\r");  
  delay(2000);
  check_gprs = ShowSerialData('N');
  if(check_gprs == -1){
    soundIndicator(3,10);
    return -1;
  
  }
  delay(1000);
  return 0;
}

// visualize Serial Data
int ShowSerialData(char c){  
 delay(1000);
 char retval;
 sp="";
 while(Serial1.available()!=0) {
  retval=Serial1.read(); 
  sp += retval;
  Serial.write(retval);
  if(retval == c)
    return 0;  
 }
 if(c== 'N')
   return 0;
 else
  return -1;
}

uint8_t waitResponse(uint32_t timeout, const String expected)
{

    unsigned long start = millis();

    String response = String("");
    bool status = false;
    bool check = false;

    if (expected != "")
        check = true;

    do
    {
        while(Serial1.available() > 0)
        {
            char c = Serial1.read();
            response += c;

            if(check && response.endsWith(expected))
            {
                status = true;
                goto finish;
            }
            if(response.endsWith("OK"))
            {
                status = true;
                goto finish;
            }
            else if(response.endsWith("ERROR"))
            {
                goto finish;
            }
        }
    }while( millis() - start < timeout);

  finish:

    Serial1.flush();

    return status;
}



// get data sender
void sendGPRSData(){
  printStr("Send SLPIOT");
  // Start the connection, TCP, domain name, port 
  Serial1.println("AT+CIPSTART=\"TCP\",\"slpiot.org\",80");
  delay(1000);
  ShowSerialData('N');

  // Random Data
  Serial1.print("AT+CIPSEND\r\n"); 
  ShowSerialData('N');

 //send the request
  Serial1.print("GET /insert_data.php?");
  Serial1.print("H=");
  Serial1.print(ext_humidity);
  Serial1.print("&TE=");
  Serial1.print(ext_temperature);// ext_temperature
  Serial1.print("&L=");
  Serial1.print(lux_value);
  Serial1.print("&TI=");
  Serial1.print(int_temperature-2);
  Serial1.print("&WS=");
  Serial1.print(wind_speed);
  Serial1.print("&WD=");
  Serial1.print(wind_direction);
  Serial1.print("&RG=");
  Serial1.print(rain_gauge);         
  Serial1.print("&P=");
  Serial1.print(pressure_value);
  Serial1.print("&SM=");
  Serial1.print(soilemoisture_value);
  Serial1.print("&WL=");
  Serial1.print(water_level);
  Serial1.print("&AT=");
  Serial1.print(altitude_value);
  Serial1.print("&BV=");
  Serial1.print(battery_value); 
  Serial1.print("&dt=");
  //Serial1.print(localDateTime);
  Serial1.print("&GUID=");
  Serial1.print("5bf82c59-7ec0-4f");
  Serial1.print(" HTTP/1.1\r\nHost: www.slpiot.org\r\nConnection:keep-alive\r\n\r\n");
  ShowSerialData('N');
  
  // Random Data
  Serial1.write(0x1A);
  ShowSerialData('N');

  delay(1000);
  Serial1.print("AT+CIPSHUT\r\n"); 
  delay(2000);
  ShowSerialData('N');
  
  printStr("Data Sent");
  
}

