// ================================================================
// ===                        LIBRARIES                         ===
// ================================================================
#include <Wire.h>
#include <Adafruit_Sensor.h>

// library for interfacing with the accelerometer
#include <Adafruit_MMA8451.h>
#define mms8451_addr 0x1D
#define mux_addr 0x71

class MMA8451
{
private:
    struct state_mma8451_t
    {
        Adafruit_MMA8451 inst = Adafruit_MMA8451();
        sensors_event_t event;
        uint orient;
    };

    struct state_mma8451_t state_mma8451;
    const uint8_t port = 2;

    // --- CONFIGURE mma8451 ---
    //
    //
    void configure_mma8451()
    {
        state_mma8451.inst.setRange(MMA8451_RANGE_2_G);
        state_mma8451.inst.setDataRate(MMA8451_DATARATE_6_25HZ);
        configInterrupts();
    }

    void select(uint8_t i)
    {
        if (i > 7)
            return;

        Wire.beginTransmission(mux_addr);
        Wire.write(1 << i);
        Wire.endTransmission();
    }

    void configInterrupts()
    {
        uint8_t dataToWrite = 0;
        // MMA8451_REG_CTRL_REG2
        // sysatem control register 2

        //dataToWrite |= 0x80;    // Auto sleep/wake interrupt
        //dataToWrite |= 0x40;    // FIFO interrupt
        //dataToWrite |= 0x20;    // Transient interrupt - enabled
        //dataToWrite |= 0x10;    // orientation
        //dataToWrite |= 0x08;    // Pulse interrupt
        //dataToWrite |= 0x04;    // Freefall interrupt
        //dataToWrite |= 0x01;    // data ready interrupt, MUST BE ENABLED FOR USE WITH ARDUINO

        // MMA8451_REG_CTRL_REG3
        // Interrupt control register

        dataToWrite = 0;
        dataToWrite |= 0x80; // FIFO gate option for wake/sleep transition, default 0, Asserting this allows the accelerometer to collect data the moment an impluse happens and percerve that data because the FIFO buffer is blocked. Thus at the end of a wake cycle the data from the initial transient wake up is still in the buffer
        dataToWrite |= 0x40; // Wake from transient interrupt enable
        //dataToWrite |= 0x20;    // Wake from orientation interrupt enable
        //dataToWrite |= 0x10;    // Wake from Pulse function enable
        //dataToWrite |= 0x08;    // Wake from freefall/motion decect interrupt
        //dataToWrite |= 0x02;    // Interrupt polarity, 1 = active high
        dataToWrite |= 0x00; // (0) Push/pull or (1) open drain interrupt, determines whether bus is driven by device, or left to hang

        state_mma8451.inst.writeRegister8_public(MMA8451_REG_CTRL_REG3, dataToWrite);

        dataToWrite = 0;

        // MMA8451_REG_CTRL_REG4
        // Interrupt enable register, enables interrupts that are not commented

        //dataToWrite |= 0x80;    // Auto sleep/wake interrupt
        //dataToWrite |= 0x40;    // FIFO interrupt
        dataToWrite |= 0x20; // Transient interrupt - enabled
        //dataToWrite |= 0x10;    // orientation
        //dataToWrite |= 0x08;    // Pulse interrupt
        //dataToWrite |= 0x04;    // Freefall interrupt
        dataToWrite |= 0x01; // data ready interrupt, MUST BE ENABLED FOR USE WITH ARDUINO
        state_mma8451.inst.writeRegister8_public(MMA8451_REG_CTRL_REG4, dataToWrite | 0x01);

        dataToWrite = 0;

        // MMA8451_REG_CTRL_REG5
        // Interrupt pin 1/2 configuration register, bit == 1 => interrupt to pin 1
        // see datasheet for interrupt's description, threshold int routed to pin 1
        // comment = int2, uncoment = int1

        //dataToWrite |= 0x80;    // Auto sleep/wake
        //dataToWrite |= 0x40;    // FIFO
        dataToWrite |= 0x20; // Transient, asserting this routes transients interrupts to INT1 pin
        //dataToWrite |= 0x10;    // orientation
        //dataToWrite |= 0x08;    // Pulse
        //dataToWrite |= 0x04;    // Freefall
        //dataToWrite |= 0x01;    // data ready

        state_mma8451.inst.writeRegister8_public(MMA8451_REG_CTRL_REG5, dataToWrite);

        dataToWrite = 0;

        // MMA8451_REG_TRANSIENT_CFG
        //dataToWrite |= 0x10;  // Latch enable to capture accel values when interrupt occurs
        dataToWrite |= 0x08; // Z transient interrupt enable
        dataToWrite |= 0x04; // Y transient interrupt enable
        dataToWrite |= 0x02; // X transient interrupt enable
        //dataToWrite |= 0x01;    // High-pass filter bypass
        state_mma8451.inst.writeRegister8_public(MMA8451_REG_TRANSIENT_CFG, dataToWrite);

        Serial.print("MMA8451_REG_TRANSIENT_CFG: ");
        Serial.println(state_mma8451.inst.readRegister8(MMA8451_REG_TRANSIENT_CFG), HEX);

        dataToWrite = 0;

        // MMA8451_REG_TRANSIENT_THS
        // Transient interrupt threshold in units of .06g
        //Acceptable range is 1-127
        dataToWrite = 0x01;
        state_mma8451.inst.writeRegister8_public(MMA8451_REG_TRANSIENT_THS, dataToWrite);

        dataToWrite = 0;

        // MMA8451_REG_TRANSIENT_CT  0x20
        dataToWrite = 0; // value is 0-255 for numer of counts to debounce for, depends on ODR
        state_mma8451.inst.writeRegister8_public(MMA8451_REG_TRANSIENT_CT, dataToWrite);

        dataToWrite = 0;
    }

public:
    MMA8451() {}

    virtual ~MMA8451() {}

    // ================================================================
    // ===                          SETUP                           ===
    // ================================================================
    //
    // Runs any mma8451 setup and initialization
    //
    // @return setup success; 0 if success, -1 if failure
    //
    bool setup()
    {
        state_mma8451.inst = Adafruit_MMA8451();
        select(port);
        if (state_mma8451.inst.begin())
        {
            configure_mma8451();
            Serial.println("Initialized mma8451");
            return true;
        }
        else
        {
            Serial.print("Failed to initialize mma8451");
            return false;
        }
    }

    // --- MEASURE MMA8451 ---
    //
    // Gets the current sensor readings of the mma8451 and stores into its state struct
    //
    void measure()
    {
        select(port);
        state_mma8451.inst.getEvent(&(state_mma8451.event));
        state_mma8451.orient = state_mma8451.inst.getOrientation();

        if (LOOM_DEBUG == 1)
        {
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
            Serial.print("\n");
            // Serial.println(F("  "));
        }
    }

    // --- DETAILS mma8451  ---
    //
    // Allows for the printing of the mma8451 details to be
    // printed to the Serial monitor
    //
    void details()
    {
        select(port);
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
};

// ================================================================
// ===                        FUNCTION DEFINITIONS              ===
// ================================================================
// void package_mma8451(OSCBundle *bndl, char packet_header_string[], uint8_t port);

// --- PACKAGE mma8451 ---
//
// Adds an OSC Message of last read mma8451 sensor readings to provided OSC bundle
//
// @param bndl                  The OSC bundle to be added to
// @param packet_header_string  The device-identifying string to prepend to OSC messages
// @param port                  Which port of the multiplexer the device is plugged into
//
// void package_mma8451(OSCBundle *bndl, char packet_header_string[], uint8_t port)
// {
//     char address_string[255];
//     sprintf(address_string, "%s%s%d%s", packet_header_string, "/port", port, "/mma8451/data");

//     OSCMessage msg = OSCMessage(address_string);

//     msg.add("x").add(state_mma8451.event.acceleration.x);
//     msg.add("y").add(state_mma8451.event.acceleration.y);
//     msg.add("z").add(state_mma8451.event.acceleration.z);
//     msg.add("orient").add(state_mma8451.orient);

//     bndl->add(msg);
// }