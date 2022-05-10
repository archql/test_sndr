#include "control.h"

control::control(QObject *parent) :
    QObject(parent), _thread(this)
{
    _is_ready = false;

    // add letters to priorities maps
    for (const auto &chr : _letters)
    {
        _prstack[chr] = 8;
        _prrel[chr] = 7;
    }
}

control::~control()
{
    stop();
}

bool control::state()
{
    return _is_ready;
}

QString control::state_str()
{
    return _state_str;
}

bool control::setup(QString ip_str, QString port_str, QString expr_str)
{
    // try to set port
    _port_num = port_str.toInt(&_is_ready);
    if (!_is_ready)
    {
        _state_str = "Port setup error!";
        return false;
    }
    // try to set ip
    _is_ready = _dest_addr.setAddress(ip_str);
    if (!_is_ready)
        _state_str = "IPv4 setup error!";

    // try to parse expr
    parse_expr(expr_str);

    return _is_ready;
}

bool control::start()
{
    if (_is_ready) {
        _thread.start();
    }
    else {
        _state_str = "Cannot start thread!";
    }
    return _is_ready;
}
bool control::stop()
{
    if (_thread.isRunning())
    {
        qInfo() << "Stop";
        _is_ready = false;
        _thread.quit();
        _thread.wait();
    }

    return !_is_ready;
}
void control::process()
{
    dgram info;

    while (_is_ready)
    {
        // get some data
        info.some_val = cnt_expr();
        // send some data
        auto send = _sock.writeDatagram(QByteArray((char *)&info, sizeof(info)), _dest_addr, _port_num); // .writeDatagram(datagram);
        if (send <= 0)
        {
            _is_ready = false;
            _state_str = _sock.errorString();
            break;
        }
        //qDebug() << "Sended!" << send << " bytes! (val = " << info.some_val << ")";
        // sleep to send next
        QThread::msleep(10);
    }
}

int control::get_expr_range(const QString &expr)
{
    int result = 0; bool last_were_digit = false;
    for (const auto &chr : expr)
    {
        if (chr.isDigit()) {
            if (!last_were_digit) {
                last_were_digit = true; result++;
            }
            continue;
        }
        last_were_digit = false;

        if (_letters.contains(chr))
            result++;
        else if (_signs.contains(chr))
            result--;
        else if (_brackets.contains(chr) || _signs_unary.contains(chr)) // EXPEREMENT
            result+= 0; // do nothing
        else
        {
            result = -1;
            break;
        }
    }
    return result;
}

inline QChar control::ascii_filter(const QChar &a)
{
    return a > 0xFF ? 0 : a;
}

bool control::parse_expr(QString &expr)
{
    // get expr range
    int a;
    if ((a = get_expr_range(expr)) != 1)
    {
        _state_str = "EXPR ERR: Invalid range! " + QString::number(a);
        return false;
    }

    // extract all numeric constants & replace with special char
    _temp_val_arr.clear(); QString temp_num; int i = 0x0100, len = -1;
    for (int j = 0; j < expr.length(); j ++)
    {
        while ((j < expr.length()) && expr[j].isDigit()) {
            temp_num += expr[j++];
            len++;
        }
        if (len >= 0) // digit founded
        {
            _temp_val_arr.append(temp_num.toInt());
            temp_num.clear();
            expr.remove(j - len, len);
            expr[j - len - 1] = i;

            // reset ctrs
            j -= len + 1;
            len = -1;
            i++;
        }
    }

    // init stack
    _op_stack.clear();
    QStack<QChar> tmp_stack;
    // do algorithm
    for (const auto &chr : qAsConst(expr))
    {
        while (!tmp_stack.empty() &&
               (_prrel[ascii_filter(chr)] < _prstack[ascii_filter(tmp_stack.top())]))
        {
            _op_stack.append(tmp_stack.pop());
        }
        if (chr == ')') { // if '()' eq founded
            tmp_stack.pop();
        } else {
            tmp_stack.push(chr);
        }
    }
    while (!tmp_stack.empty())
        _op_stack.append(tmp_stack.pop());
    // EXPEREMENT
    qInfo() << _op_stack;

    _state_str = "EXPR ok!";
    return true;
}

qint64 control::cnt_expr()
{
    QStack<qint64> tmp_stack;
    for (const auto &it : qAsConst(_op_stack))
    {
        if (_signs_unary.contains(it)) // EXPEREMENT!
        {
            qint64 x = tmp_stack.pop();
            //qInfo() << it << " y " << x << " = " << _sign_action_unary[it](x);
            tmp_stack.push(_sign_action_unary[it](x));
        }
        else if (_signs.contains(it))
        {
            qint64 x = tmp_stack.pop();
            qint64 y = tmp_stack.pop();
            //qInfo() << " x " << y << " " << it << " y " << x << " = " << _sign_action[it](x, y);
            tmp_stack.push(_sign_action[it](x, y));
        }
        else if (it > 0xFF) // reserved for variables arr
        {
            tmp_stack.push(_temp_val_arr[it.unicode() - 0x0100]);
        }
        else
        {
            tmp_stack.push(_letter_action[it]());
        }
    }
    return tmp_stack.empty() ? 0 : tmp_stack.pop();
}
