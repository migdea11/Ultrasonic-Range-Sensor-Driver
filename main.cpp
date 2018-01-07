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
    
    while(true)
    {
        uint16_t dist = 0;
        sensor->Read(dist);
        cout<<dist<<", ";
        usleep(100000);
    }
    return 0;
}
