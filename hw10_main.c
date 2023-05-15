#include "nu32dip.h" // constants, functions for startup and UART
#include "ws2812b.h"
#include <stdio.h>

#define NUM_LED 8

//number of core ticks per second
#define CORE_PERIOD 24000000

//seconds to go 360deg around the color wheel
#define LOOP_PERIOD 4

int main(void) {
    NU32DIP_Startup();
    ws2812b_setup();
    float hue_shift = 0;
    float sat = 1;
    float bri = .1;
    NU32DIP_YELLOW = 1;
    NU32DIP_GREEN = 1;
    _CP0_SET_COUNT(0);
    
    while (1) {
        
        if(_CP0_GET_COUNT() > (CORE_PERIOD*LOOP_PERIOD)){
            _CP0_SET_COUNT(0);
        }
        
        //calculate the hue shift so that it goes linearly from 0 to 360 in
        //LOOP_PERIOD seconds
        hue_shift = (float)360*_CP0_GET_COUNT()/(CORE_PERIOD*LOOP_PERIOD);
    
        //Make color array, use HSBtoRGB(floats h,s,b)
        //base hue of each subsequent LED is shifted evenly
        //add the time based hue shift onto each 
        wsColor c[NUM_LED];
        for(int i = 0; i < NUM_LED; i++){
            float h = hue_shift + (float)(i*360)/NUM_LED;
            
            //wrap around if the maximum angle is exceeded
            if(h >= 360.0){
                h = h - 360.0;
            }
            c[i] = HSBtoRGB(h, sat, bri);
        }
        //send colors to be displayed by LED
        ws2812b_setColor(c, NUM_LED);
    }
}