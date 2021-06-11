#include <JetsonGPIO.h>
#include <unistd.h>
#include "buttons.h"

using namespace buttons;

Buttons::Buttons()
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
    m_polling_complete = false;

    while (m_polling){
        
        if (GPIO::input(UP_CHANNEL) == GPIO::LOW){
            // Send qt event
            start_button_clicked = true;
        }

        if (GPIO::input(DOWN_CHANNEL) == GPIO::LOW){
            // Send qt event
            stop_button_clicked = true;
        }

        if (GPIO::input(LEFT_CHANNEL) == GPIO::LOW){
            // Send qt event
            yes_button_clicked = true;
        }

        if (GPIO::input(RIGHT_CHANNEL) == GPIO::LOW){
            // Send qt event
            no_button_clicked = true;
        }

        if (GPIO::input(ENTER_CHANNEL) == GPIO::HIGH){
            // Send qt event
            exit_button_clicked = true;
        }
        usleep(200000); // sleep for 200 ms to avoid lots of key press events
    }
    m_polling_complete = true;
}

std::thread Buttons::PollInputsThread(){
    return std::thread(&Buttons::PollInputs, this);
}

void Buttons::Close(){
    printf("***SETTING BUTTON POLLING TO FALSE");
    m_polling=false;
    while(m_polling_complete){
        usleep(100000);
    }
    try
    {
        //TODO: This is causing segmentation faults. Need to figure out why.
        //GPIO::cleanup();
    }
    catch(const std::exception& e)
    {
        printf("Error in Buttons::Close %s\n",e.what());
    }
    
    
}

