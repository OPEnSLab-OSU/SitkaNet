// ================================================================
// ===              INCLUDE CONFIGURATION FILE                  ===
// ===    INCLUDE DECLARATIONS, STRUCTS, AND FUNCTIONS FROM     ===
// ===            OTHER FILES AS SET IN CONFIG.H                ===
// ================================================================

// Config has to be first has it hold all user specified options
#include "config.h"

// Preamble includes any relevant subroutine files based 
// on options specified in the above config
#include "loom_preamble.h"

int tipCount = 0; //Tipping Bucket counter variable

// ================================================================ 
// ===                           SETUP                          ===
// ================================================================ 
void setup() 
{
	// LOOM_begin calls any relevant (based on config) LOOM device setup functions
	Loom_begin();	

	// Any custom setup code
  attachInterrupt(tipBucket_pin, tipBucket_ISR, FALLING);
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
	 append_to_bundle_key_value(&bndl, "Tip Count: ", tipCount);
	 print_bundle(&bndl);

	 //log_bundle(&bndl, SDCARD, "savefile.csv");
	// send_bundle(&bndl, LORA);

	 delay(1000);

	 additional_loop_checks();	// Miscellaneous checks
	// // --- End Example ---
}

// ================================================================ 
// ===                Interrupt Service Routines                ===
// ================================================================
void tipBucket_ISR(void)
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
