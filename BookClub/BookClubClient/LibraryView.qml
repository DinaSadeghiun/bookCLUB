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

        Text { text: "My Library"; color: "#D4AF37"; font.pixelSize: 28; font.bold: true }

        TabBar {
            id: libraryTabBar
            Layout.fillWidth: true
            background: Rectangle { color: "transparent" }

            TabButton {
                text: "My Books"
                implicitWidth: 110
                contentItem: Text {
                    text: parent.text
                    color: parent.checked ? "#D4AF37" : "#A08EAD"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "transparent" }
            }
            TabButton {
                text: "Saved / Wishlist"
                implicitWidth: 150
                contentItem: Text {
                    text: parent.text
                    color: parent.checked ? "#D4AF37" : "#A08EAD"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "transparent" }
            }
            TabButton {
                text: "My Shelves"
                implicitWidth: 120
                contentItem: Text {
                    text: parent.text
                    color: parent.checked ? "#D4AF37" : "#A08EAD"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "transparent" }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: libraryTabBar.currentIndex

            Item {
                Text { anchors.centerIn: parent; text: "Your purchased books will appear here"; color: "#A08EAD" }
            }
            Item {
                Text { anchors.centerIn: parent; text: "Your saved books will appear here"; color: "#A08EAD" }
            }
            Item {
                Text { anchors.centerIn: parent; text: "Your custom shelves will appear here"; color: "#A08EAD" }
            }
        }
    }
}
