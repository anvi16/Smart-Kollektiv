

#include "HWClass.h"
#include <Arduino.h>



// Reads out the current button state. To be called in the start of every loop
void Button::setState(bool state){
    
    // Read state of input
    buttonState = state;

    // Check for millis rollover
    if (millisPrev > millis()){
        buttonPosEdgeTS = millis();
        buttonNegEdgeTS = millis();
    }

    
    // Positive edge detection
    if (!buttonStatePrev && buttonState){
        buttonPosEdge = 0x1;                // Set to HIGH
        buttonPosEdgeTS = millis();
    }
    else{
        buttonPosEdge = 0x0;                // Set to LOW
    }


    // Negative edge detection
    if (buttonStatePrev && !buttonState){
        buttonNegEdge = 0x1;                // Set to HIGH
        buttonNegEdgeTS = millis();
    }
    else{
        buttonNegEdge = 0x0;                // Set to LOW
    }

   
    // Update prev button state at the end of method
    buttonStatePrev = buttonState;
    buttonPosEdgePrev = buttonPosEdge;
    buttonNegEdgePrev = buttonNegEdge;

    millisPrev = millis();

}



// POSITIVE EDGE

// Returns HIGH value when positive edge is detected
bool Button::posEdge(){
    return buttonPosEdge;
}

// Returns [unsigned long] value of positive edge detection timestamp
unsigned long Button::posEdgeTS(){
    return buttonPosEdgeTS;
}




// NEGATIVE EDGE
// Returns HIGH value when negative edge is detected
bool Button::negEdge(){
    return buttonNegEdge;
}

// Returns [unsigned long] value of negative edge detection timestamp
unsigned long Button::negEdgeTS(){
    return buttonNegEdgeTS;
}




// On delay timer in millisecond format. Button must be kept pressed for more than "ms" milliseconds to go HIGH.  
bool Button::onDelayMS(int ms){
    
    if (buttonState   &&   (buttonPosEdgeTS > buttonNegEdgeTS)   &&   (millis() > (buttonPosEdgeTS + ms))){                 // Control that button has not been let go during the time 
        return 0x1;
    }
    else{
        return 0x0;
    }
}

// On delay timer in second format. Button must be kept pressed for more than "s" seconds to go HIGH.  
bool Button::onDelayS(int s){

    if (buttonState   &&   (buttonPosEdgeTS > buttonNegEdgeTS)   &&   (millis() > (buttonPosEdgeTS + s*1000))){                 // Control that button has not been let go during the time 
        return 0x1;
    }
    else{
        return 0x0;
    }
}



// Returns a HIGH value for "ms" milliseconds after button is let go
bool Button::offDelayMS(int ms){
    
    if (buttonState){                                               // If button is pressed, returns HIGH
        return 0x1;
    }

    else if (!buttonState && ((millis() - ms) < buttonNegEdgeTS)){  // If less than "ms" milliseconds since button was let go, button stays HIGH
        return 0x1;
    }

    else{                                                           // Else return LOW
        return 0x0;
    }
}

// Returns a HIGH value for "s" seconds after button is let go
bool Button::offDelayS(int s){
    
    if (buttonState){                                                   // If button is pressed, returns HIGH
        return 0x1;
    }

    else if (!buttonState && ((millis() - s*1000) < buttonNegEdgeTS)){  // If less than "s" seconds since button was let go, button stays HIGH
        return 0x1;
    }

    else{                                                               // Else return LOW
        return 0x0;
    }
}

