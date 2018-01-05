/*
 * Driver for Ultrasonic Range Sensor HC-SR04
 *
 *  Device is operated by sending a 10us pulse on TRIG
 *  Then the device will send a pulse from 150us to 25ms (38ms if no return) on ECHO
 *  distance ranges from 2cm to 500cm
 *  recommended polling interval 50ms+
 *
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>

pthread_mutex_t mutex_time = PTHREAD_MUTEX_INITIALIZER;
struct timespec tick;
struct timespec tock;

class HC_SR04
{
    // pins used
    const uint8_t TRIG, ECHO;

    // delays in us
    static const uint16_t TrigWidth =    10;
    static const uint16_t EchoMin   =   150;
    static const uint16_t EchoMax   = 25000;
    static const uint16_t EchoFail  = 38000;
    static const uint32_t PollMax   = 50000;

    // cached distance and error
    uint16_t distance_;
    bool out_of_range_;

    // status of triggering thread
    pthread_mutex_t mutex_trig_ = PTHREAD_MUTEX_INITIALIZER;
    bool keep_triggering_ = false;

    HC_SR04(uint8_t trig_pin, uint8_t echo_pin);
    ~HC_SR04();
public:
    // Get the instance of the device
    // parameters only required for first call
    static HC_SR04 Instance(uint8_t trig_pin=0, uint8_t echo_pin=0);

    // Initializes a thread that polls the sensor
    // returns true if error occurred
    bool Start(uint32_t polling_interval);
    // Kills the thread
    // returns true if error occurred
    bool Stop();

    // when valid distance returns false and distance by ref
    // otherwise returns true and a distance of 0
    bool Read(uint16_t &distance);
};
