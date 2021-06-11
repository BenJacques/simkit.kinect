#ifndef BUTTONS_H
#define BUTTONS_H
#include <atomic>
#include <thread>
namespace buttons
{
class Buttons
{


public:

    std::atomic <bool> start_button_clicked (false);
    std::atomic <bool> stop_button_clicked (false);
    std::atomic <bool> exit_button_clicked (false);
    std::atomic <bool> yes_button_clicked (false);
    std::atomic <bool> no_button_clicked (false);

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