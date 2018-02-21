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
double baseline; // baseline pressure

#include <dht.h>
#define DHT11PIN 4

dht DHT11;
#define ERR_FLOAT 3.4028235E+38;

#include <SoftwareSerial.h>
SoftwareSerial s_serial(2, 3);      // TX, RX
#define sensor s_serial

const unsigned char cmd_get_sensor[] = {
    0xff, 0x01, 0x86, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x79
};

//constant for O2
const float VRefer = 3.3;       // voltage of adc reference
const int pinAdc   = A3;

//constant for mq7
const int AOUTpin=0;//the AOUT pin of the CO sensor goes into analog pin A0 of the arduino

unsigned char dataRevice[9];
int temperature;
int CO2PPM;

int value;

void setup() {
  sensor.begin(9600);
  Serial.begin(9600);
  Serial.println("get a 'g', begin to read from sensor!");
  Serial.println("********************************************************");
  Serial.println();

  if (pressure.begin()) {
    Serial.println("BMP180 init success");
  } else {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.
    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while(1); // Pause forever.
  }

  // Get the baseline pressure:
  baseline = readPressure();

  Serial.print("baseline pressure: ");
  Serial.print(baseline);
  Serial.println(" mb");
}

void loop() {
  if (dataRecieve()) {
    //Serial.print("Temperature: ");
    //Serial.print(temperature);
    Serial.print("co2(): ");
    Serial.println(CO2PPM);
    //Serial.println("");
  }

  float Vout =0;
  //Serial.print("Vout =");

  Vout = readO2Vout();
  //Serial.print(Vout);
  Serial.print("02(): ");
  Serial.println(readConcentration());

  value= analogRead(AOUTpin);//reads the analaog value from the CO sensor's AOUT pin
  Serial.print("co(): ");
  Serial.println(value);//prints the CO value

  Serial.println("\n");

  int chk = DHT11.read11(DHT11PIN);

  Serial.print("Read sensor: ");
  switch (chk) {
    case 0: Serial.println("OK"); break;
    case -1: Serial.println("Checksum error"); break;
    case -2: Serial.println("Time out error"); break;
    default: Serial.println("Unknown error"); break;
  }

  Serial.print("humidity(): ");
  Serial.println((float)DHT11.humidity, 2);

  //Serial.print("temp(): ");
  //Serial.println((float)DHT11.temperature, 2);

  Serial.print("temp(): ");
  Serial.println(Fahrenheit(DHT11.temperature), 2);

  double a,P;

  // Get a new pressure reading:
  P = readPressure();

  // Show the relative altitude difference between
  // the new reading and the baseline reading:
  a = pressure.altitude(P,baseline);

  Serial.print("altitude(): ");
  if (a >= 0.0) {
    Serial.print(" "); // add a space for positive numbers
  }

  Serial.print(a, 1);
  Serial.println(" meters");
  //if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
  //Serial.print(a*3.28084,0);
  //Serial.println(" feet");

  delay(10000);
}

//for CO2
bool dataRecieve(void) {
  byte data[9];
  int i = 0;

  //transmit command data
  for(i = 0; i < sizeof(cmd_get_sensor); i++) {
    sensor.write(cmd_get_sensor[i]);
  }
  delay(10);
  //begin reveiceing data
  if(sensor.available()) {
    while(sensor.available()) {
      for(int i = 0; i < 9; i++) {
        data[i] = sensor.read();
      }
    }
  }

  for(int j=0; j<9; j++) {
  //       Serial.print(data[j]);
  //       Serial.print(" ");
  }
  //   Serial.println("");

  if((i != 9) || (1 + (0xFF ^ (byte)(data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7]))) != data[8]) {
    return false;
  }

  CO2PPM = (int)data[2] * 256 + (int)data[3];
  temperature = (int)data[4] - 40;

  return true;
}

//for 02
float readO2Vout() {
  long sum = 0;
  for(int i = 0; i < 32; i++) {
    sum += analogRead(pinAdc);
  }

  sum >>= 5;

  float MeasuredVout = sum * (VRefer / 1023.0);
  return MeasuredVout;
}

// for 02
float readConcentration() {
  // Vout samples are with reference to 3.3V
  float MeasuredVout = readO2Vout();

  //float Concentration = FmultiMap(MeasuredVout, VoutArray,O2ConArray, 6);
  //when its output voltage is 2.0V,
  float Concentration = MeasuredVout * 0.21 / 2.0;
  float Concentration_Percentage=Concentration*100;
  return Concentration_Percentage;
}

double fahrenheit(double celsius) {
  return 1.8 * celsius + 32;
}


double readPressure() {
  double t, p;

  // First get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.
  int temperatureWait = pressure.startTemperature()
  if (temperatureWait == 0) {
    Serial.println("error retrieving temperature measurement\n");
    return ERR_FLOAT;
  }
  delay(temperatureWait);

  // Retrieve the completed temperature measurement:
  // Note that the measurement is stored in the variable T.
  // Use '&T' to provide the address of T to the function.
  // Function returns 1 if successful, 0 if failure.
  if (!pressure.getTemperature(t)) {
    Serial.println("error starting temperature measurement\n")
    return ERR_FLOAT;
  }

  // Start a pressure measurement:
  // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.
  int pressureWait = pressure.startPressure(3)
  if (pressureWait == 0) {
    Serial.println("error starting pressure measurement\n");
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
    Serial.println("error retrieving pressure measurement\n");
    return ERR_FLOAT;
  }
  return p;
}
