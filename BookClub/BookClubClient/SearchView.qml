import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: searchRoot
    anchors.fill: parent
    color: "#1A0F1F"

    property string selectedFilter: "All"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // Search Bar Section
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#2d1b4e"
            radius: 10
            border.color: "#FFD700"
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
                    placeholderText: "Search by title, author, or publisher..."
                    color: "white"
                    placeholderTextColor: "#aaaaaa"
                    background: Rectangle { color: "transparent" }
                    font.pixelSize: 16
                    verticalAlignment: TextInput.AlignVCenter
                }
            }
        }

        // Advanced Filters Section (Based on Project Docs: Title, Author, Publisher)
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "Search In:"
                color: "#FFD700"
                font.bold: true
                font.pixelSize: 14
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Repeater {
                    model: ["All", "Book Title", "Author", "Publisher"]
                    Button {
                        id: filterBtn
                        Layout.preferredHeight: 35
                        Layout.fillWidth: true

                        contentItem: Text {
                            text: modelData
                            color: searchRoot.selectedFilter === modelData ? "#1A0F1F" : "#FFD700"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: searchRoot.selectedFilter === modelData ? "#FFD700" : "#2d1b4e"
                            border.color: "#FFD700"
                            border.width: 1
                            radius: 5
                        }

                        onClicked: {
                            searchRoot.selectedFilter = modelData;
                        }
                    }
                }
            }
        }

        // Results Status Header
        RowLayout {
            Layout.fillWidth: true
            Text {
                text: searchInput.text !== "" ? "Results for: \"" + searchInput.text + "\"" : "Popular / Recent Searches"
                color: "#FFD700"
                font.bold: true
                font.pixelSize: 18
                Layout.fillWidth: true
            }
            Text {
                text: "Active Filter: " + searchRoot.selectedFilter
                color: "#A08EAD"
                font.pixelSize: 12
            }
        }

        // Results Grid
        GridView {
            id: resultsGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: 150
            cellHeight: 250
            clip: true
            model: searchInput.text === "" ? 0 : 6

            delegate: Item {
                width: 140
                height: 240

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 5

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 170
                        color: "#2D1B33"
                        radius: 8
                        border.color: "#FFD700"
                        border.width: 1

                        Image {
                            anchors.fill: parent
                            anchors.margins: 5
                            source: "qrc:/assets/images/giraffe.png"
                            fillMode: Image.PreserveAspectFit
                        }
                    }

                    Text {
                        text: "Sample Book Title"
                        color: "white"
                        font.bold: true
                        font.pixelSize: 12
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }

                    Text {
                        text: "Author Name"
                        color: "#A08EAD"
                        font.pixelSize: 10
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }

                    Text {
                        text: "Publisher Name"
                        color: "#FFD700"
                        font.pixelSize: 9
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }
}
