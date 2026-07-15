import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: loginPage
    anchors.fill: parent

    property string username: ""
    property string userRole: ""

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#4B0082" }
            GradientStop { position: 1.0; color: "#2C003E" }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20
        width: parent.width * 0.8

        Image {
            id: mascotIcon
            source: "qrc:/assets/images/giraffe.png"
            Layout.preferredWidth: 100
            Layout.preferredHeight: 100
            Layout.alignment: Qt.AlignHCenter
            fillMode: Image.PreserveAspectFit
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 5

            Text {
                text: "Welcome to"
                font.pixelSize: 22
                color: "#B0A4E3"
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: "BOOK CLUB"
                font.pixelSize: 38
                font.bold: true
                font.letterSpacing: 2
                color: "#FFD700"
                Layout.alignment: Qt.AlignHCenter
            }
        }

        Item { Layout.preferredHeight: 10 }

        TextField {
            id: usernameInput
            placeholderText: "Username"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#2C003E"
            leftPadding: 15

            background: Rectangle {
                radius: 12
                color: "#F5F5F5"
                border.color: usernameInput.activeFocus ? "#FFD700" : "transparent"
                border.width: 2
                Behavior on border.color { ColorAnimation { duration: 200 } }
            }
        }

        TextField {
            id: passwordInput
            placeholderText: "Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#2C003E"
            leftPadding: 15

            background: Rectangle {
                radius: 12
                color: "#F5F5F5"
                border.color: passwordInput.activeFocus ? "#FFD700" : "transparent"
                border.width: 2
                Behavior on border.color { ColorAnimation { duration: 200 } }
            }
        }

        Button {
            id: loginButton
            text: "LOGIN"
            Layout.fillWidth: true
            Layout.preferredHeight: 55
            Layout.topMargin: 10

            contentItem: Text {
                text: loginButton.text
                font.pixelSize: 16
                font.bold: true
                color: "#2C003E"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                radius: 12
                color: loginButton.pressed ? "#E6C200" : (loginButton.hovered ? "#FFEA00" : "#FFD700")
                Behavior on color { ColorAnimation { duration: 150 } }
            }

            onClicked: {
                console.log("Login clicked. Username:", usernameInput.text)
            }
        }

        Text {
            text: "Forgot Password?"
            font.pixelSize: 13
            color: "#B0A4E3"
            Layout.alignment: Qt.AlignHCenter

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    stackView.push("ForgotPassword.qml")
                }
            }
        }

        Item { Layout.preferredHeight: 10 }
        // Place this inside the root element of Login.qml






        Row {
            spacing: 5
            Layout.alignment: Qt.AlignHCenter

            Text {
                text: "Don't have an account?"
                font.pixelSize: 14
                color: "#FFFFFF"
            }

            Text {
                text: "Sign Up"
                font.pixelSize: 14
                font.bold: true
                color: "#4CAF50"

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if (typeof rootStackView !== "undefined" && rootStackView !== null) {
                            rootStackView.push("qrc:/SignUp.qml")
                        } else {
                            console.error("rootStackView is not accessible!")
                        }
                    }
                }
            }
        }
    }
}
