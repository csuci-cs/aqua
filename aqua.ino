/*
  github.com/csuci-cs/aqua
*/

#include <SFE_BMP180.h>
#include <Wire.h>
// You will need to create an SFE_BMP180 object, here called "pressure":
SFE_BMP180 pressureSensor;
double baselinePressure;

#include <dht.h>
#define DHT11PIN 4
dht DHT11;

#include <rgb_lcd.h>
rgb_lcd lcd; //declare lcd thingie

const float ERR_FLOAT = 3.4028235E+38; // max values
const int ERR_INT = 2^15 - 1; // max values

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

void setup() {
  lcd.begin(16,1);

  sensor.begin(9600);
  Serial.begin(9600);

  if (!pressureSensor.begin()) {
    Serial.println("ERROR: BMP180 init fail (disconnected?)");
    while(1); // Pause forever.
  }

  // Get the baseline pressure:
  baselinePressure = readPressure();

  Serial.print("baseline pressure: ");
  Serial.println(baselinePressure);
  Serial.print("\n\n");
}

int co2;
int co;
float o2;
double pressure;
double altitude;
double humidity;
double temperature;

void loop() {
  co2 = readCO2();
  if (co2 != ERR_INT) {
    Serial.print("co2: ");
    Serial.println(co2);
  } else {
    Serial.println("ERROR: co2 some error"); // TODO needs descriptive error message
  }

  o2 = readO2();
  Serial.print("o2: ");
  Serial.println(readO2());

  co = readCO();
  Serial.print("co: ");
  Serial.println(co);

  switch (DHT11.read11(DHT11PIN)) {
    case 0: break; // only complain to the log if there is an error
    case -1: Serial.println("ERROR: DHT11 checksum error"); break;
    case -2: Serial.println("ERROR: DHT11 time out error"); break;
    default: Serial.println("ERROR: DHT11 unknown error"); break;
  }

  humidity = DHT11.humidity;
  Serial.print("humidity: ");
  Serial.println(humidity , 2);

  temperature = DHT11.temperature;
  Serial.print("temperature: ");
  Serial.println(temperature, 2);

  pressure = readPressure();
  if (pressure != ERR_FLOAT) {
    altitude = pressureSensor.altitude(pressure, baselinePressure);
    Serial.print("pressure: ");
    Serial.println(pressure);
    Serial.print("altitude: ");
    Serial.println(altitude, 1);
  } // else {
    // Serial.println("ERROR: pressure some error"); // error printed in readPressure function
  // }

  Serial.print("\n\n");
  displayCycle(co2, co, o2, pressure, altitude, humidity, temperature);
}

#define TOTAL_DISPLAY_TIME 10000
#define DISPLAY_COUNT 7

void displayCycle(int co2, int co, float o2, double pressure, double altitude, double humidity, double temperature) {
  lcd.print("CO2:"); lcd.print(co2);
  delay(TOTAL_DISPLAY_TIME / DISPLAY_COUNT); lcd.clear();
  lcd.print("CO:"); lcd.print(co);
  delay(TOTAL_DISPLAY_TIME / DISPLAY_COUNT); lcd.clear();
  lcd.print("O2:"); lcd.print(o2);
  delay(TOTAL_DISPLAY_TIME / DISPLAY_COUNT); lcd.clear();
  lcd.print("pr:"); lcd.print(pressure);
  delay(TOTAL_DISPLAY_TIME / DISPLAY_COUNT); lcd.clear();
  lcd.print("alt:"); lcd.print(altitude);
  delay(TOTAL_DISPLAY_TIME / DISPLAY_COUNT); lcd.clear();
  lcd.print("hum:"); lcd.print(humidity);
  delay(TOTAL_DISPLAY_TIME / DISPLAY_COUNT); lcd.clear();
  lcd.print("temp:"); lcd.print(temperature);
  delay(TOTAL_DISPLAY_TIME / DISPLAY_COUNT); lcd.clear();
}

// readCO2
// TODO: What are the units?
int readCO2() {
  byte data[9];
  int i = 0;

  // transmit command data
  for (i = 0; i < sizeof(cmd_get_sensor); i++) {
    sensor.write(cmd_get_sensor[i]);
  }
  delay(10);
  // begin reveiceing data
  if (sensor.available()) {
    while (sensor.available()) {
      for (int i = 0; i < 9; i++) {
        data[i] = sensor.read();
      }
    }
  }

  if ((i != 9) || (1 + (0xFF ^ (byte)(data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7]))) != data[8]) {
    return ERR_INT;
  }

  return (int)data[2] * 256 + (int)data[3];
}

// readCO
// TODO: What are the units?
int readCO () {
  return analogRead(AOUT_PIN);
}

// readO2
// TODO: What are the units?
float readO2() {
  // part 1: calculate voltageOut
  long sum = 0;

  for (int i = 0; i < 32; i++) {
    sum += analogRead(O2_PIN);
  }

  sum >>= 5;
  float voltageOut = sum * (VOLTAGE_REFERENCE / 1023.0);

  // part 2: calculate concentration

  // when its output voltage is 2.0V, // <- what does this mean?
  float concentration = voltageOut * 0.21 / 2.0;
  return concentration * 100;
}

// readPressure
// TODO: What are the units?
double readPressure() {
  double temp, pres;

  // First get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.
  int temperatureWait = pressureSensor.startTemperature();
  if (temperatureWait == 0) {
    Serial.println("ERROR: SFE_BMP180 unsuccessful temperature measurement start\n");
    return ERR_FLOAT;
  }
  delay(temperatureWait);

  // Retrieve the completed temperature measurement:
  // Note that the measurement is stored in the variable T.
  // Use '&T' to provide the address of T to the function.
  // Function returns 1 if successful, 0 if failure.
  if (!pressureSensor.getTemperature(temp)) {
    Serial.println("ERROR: SFE_BMP180 failed to get temperature\n");
    return ERR_FLOAT;
  }

  // Start a pressure measurement:
  // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.
  int pressureWait = pressureSensor.startPressure(3);
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
  if (!pressureSensor.getPressure(pres,temp)) {
    Serial.println("ERROR: SFE_BMP180 failed to get pressure\n");
    return ERR_FLOAT;
  }
  return pres;
}
