/*
Decagon shield polling functionality demo
*/

#include "SDI12.h"
#include <string.h>
#include "SensorList.h"
#define EI_NOTEXTERNAL
#include <EnableInterrupt.h>

//#define F_M0
#define F_32U4

#ifdef F_M0
#define DATAPIN1 A0  // change to the proper pin
#define DATAPIN2 A1
#define DATAPIN3 A2
#define DATAPIN4 A3
#define DATAPIN5 A4
#define DATAPIN6 A5
#endif

#ifdef F_32U4
#define DATAPIN1 9  // change to the proper pin
#define DATAPIN2 10
#define DATAPIN3 11
#define DATAPIN4 14
#define DATAPIN5 15
#define DATAPIN6 16
#endif

#define SENSOR_ADDRESS "?"


//Declare 6 SDI-12 objects initialized with DATAPIN1-6
SDI12 mySDI12[6] = {DATAPIN1, DATAPIN2, DATAPIN3, DATAPIN4, DATAPIN5, DATAPIN6};

//Struct used to manage sensor state data
struct SensorList data;

String sdiResponse = "";

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  for (int i = 0; i < 6; i++) {
    mySDI12[i].begin();
    mySDI12[i].forceHold();
  }

#ifdef SDI12_EXTERNAL_PCINT
  pinMode(DATAPIN1, INPUT_PULLUP);
  enableInterrupt(DATAPIN1, SDI12::handleInterrupt, CHANGE);
  pinMode(DATAPIN2, INPUT_PULLUP);
  enableInterrupt(DATAPIN2, SDI12::handleInterrupt, CHANGE);
  pinMode(DATAPIN3, INPUT_PULLUP);
  enableInterrupt(DATAPIN3, SDI12::handleInterrupt, CHANGE);
  pinMode(DATAPIN4, INPUT_PULLUP);
  enableInterrupt(DATAPIN4, SDI12::handleInterrupt, CHANGE);
  pinMode(DATAPIN5, INPUT_PULLUP);
  enableInterrupt(DATAPIN5, SDI12::handleInterrupt, CHANGE);
  pinMode(DATAPIN6, INPUT_PULLUP);
  enableInterrupt(DATAPIN6, SDI12::handleInterrupt, CHANGE);
#endif
}

void loop() {

  // ===== Poll Sensors =====

  data = poll_sensors(mySDI12);

  // ===== Send Measure to Active Sensors =====
  
  for (int i = 0; i < data.count; i++) {
    int num = data.list[i];
    get_measure(&mySDI12[num]);
  }

  delay(1000);     // delay between taking reading and requesting data

  // ===== Retrieve Data from Active Sensors =====

  for (int i = 0; i < data.count; i++) {
    int num = data.list[i];
    sdiResponse = get_data(&mySDI12[num]);

    set_data(&data, sdiResponse, i);
    /*Serial.print("Data from pin A");
    Serial.print(num);
    Serial.print(": ");
    Serial.println(sdiResponse);*/
    sdiResponse = "";
  }

  // ===== Print Data to Serial Monitor =====

  print_data(data);

  delay(3000);
}