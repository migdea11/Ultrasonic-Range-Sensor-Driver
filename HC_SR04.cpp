#include "HC_SR04.h"

#include <pigpio.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>


void interval_trigger_thread()
{   
    HC_SR04* sensor = HC_SR04::Instance();

    while(sensor->GetTriggerStatus())
    {
        // delay between pulses is in SendTrigger
        sensor->SendTrigger();
        uint32_t rise_time;
        uint32_t fall_time;
        struct timespec gettime_now;

        while(!sensor->PollEcho());
        clock_gettime(CLOCK_REALTIME, &gettime_now);
        rise_time = gettime_now.tv_nsec / 1000; // time in ms
        while(sensor->PollEcho());
        clock_gettime(CLOCK_REALTIME, &gettime_now);
        fall_time = gettime_now.tv_nsec / 1000; // time in ms
        
        if (fall_time < rise_time)
        {
            fall_time = UINT32_MAX - rise_time;
        }
        sensor->Write(rise_time, fall_time);
    }
}

HC_SR04::HC_SR04(uint8_t trig_pin, uint8_t echo_pin) :
    TRIG(trig_pin), ECHO(echo_pin)
{
    int32_t sec = 0, msec = 0;
    gpioTime(PI_TIME_RELATIVE, &sec, &msec);
    if (sec == 0)
    {
        gpioInitialise();
    }
}

HC_SR04::~HC_SR04()
{
    Stop();
}

HC_SR04* HC_SR04::Init(uint8_t trig_pin, uint8_t echo_pin)
{
    static HC_SR04 sensor(trig_pin, echo_pin);
    return &sensor;
}

HC_SR04* HC_SR04::Instance()
{
    return Init(0,0);
}

bool HC_SR04::GetTriggerStatus()
{
    return trigger_status_;
}

void HC_SR04::SendTrigger()
{
	gpioDelay(trigger_interval_); 	

	gpioWrite(TRIG, PI_OFF);
	gpioDelay(TRIG_WIDTH);
	gpioWrite(TRIG, PI_ON);
	gpioDelay(TRIG_WIDTH); 	
	gpioWrite(TRIG, PI_OFF);
}

void HC_SR04::Start(uint32_t trigger_interval, uint32_t polling_interval)
{
    gpioSetMode(TRIG, PI_OUTPUT);
    gpioSetMode(ECHO, PI_INPUT); 
    
    trigger_status_ = true;
    trigger_interval_ = trigger_interval;
    polling_interval_ = polling_interval;
    trigger_thread_ = std::thread(interval_trigger_thread);
}

void HC_SR04::Stop()
{
    trigger_status_ = false;
    trigger_thread_.join();
}

bool HC_SR04::PollEcho()
{
    gpioSetMode(ECHO, PI_INPUT);
    bool read = gpioRead(ECHO);
    gpioDelay(polling_interval_);
    return read;
}

bool HC_SR04::Read(uint16_t& distance)
{
    distance = distance_;
    return valid_range_;
}

void HC_SR04::Write(uint32_t rise, uint32_t fall)
{
    distance_ = 1000 * (fall - rise) / 58;
    // add bound checking
    valid_range_ = true;
}

