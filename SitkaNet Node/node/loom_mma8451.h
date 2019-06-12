// ================================================================
// ===                        LIBRARIES                         ===
// ================================================================
#include <Wire.h>
#include <Adafruit_Sensor.h>
// library for interfacing with the accelerometer
#include <Adafruit_MMA8451.h>

// ================================================================
// ===                        STRUCTURES                        ===
// ================================================================
struct state_mma8451_t
{
    Adafruit_MMA8451 inst;
    sensors_event_t event;
    uint orient;
};

// ================================================================
// ===                   GLOBAL DECLARATIONS                    ===
// ================================================================
struct state_mma8451_t state_mma8451;
#define mms8451_addr 0x1D
const uint8_t mma8451_port = 5; 

// ================================================================
// ===                   FUNCTION PROTOTYPES                    ===
// ================================================================
bool setup_mma8451();
void package_mma8451(OSCBundle *bndl, char packet_header_string[], uint8_t port);
void measure_mma8451();
void configure_mma8451();
#if LOOM_DEBUG == 1
void details_mma8451();
#endif

// ================================================================
// ===                          SETUP                           ===
// ================================================================
//
// Runs any mma8451 setup and initialization
//
// @return setup success; 0 if success, -1 if failure
//
bool setup_mma8451()
{
    state_mma8451.inst = Adafruit_MMA8451();
    if (state_mma8451.inst.begin())
    {
        LOOM_DEBUG_Println("Initialized mma8451");
        configure_mma8451();
        return true;
    }
    else
    {
        LOOM_DEBUG_Println("Failed to initialize mma8451");
        return false;
    }

}

// ================================================================
// ===                        FUNCTION DEFINITIONS              ===
// ================================================================

// --- PACKAGE mma8451 ---
//
// Adds an OSC Message of last read mma8451 sensor readings to provided OSC bundle
//
// @param bndl                  The OSC bundle to be added to
// @param packet_header_string  The device-identifying string to prepend to OSC messages
// @param port                  Which port of the multiplexer the device is plugged into
//
void package_mma8451(OSCBundle *bndl, char packet_header_string[], uint8_t port)
{
    char address_string[255];
    sprintf(address_string, "%s%s%d%s", packet_header_string, "/port", port, "/mma8451/data");

    OSCMessage msg = OSCMessage(address_string);

    msg.add("x").add(state_mma8451.event.acceleration.x);
    msg.add("y").add(state_mma8451.event.acceleration.y);
    msg.add("z").add(state_mma8451.event.acceleration.z);
    msg.add("orient").add(state_mma8451.orient);

    bndl->add(msg);
}

// --- MEASURE MMA8451 ---
//
// Gets the current sensor readings of the mma8451 and stores into its state struct
//
void measure_mma8451()
{

    state_mma8451.inst.getEvent(&(state_mma8451.event));
    state_mma8451.orient = state_mma8451.inst.getOrientation();

    if(LOOM_DEBUG == 1){
        Serial.print(F("[ "));
        Serial.print(millis());
        Serial.print(F(" ms ] "));
        Serial.print(F("X: "));
        Serial.print(state_mma8451.event.acceleration.x);
        Serial.print(F("  "));
        Serial.print(F("Y: "));
        Serial.print(state_mma8451.event.acceleration.y);
        Serial.print(F("  "));
        Serial.print(F("Z: "));
        Serial.print(state_mma8451.event.acceleration.z);
        Serial.print(F("  "));
        Serial.print(F("Orientation: "));
        Serial.print(state_mma8451.orient);
        // Serial.println(F("  "));
    }
}

// --- CONFIGURE mma8451 ---
//
//
//
void configure_mma8451()
{
    state_mma8451.inst.setRange(MMA8451_RANGE_2_G);
    state_mma8451.inst.setDataRate(MMA8451_DATARATE_6_25HZ);
    
}


// --- DETAILS mma8451  ---
//
// With Loom debug enabled, allows for the printing of the mma8451 details to be
// printed to the Serial monitor
//
#if LOOM_DEBUG == 1
void details_mma8451()
{
    sensor_t sensor;
    state_mma8451.inst.getSensor(&sensor);
    Serial.println(F("------------------------------------"));
    Serial.print(F("Sensor:       "));
    Serial.println(sensor.name);
    Serial.print(F("Driver Ver:   "));
    Serial.println(sensor.version);
    Serial.print(F("Unique ID:    "));
    Serial.println(sensor.sensor_id);
    Serial.print(F("Max Value:    "));
    Serial.print(sensor.max_value);
    Serial.println(F(" lux"));
    Serial.print(F("Min Value:    "));
    Serial.print(sensor.min_value);
    Serial.println(F(" lux"));
    Serial.print(F("Resolution:   "));
    Serial.print(sensor.resolution, 4);
    Serial.println(F(" lux"));
    Serial.println(F("------------------------------------"));
    Serial.println(F(""));
    delay(500);
}
#endif
