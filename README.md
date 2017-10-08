# ISTSOS Open source Whether station

ISTSOS is a Free and Open Source Sensor Observation Service Data Management System.istSOS allows for 
managing and dispatch observations from monitoring sensors according to the Sensor Observation Service standard.
Istsos has oppotunity to connect datacollectors wia REST API. This Source explains the data collector internal structure.

 [More About ISTSOS](http://istsos.org/en/latest/doc/index.html)

## Getting Started

This Is a Whether station Internal code. this is requested to buy the hardware implementation. this is fully Arduino based implemantation.

### Prerequisites

1. [Arduino IDE](https://www.arduino.cc/en/Main/Software) - Download for Source edit
2. Hardware Implementation

### Installing

1. Install Arduino To Your Computer
2. Copy Library files in to you extarnal library direcory. 

```
C:\[user]\Documents\Arduino\libraries
```
  External Libraries:-
  ```
    1.Adafruit_BME280
    2.BH1750
    3.DallasTemperature
    4.DHT Sensor Library
    5.DS1307RTC
    6.istsos Communication Library
    7.OneWire
    8.TinyGSM
```


## Running the tests

1. Run Arduino and open esos.ino file from arduino IDE.
2. Change Your Communication parameters here
    ```
    // GPRS SETTINGS FOR ISTSOS

      #define APN "mobitel"               
      #define USERNAME ""
      #define PASSWORD ""
      #define PROCEDURE "bb3a14a0988311e78b760800273cbaca"
      #define POSTREQ "/istsos/wa/istsos/services/sl/operations/fastinsert"
    ```
 3. Upload the Changed source to your station
 4. Run the station.

