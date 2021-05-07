
/*-----------------------------------------------------

Creator:            Jorgen Andreas Mo
Creation date:      05.05-2021
Current version:    V0.0
Update log:
    - V0.0  - 05.05-2021

------------------------------------------------------*/


#pragma once




class Button{
    private:
        bool buttonState;
        bool buttonStatePrev;

        bool buttonPosEdge;
        bool buttonPosEdgePrev;
        unsigned long buttonPosEdgeTS;

        bool buttonNegEdge;
        bool buttonNegEdgePrev;
        unsigned long buttonNegEdgeTS;

        unsigned long millisPrev;        

    public:
        void            setState(bool);                 // Reads out the current button state. To be called in the start of every loop
        bool            readState();                    // Returns the current state of the button

        bool            posEdge();                      // Positive edge detection
        unsigned long   posEdgeTS();                    // Positive edge detection timestamp

        bool            negEdge();                      // Negative edge detection
        unsigned long   negEdgeTS();                    // Negative edge detection timestamp

        bool            onDelayMS(int);                 // Decides how many milliseconds on-delay for current button
        bool            onDelayS(int);                  // Decides how many seconds on-delay for current button


        bool            offDelayMS(int);                // Decides how many milliseconds off-delay for current button
        bool            offDelayS(int);                 // Decides how many seconds off-delay for current button
        
};