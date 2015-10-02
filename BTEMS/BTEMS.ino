// ------------------------------------------------------------------------------------------------
// Filename: BTEMS.ino
//
// Authors: Elizabeth Prucka
//	    Nipun Gunawardena
//
// Notes:
//	 Tested on Arduino Mini Pro 3.3V
// ------------------------------------------------------------------------------------------------




// Includes ---------------------------------------------------------------------------------------
#include <Narcoleptic.h>	// Sleep Library
#include <SD.h> 		// SD card
#include <SPI.h> 		// SPI Library
#include <SHT2x.h>		// SHT21 Library
#include <Wire.h>		// I2C Library
#include <Adafruit_MLX90614.h>  // Infrared Temp Library




// Includes ---------------------------------------------------------------------------------------
#define DEBUG 1




// Initializations --------------------------------------------------------------------------------
// @prucka Comment code descriptively
float mlxIR;		//
float mlxAmb;
float shtAmb;
float shtHum;
float mSecs;
int chipSelect = 4; 				// chipSelect pin for the SD card Reader
File logfile; 					// Data object you will write your sensor data to
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
char filename[] = "BXX_00.CSV";			// Prototype filename




// Function Prototypes ----------------------------------------------------------------------------
void error(char *str);          // Error function prototype




// Setup Function ---------------------------------------------------------------------------------
void setup() {
#if DEBUG
  Serial.begin(9600); //turn on serial monitor
#endif

  // Initialize libraries
  Wire.begin();
  mlx.begin();

  // Initialize SD Card
  pinMode(10, OUTPUT); 	// Must declare 10 an output and reserve it
  SD.begin(chipSelect); 		//Initialize the SD card reader

  // @prucka Blink LED via pins instead of current method. Initialize LEDs here

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
  if (!logfile) {								// File successfully opened check
    error("Couldnt create file!");
  }
  logfile.println("Millis, SHT_Amb, SHT_Hum, MLX_IR, MLX_Amb");
  logfile.flush();

  // @prucka Turn on LED for 2 seconds and then turn off here
}




// Loop Function ----------------------------------------------------------------------------------
void loop() {

  // Gather measurements
  mSecs = millis();
  shtAmb = SHT2x.GetTemperature();
  // @prucka add mlx ambient read
  mlxIR = mlx.readObjectTempC();
  shtHum = SHT2x.GetshtHum();


  // Write to file
  if (logfile) {
#if DEBUG
    Serial.print("Time Elapsed: ");
    Serial.println(mSecs);
    Serial.print("SHT21 Ambient Temp: "); //Print results
    Serial.println(shtAmb);
    Serial.print("SHT21 Humidity");
    Serial.println(shtHum);
    Serial.print("MLX90614 Object Temp: ");
    Serial.println(mlxIR);
    Serial.println();
#endif

    // Write to file
    logfile.print(mSecs);
    logfile.print(", ");
    logfile.print(shtAmb);
    logfile.print(", ");
    logfile.print(shtHum);
    logfile.print(", ");
    logfile.print(mlxIR);
    logfile.print(", ");
    // @prucka print mlxAmb to SD card
    logfile.println();
    logfile.flush();                                  //close the file

    // @prucka Blink green LED for 150 ms.

    // Sleep
    Narcoleptic.delay(9522);
  }
}




// Functions --------------------------------------------------------------------------------------
void error(char *str) {			                        	// Used when initializing SD card...
  // @prucka Modify to turn on (but not turn off) red LED
  Serial.print("Error: ");
  Serial.println(str);
  while (1);
} //error()
