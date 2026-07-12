import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    clip: true

    ColumnLayout {
        width: parent.width - 60
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20

        Item { Layout.preferredHeight: 20 }

        Text { text: "Search Books"; color: "#D4AF37"; font.pixelSize: 28; font.bold: true }

        TextField {
            Layout.fillWidth: true
            placeholderText: "Search by title, author or publisher..."
            color: "white"
            background: Rectangle { color: "#2D1B33"; radius: 10; border.color: "#D4AF37" }
        }

        Item {
            Layout.fillHeight: true
            Text { anchors.centerIn: parent; text: "Search results will appear here"; color: "#A08EAD" }
        }
    }
}
