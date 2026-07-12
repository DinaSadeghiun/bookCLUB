import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    id: homeView
    clip: true

    required property string username
    required property var selectedGenres

    ColumnLayout {
        width: parent.width - 60
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 25

        Item { Layout.preferredHeight: 20 }

        RowLayout {
            spacing: 20
            Image {
                source: "qrc:/assets/images/giraffe.png"
                Layout.preferredWidth: 65; Layout.preferredHeight: 65
            }
            ColumnLayout {
                Text {
                    text: "Welcome Back, " + homeView.username + "! 🦒"
                    color: "#D4AF37"; font.pixelSize: 28; font.bold: true
                }
                Text {
                    text: "Discover your next favorite book today"; color: "#A08EAD"; font.pixelSize: 16
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 280
            color: "#2D1B33"; radius: 20; border.color: "#D4AF37"; border.width: 2

            RowLayout {
                anchors.fill: parent; anchors.margins: 20; spacing: 25
                Rectangle {
                    Layout.preferredWidth: 130; Layout.preferredHeight: 190
                    color: "#1A0F1F"; radius: 10; border.color: "#D4AF37"
                    Text { anchors.centerIn: parent; text: "COVER"; color: "#D4AF37" }
                }
                ColumnLayout {
                    Layout.fillWidth: true; spacing: 8
                    Text { text: "TRENDING NOW"; color: "#D4AF37"; font.pixelSize: 11; font.bold: true }
                    Text { text: "The Great Gatsby"; color: "white"; font.pixelSize: 26; font.bold: true }
                    Text {
                        text: "A story of hope and tragedy in the Jazz Age."; color: "#A08EAD"
                        wrapMode: Text.WordWrap; Layout.fillWidth: true; font.pixelSize: 14
                    }
                    Item { Layout.fillHeight: true }
                    RowLayout {
                        spacing: 15
                        Button {
                            text: "Read Now"
                            background: Rectangle { color: "#D4AF37"; radius: 8; implicitWidth: 120; implicitHeight: 40 }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            spacing: 15
            Text { text: "Your Interests"; color: "#D4AF37"; font.pixelSize: 20; font.bold: true }
            Row {
                spacing: 10
                Repeater {
                    model: homeView.selectedGenres
                    Rectangle {
                        width: 100; height: 35; color: "transparent"; radius: 17
                        border.color: "#D4AF37"; border.width: 1
                        Text { anchors.centerIn: parent; text: modelData; color: "white" }
                    }
                }
            }
        }

        Item { Layout.preferredHeight: 20 }
    }
}
