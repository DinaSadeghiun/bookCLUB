import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: libraryView
    contentWidth: availableWidth
    clip: true

    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
        z: -1
    }

    ColumnLayout {
        width: parent.width - 40
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 25

        Text {
            text: "Personal Library"
            color: "#D4AF37"
            font.pixelSize: 32
            font.bold: true
        }

        TabBar {
            id: libraryTabBar
            Layout.fillWidth: true
            background: Rectangle { color: "transparent" }
            Repeater {
                model: ["All Books", "Favorites", "Wishlist", "Shelves"]
                TabButton {
                    text: modelData
                    contentItem: Text {
                        text: modelData
                        color: parent.checked ? "#D4AF37" : "#A08EAD"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: parent.checked ? "#2D1B33" : "transparent"
                        border.color: parent.checked ? "#D4AF37" : "transparent"
                        border.width: 1
                        radius: 5
                    }
                }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 600
            currentIndex: libraryTabBar.currentIndex

            // 1. All Books List
            ListView {
                model: 5
                spacing: 10
                delegate: Rectangle {
                    width: parent.width
                    height: 100
                    color: "#2D1B33"
                    radius: 10
                    border.color: "#D4AF37"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 15

                        Rectangle {
                            width: 55
                            height: 80
                            color: "#1A0F1F"
                            radius: 4
                            border.color: "#A08EAD"
                            border.width: 1
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 5
                            Text { text: "Book Title"; color: "white"; font.bold: true; font.pixelSize: 16 }
                            Text { text: "Author: John Doe | Year: 2024"; color: "#A08EAD"; font.pixelSize: 12 }
                        }

                        Button {
                            text: "Details"
                            font.pixelSize: 12
                            contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter }
                            background: Rectangle { color: "#D4AF37"; radius: 5 }
                        }
                    }
                }
            }

            // 2. Favorites Grid
            GridView {
                cellWidth: 160
                cellHeight: 230
                model: 4
                clip: true
                delegate: Item {
                    width: 150
                    height: 220
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 5
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "#2D1B33"
                            border.color: "#FF5555"
                            border.width: 2
                            radius: 8
                        }
                        Text {
                            text: "Favorite Title"
                            color: "white"
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 14
                        }
                    }
                }
            }

            // 3. Wishlist List
            ListView {
                model: 3
                spacing: 10
                delegate: Rectangle {
                    width: parent.width
                    height: 80
                    color: "#2D1B33"
                    radius: 8
                    border.color: "#A08EAD"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 15

                        Rectangle { width: 40; height: 60; color: "#1A0F1F"; radius: 4 }

                        Text {
                            text: "Future Read Title"
                            color: "white"
                            Layout.fillWidth: true
                            font.bold: true
                        }

                        Button {
                            text: "Remove"
                            contentItem: Text { text: parent.text; color: "white" }
                            background: Rectangle { color: "#FF5555"; radius: 5 }
                            onClicked: console.log("Removed from wishlist")
                        }
                    }
                }
            }

            // 4. Shelves Management
            ColumnLayout {
                spacing: 15
                Button {
                    text: "+ Add New Shelf"
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 40
                    contentItem: Text { text: parent.text; color: "#1A0F1F"; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                    background: Rectangle { color: "#D4AF37"; radius: 5 }
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 10
                    model: ["Science", "History"]
                    delegate: Rectangle {
                        width: parent.width
                        height: 60
                        color: "#2D1B33"
                        radius: 5
                        border.color: "#D4AF37"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10

                            Text {
                                text: modelData
                                color: "white"
                                font.bold: true
                                Layout.fillWidth: true
                            }

                            Button {
                                text: "Edit Name"
                                contentItem: Text { text: parent.text; color: "white" }
                                background: Rectangle { color: "#5c3d75"; radius: 4 }
                            }

                            Button {
                                text: "Move Books"
                                contentItem: Text { text: parent.text; color: "#1A0F1F" }
                                background: Rectangle { color: "#D4AF37"; radius: 4 }
                            }
                        }
                    }
                }
            }
        }
    }
}
