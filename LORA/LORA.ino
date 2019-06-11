// ================================================================
// ===              INCLUDE CONFIGURATION FILE                  ===
// ===    INCLUDE DECLARATIONS, STRUCTS, AND FUNCTIONS FROM     ===
// ===            OTHER FILES AS SET IN CONFIG.H                ===
// ================================================================

// Config has to be first has it hold all user specified options
#include "config.h"

// sleep functions
//#include "sleep_rtc.h"

int tipCount = 0; //Tipping Bucket counter variable


// Preamble includes any relevant subroutine files based 
// on options specified in the above config
#include "loom_preamble.h"


// ================================================================ 
// ===                           SETUP                          ===
// ================================================================ 
void setup() 
{
	// LOOM_begin calls any relevant (based on config) LOOM device setup functions
	Loom_begin();	

//  countdown();
//
//
//  int rtcOK = InitializeRTC();
//  if(rtcOK < 0){
//    DEBUG_Println("error at InitializeRTC()");
//    return;
//  } 

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

	// // --- LoRa Node Example ---

	measure_sensors();			// Read sensors, store data in sensor state struct
	package_data(&bndl);			// Copy sensor data from state to provided bundle
  append_to_bundle_key_value(&bndl, "Tip_Ct", tipCount);
	
	print_bundle(&bndl);

	log_bundle(&bndl, SDCARD, "savefile.csv");
	send_bundle(&bndl, LORA);

//	delay(1000);

	additional_loop_checks();	// Miscellaneous checks

  // Set alarms (5 seconds here)
//  setRTCAlarm_Relative(0, 0, 5);
//
//  // Go to sleep, upon wakeup continue with loop
//  sleep();
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
