#include <JetsonGPIO.h>
#include <unistd.h>
#include "buttons.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <fstream>
#include "date.h"

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
    Display *display;
    
    display = XOpenDisplay(NULL);
    unsigned int keycode;

    std::ofstream file_object("logging.log");
    std::string s = date::format("%T", std::chrono::system_clock::now());
    char *output;
    int len = asprintf(&output, "%s | Polling buttons started.\n",s.c_str());
    file_object.write(output,len);
    free(output);
    file_object.flush();
    printf("Polling buttons started...\n");
    //LOG_F(INFO, "Polling buttons started...\n");
    while (m_polling){
       
        if (GPIO::input(UP_CHANNEL) == GPIO::LOW && !up_button_clicked){
            // Send up key press event
            //LOG_F(INFO, "Up button clicked.\n");
            len = asprintf(&output, "%s | Up button clicked.\n",s.c_str());
            file_object.write(output,len);
            free(output);
            file_object.flush();
            keycode = XKeysymToKeycode(display, XK_Up);
            XTestFakeKeyEvent(display, keycode, True, 0);
            XTestFakeKeyEvent(display, keycode, False, 0);
            XFlush(display);
            up_button_clicked = true;
        }
        else
        {
            up_button_clicked = false;
        }

        if (GPIO::input(DOWN_CHANNEL) == GPIO::LOW && !down_button_clicked){
            // Send down key press event
            //LOG_F(INFO, "Down button clicked.\n");
            len = asprintf(&output, "%s | Down button clicked.\n",s.c_str());
            file_object.write(output,len);
            free(output);
            file_object.flush();
            keycode = XKeysymToKeycode(display, XK_Down);
            XTestFakeKeyEvent(display, keycode, True, 0);
            XTestFakeKeyEvent(display, keycode, False, 0);
            XFlush(display);
            down_button_clicked = true;
        }
        else
        {
            down_button_clicked = false;
        }

        if (GPIO::input(LEFT_CHANNEL) == GPIO::LOW && !left_button_clicked){
            // Send left key press event
            //LOG_F(INFO, "Left button clicked.\n");
            len = asprintf(&output, "%s | Left button clicked.\n",s.c_str());
            file_object.write(output,len);
            free(output);
            file_object.flush();
            keycode = XKeysymToKeycode(display, XK_Left);
            XTestFakeKeyEvent(display, keycode, True, 0);
            XTestFakeKeyEvent(display, keycode, False, 0);
            XFlush(display);
            left_button_clicked = true;
        }
        else
        {
            left_button_clicked = false;
        }

        if (GPIO::input(RIGHT_CHANNEL) == GPIO::LOW && !right_button_clicked){
            // Send right key press event
            //LOG_F(INFO, "Right button clicked.\n");
            len = asprintf(&output, "%s | Right button clicked.\n",s.c_str());
            file_object.write(output,len);
            free(output);
            file_object.flush();
            keycode = XKeysymToKeycode(display, XK_Right);
            XTestFakeKeyEvent(display, keycode, True, 0);
            XTestFakeKeyEvent(display, keycode, False, 0);
            XFlush(display);
            right_button_clicked = true;
        }
        else
        {
            right_button_clicked = false;
        }

        if (GPIO::input(ENTER_CHANNEL) == GPIO::LOW && !enter_button_clicked){
            // Send space bar event
            //LOG_F(INFO, "Enter button clicked.\n");
            len = asprintf(&output, "%s | Enter button clicked.\n",s.c_str());
            file_object.write(output,len);
            free(output);
            file_object.flush();
            keycode = XKeysymToKeycode(display, XK_space);
            XTestFakeKeyEvent(display, keycode, True, 0);
            XTestFakeKeyEvent(display, keycode, False, 0);
            XFlush(display);
            enter_button_clicked = true;
        }
        else
        {
            enter_button_clicked = false;
        }

        usleep(200000); // sleep for 200 ms to avoid lots of key press events
    }
    //LOG_F(INFO, "Polling buttons stopped.\n");
    m_polling_complete = true;
}

std::thread Buttons::PollInputsThread(){
    return std::thread(&Buttons::PollInputs, this);
}

void Buttons::Close(){
    printf("***SETTING BUTTON POLLING TO FALSE");
    m_polling=false;
    while(m_polling_complete == false){
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

