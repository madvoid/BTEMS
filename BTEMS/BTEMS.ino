// ------------------------------------------------------------------------------------------------
// Filename: BTEMS.ino
//
// Authors: Elizabeth Prucka
//      Nipun Gunawardena
//
// Notes:
//   Tested on Arduino Mini Pro 3.3V
// ------------------------------------------------------------------------------------------------




// Includes ---------------------------------------------------------------------------------------
#include <Narcoleptic.h>  // Sleep Library
#include <SD.h>           // SD card
#include <SPI.h>          // SPI Library
#include <SHT2x.h>        // SHT21 Library
#include <Wire.h>         // I2C Library
#include <Adafruit_MLX90614.h>  // Infrared Temp Library
#include <RTClib.h> //RTC Library




// Defining ---------------------------------------------------------------------------------------
#define DEBUG 1 //Used to Show Serial Monitor during Debugging


// Initializations --------------------------------------------------------------------------------
float mlxIR;    // IR values from MLX90614
float mlxAmb;   // Ambient temp values from MLX90614
float shtAmb;   // Ambient temp values from SHT21
float shtHum;   // Relative humidity values from SHT21
float mSecs;    // Milliseconds from Arduino
int chipSelect = 4;         // chipSelect pin for the SD card Reader
File logfile;               // Data object you will write your sensor data to
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
RTC_DS1307 RTC;
char filename[] = "BXX_00.CSV";     // Prototype filename




// Function Prototypes ----------------------------------------------------------------------------
void error(char *str);          // Error function prototype
// @prucka make a blink green led and blink red led function void blinkGreenLED(unsigned long m)



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
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);


  // Create filename, open SD Card File
  for (uint8_t i = 0; i < 100; i++) {            // Indexes file every time program is restarted
    filename[4] = i / 10 + '0';                // !! Will stop at 99 !!
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
  logfile.println("Year, Month, Day, Hour, Minute, Second, Millis, SHT_Amb, SHT_Hum, MLX_IR, MLX_Amb"); //List order of data on SD Card
  logfile.flush();                                                                                       //Save file

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
    // @prucka blink red led here
  }

  digitalWrite(10, HIGH);       //Light Green LED for 2 seconds
  delay(2000);
  digitalWrite(10, LOW);
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
  Serial.print("Time Elapsed: ");
  Serial.println(mSecs);
  Serial.print("SHT21 Ambient Temp: "); //Print results
  Serial.println(shtAmb);
  Serial.print("SHT21 Humidity");
  Serial.println(shtHum);
  Serial.print("MLX90614 Object Temp: ");
  Serial.println(mlxIR);
  Serial.print("MLX90614 Ambient Temp: ");
  Serial.println(mlxAmb);
  Serial.println();
  Serial.print("Current time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
#endif
  if (logfile) {

    // Write to file
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

    //Blink Green LED
    digitalWrite(10, HIGH); // @prucka change 10 to a variable greenPin
    delay(200);
    digitalWrite(10, LOW);

    // Sleep
    Narcoleptic.delay(9522);
  }
}




//Error Function --------------------------------------------------------------------------------------
void error(char *str) {                               // Used when initializing SD card...
  //Turns on red LED
  digitalWrite(10, HIGH); // @prucka setup red led and change to red led variable redPin

  //Print Error to Serial Monitor
  Serial.print("Error: ");
  Serial.println(str);
  while (1);
} //error()
