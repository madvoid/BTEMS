// ------------------------------------------------------------------------------------------------
// Filename: BTEMS.ino
//
// Authors: Nipun Gunawardena
//          Elizabeth Prucka
//
// Notes:
//   - Tested on Arduino Mini Pro 3.3V
//   - After uploading code, the user should wait until the program has fully started and taken a few
//     readings before unplugging from the computer
//   - Current delay in realtime is a little less than 10 seconds
//   - If four AA batteries are used, the resistor divider should consist of a 24k high-side resistor (R1)
//     and a 30k low-side resistor (R2)
//
// TODO:
//   - Add support for 999 files
//
// Compile Settings: Board: "Arduino Pro or Pro Mini"
//                   Processor: "ATmega 328 (3.3V, 8MHz)"
//
// Current consumption values:
//   - 3.60 to 4.40 mA during sleep with knockoff RTC
//   - At least 12.5mA during write, assume 20mA-40mA during write
//   - 2.02 mA during sleep with adafruit RTC
//
// ------------------------------------------------------------------------------------------------

// !! Change Filename depending on station. Change the XX below to hexadecimal number !!
char filename[] = "BTM07_00.CSV";     // Prototype filename



// Includes ---------------------------------------------------------------------------------------
#include <Narcoleptic.h>  // Sleep Library                  https://code.google.com/archive/p/narcoleptic/
#include <SD.h>           // SD card                        Included
#include <SPI.h>          // SPI Library                    Included
#include <SHT2x.h>        // SHT21 Library                  https://github.com/misenso/SHT2x-Arduino-Library            
#include <Wire.h>         // I2C Library                    Included
#include <Adafruit_MLX90614.h>  // Infrared Temp Library    https://github.com/adafruit/Adafruit-MLX90614-Library
#include <RTClib.h>       // RTC Library                    https://github.com/mizraith/RTClib
//#include <RTC_DS3231.h>   // From RTClib, DS3231 specific library




// Defining ---------------------------------------------------------------------------------------
#define DEBUG 0           // Used to Show Serial Monitor during Debugging
#define RED_LED_PIN 6     // Pin used for red LED
#define GRN_LED_PIN 9     // Pin used for green LED
#define BAT_PIN A3        // Pin used to measure battery level
#define CHIP_SELECT 10    // Chip select pin for SD card




// Initializations --------------------------------------------------------------------------------
float mlxIR;                // IR values from MLX90614
float mlxAmb;               // Ambient temp values from MLX90614
float shtAmb;               // Ambient temp values from SHT21
float shtHum;               // Relative humidity values from SHT21
float mSecs;                // Milliseconds from Arduino
float batLvl;               // Battery voltage level
unsigned int batRaw;        // Raw battery input

const unsigned int R1 = 24000;   // High-side resistor for battery measuring voltage divider
const unsigned int R2 = 30000;   // Low-side resistor for battery measuring voltage divider

Adafruit_MLX90614 mlx = Adafruit_MLX90614();  // MLX90614 Class
RTC_DS3231 RTC;                               // RTC Class

File logfile;               // Data object you will write your sensor data to





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
  SD.begin(CHIP_SELECT);     //Initialize the SD card reader

  //Declare Pins for LED
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GRN_LED_PIN, OUTPUT);

  // Create filename, open SD Card File
  for (uint8_t i = 0; i < 100; i++) {            // Indexes file every time program is restarted
    filename[6] = i / 10 + '0';                  // !! Will stop at 99 !!
    filename[7] = i % 10 + '0';
    if (! SD.exists(filename)) {
      logfile = SD.open(filename, FILE_WRITE);
      break;
    }
  }

  // Prepare file
  if (!logfile) {               // File successfully opened check
    error("Couldnt create file!");
  }
  logfile.println("Year, Month, Day, Hour, Minute, Second, SHT_Amb_C, SHT_Hum_Pct, MLX_IR_C, MLX_Amb_C, Bat_Lvl_V"); // Print header to SD
  logfile.flush();             //Save file

  // Print header for debugging
#if DEBUG
  Serial.println("Year, Month, Day, Hour, Minute, Second, SHT_Amb_C, SHT_Hum_Pct, MLX_IR_C, MLX_Amb_C, Bat_Lvl_V");
#endif

  // Check if the RTC is running.
  // TODO: Determine whether chunk is necessary
  // Doesn't work when uncommented, why?
  //  if (! RTC.isrunning()) {
  //    Serial.println("RTC is NOT running");
  //    error("RTC is NOT running");
  //  }

  // Gets current datetime and compares it to compilation time
  DateTime now = RTC.now();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (now.unixtime() < compiled.unixtime()) {
    Serial.println("RTC is older than compile time! Updating");
    RTC.adjust(DateTime(__DATE__, __TIME__));
    for (int i = 0; i < 3; i++) {
      blinkRedLED(100);
      blinkGreenLED(100);
    }
  }

  delay(1000);              // Give user time to look at LED
  blinkGreenLED(2000);      // Blink Green LED for 2 seconds
}




// Loop Function ----------------------------------------------------------------------------------
void loop() {

  // Gather measurements
  digitalWrite(GRN_LED_PIN, HIGH);  // Start blink
  shtAmb = SHT2x.GetTemperature();
  mlxAmb = mlx.readAmbientTempC();
  mlxIR = mlx.readObjectTempC();
  shtHum = SHT2x.GetHumidity();

  batRaw = 0;
  for (int i = 0; i < 3; i++) {
    batRaw += analogRead(BAT_PIN);    // Average battery readings, division happens below
  }

  batLvl = ( ((float)batRaw) * (3.3 / 1024.0 / 3.0) * (float)(R1 + R2) ) / ((float)R2);

  // Get the current time
  DateTime now = RTC.now();

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
    logfile.print(shtAmb);
    logfile.print(", ");
    logfile.print(shtHum);
    logfile.print(", ");
    logfile.print(mlxIR);
    logfile.print(", ");
    logfile.print(mlxAmb);
    logfile.print(", ");
    logfile.print(batLvl);
    logfile.println();
    logfile.flush();                                  //Save file
  }

  // End Blink Green LED
  digitalWrite(GRN_LED_PIN, LOW);

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
  Serial.print(shtAmb);
  Serial.print(", ");
  Serial.print(shtHum);
  Serial.print(", ");
  Serial.print(mlxIR);
  Serial.print(", ");
  Serial.print(mlxAmb);
  Serial.print(", ");
  Serial.print(batLvl);
  Serial.println();
  Serial.flush();
#endif

  // Delay/Sleep
  Narcoleptic.delay(8200);
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
void error(const char *str) {
  //Turns on red LED
  digitalWrite(RED_LED_PIN, HIGH);

  //Print Error to Serial Monitor
#if DEBUG
  Serial.print("Error: ");
  Serial.println(str);
#endif

  while (1);
}
