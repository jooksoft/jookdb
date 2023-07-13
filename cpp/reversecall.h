#ifndef REVERSECALL_H
#define REVERSECALL_H

#include <QJsonObject>


#define DECODE_PWD_FROM_B64 1001

class ReverseCall
{
public:
    ReverseCall();

    static void call(const QJsonObject &json);
};

#endif // REVERSECALL_H
