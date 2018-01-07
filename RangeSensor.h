#include <cstdint>

class RangeSensor
{
public:
    virtual void Start(uint32_t polling_interval);
    virtual void Stop();
    virtual bool Read(uint16_t& distance);
};
