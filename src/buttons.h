#ifndef BUTTONS_H
#define BUTTONS_H

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

private:

    bool m_polling;
};
} // end namespace Buttons
#endif