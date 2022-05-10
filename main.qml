import QtQuick
import QtQuick.Controls 2.5
import QtQuick.Window 2.12
import QtQuick.Layouts 1.2

import control 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Control {
        id: ctrl
    }

    ColumnLayout {
        RowLayout {
            Text {
                text: "input IP address: "
            }
            TextField {
                id: input_ip
                placeholderText: "IP"
                text: "127.0.0.1"
                inputMask: "000.000.000.000;_"
            }
            Text {
                text: "input port: "
            }
            TextField {
                id: input_port
                placeholderText: "port"
                validator: IntValidator { bottom: 0; top: 65535; }
            }
            Button {
                id: bt_set
                text: "set up sender"
                onClicked: {
                    ctrl.setup(input_ip.text.toString(), input_port.text.toString(), input_expr.text.toString())

                    if (ctrl.state()) { // if eok
                        input_port.enabled = false
                        input_ip.enabled = false
                        input_expr.enabled = false
                    }

                    txt_state.update()
                }
            }
        }
        RowLayout {
            Text {
                text: "input expr: "
            }
            TextField {
                id: input_expr
                placeholderText: "Ex: T^2+2"
                text: ""
                validator: RegularExpressionValidator { regularExpression: /[0-9A-Za-z\%\-\+\*\^\(\)\/\!]+/ }
            }
        }
        RowLayout {
            Text {
                text: "State: "
            }
            Text {
                id: txt_state
                function update() {
                    text = ctrl.state_str()
                }
            }
        }
        RowLayout {
            Button {
                id: bt_start
                text: "start sending"
                onClicked: {
                    ctrl.start()
                    txt_state.update()
                }
            }
            Button {
                id: bt_stop
                text: "stop sending"
                onClicked: {
                    ctrl.stop()
                    txt_state.update()

                    input_port.enabled = true
                    input_ip.enabled = true
                    input_expr.enabled = true
                }
            }
        }
        Text {
            wrapMode: Text.WordWrap
            text: "<b>Expr legend (support only int32 math):</b><br> <b>T</b> - cur time in ms;
<b>s</b> - cur s, 0..60; <b>r</b> - random 0..1; <b>R</b> - random int32; <b>N</b> - random normal distributed, params(0, 1000)
<br> <b>Signs</b>: <b>+</b>; <b>-</b>; <b>%</b>; <b>/</b>; <b>*</b>; <b>^</b> (as pow(a, b));
<b>!</b> (as -a); <b>C</b> (as cos(a)*1000); <b>S</b> (as sin(a)*1000);
<br> <i> Warn! All illegal letters 'll be interprited as expr = 0! Div by 0 leads to 0!</i>  "
        }
    }
}
