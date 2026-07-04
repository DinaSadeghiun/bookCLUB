import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: dashboardRoot
    width: 1000
    height: 700
    color: "#f5f6fa"

    property string userRole: "User"
    property string username: "Guest"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 250
            color: "#2f3640"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15

                Column {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 10
                    Rectangle {
                        width: 70; height: 70; radius: 35
                        color: userRole === "Admin" ? "#e74c3c" : (userRole === "Publisher" ? "#2ecc71" : "#f1c40f")
                        anchors.horizontalCenter: parent.horizontalCenter
                        Text {
                            text: username.length > 0 ? username[0].toUpperCase() : "U"
                            font.pixelSize: 32
                            font.bold: true
                            anchors.centerIn: parent
                            color: "white"
                        }
                    }
                    Text {
                        text: username
                        color: "white"
                        font.bold: true
                        font.pixelSize: 16
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Text {
                        text: "[" + userRole + "]"
                        color: "#bdc3c7"
                        font.pixelSize: 12
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }

                Label {
                    text: "Navigation Menu"
                    color: "#7f8c8d"
                    font.pixelSize: 11
                    Layout.topMargin: 15
                }

                Button {
                    text: "Home / Dashboard"
                    Layout.fillWidth: true
                    onClicked: contentLoader.sourceComponent = homePage
                }

                ColumnLayout {
                    visible: userRole === "User"
                    Layout.fillWidth: true
                    spacing: 8

                    Button { text: "Search Books"; Layout.fillWidth: true }
                    Button { text: "Shopping Cart"; Layout.fillWidth: true }
                    Button { text: "My Library / PDF Reader"; Layout.fillWidth: true }
                    Button { text: "Purchase History"; Layout.fillWidth: true }
                }

                ColumnLayout {
                    visible: userRole === "Publisher"
                    Layout.fillWidth: true
                    spacing: 8

                    Button { text: "Manage My Books"; Layout.fillWidth: true }
                    Button { text: "Sales & Stats (Charts)"; Layout.fillWidth: true }
                }

                ColumnLayout {
                    visible: userRole === "Admin"
                    Layout.fillWidth: true
                    spacing: 8

                    Button { text: "Manage Users"; Layout.fillWidth: true }
                    Button { text: "Approve Books"; Layout.fillWidth: true }
                    Button { text: "System Notifications"; Layout.fillWidth: true }
                }

                Item { Layout.fillHeight: true }

                Button {
                    text: "Log Out"
                    Layout.fillWidth: true
                    onClicked: stackView.pop()
                }
            }
        }

        Loader {
            id: contentLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            sourceComponent: homePage
        }
    }

    Component {
        id: homePage
        ScrollView {
            contentWidth: -1
            ColumnLayout {
                width: parent.width - 40
                anchors.margins: 20
                spacing: 20

                Text {
                    text: "Welcome, " + username + "!"
                    font.pixelSize: 26
                    font.bold: true
                    color: "#2c3e50"
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 120
                    color: "#ffffff"
                    radius: 8
                    border.color: "#dcdde1"

                    Text {
                        anchors.centerIn: parent
                        text: "You are logged in as " + userRole + ".\nUse the sidebar to manage your activities."
                        horizontalAlignment: Text.AlignHCenter
                        font.pixelSize: 16
                        color: "#7f8c8d"
                    }
                }
            }
        }
    }
}
