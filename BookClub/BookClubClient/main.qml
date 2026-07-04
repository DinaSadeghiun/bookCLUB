import QtQuick
import QtQuick.Controls

Window {
    width: 1000
    height: 700
    visible: true
    title: "BookClub Project"

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: "qrc:/Login.qml"
    }
}

