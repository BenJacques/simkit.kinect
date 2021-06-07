#include <JetsonGPIO.h>

const int UP_CHANNEL = 16;
const int DOWN_CHANNEL = 18;
const int LEFT_CHANNEL = 19;
const int RIGHT_CHANNEL = 21;
const int ENTER_CHANNEL = 22;

void setup_pins(){
    // UP key
    GPIO::setup(UP_CHANNEL,GPIO::IN);

    // DOWN key
    GPIO::setup(DOWN_CHANNEL,GPIO::IN);

    // LEFT key
    GPIO::setup(LEFT_CHANNEL,GPIO::IN);

    // RIGHT key
    GPIO::setup(RIGHT_CHANNEL,GPIO::IN);

    // ENTER key
    GPIO::setup(ENTER_CHANNEL,GPIO::IN);
}

void poll_inputs(){

    while (true){
        if (GPIO::input(UP_CHANNEL) == GPIO::HIGH){
            // Send qt event
        }

        if (GPIO::input(DOWN_CHANNEL) == GPIO::HIGH){
            // Send qt event
        }

        if (GPIO::input(LEFT_CHANNEL) == GPIO::HIGH){
            // Send qt event
        }

        if (GPIO::input(RIGHT_CHANNEL) == GPIO::HIGH){
            // Send qt event
        }

        if (GPIO::input(ENTER_CHANNEL) == GPIO::HIGH){
            // Send qt event
        }
    }
}

void close(){
    GPIO::cleanup();
}