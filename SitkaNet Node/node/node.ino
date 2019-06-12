// ================================================================
// ===              INCLUDE CONFIGURATION FILE                  ===
// ===    INCLUDE DECLARATIONS, STRUCTS, AND FUNCTIONS FROM     ===
// ===            OTHER FILES AS SET IN CONFIG.H                ===
// ================================================================

// Config has to be first has it hold all user specified options
#include "config.h"

// custom sleep functions for use with DS3231
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
volatile bool omgFlag = false;

// Declare a MMA8451 object, does not rely on loom code
MMA8451 mma;


void omgISR(){
	detachInterrupt(digitalPinToInterrupt(OMG_PIN));
	// EIC->INTFLAG.reg = 0x01ff; // clear interrupt flags pending
	omgFlag = true;
}

void alarmISR(){
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
	// Any rtc related functionality is handled by sleep_rtc.h
	Loom_begin();
	
	if(InitializeRTC(ALARM_PIN, OMG_PIN) < 0) DEBUG_Println("error at InitializeRTC()");
	
	mma.setup();

	// countdown();

	// Any custom setup code
//   attachInterrupt(tipBucket_pin, tipBucket_ISR, FALLING);
}


// ================================================================ 
// ===                        MAIN LOOP                         ===
// ================================================================ 
void loop() 
{
	OSCBundle bndl;
	register_ISR(ALARM_PIN, alarmISR);
	register_ISR(OMG_PIN, omgISR);

	if(omgFlag == true){
		Serial.println("Wake from Accelerometer");
		omgFlag = false;
	}

	if(alarmFlag == true){
		Serial.println("Wake from alarm");
		alarmFlag = false;
	}

	// measure_sensors();			// Read sensors, store data in sensor state struct
	// package_data(&bndl);		// Copy sensor data from state to provided bundle
	// append_to_bundle_key_value(&bndl, "Tip Count: ", tipCount);
	// print_bundle(&bndl);

	// log_bundle(&bndl, SDCARD, "savefile.csv");
	// send_bundle(&bndl, LORA);
	// delay(1000);

	// additional_loop_checks();	// Miscellaneous checks

	// Set alarms (15 seconds here)
	setRTCAlarm_Relative(0, 0, 30);
	
	// Go to sleep, upon wakeup continue with loop
	sleep();

	// tipBucket();
}

// ================================================================ 
// ===                Interrupt Service Routines                ===
// ================================================================
void tipBucket()
{	
	// wakeUp_RTC();
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
