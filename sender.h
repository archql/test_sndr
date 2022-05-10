#ifndef SENDER_H
#define SENDER_H

#include <QThread>

class control;

class Sender : public QThread
{
public:
    explicit Sender(control *ctrl);

    void run();
private:
    control *ctrl;
};


#endif // SENDER_H
