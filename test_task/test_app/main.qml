import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import AppController 1.0

ApplicationWindow {

    visible: true

    width: 400
    height: 300

    title: "test_app"

    ListView {
        anchors.fill: parent
        model: AppController.model

        delegate: Item {
            width: parent.width
            height: 40

            Rectangle {
                width: parent.width
                height: parent.height
                color: ListView.isCurrentItem ? "lightblue" : "white"

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.filename
                    width: parent.width * 0.4
                    horizontalAlignment: Text.AlignLeft
                    elide: Text.ElideRight
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.width
                    width: parent.width * 0.3
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: model.height
                    width: parent.width * 0.3
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Component.onCompleted: {
                console.warn("#########");
            }
        }
    }
/*
    Button {
        text: "Choose file to compress"
        anchors.centerIn: parent
        onClicked: fd.open()
    }
*/
    FileDialog {
        id: fd

        title: "Open File"

        onAccepted: {
            console.log("File dialog ended with:", fd.currentFile);

            AppController.runCompress(fd.currentFile);
        }

        onRejected: {
            console.log("Dialog was rejected");
        }
    }
}
