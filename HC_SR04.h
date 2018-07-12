/*
 * Driver for Ultrasonic Range Sensor HC-SR04
 *
 *  Device is operated by sending a 10us pulse on TRIG
 *  Then the device will send a pulse from 150us to 25ms (38ms if no return) on ECHO
 *  distance ranges from 2cm to 500cm
 *  recommended polling interval 50ms+
 *
 */

#include "RangeSensor.h"

#include <cstdio>
#include <ctime>
#include <cstdint>
#include <mutex>
#include <thread>


class HC_SR04 : public RangeSensor
{
private:
    // pins used
    const uint8_t TRIG, ECHO;

    // delays in us
    static const uint16_t TRIG_WIDTH   =    10;
    static const uint32_t TRIG_MIN_INT = 50000;
    static const uint16_t ECHO_MIN     =   150;
    static const uint16_t ECHO_MAX     = 25000;
    static const uint16_t ECHO_FAIL    = 38000;

    // size of median filter, must be odd
    static const uint8_t FILTER_SIZE = 5;

    // cached distance and error
    uint16_t distance_ = 0;
    bool valid_range_ = true;

    bool trigger_status_ = false;
    uint32_t trigger_interval_;
    std::thread trigger_thread_;
    uint32_t polling_interval_;

    HC_SR04(uint8_t trig_pin, uint8_t echo_pin);
    ~HC_SR04();
public:
    // Creates Sensor
    static HC_SR04* Init(uint8_t trig_pin, uint8_t echo_pin);
    
    // Get the instance of the device
    static HC_SR04* Instance();

    // gets the status variable used to stop the trigger thread
    bool GetTriggerStatus();

    // will send the pulse to trigger the sensor
    void SendTrigger();

    // Initializes a thread that polls the sensor
    void Start(uint32_t trigger_interval=TRIG_MIN_INT, uint32_t polling_interval=50) override final;
    // Kills the thread
    void Stop() override final;

    // used to poll echo pin
    bool PollEcho();

    // when valid distance returns true and distance by ref
    // otherwise returns false and a distance of 0
    bool Read(uint16_t& distance) override final;

    // will take the times of the rising and falling edge and 
    void Write(uint32_t rise, uint32_t fall);
};
