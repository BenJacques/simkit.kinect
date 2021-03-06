#ifndef BUTTONS_H
#define BUTTONS_H
#include <atomic>
#include <thread>
namespace buttons
{
class Buttons
{


public:

    std::atomic<bool> up_button_clicked;
    std::atomic<bool> down_button_clicked;
    std::atomic<bool> enter_button_clicked;
    std::atomic<bool> left_button_clicked;
    std::atomic<bool> right_button_clicked;

    Buttons();
    ~Buttons();
    void SetupPins();
    void PollInputs();
    void Close();
    void Stop();
    std::thread PollInputsThread();
private:

    std::atomic<bool> m_polling;
    std::atomic<bool> m_polling_complete;
};
} // end namespace Buttons
#endif