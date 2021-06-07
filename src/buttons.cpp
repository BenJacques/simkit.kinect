#include <JetsonGPIO.h>
#include <unistd.h>
#include "buttons.h"

using namespace buttons;

Buttons::Buttons():
    m_polling(false)
{
   
}

Buttons::~Buttons()
{
    // Close();
}

const int UP_CHANNEL = 16;
const int DOWN_CHANNEL = 18;
const int LEFT_CHANNEL = 19;
const int RIGHT_CHANNEL = 21;
const int ENTER_CHANNEL = 22;

void Buttons::SetupPins(){
    // Board mode must be set first
    GPIO::setmode(GPIO::BOARD);

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

void Buttons::PollInputs(){
    m_polling = true;
    while (m_polling){
        printf("polling buttons...\n");
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
        usleep(200000); // sleep for 200 ms to avoid lots of key press events
    }
}

void Buttons::Close(){
    m_polling=false;
    GPIO::cleanup();
}

