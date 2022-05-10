#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtGlobal>

struct dgram
{
    qint32 some_val;
};

struct gpoint
{
    qint32 time, val;
};


#endif // PROTOCOL_H
