//includes
#include <DallasTemperature.h>
#include <Wire.h>
#include <dht.h> 
#include <BH1750.h> 
#include "RTClib.h"
#include <SD.h>
#include <istsos.h>
#include <com/sim800.h>

// definitins
#define EXTERNAL_TEMP_PIN 11  // External temperature pin
#define DHT11_IN_PIN 13       // internal temperature
#define BUZZER 12             // buzzer pin
#define SM_PIN 8            //  for SM sensor
#define BMP085_ADDRESS 0x77   // bmp sensor Address  
#define BATT 0                // get battry meter value
#define WIN_DIRECTION 1         // win directionPin
// rain guage
#define RAIN_GAUGE_PIN 2
#define RAIN_GAUGE_INT 0
#define RAIN_FACTOR 0.2       // rain factor for one tip bucket
#define POWER_UP_GSM 9        // powerup pin
#define TEMP_UP 33            // upeer temp for the fan
#define TEMP_DOWN 30          // lower temperature or fan
#define FAN_PIN 10            // fan pin
#define SERVER_SETUP 1        // if SERVER_SETUP==0 SLPIOT.org else SERVER_SETUP=1 esos
#define TIME_RATE 15          // set as sending after every Time rate=15minute
// GPRS SETTINGS FOR ISTSOS

#define APN "mobitel"
#define USERNAME ""
#define PASSWORD ""
#define PROCEDURE "bb3a14a0988311e78b760800273cbaca"
#define POSTREQ "/istsos/wa/istsos/services/sl/operations/fastinsert"


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
String datetime_;
byte l_hour=0,l_minute=0; // to taken time differece of TIME_RATE defined time rate

// saving log file
File myFile;
int SDOK=0;
const int chipSelect = 53;  // chip select pin for the SD module.it should be connected to 53 of module

// SIM800
Sim800 istsos = Sim800(Serial1, APN, USERNAME, PASSWORD);
const char server[] = "istsos.org";
const char uri[] = POSTREQ;

// log file datetime 
String logfile="log.txt";

// GSM power up pin
int isGSM_POWERUP=0;
String sp;

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
double wind_direction=0;    // win direction value
double wind_speed=0;        // wind speed value
double water_level=0;       // water level
// rain guage variables
double rain_guarge=0;
volatile unsigned long rain_count=0;
volatile unsigned long rain_last=0; 

double battery_value=0;     // battry value
 
void setup() {
  Serial.begin(9600);   // serial monitor for showing 
  while (!Serial){}     // wait for Serial Monitor On
  Serial1.begin(9600);  // serial  for GPRS 
  while (!Serial1){}    // wait for GPRS Monitor

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
  if(SERVER_SETUP==0){
    sendGPRSDataASGet();
  }else{
    sendGPRSDataASPOST();
  }
}

//send data as GPRS
int sendGPRSDataASPOST(){
    Serial.println("Sending Data");
     String data =PROCEDURE;
    data.concat(";");
    data.concat(datetime_);
    data.concat(",");
    data.concat(battery_value);
    data.concat(",");
    data.concat(lux_value/1000);
    data.concat(",");
    data.concat(rain_count);
    rain_count=0;   // reset rain counter
    data.concat(",");
    data.concat(wind_direction);
    data.concat(",");
    data.concat(wind_speed);
    data.concat(",");
    data.concat(water_level);
    data.concat(",");
    data.concat(soilemoisture_value);
    data.concat(",");
    data.concat(altitude_value);
    data.concat(",");
    data.concat(pressure_value/1000);
    data.concat(",");
    data.concat(ext_temperature);
    data.concat(",");
    data.concat(ext_humidity);

    Serial.println(data);
    int response = istsos.executePost(server, uri, data);
  
    if (response != REQUEST_SUCCESS)
    {
      Serial.println(F("\nSend Failed"));
      Serial.println(response);
      delay(1000);
      return -1;
    }
    else
    {
      Serial.println(F("\nSend Success"));
      delay(1000);
      return 0;
    }
}

// send data as GPRS
void sendGPRSDataASGet(){
  int check_gprs;
  sendGPRSData();
  check_gprs = ShowSerialData('K');
  if(check_gprs != 0){      // id GPRS data false to send it tryis 5 times; then print the communication Error
    for(int i=0;i<5;i++){
        sendGPRSData();
        delay(2000);
        int check_gprs = ShowSerialData('K');
        if(check_gprs ==0)
          break;
    }
    printError("GPRS IS NOT CONNECTED TO SERVER");
    writeLogFile();     // write data to log
  }
  l_hour=now.hour();
  l_minute=now.minute();
}

// read current time value
void RTCDateTime()
{
    now = rtc.now(); 
    now =now - TimeSpan(0, 5, 30, 00);
    datetime_=String(now.year(),DEC);
    datetime_.concat('-');
    if(now.month()<10)
      datetime_.concat("0");
    datetime_.concat(String(now.month(), DEC));
    datetime_.concat('-');
    if(now.day()<10)
      datetime_.concat("0");
    datetime_.concat(String(now.day(), DEC));
    datetime_.concat('T');
    if(now.hour()<10)
      datetime_.concat("0");
    datetime_.concat(String(now.hour(), DEC));
    datetime_.concat(':');
    int miniute=now.minute()<2?59:now.minute();
     if(miniute<10)
      datetime_.concat("0");
    datetime_.concat(String(miniute, DEC));
    datetime_.concat(':');
     if(now.second()<10)
      datetime_.concat("0");
    datetime_.concat(String(now.second(), DEC));
    datetime_.concat("+0000");
    
    
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

    // win direction
    wind_direction=readWinDirection();
    printValues("Win Direction (ang):",wind_direction);

    // rain guarge
    rain_guarge=readRainGuarge();
    printValues("Rain Guarge:",rain_guarge);
    
    // get battery voltage
    battery_value=readBatteryVoltage();
    printValues("Battry Value:",battery_value);
    
    
    // current time and date
    printValues("Time : ",datetime_);

    // Fan operator
    funcFan();
    // station is up
    soundIndicator(1);
      
}

// print and show values
void printValues(String name_index,double value){
    Serial.print(name_index);
    Serial.println(value);
    delay(1000);
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

// read wind direction
double readWinDirection(){
  return (360/ (MAX_WIND_FACTOR-MIN_WIND_FACTOR))* (analogRead(WIN_DIRECTION)-MIN_WIND_FACTOR);
}

// read rain guarge
double readRainGuarge(){
  return rain_count* RAIN_FACTOR;
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

    // win speed // wind componant
    pinMode(RAIN_GAUGE_PIN,INPUT);
    digitalWrite(RAIN_GAUGE_PIN,HIGH);  // Turn on the internal Pull Up Resistor
    attachInterrupt(RAIN_GAUGE_INT,rainGageClick,FALLING);

    // turn on interrupts
    interrupts();

    // check and initialize fan
    pinMode(FAN_PIN,OUTPUT);
    digitalWrite(FAN_PIN,HIGH);

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

    if(SDOK==0){
    if (!SD.begin(chipSelect)) 
    {
      printError("SD Error ... !");
      setup();
    }
    else
    {
      SDOK=1;
      Serial.println("SD Initializes.");
    }  
    }

    Serial.println("Initialize GPRS");
    // setup GPRS
    if(SERVER_SETUP==0){
       // POWER UP GSM
      gsmPower();     // for GET requests
      if(setupGPRS()==-1){
           printError("GPRS Init Failed");
           setup();  
      }
    }else{
      // GSM server
      if(!istsos.begin()){
        printError("GPRS Error ... !");
      }
      else
      {
        Serial.println(F("GPRS Initialization Done : SIM 800"));
        digitalWrite(FAN_PIN,LOW);// check fan
        delay(5000);
        digitalWrite(FAN_PIN,HIGH);
      }  
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

// GSM power UP
void gsmPower(){
  if(isGSM_POWERUP==0){
    digitalWrite(POWER_UP_GSM,HIGH);
    digitalWrite(FAN_PIN,LOW);// check fan
    delay(2000);
    digitalWrite(POWER_UP_GSM,LOW); 
    digitalWrite(FAN_PIN,HIGH);
    Serial.print("Power UP");
    delay(100); 
    isGSM_POWERUP=1;
  }else if(isGSM_POWERUP==5){
    Serial.print("Power RESET"); 
    digitalWrite(POWER_UP_GSM,HIGH);
    digitalWrite(FAN_PIN,LOW);// check fan
    delay(2000);
    digitalWrite(POWER_UP_GSM,LOW); 
    digitalWrite(FAN_PIN,HIGH);
    delay(1000);
    Serial.print("Power UP"); 
    digitalWrite(POWER_UP_GSM,HIGH);
    digitalWrite(FAN_PIN,LOW);// check fan
    delay(2000);
    digitalWrite(POWER_UP_GSM,LOW); 
    digitalWrite(FAN_PIN,HIGH);
    delay(1000);
    isGSM_POWERUP=1;
  }else{
    isGSM_POWERUP++;
  }
}

void writeFileSD(String fileName)
{


  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) 
  {
Serial.println("Writing to "+fileName+ "...");
    myFile.println("");
    myFile.print(datetime_);
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

// =================   GPRS SETUP FOR GET STATEMENT ==============

int setupGPRS(){
  int check_gprs;  // check each module is OK;
  Serial.println("Check The GPRS module : ");
  
  Serial1.print("AT\r"); 
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1)
    return -1;
    
  // check pin reset happened : unlocked
  Serial1.print("AT+CPIN?\r");
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1)
    return -1;
    
  // check sim registered
  Serial1.print("AT+CREG?\r"); 
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1)
    return -1;
    
  //check GPRS attached :
  
  Serial1.print("AT+CGATT?\r"); 
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    return -1;
  }  
  // Reset the IP session if any
  Serial1.print("AT+CIPSHUT\r");
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1)
    return -1;
    
 //Check if the IP stack is initialized
  Serial1.print("AT+CIPSTATUS\r");
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1)
    return -1;
    
 // To keep things simple, I’m setting up a single connection mode
  Serial1.print("AT+CIPMUX=0\r"); 
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1)
    return -1;
    
  // Start the task, based on the SIM card you are using, you need to know the APN, username and password for your service provider
  Serial1.print("AT+CSTT= \"mobitel\", \"\", \"\"\r"); 
  delay(1000);
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1)
    return -1;
    
  // Now bring up the wireless. Please note, the response to this might take some time
  Serial1.print("AT+CIICR\r");
  delay(2000);
  check_gprs = ShowSerialData('K');
  if(check_gprs == -1){
    return -1;
  }
    
  //Get the local IP address. Some people say that this step is not required, but if I do not issue this, it was not working for my case. So I made this mandatory, no harm.
  Serial1.print(" AT+CIFSR\r");  
  delay(2000);
  check_gprs = ShowSerialData('N');
  if(check_gprs == -1)
    return -1;
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

// get data sender
void sendGPRSData(){
  
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
  Serial1.print(rain_count);
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
  Serial1.print(datetime_);
  Serial1.print("&GUID=");
  Serial1.print("5bf82c59-7ec0-4f");
  Serial1.print(" HTTP/1.1\r\nHost: www.slpiot.org\r\nConnection:keep-alive\r\n\r\n");
  ShowSerialData('N');
  
  // Random Data
  Serial1.write(0x1A);
  ShowSerialData('N');
  
  Serial.println("Data Sent");
}

