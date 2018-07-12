#include <cstdint>

class RangeSensor
{
public:
    virtual void Start(uint32_t trigger_interval, uint32_t polling_interval) = 0;
    virtual void Stop() = 0;
    virtual bool Read(uint16_t& distance) = 0;
};
