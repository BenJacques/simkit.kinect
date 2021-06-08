#ifndef BUTTONS_H
#define BUTTONS_H
#include <atomic>
#include <thread>
namespace buttons
{
class Buttons
{


public:
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