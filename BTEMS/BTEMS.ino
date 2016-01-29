// ------------------------------------------------------------------------------------------------
// Filename: BTEMS.ino
//
// Authors: Nipun Gunawardena
//          Elizabeth Prucka
//
// Notes:
//   Tested on Arduino Mini Pro 3.3V
//
// TODO:
//   - Add support for 999 files
//   - Add battery checking
//   - Figure out fix for broken millis()?
//
// Current consumption values:
// 
// ------------------------------------------------------------------------------------------------




// Includes ---------------------------------------------------------------------------------------
#include <Narcoleptic.h>  // Sleep Library                  https://code.google.com/archive/p/narcoleptic/
#include <SD.h>           // SD card                        Included
#include <SPI.h>          // SPI Library                    Included
#include <SHT2x.h>        // SHT21 Library                  https://github.com/misenso/SHT2x-Arduino-Library            
#include <Wire.h>         // I2C Library                    Included
#include <Adafruit_MLX90614.h>  // Infrared Temp Library    https://github.com/adafruit/Adafruit-MLX90614-Library
#include <RTClib.h>       // RTC Library                    https://github.com/mizraith/RTClib
#include <RTC_DS3231.h>   // From RTClib, DS3231 specific library




// Defining ---------------------------------------------------------------------------------------
#define DEBUG 0           //Used to Show Serial Monitor during Debugging
#define RED_LED_PIN 6
#define GRN_LED_PIN 9




// Initializations --------------------------------------------------------------------------------
float mlxIR;                // IR values from MLX90614
float mlxAmb;               // Ambient temp values from MLX90614
float shtAmb;               // Ambient temp values from SHT21
float shtHum;               // Relative humidity values from SHT21
float mSecs;                // Milliseconds from Arduino
int chipSelect = 4;         // chipSelect pin for the SD card Reader
File logfile;               // Data object you will write your sensor data to
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
RTC_DS3231 RTC;
char filename[] = "BTM_00.CSV";     // Prototype filename




// Function Prototypes ----------------------------------------------------------------------------
void error(char *str);                  // Error function prototype
void blinkGreenLED(unsigned long m);    //Blinking Green LED function
void blinkRedLED(unsigned long m);      //Blinking Red LED function




// Setup Function ---------------------------------------------------------------------------------
void setup() {
#if DEBUG
  Serial.begin(9600); //turn on serial monitor
#endif

  // Initialize libraries
  Wire.begin();
  mlx.begin();
  RTC.begin();

  // Initialize SD Card
  SD.begin(chipSelect);     //Initialize the SD card reader

  //Declare Pins for LED
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GRN_LED_PIN, OUTPUT);

  // Create filename, open SD Card File
  for (uint8_t i = 0; i < 100; i++) {            // Indexes file every time program is restarted
    filename[4] = i / 10 + '0';                  // !! Will stop at 99 !!
    filename[5] = i % 10 + '0';
    if (! SD.exists(filename)) {
      logfile = SD.open(filename, FILE_WRITE);
      break;
    }
  }

  // Prepare file
  if (!logfile) {               // File successfully opened check
    error("Couldnt create file!");
  }
  logfile.println("Year, Month, Day, Hour, Minute, Second, Millis, SHT_Amb, SHT_Hum, MLX_IR, MLX_Amb"); // Print header to SD
  logfile.flush();             //Save file

  // Print header for debugging
#if DEBUG
  Serial.println("Year, Month, Day, Hour, Minute, Second, Millis, SHT_Amb, SHT_Hum, MLX_IR, MLX_Amb");
#endif

  // Check if the RTC is running.
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running");
    error("RTC is NOT running");
  }

  // Gets current datetime and compares it to compilation time
  DateTime now = RTC.now();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (now.unixtime() < compiled.unixtime()) {
    Serial.println("RTC is older than compile time! Updating");
    RTC.adjust(DateTime(__DATE__, __TIME__));
    blinkRedLED(1000);
  }

  blinkGreenLED(2000);      //Blink Green LED for 2 seconds
}




// Loop Function ----------------------------------------------------------------------------------
void loop() {

  // Gather measurements
  mSecs = millis();
  shtAmb = SHT2x.GetTemperature();
  mlxAmb = mlx.readAmbientTempC();
  mlxIR = mlx.readObjectTempC();
  shtHum = SHT2x.GetHumidity();

  // Get the current time
  DateTime now = RTC.now();

  // Write to Serial Monitor
#if DEBUG
  Serial.print(now.year(), DEC);
  Serial.print(", ");
  Serial.print(now.month(), DEC);
  Serial.print(", ");
  Serial.print(now.day(), DEC);
  Serial.print(", ");
  Serial.print(now.hour(), DEC);
  Serial.print(", ");
  Serial.print(now.minute(), DEC);
  Serial.print(", ");
  Serial.print(now.second(), DEC);
  Serial.print(", ");
  Serial.print(mSecs);
  Serial.print(", ");
  Serial.print(shtAmb);
  Serial.print(", ");
  Serial.print(shtHum);
  Serial.print(", ");
  Serial.print(mlxIR);
  Serial.print(", ");
  Serial.print(mlxAmb);
  Serial.println();
  Serial.flush();
#endif

  // Write to SD card
  if (logfile) {
    logfile.print(now.year(), DEC);
    logfile.print(", ");
    logfile.print(now.month(), DEC);
    logfile.print(", ");
    logfile.print(now.day(), DEC);
    logfile.print(", ");
    logfile.print(now.hour(), DEC);
    logfile.print(", ");
    logfile.print(now.minute(), DEC);
    logfile.print(", ");
    logfile.print(now.second(), DEC);
    logfile.print(", ");
    logfile.print(mSecs);
    logfile.print(", ");
    logfile.print(shtAmb);
    logfile.print(", ");
    logfile.print(shtHum);
    logfile.print(", ");
    logfile.print(mlxIR);
    logfile.print(", ");
    logfile.print(mlxAmb);
    logfile.println();
    logfile.flush();                                  //Save file
  }

  //Blink Green LED
  blinkGreenLED(200);

  // Delay/Sleep
  Narcoleptic.delay(9500);
  // delay(9421);
}




//Blink LED Functions --------------------------------------------------------------------------------
void blinkGreenLED(unsigned long m) {
  //Blinks Green LED
  digitalWrite(GRN_LED_PIN, HIGH);
  delay(m);
  digitalWrite(GRN_LED_PIN, LOW);
}

void blinkRedLED(unsigned long m) {
  //Blinks Red LED
  digitalWrite(RED_LED_PIN, HIGH);
  delay(m);
  digitalWrite(RED_LED_PIN, LOW);
}




//Error Function --------------------------------------------------------------------------------------
void error(const char *str) {                               // Used when initializing SD card...
  //Turns on red LED
  digitalWrite(RED_LED_PIN, HIGH);

  //Print Error to Serial Monitor
#if DEBUG
  Serial.print("Error: ");
  Serial.println(str);
#endif

  while (1);
}
