// ================================================================
// ===              INCLUDE CONFIGURATION FILE                  ===
// ===    INCLUDE DECLARATIONS, STRUCTS, AND FUNCTIONS FROM     ===
// ===            OTHER FILES AS SET IN CONFIG.H                ===
// ================================================================

//*** NOTE: This program uses a modified Adafruit_mma8451 source file. 
//*** DO NOT USE the arduino provided source file.
//*** Use the one in this directory, /Adaruit_MMA8451_library, and put in your arduino libraries directory
// ** overwriting the existing files 

// Config has to be first has it hold all user specified options
#include "config.h"

// sleep functions
#include "sleep_rtc.h"

// The mma8451 accelerometer is not managed by Loom
#include "sitkaNet_mma8451.h"

// Preamble includes any relevant subroutine files based 
// on options specified in the above config
#include "loom_preamble.h"

int tipCount = 0; //Tipping Bucket counter variable
#define OMG_PIN 5
#define ALARM_PIN 6
volatile bool alarmFlag = false;
int omgCnt = 0;
volatile bool omgFlag = false;


// Declare a global MMA8451 object, does not rely on loom code
MMA8451 mma;

// ISR's for alarm and accelerometer
void omgISR()
{
	detachInterrupt(digitalPinToInterrupt(OMG_PIN));
	// EIC->INTFLAG.reg = 0x01ff; // clear interrupt flags pending
	omgFlag = true;

}

void alarmISR()
{
	detachInterrupt(digitalPinToInterrupt(ALARM_PIN));
	// EIC->INTFLAG.reg = 0x01ff; // clear interrupt flags pending
	alarmFlag = true;
}


// ================================================================ 
// ===                           SETUP                          ===
// ================================================================ 
void setup() 
{
	// LOOM_begin calls any relevant (based on config) LOOM device setup functions
	Loom_begin();

	if (InitializeRTC(ALARM_PIN, OMG_PIN) < 0)
		DEBUG_Println("error at InitializeRTC()");

	// public method for establishing all MMA related parameters and settings
	mma.setup();

	// Any custom setup code
  pinMode(12, INPUT_PULLUP);
  attachInterrupt(12, tipBucket_ISR, FALLING);
}

// ================================================================ 
// ===                        MAIN LOOP                         ===
// ================================================================ 
void loop() 
{
	OSCBundle bndl;

	// attach isr's to a specific pin
	register_ISR(ALARM_PIN, alarmISR);
	register_ISR(OMG_PIN, omgISR);

	if (omgFlag == true)
	{
		Serial.println("Wake from Accelerometer");
		omgFlag = false;
		omgCnt++;
		Serial.print("omgCnt: ");
		Serial.println(omgCnt);
	}

	if (alarmFlag == true)
	{
		Serial.println("Wake from alarm");
		alarmFlag = false;
	}

	// // --- LoRa Node Example ---

	measure_sensors();			// Read sensors, store data in sensor state struct
	package_data(&bndl);			// Copy sensor data from state to provided bundle
  	append_to_bundle_key_value(&bndl, "Tip_Ct", tipCount);
	
	print_bundle(&bndl);

	log_bundle(&bndl, SDCARD, "savefile.csv");
	send_bundle(&bndl, LORA);

	additional_loop_checks();	// Miscellaneous checks

  	
	// Go to sleep, if the accelerometer was not disturbed
	// The acc. was getting triggered incorerctly upon first sleep cycle,
	// so a simple counter is used to "ignore" the first acc. alarm.
	// If the omgCnt == 2, then we know that the acc event was for real. 
	if(omgCnt < 2){
 		setRTCAlarm_Relative(0, 0, 15);
 		sleep();
	}

	// // --- End Example ---



	// // --- LoRa Evaporometer Example ---

	// measure_sensors();			// Read sensors, store data in sensor state struct
	// package_data(&bndl);			// Copy sensor data from state to provided bundle
	
	// print_bundle(&bndl);

	// log_bundle(&bndl, SDCARD, "savefile.csv");
	// send_bundle(&bndl, LORA);

	// sleep_for(5, MINUTES, STANDBY);

	// additional_loop_checks();	// Miscellaneous checks
	// // --- End Example ---



	// // --- Common Example ---

	// receive_bundle(&bndl, WIFI);	// Receive messages
	// if (bndl.size()) {
	// 	print_bundle(&bndl);		// Print bundle if LOOM_DEBUG enabled
	// }
	// process_bundle(&bndl);			// Dispatch message to correct handling functions

	// measure_sensors();				// Read sensors, store data in sensor state struct
	// package_data(&bndl);			// Copy sensor data from state to provided bundle

	// // print_bundle(&bndl);			// Print bundle if LOOM_DEBUG enabled

	// send_bundle(&bndl, WIFI);		// Send bundle of packaged data
	// // log_bundle(&bndl, PUSHINGBOX);	// Send bundle to Google Sheet
	// // log_bundle(&bndl, SDCARD, "Ishield.csv");	// Send bundle to Google Sheet

	// additional_loop_checks();			// Miscellaneous checks
	// // --- End Example ---
}

// ================================================================ 
// ===                Interrupt Service Routines                ===
// ================================================================
void tipBucket_ISR()
{
  unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 200)
  {
    tipCount++;
  }
  lastInterruptTime = interruptTime;
}


// ================================================================ 
// ===                 High-Level API Functions                 === 
// ================================================================ 

// void receive_bundle(OSCBundle *bndl, CommPlatform platform);
// void process_bundle(OSCBundle *bndl);
// void measure_sensors();
// void package_data(OSCBundle *bndl);
// void send_bundle(OSCBundle *bndl, CommPlatform platform, int port);
// void send_bundle(OSCBundle *bndl, CommPlatform platform);
// void log_bundle(OSCBundle *bndl, LogPlatform platform, char* file); // filename required for SD files
// void log_bundle(OSCBundle *bndl, LogPlatform platform);
// bool bundle_empty(OSCBundle *bndl);
// void additional_loop_checks();
// void sleep_for(int amount, TimeUnits units, SleepMode mode);
// void sleep_until_time(SleepMode mode, int hour, int min, int sec);
// void append_to_bundle_key_value(OSCBundle *bndl, char* key, T elem);

// CommPlatforms: WIFI, LORA, NRF
// LogPlatforms:  PUSHINGBOX, SDCARD, OLED
// TimeUnits: MINUTES, SECONDS
// SleepMode: STANDBY, SLEEPYDOG

// Print Macro
// LOOM_DEBUG_Println
