import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: adminRoot
    anchors.fill: parent
    color: "#2D1B33" // SRK Purple background

    // Custom signals for communicating navigation events
    signal logoutRequested()

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Sidebar Navigation Container
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.22
            color: "#1A0F21" // Darker purple for sidebar distinction
            border.color: "#D4AF37" // Gold border for Flat Comic styling
            border.width: 2

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 15

                // Sidebar Header
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "ADMIN PANEL"
                        color: "#D4AF37" // Gold text
                        font.pixelSize: 22
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        width: parent.width
                    }

                    Rectangle {
                        width: parent.width
                        height: 2
                        color: "#D4AF37"
                    }
                }

                // Sidebar Buttons to switch between view tabs
                Column {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 10

                    Button {
                        text: "Dashboard Overview"
                        highlighted: true
                        Layout.fillWidth: true
                        onClicked: adminStack.currentIndex = 0
                    }

                    Button {
                        text: "Manage Books"
                        Layout.fillWidth: true
                        onClicked: adminStack.currentIndex = 1
                    }

                    Button {
                        text: "Manage Users"
                        Layout.fillWidth: true
                        onClicked: adminStack.currentIndex = 2
                    }
                }

                // Logout Action Button
                Button {
                    text: "Logout"
                    Layout.fillWidth: true
                    contentItem: Text {
                        text: "Logout"
                        color: "#FF4D4D" // Highlighted red for logout/danger zone
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "transparent"
                        border.color: "#FF4D4D"
                        border.width: 2
                        radius: 4
                    }
                    onClicked: {
                        adminRoot.logoutRequested();
                    }
                }
            }
        }

        // Main Content Switcher
        StackLayout {
            id: adminStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: 0

            // View 1: Overview Statistics
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 25
                    spacing: 20

                    Text {
                        text: "System Statistics"
                        color: "#D4AF37"
                        font.pixelSize: 24
                        font.bold: true
                    }

                    GridLayout {
                        columns: 2
                        Layout.fillWidth: true
                        columnSpacing: 15
                        rowSpacing: 15

                        // Total Users Metric Card
                        Rectangle {
                            Layout.fillWidth: true
                            height: 120
                            color: "#3D2B47"
                            border.color: "#D4AF37"
                            border.width: 1
                            radius: 8
                            ColumnLayout {
                                anchors.centerIn: parent
                                Text { text: "Total Users"; color: "#FFFFFF"; font.pixelSize: 16 }
                                Text { text: "1,240"; color: "#D4AF37"; font.pixelSize: 28; font.bold: true }
                            }
                        }

                        // Total Books Metric Card
                        Rectangle {
                            Layout.fillWidth: true
                            height: 120
                            color: "#3D2B47"
                            border.color: "#D4AF37"
                            border.width: 1
                            radius: 8
                            ColumnLayout {
                                anchors.centerIn: parent
                                Text { text: "Total Books Available"; color: "#FFFFFF"; font.pixelSize: 16 }
                                Text { text: "450"; color: "#D4AF37"; font.pixelSize: 28; font.bold: true }
                            }
                        }
                    }
                    Item { Layout.fillHeight: true } // Spacer pushes everything to the top
                }
            }

            // View 2: Manage Books Tab
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 25
                    Text {
                        text: "Book Management"
                        color: "#D4AF37"
                        font.pixelSize: 24
                        font.bold: true
                    }
                    // TODO: Implement TableView or ListView for listing/adding/deleting books
                    Item { Layout.fillHeight: true }
                }
            }

            // View 3: Manage Users Tab
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 25
                    Text {
                        text: "User Management"
                        color: "#D4AF37"
                        font.pixelSize: 24
                        font.bold: true
                    }
                    // TODO: Implement TableView or ListView for managing registered user accounts
                    Item { Layout.fillHeight: true }
                }
            }
        }
    }
}
