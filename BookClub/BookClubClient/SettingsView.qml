import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    id: settingsView
    clip: true

    required property string username
    required property string userRole
    required property var selectedGenres

    signal logoutRequested()

    ColumnLayout {
        width: parent.width - 60
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 25

        Item { Layout.preferredHeight: 20 }

        Text { text: "Account Settings"; color: "#D4AF37"; font.pixelSize: 28; font.bold: true }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            color: "#2D1B33"; radius: 15; border.color: "#D4AF37"; border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10

                Text { text: "Account Info"; color: "#D4AF37"; font.pixelSize: 16; font.bold: true }
                RowLayout {
                    Text { text: "Username:"; color: "#A08EAD"; Layout.preferredWidth: 100 }
                    Text { text: settingsView.username; color: "white" }
                }
                RowLayout {
                    Text { text: "Role:"; color: "#A08EAD"; Layout.preferredWidth: 100 }
                    Text { text: settingsView.userRole; color: "white" }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 180
            color: "#2D1B33"; radius: 15; border.color: "#D4AF37"; border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12

                Text { text: "Change Password"; color: "#D4AF37"; font.pixelSize: 16; font.bold: true }
                TextField {
                    Layout.fillWidth: true
                    placeholderText: "Current Password"
                    echoMode: TextInput.Password
                    color: "white"
                    background: Rectangle { color: "#1A0F1F"; radius: 8; border.color: "#D4AF37" }
                }
                TextField {
                    Layout.fillWidth: true
                    placeholderText: "New Password"
                    echoMode: TextInput.Password
                    color: "white"
                    background: Rectangle { color: "#1A0F1F"; radius: 8; border.color: "#D4AF37" }
                }
                Button {
                    text: "Update Password"
                    background: Rectangle { color: "#D4AF37"; radius: 8; implicitWidth: 160; implicitHeight: 40 }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 140
            color: "#2D1B33"; radius: 15; border.color: "#D4AF37"; border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12

                Text { text: "Manage Interests"; color: "#D4AF37"; font.pixelSize: 16; font.bold: true }
                Row {
                    spacing: 10
                    Repeater {
                        model: settingsView.selectedGenres
                        Rectangle {
                            width: 100; height: 35; color: "transparent"; radius: 17
                            border.color: "#D4AF37"; border.width: 1
                            Text { anchors.centerIn: parent; text: modelData; color: "white" }
                        }
                    }
                }
            }
        }

        Button {
            text: "Logout"
            Layout.fillWidth: true
            background: Rectangle { color: "#3A1F2C"; radius: 8; implicitHeight: 45; border.color: "#D4AF37" }
            contentItem: Text { text: "Logout"; color: "#D4AF37"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
            onClicked: settingsView.logoutRequested()
        }

        Item { Layout.preferredHeight: 20 }
    }
}
