#include "sender.h"
#include "control.h"

Sender::Sender(control *ctrl) : ctrl(ctrl)
{
}

void Sender::run()
{
    if (ctrl)
        ctrl->process();
}
