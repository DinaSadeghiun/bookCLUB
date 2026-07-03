import QtQuick
import QtQuick.Controls

Window {
    id: window
    width: 400
    height: 600
    visible: true
    title: "Book Club"

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: "Login.qml"
    }
}
