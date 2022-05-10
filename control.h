#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QRandomGenerator>
#include <QUdpSocket>
#include <QThread>
#include <QDateTime>
#include <QString>
#include <QSet>
#include <QStack>

#include "sender.h"
#include "protocol.h"

class control : public QObject
{
    Q_OBJECT
public:
    explicit control(QObject *parent = nullptr);
    ~control();

    Q_INVOKABLE bool state();
    Q_INVOKABLE QString state_str();
    Q_INVOKABLE bool setup(QString ip_str, QString port_str, QString expr_srt);

    Q_INVOKABLE bool start();
    Q_INVOKABLE bool stop();

public:
    void process(); // were static

private:
    QUdpSocket _sock;
    Sender _thread;
    QHostAddress _dest_addr;

    bool _is_ready;
    QString _state_str;

    QString _ip_str;
    int _port_num;

    bool parse_expr(QString &expr);
    int get_expr_range(const QString &expr);
    qint64 cnt_expr();

    //const QSet<QChar> letters = QSet<QChar>(QString("4").begin(), QString("4").end());
    const QString _letters = "TsRNr";
    const QString _signs = "+-*%/^";
    const QString _signs_unary = "CS!";
    const QString _brackets = "()";

    QMap<QChar, int> _prstack = {
        {0 , 8},
        {'+' , 2},
        {'-' , 2},
        {'*' , 4},
        {'/' , 4},
        {'%' , 4},
        {'^' , 5},
        {')' , 0xFF},
        {'(' , 0},
// experement
        {'C' , 7},
        {'S' , 7},
        {'!' , 7}
    };
    QMap<QChar, int> _prrel = {
        {0 , 7},
        {'+' , 1},
        {'-' , 1},
        {'*' , 3},
        {'/' , 3},
        {'%' , 3},
        {'^' , 6},
        {')' , 0},
        {'(' , 9},
// experement
        {'C' , 8},
        {'S' , 8},
        {'!' , 8}
    };

    QStack<QChar> _op_stack;
    QVector<qint32> _temp_val_arr;

    const QMap<QChar, qint32(*)()> _letter_action = {
            { 'T', [] () { return (qint32)QDateTime::currentMSecsSinceEpoch(); } },
            { 's', [] () { return (qint32)QDateTime::currentDateTime().time().second(); } },
            { 'r', [] () { return (qint32)random().bounded(0, 2); } },
            { 'R', [] () { return (qint32)random().generate(); } },
            { 'N', [] () { return random_normal(); } }
    };
    const QMap<QChar, qint32(*)(qint32, qint32)> _sign_action = {
            { '+', [] (qint32 b, qint32 a) { return a+b; } },
            { '-', [] (qint32 b, qint32 a) { return a-b; } },
            { '^', [] (qint32 b, qint32 a) { return (qint32)pow(a, b); } },
            { '*', [] (qint32 b, qint32 a) { return a * b; } },
            { '/', [] (qint32 b, qint32 a) { return b != 0 ? a / b : 0; } },
            { '%', [] (qint32 b, qint32 a) { return b != 0 ? a % b : 0; } }
    };
    const QMap<QChar, qint32(*)(qint32)> _sign_action_unary = {
            { 'C', [] (qint32 a) { return (qint32)(cos(a / 180. * 3.14 ) * 1000); } },
            { 'S', [] (qint32 a) { return (qint32)(sin(a / 180. * 3.14 ) * 1000); } },
            { '!', [] (qint32 a) { return -a; } }
    };
    static QRandomGenerator64 &random()
    {
        static QRandomGenerator64 rnd(QDateTime::currentMSecsSinceEpoch());
        return rnd;
    }
    static qint32 random_normal()
    {
        static std::default_random_engine generator(random().generate64());
        static std::normal_distribution<double> distribution(0.,1000.);
        return (qint32)distribution(generator);
    }

    inline QChar ascii_filter(const QChar &a);

    };

#endif // CONTROL_H
