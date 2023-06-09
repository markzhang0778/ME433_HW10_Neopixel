// WS2812B library

#include "ws2812b.h"
// other includes if necessary for debugging

// Timer2 delay times, you can tune these if necessary
#define LOWTIME 15 // number of 48MHz cycles to be low for 0.35uS
#define HIGHTIME 65 // number of 48MHz cycles to be high for 1.35uS

// setup Timer2 for 48MHz, and setup the output pin
void ws2812b_setup() {
    T2CONbits.TCKPS = 0; // Timer2 prescaler N=1 (1:1)
    PR2 = 65535; // maximum period
    TMR2 = 0; // initialize Timer2 to 0
    T2CONbits.ON = 1; // turn on Timer2

    //Initialize A0 as output, start off
    TRISBbits.TRISB6 = 0;
    LATBbits.LATB6 = 0;
}

// build an array of high/low times from the color input array, then output the high/low bits
void ws2812b_setColor(wsColor * c, int numLEDs) {
    int numBits = 3 * 8 * numLEDs; //number of logic bits, 24 per color, 1 color per LED 
    volatile unsigned int delay_times[(2*numBits)+1]; 
    // start at time at 0
    delay_times[0] = 0;
    
    int nB = 0; // which high/low bit you are storing, 2 per color bit, 24 color bits per WS2812B
	
    // loop through each WS2812B    
    for (int i = 0; i < numLEDs; i++) {
        // loop through each color bit, MSB first
        //send red color bits first
        for (int j = 7; j >= 0; j--) {
            // if the bit is a 1
            if (c[i].r & (1<<j)) {
                // the high is longer
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                
            } 
            // if the bit is a 0
            else {
                // the low is longer
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;                
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
            }
        }
        // do it again for green
        for (int j = 7; j >= 0; j--) {
            // if the bit is a 1
            if (c[i].g & (1<<j)) {
                // the high is longer
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                
            } 
            // if the bit is a 0
            else {
                // the low is longer
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;                
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
            }
        }
        
		// do it again for blue
        for (int j = 7; j >= 0; j--) {
            // if the bit is a 1
            if (c[i].b & (1<<j)) {
                // the high is longer
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                
            } 
            // if the bit is a 0
            else {
                // the low is longer
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;                
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
            }
        }
    }

    // turn on the pin for the first high/low
    
    TMR2 = 0; // start the timer
    int x = 0;
    while (x <= numBits * 2){
        //logic 1 or 0 always starts high, so pull data pin high and wait
        x += 1;
        LATBbits.LATB6 = 1;
        while (TMR2 < delay_times[x]) {
            Nop();
        }
        
        //pull the data pin low and wait the defined duration
        x += 1;
        LATBbits.LATB6 = 0;
        while(TMR2 < delay_times[x]){
            Nop();
        }
        
    }
    
    //reset
    LATBbits.LATB6 = 0;
    TMR2 = 0;
    while(TMR2 < 2400){Nop();} // wait 50uS, reset condition
}

// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}