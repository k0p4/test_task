import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import AppController 1.0

ApplicationWindow {

    visible: true

    width: 400
    height: 300

    title: "test_app"

    Button {
        text: "Choose file to compress"
        anchors.centerIn: parent
        onClicked: fd.open()
    }

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
