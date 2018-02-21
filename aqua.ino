/*
  This test code is write for Arduino AVR Series(UNO, Leonardo, Mega)
  If you want to use with LinkIt ONE, please connect the module to D0/1 and modify:

  // #include <SoftwareSerial.h>
  // SoftwareSerial s_serial(2, 3);      // TX, RX

  #define sensor Serial1
*/

#include <SFE_BMP180.h>
#include <Wire.h>
// You will need to create an SFE_BMP180 object, here called "pressure":
SFE_BMP180 pressure;
double baselinePressure;

#include <dht.h>
#define DHT11PIN 4

dht DHT11;
const float ERR_FLOAT 3.4028235E+38;
const int ERR_INT ~(1 << 31);


#include <SoftwareSerial.h>
SoftwareSerial s_serial(2, 3); // TX, RX
// TODO sensor is unclear name... what is the actual sensor name?
#define sensor s_serial

const unsigned char cmd_get_sensor[] = {
    0xff, 0x01, 0x86, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x79
};

// O2
const float VOLTAGE_REFERENCE = 3.3;
const int O2_PIN = A3;

// mq7 (i.e. CO)
const int AOUT_PIN = A0;

unsigned char dataRevice[9];
int temperature;

void setup() {
  sensor.begin(9600);
  Serial.begin(9600);

  if (!pressure.begin()) {
    Serial.println("ERROR: BMP180 init fail (disconnected?)\n\n");
    while(1); // Pause forever.
  }

  // Get the baseline pressure:
  baselinePressure = readPressure();

  Serial.print("baseline pressure: ");
  Serial.print(baselinePressure);
}

void loop() {
  Serial.println("\n");

  int co2 = readCO2();
  if (co2 != ERR_INT) {
    Serial.println("ERROR: some error"); break;
  }
  Serial.print("co2: ");
  Serial.println(co2);

  Serial.print("o2: ");
  Serial.println(readO2());

  Serial.print("co: ");
  Serial.println(readCO());

  int chk = DHT11.read11(DHT11PIN);

  switch (chk) {
    case 0: break; // only complain to the log if there is an error
    case -1: Serial.println("ERROR: DHT11 checksum error"); break;
    case -2: Serial.println("ERROR: DHT11 time out error"); break;
    default: Serial.println("ERROR: DHT11 unknown error"); break;
  }

  Serial.print("humidity: ");
  Serial.println((float)DHT11.humidity, 2);


  Serial.print("temperature: ");
  Serial.println(DHT11.temperature, 2);

  double currentPressure = readPressure();
  double currentAltitude = pressure.altitude(currentPressure, baselinePressure);

  Serial.print("altitude: ");
  if (currentAltitude >= 0.0) {
    Serial.print(" "); // add a space for positive numbers
  }

  Serial.print(currentAltitude, 1);

  delay(10000);
}

// readCO2
// TODO: What are the units?
int readCO2() {
  byte data[9];
  int i = 0;

  // transmit command data
  for(i = 0; i < sizeof(cmd_get_sensor); i++) {
    sensor.write(cmd_get_sensor[i]);
  }
  delay(10);
  // begin reveiceing data
  if(sensor.available()) {
    while(sensor.available()) {
      for(int i = 0; i < 9; i++) {
        data[i] = sensor.read();
      }
    }
  }

  if((i != 9) || (1 + (0xFF ^ (byte)(data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7]))) != data[8]) {
    return ERR_INT;
  }

  temperature = (int)data[4] - 40;
  return (int)data[2] * 256 + (int)data[3];
}


// readCO
// TODO: What are the units?
int readCO () {
  return analogRead(AOUT_PIN);
}

// readO2Vout
// TODO: What are the units?
float readO2Vout() {
  long sum = 0;

  for(int i = 0; i < 32; i++) {
    sum += analogRead(O2_PIN);
  }

  sum >>= 5;
  return sum * (VOLTAGE_REFERENCE / 1023.0);
}

// readO2
// TODO: What are the units?
float readO2() {
  // Vout samples are with reference to 3.3V
  float voltageOut = readO2Vout();

  // when its output voltage is 2.0V,
  float concentration = voltageOut * 0.21 / 2.0;
  return concentration * 100;
}

// readPressure
// TODO: What are the units?
double readPressure() {
  double t, p;

  // First get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.
  int temperatureWait = pressure.startTemperature()
  if (temperatureWait == 0) {
    Serial.println("ERROR: SFE_BMP180 unsuccessful temperature measurement start\n");
    return ERR_FLOAT;
  }
  delay(temperatureWait);

  // Retrieve the completed temperature measurement:
  // Note that the measurement is stored in the variable T.
  // Use '&T' to provide the address of T to the function.
  // Function returns 1 if successful, 0 if failure.
  if (!pressure.getTemperature(t)) {
    Serial.println("ERROR: SFE_BMP180 failed to get temperature\n")
    return ERR_FLOAT;
  }

  // Start a pressure measurement:
  // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.
  int pressureWait = pressure.startPressure(3)
  if (pressureWait == 0) {
    Serial.println("ERROR: SFE_BMP180 unsuccessful temperature pressure start\n");
    return ERR_FLOAT;
  }
  delay(pressureWait);

  // Retrieve the completed pressure measurement:
  // Note that the measurement is stored in the variable P.
  // Use '&P' to provide the address of P.
  // Note also that the function requires the previous temperature measurement (T).
  // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
  // Function returns 1 if successful, 0 if failure.
  if (!pressure.getPressure(p,t)) {
    Serial.println("ERROR: SFE_BMP180 failed to get pressure\n");
    return ERR_FLOAT;
  }
  return p;
}
