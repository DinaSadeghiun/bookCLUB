import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects // For shadows and glows

Rectangle {
    id: dashboardRoot
    anchors.fill: parent
    color: "#1A0F1F"

    property var userGenres: []
    property string username: "Dina"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // --- 1. Sidebar (Navigation) ---
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 80
            color: "#120914" // Slightly darker purple
            border.color: "#D4AF37"
            border.width: 0

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 40
                spacing: 30

                // Logo Placeholder
                Text {
                    text: "BC"
                    color: "#D4AF37"
                    font.pixelSize: 24
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Item { Layout.fillHeight: true } // Spacer

                // Icon Buttons (Simplified Icons)
                Repeater {
                    model: ["🏠", "📚", "🔍", "⚙️"]
                    Text {
                        text: modelData
                        color: "#D4AF37"
                        font.pixelSize: 22
                        Layout.alignment: Qt.AlignHCenter
                        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor }
                    }
                }

                Item { Layout.fillHeight: true } // Spacer
            }
        }

        // --- 2. Main Content Area ---
        Flickable {
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentHeight: mainColumn.height + 100
            clip: true

            ColumnLayout {
                id: mainColumn
                width: parent.width
                spacing: 30
                anchors.topPadding: 40
                anchors.leftPadding: 40
                anchors.rightPadding: 40

                // Header
                RowLayout {
                    Layout.fillWidth: true
                    ColumnLayout {
                        Text {
                            text: "Welcome Back, " + username + "!"
                            color: "#D4AF37"
                            font.pixelSize: 32
                            font.bold: true
                        }
                        Text {
                            text: "The library is waiting for you."
                            color: "#D4AF37"
                            font.pixelSize: 16
                            opacity: 0.7
                        }
                    }
                    Item { Layout.fillWidth: true }
                }

                // --- Featured Book (Hero Section) ---
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 250
                    radius: 20
                    color: "#2D1B33"
                    border.color: "#D4AF37"
                    border.width: 2

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 20

                        Image {
                            source: "qrc:/assets/genres/fiction.png" // Placeholder
                            Layout.preferredWidth: 150
                            Layout.fillHeight: true
                            fillMode: Image.PreserveAspectFit
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Text {
                                text: "EDITOR'S PICK"
                                color: "#D4AF37"
                                font.pixelSize: 12
                                font.letterSpacing: 2
                                font.bold: true
                            }
                            Text {
                                text: "The Midnight Library"
                                color: "#FFFFFF"
                                font.pixelSize: 28
                                font.bold: true
                            }
                            Text {
                                text: "Between life and death there is a library..."
                                color: "#D4AF37"
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                opacity: 0.8
                            }
                            Button {
                                text: "Read Now"
                                Layout.preferredWidth: 120
                                background: Rectangle {
                                    color: "#D4AF37"
                                    radius: 5
                                }
                                contentItem: Text {
                                    text: "Read Now"
                                    color: "#1A0F1F"
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                        }
                    }
                }

                // --- Genre Chips (Your Interests) ---
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 15
                    Text {
                        text: "Personalized Feed"
                        color: "#D4AF37"
                        font.pixelSize: 20
                        font.bold: true
                    }
                    Row {
                        spacing: 12
                        Repeater {
                            model: userGenres.length > 0 ? userGenres : ["Fiction", "Mystery", "SciFi"]
                            Rectangle {
                                width: genreLabel.width + 30
                                height: 35
                                color: "#D4AF37"
                                radius: 10
                                Text {
                                    id: genreLabel
                                    text: "#" + modelData
                                    color: "#1A0F1F"
                                    anchors.centerIn: parent
                                    font.bold: true
                                }
                            }
                        }
                    }
                }

                // --- Book Recommendations Grid ---
                GridLayout {
                    columns: 3
                    rowSpacing: 25
                    columnSpacing: 25
                    Layout.fillWidth: true

                    Repeater {
                        model: 6 // 6 dummy books
                        Rectangle {
                            Layout.preferredWidth: 160
                            Layout.preferredHeight: 280
                            color: "transparent"

                            Column {
                                anchors.fill: parent
                                spacing: 10
                                Rectangle {
                                    width: 160; height: 220
                                    color: "#2D1B33"
                                    radius: 15
                                    border.color: "#D4AF37"
                                    border.width: 1
                                    clip: true
                                    Image {
                                        source: "qrc:/assets/genres/mystery.png"
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        fillMode: Image.PreserveAspectFit
                                    }
                                }
                                Text {
                                    text: "Mystery Title " + (index + 1)
                                    color: "#D4AF37"
                                    font.bold: true
                                    width: 160
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
