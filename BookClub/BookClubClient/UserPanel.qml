import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: userPanel
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        RowLayout {
            Layout.fillWidth: true

            Column {
                spacing: 5
                Text {
                    text: "Welcome back, User!"
                    font.pixelSize: 24
                    font.bold: true
                    color: "#2C3E50"
                }
                Text {
                    text: "Explore your favorite books and recommendations"
                    font.pixelSize: 12
                    color: "#7F8C8D"
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "🛒 Cart (0)"
                onClicked: console.log("Navigate to Shopping Cart")
            }

            Button {
                text: "👤 Profile"
                onClicked: console.log("Navigate to Profile Settings")
            }
        }

        Row {
            spacing: 10

            Text {
                text: "Your Favorite Genres:"
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 14
            }

            Repeater {
                model: ["Fiction", "Science", "History"]
                Button {
                    text: modelData
                    flat: true
                    checkable: true
                }
            }
        }

        Text {
            text: "Recommended for You"
            font.pixelSize: 18
            font.bold: true
            color: "#2C3E50"
        }

        GridView {
            id: bookGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: 160
            cellHeight: 230
            clip: true

            model: 6

            delegate: Item {
                width: 150
                height: 220

                Rectangle {
                    anchors.fill: parent
                    color: "#FFFFFF"
                    radius: 8
                    border.color: "#BDC3C7"
                    border.width: 1

                    Column {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 8

                        Rectangle {
                            width: parent.width
                            height: 110
                            color: "#ECF0F1"
                            radius: 4

                            Text {
                                anchors.centerIn: parent
                                text: "Book Cover"
                                color: "#95A5A6"
                                font.pixelSize: 12
                            }
                        }

                        Text {
                            text: "Book Title " + (index + 1)
                            font.bold: true
                            elide: Text.ElideRight
                            width: parent.width
                            font.pixelSize: 14
                        }

                        Text {
                            text: "Price: $14.99"
                            color: "#27AE60"
                            font.pixelSize: 12
                        }

                        Button {
                            text: "Add to Cart"
                            width: parent.width
                            height: 28
                            onClicked: console.log("Added book " + (index + 1) + " to cart")
                        }
                    }
                }
            }
        }
    }
}
