#include <iostream>
#include <cstdint>
#include <unistd.h>
using namespace std;

#include "HC_SR04.h"

int main()
{
    cout<<"init"<<endl;
    HC_SR04* sensor = HC_SR04::Init(20, 21);

    cout<<"start"<<endl;
    sensor->Start();
   
    uint16_t numRead = 0;
    while(numRead++ < 500)
    {
        uint16_t poll = 100; // in ms
        uint16_t dist = 0;
        sensor->Read(dist);
        if ((numRead % 10) == 0)
        {
            cout << flush;
        }
        cout<<dist<<", ";
        usleep(poll * 1000);
    }
    sensor->Stop();
    return 0;
}
