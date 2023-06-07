import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import AppController 1.0

ApplicationWindow
{
    width: 550
    height: 350

    visible: true

    title: "test_app"

    Connections {
        target: AppController

        function onError(err) {
            md.text = err;
            md.open();
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ListView {
            width: 400
            height: 300

            Layout.leftMargin: 10
            Layout.topMargin: 10

            model: AppController.mainModel

            delegate: Item {
                width: ListView.view.width
                height: 40

                Rectangle {
                    width: parent.width
                    height: parent.height

                    color: ListView.isCurrentItem ? "lightblue" : "white"

                    RowLayout {
                        Text {
                            width: parent.width * 0.4

                            text: model.filename

                            horizontalAlignment: Text.AlignLeft
                            elide: Text.ElideRight
                        }

                        Text {
                            width: parent.width * 0.3

                            text: model.width
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Text {
                            width: parent.width * 0.3

                            text: model.height
                            horizontalAlignment: Text.AlignHCenter
                        }

                        Text {
                            width: parent.width * 0.3

                            text: model.active ? "Active" : "Not active"
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        console.log("Clicked on item:", index, model.filename);

                        AppController.process(model.filename);
                    }
                }
            }
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            text: "Choose folder to compress"
            onClicked: fd.open()
        }

        Item {
            Layout.fillHeight: true
        }
    }

    FolderDialog {
        id: fd

        title: "Set search path"

        onAccepted: {
            AppController.setSearchPath(fd.currentFolder);
        }

        onRejected: {
            console.log("Dialog was rejected");
        }
    }

    MessageDialog {
        id: md

        title: "Error"
        text: ""

        buttons: StandardButton.Ok
        visible: false

        onAccepted: {
            md.close();
        }
    }
}
