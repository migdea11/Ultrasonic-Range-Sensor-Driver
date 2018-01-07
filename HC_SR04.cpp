#include "HC_SR04.h"

#include <pigpio.h>
#include <unistd.h>
#include <wiringPi.h>


void echo_IRQ(int gpio, int level, uint32_t ticks)                  
{
    static uint32_t rise_time;
    static uint32_t fall_time;
    
    if (level == 1)
    {
        rise_time = ticks;
    }
    else if (level == 0)
    {
        fall_time = ticks;
        if (fall_time < rise_time)
        {
            fall_time += UINT32_MAX - rise_time;
        }
        HC_SR04* sensor = HC_SR04::Init(); 
        sensor->Write(rise_time, fall_time);
    }
}


void interval_trigger_thread()
{   
    HC_SR04* sensor = HC_SR04::Init();

    while(sensor->GetTriggerStatus())
    {
        // delay between pulses is in SendTrigger
        sensor->SendTrigger();
    }
}

HC_SR04::HC_SR04(uint8_t trig_pin, uint8_t echo_pin) :
    TRIG(trig_pin), ECHO(echo_pin)
{
    gpioInitialise();
    gpioSetMode(ECHO, PI_INPUT); 
    gpioSetISRFunc(ECHO, EITHER_EDGE, 0, echo_IRQ);
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

bool HC_SR04::GetTriggerStatus()
{
    return trigger_status_;
}

void HC_SR04::SendTrigger()
{
    gpioSetMode(TRIG, PI_OUTPUT);

	gpioWrite(TRIG, PI_OFF);
	gpioDelay(TRIG_WIDTH);
	gpioWrite(TRIG, PI_ON);
	gpioDelay(TRIG_WIDTH); 	
	gpioWrite(TRIG, PI_OFF);

	gpioDelay(trigger_interval_); 	
}

void HC_SR04::Start(uint32_t trigger_interval)
{
    trigger_status_ = true;
    trigger_interval_ = trigger_interval;
    trigger_thread_ = std::thread(interval_trigger_thread);
}

void HC_SR04::Stop()
{
    trigger_status_ = false;
    trigger_thread_.join();
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

