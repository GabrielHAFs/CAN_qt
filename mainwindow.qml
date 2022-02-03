import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import io.qt.examples.backend 1.0


Window {
    id: window
    width: 800
    height: 480
    visible: true
    title: qsTr("CAN on fire")

    BackEnd {
        id: backend
    }

    RowLayout{
        anchors.fill: parent
        clip: false
        spacing: 0

        ColumnLayout{
            Slider{ id: control
                width: 200
                layer.format: ShaderEffectSource.RGBA
                layer.enabled: false
                live: true
                snapMode: RangeSlider.SnapOnRelease
                Layout.fillWidth: true
                transformOrigin: Item.Center
                from: -1
                value: 0
                to: 1
                stepSize: 0.1

                onMoved: {
                    console.log("Acceleration:"+value)
                    backend.acceleration = control.value
                }
                
                background: Rectangle {
                    x: control.leftPadding
                    y: control.topPadding + control.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 4
                    width: control.availableWidth
                    height: implicitHeight
                    radius: 2
                    color: "#bdbebf"

                    Rectangle {
                        width: control.visualPosition * parent.width
                        height: parent.height
                        color: "#21be2b"
                        radius: 2
                    }
                }   
            }
            Label {
                text: "Acceleration"
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            }
        }

        ColumnLayout{
            Layout.fillHeight: false
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

            Slider{ id: sl2
                width: 200
                snapMode: RangeSlider.SnapOnRelease
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                from: -1
                value: 0
                to: 1
                stepSize: 0.1

                onMoved: {
                    console.log("Steering:"+sl2.value)
                    backend.steer = sl2.value
                }

                background: Rectangle {
                    x: sl2.leftPadding
                    y: sl2.topPadding + sl2.availableHeight / 2 - height / 2
                    implicitWidth: 200
                    implicitHeight: 4
                    width: sl2.availableWidth
                    height: implicitHeight
                    radius: 2
                    color: "#bdbebf"

                    Rectangle {
                        width: sl2.visualPosition * parent.width
                        height: parent.height
                        color: "#21be2b"
                        radius: 2
                    }
                } 
            }
            Label {
                text: "steering"
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                Layout.fillHeight: false
                Layout.fillWidth: true
            }
        }

    }

    Button {
        id: button
        x: 350
        y: 258
        text: qsTr("Reset Val")
        clip: false
        wheelEnabled: true
        autoExclusive: false
        checkable: false
        display: AbstractButton.TextOnly
        highlighted: true
        flat: true
        onClicked:{
            control.value = 0
            backend.acceleration = control.value
            sl2.value = 0
            backend.steer = sl2.value
        }
    }

    Rectangle {
        id: rectangle
        x: 0
        y: 140
        width: 800
        height: 200
        opacity: 0.339
        visible: true
        color: "#b6555555"
        clip: false

        Button {
            id: button1
            x: 151
            y: 160
            opacity: 1
            visible: true
            text: qsTr("Reset Pos")
            enabled: true
            anchors.horizontalCenter: parent.horizontalCenter
            transformOrigin: Item.Center
            clip: false
            wheelEnabled: false
            display: AbstractButton.TextOnly
            highlighted: true
            flat: true

            onClicked: {
                backend.resetPosition()
            }
        }
    }

    Switch {
        id: switch1
        x: 341
        y: 43
        text: qsTr("CAN")
        anchors.horizontalCenter: parent.horizontalCenter
        checked: false

        onToggled:{
            if (checked == false)
            {
                console.log("CAN is not allright")
            }else{console.log("CAN is allright")
                backend.swCAN = true
                backend.initCANConnection()
            }
        }
    }
    
}

