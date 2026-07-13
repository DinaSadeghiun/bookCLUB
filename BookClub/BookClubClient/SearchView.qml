import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: searchRoot
    anchors.fill: parent
    color: "#1a1a1a" // Background color

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // Search Bar Section
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#2d1b4e" // SRK Purple
            radius: 10
            border.color: "#FFD700" // Gold
            border.width: 2

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 15
                anchors.rightMargin: 15

                Text {
                    text: "🔍"
                    font.pixelSize: 20
                    color: "#FFD700"
                }

                TextField {
                    id: searchInput
                    Layout.fillWidth: true
                    placeholderText: "Search by title, author, or ISBN..."
                    color: "white"
                    placeholderTextColor: "#aaaaaa"
                    background: Rectangle { color: "transparent" }
                    font.pixelSize: 16
                    verticalAlignment: TextInput.AlignVCenter
                }
            }
        }

        // Results Placeholder
        Text {
            text: searchInput.text !== "" ? "Results for: " + searchInput.text : "Recent Searches"
            color: "#FFD700"
            font.bold: true
            font.pixelSize: 18
        }

        GridView {
            id: resultsGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: 150
            cellHeight: 220
            clip: true
            model: searchInput.text === "" ? 0 : 5 // Dummy logic: show 5 items if typing

            delegate: Column {
                spacing: 10
                Rectangle {
                    width: 130
                    height: 180
                    color: "#333333"
                    radius: 5
                    border.color: "#FFD700"

                    Image {
                        anchors.fill: parent
                        source: "qrc:/assets/images/giraffe.png" // Placeholder
                        fillMode: Image.PreserveAspectFit
                    }
                }
                Text {
                    text: "Book Title"
                    color: "white"
                    width: 130
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }
            }
        }
    }
}
