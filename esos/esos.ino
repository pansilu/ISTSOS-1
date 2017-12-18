//includes
#include "Settings.h"
#include <DallasTemperature.h>
#include <OneWire.h>
#include <dht.h>
#include <BH1750.h> 
#include <RTClib.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include <Seeed_BME280.h>
#include <sim800.h>
#include <Wire.h>
#include "log.h"

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
RTC_DS1307 rtc;      
DateTime now;   // now time 
String grinichDateTime,localDateTime;;
byte l_hour=0,l_minute=0; // to taken time differece of TIME_RATE defined time rate
int returnCount =0;

// saving log file
File myFile;
int SDOK=0;
const int chipSelect = 53;  // chip select pin for the SD module.it should be connected to 53 of module

// SIM800
Sim800 istsos = Sim800(Serial1, APN, USERNAME, PASSWORD);

// log file datetime 
String logfile="log.txt";
String logger="logger.txt";

// GSM power up pin
String sp;

// water level calc
String temp;

// dht 11 internal temperature
dht internal_temperature_meter;

// BME 280
BME280 bme280;

//LCD
LiquidCrystal lcd(LCD_RS,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7);

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
 
void setup() {
  Serial.begin(9600);   // serial monitor for showing 
  while (!Serial){}     // wait for Serial Monitor On
  Serial1.begin(9600);  // serial  for GPRS 
  while (!Serial1){}    // wait for GPRS Monitor

  printStr("Station SL");
  
  initialize();

  // ====  initialy send data
  // read Datetime once
  RTCDateTime();
  // read sensor values onece
  readSensorValues();
  sendData();
}

void loop() {

  // read Datetime once
  RTCDateTime();
  // read sensor values onece
  readSensorValues(); 

  
  if(l_hour==now.hour()){
    if((now.minute()-l_minute)==TIME_RATE){
        sendData();
    }
  }else{
    if(((60+now.minute())-l_minute)==TIME_RATE){
        sendData();
    }  
  }
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
    writeLogFile();     // write data to log
  }
  RTCDateTime();
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

// read current time value
void RTCDateTime()
{
    // Time genaration for ISTSOS
    now = rtc.now(); 
    now =now - TimeSpan(0, 5, 30, 00);
    
    grinichDateTime=String(now.year(),DEC);
    grinichDateTime.concat('-');
    if(now.month()<10)
      grinichDateTime.concat("0");
    grinichDateTime.concat(String(now.month(), DEC));
    grinichDateTime.concat('-');
    if(now.day()<10)
      grinichDateTime.concat("0");
    grinichDateTime.concat(String(now.day(), DEC));
    grinichDateTime.concat('T');
    if(now.hour()<10)
      grinichDateTime.concat("0");
    grinichDateTime.concat(String(now.hour(), DEC));
    grinichDateTime.concat(':');
    int miniute=now.minute()<2?59:now.minute();
     if(miniute<10)
      grinichDateTime.concat("0");
    grinichDateTime.concat(String(miniute, DEC));
    grinichDateTime.concat(':');
     if(now.second()<10)
      grinichDateTime.concat("0");
    grinichDateTime.concat(String(now.second(), DEC));
    grinichDateTime.concat("+0000");

   // current datetime
    now = rtc.now();
    localDateTime=String(now.year(),DEC);
    localDateTime.concat('-');
    localDateTime.concat(String(now.month(), DEC));
    localDateTime.concat('-');
    localDateTime.concat(String(now.day(), DEC));
    localDateTime.concat('-');
    localDateTime.concat(String(now.hour(), DEC));
    localDateTime.concat(':');
    localDateTime.concat(String(now.minute(), DEC));
    localDateTime.concat(':');
    localDateTime.concat(String(now.second(), DEC));

    logfile=String(now.year(),DEC);
    logfile.concat('-');
    logfile.concat(String(now.month(),DEC));
    logfile.concat(".txt");
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
    ext_temperature = readExternalTemperature();
    printValues("Ext Temperature:",ext_temperature);

    // read Internal temperature
    int_temperature=readInternalTemperature();
    printValues("Int Temperature:",int_temperature);

    // read Internal humidiy
    int_humidity=readInternalHumidity();
    printValues("Int Humidity:",int_humidity);

    // read external humidity
    ext_humidity = readExternalHumidity();
    printValues("Ext Humidity:",ext_humidity);

    // soile mosture value
    soilemoisture_value=readSoileMoisture();
    printValues("Soil Moisture:",soilemoisture_value);

    // pressure value
    pressure_value=readPressure();
    printValues("Pressure:",pressure_value);

    // altitude value
    altitude_value = readAltitude();
    printValues("Altitude:",altitude_value);

    // lux value
    lux_value= readItensity();
    printValues("Intensity:",lux_value);

    // wind direction
    wind_direction=readWinDirection();
    printValues("Win Direction:",wind_direction);

    // wind speed
    wind_speed=readWindSpeed();
    printValues("Win Speed:",wind_speed);
    
    // rain guarge
    rain_gauge=readRainGuarge();
    printValues("Rain Gauge:",rain_gauge);

    // get battery voltage
    battery_value=readBatteryVoltage();
    printValues("Battry Value:",battery_value);
    
    
    // current time and date
    printValues("Gr Time : ",grinichDateTime);
    printValues("Lc Time : ",localDateTime);

    // Fan operator
    funcFan();
    // station is up
    soundIndicator(1);

}

// print and show values
void printValues(String name_index,double value){
    Serial.print(name_index);
    Serial.println(value);
    lcd.clear();
    printLCDN(name_index,0,0);
    printLCD(value,0,1);
    delay(1000);
}

void printValues(String name_index,String value){
    Serial.print(name_index);
    Serial.println(value);
    lcd.clear();
    printLCDN(name_index,0,0);
    printLCDN(value,0,1);
    delay(1000);
}

void printError(String f){
  Serial.println(f); 
  lcd.clear();
  printLCDN(f,0,0);
  delay(1000);
}

void printStr(String name_index){
    lcd.clear();
    Serial.print(name_index);
    printLCDN(name_index,0,0);
}

//==============================

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


//write to log file
void writeLogFile(){
    createFileSD(logfile);
    writeFileSD(logfile);  
}

// initialize componants
void initialize(){
    // one wire intialization
    Wire.begin();
    
    // tone startup // 2 beeps
    soundIndicator(2);
    
    // LCD 
    lcd.begin(16, 2);
    
    // Dullas temperature 
    externalTemp.begin();
    externalTemp.begin();
    externalTemp.getAddress(insideThermometer, 0);
    externalTemp.setResolution(insideThermometer, 12);

    // BME 280 calibration
    if(!bme280.init()){
      printError("BME is not Working");
      soundIndicator(1,0);
    }

    // start light meter
    lightMeter.begin();  
    // delay 

    // win speed // wind componant
    pinMode(RAIN_GAUGE_PIN,INPUT);
    digitalWrite(RAIN_GAUGE_PIN,HIGH);  // Turn on the internal Pull Up Resistor
    attachInterrupt(RAIN_GAUGE_INT,rainGageClick,FALLING);

    // turn on interrupts
    interrupts();

    // check and initialize fan
    pinMode(FAN_PIN,OUTPUT);
    digitalWrite(FAN_PIN,HIGH);

    if(SDOK==0){
    if (!SD.begin(chipSelect)) 
    {
      printError("SD Error ... !");
      soundIndicator(2,0);
      setup();
    }
    else
    {
      SDOK=1;
      Serial.println("SD Initializes.");
    }  
    }
    // LCD 

    printStr("Initialize GPRS");
    // setup GPRS
    
    while(setupGPRS()==-1){
      printError("\nGPRS ERROR");  
      soundIndicator(3,0);
    };

    printStr("Initialize RTC");
    //   clock module initialization
    if (! rtc.begin()) {
      printError("RTC Not Connected ... !");
      soundIndicator(4,0);
      setup();
    }
    
    
    if (! rtc.isrunning()) {
      printError("RTC Not Running ...!");
      soundIndicator(4,1);
      setup();
    }
    
    
    delay(1000);
}

// file extention
void createFileSD(String fileName)
{
  if (SD.exists(fileName)) 
  {
    Serial.println(fileName+" exists.");
  }
  else 
  {
    Serial.println("Creating "+fileName+"...");
    myFile = SD.open(fileName, FILE_WRITE);
    myFile.close();
 
    if (SD.exists(fileName)) 
    {
      Serial.println(fileName+" create success!");
    }
    else 
    {
      Serial.println(fileName+" create failed!");  
    }
  }
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

void writeFileSD(String fileName)
{


  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) 
  {
Serial.println("Writing to "+fileName+ "...");
    myFile.println("");
    myFile.print(grinichDateTime);
    myFile.println(":{");
    myFile.print("HUMIDITY");
    myFile.print(ext_humidity);
    myFile.print("| ");
    myFile.print("EXT_TEMP");
    myFile.print(ext_temperature);
    myFile.print("| ");
    myFile.print("INT_TEMP");
    myFile.print(int_temperature);
    myFile.print("| ");
    myFile.print("LUX");
    myFile.print(lux_value);
    myFile.print("| ");
    myFile.print("SM");
    myFile.print(soilemoisture_value);
    myFile.print("| ");
    myFile.print("PRESSURE");
    myFile.print(pressure_value);
    myFile.print("| ");
    myFile.print("WIN_SPEED");
    myFile.print(wind_speed);
    myFile.print("| ");
    myFile.print("WIN_DIR");
    myFile.print(wind_direction);
    myFile.print("| ");
    myFile.print("RAIN_GAUGE");
    myFile.print(rain_gauge);
    myFile.print("| ");
    myFile.print("WATER_LEVEL");
    myFile.print(water_level);
    myFile.print("| ");
    myFile.print("BATT");
    myFile.println(battery_value);
    myFile.println("}");
    myFile.close();
    Serial.println("File Saved.");
  } 
  else 
  {
    Serial.println("error opening "+fileName);
  }
}
// end of extention

// tone genarator
void soundIndicator(int count){ // 1KHz 100ms out 1
  for(int i=0;i<count;i++){
    tone(BUZZER,1000);
    delay(100);
    noTone(BUZZER);
    delay(10);
  }
}

void soundIndicator(int count1,int count2){ // 1KHz 100ms out 1
  for(int i=0;i<count1;i++){
    tone(BUZZER,500);
    delay(100);
    noTone(BUZZER);
    delay(10);
  }

   for(int i=0;i<count2;i++){
    tone(BUZZER,1000);
    delay(100);
    noTone(BUZZER);
    delay(10);
  }
}

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
  rain_count=0;             // set rain count value in to zero
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
  Serial1.print(localDateTime);
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

// Log on LCD 
void printLCD(double val,int i,int j){
  String s = String(val,2);  
  lcd.setCursor(i,j);
  lcd.print(s);
}

void printLCDN(char *f,int i,int j){
    lcd.setCursor(i,j);
    lcd.print(f);
}

void printLCDN(String f,int i,int j){
    lcd.setCursor(i,j);
    lcd.print(f);
}

void printLCD(char *f){
    lcd.clear();
    lcd.print(f);
}