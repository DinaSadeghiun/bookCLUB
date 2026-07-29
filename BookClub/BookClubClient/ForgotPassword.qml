import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: forgotPasswordPage
    anchors.fill: parent
    color: "#1A0F1F"

    property string currentStep: "checkUser" // "checkUser" or "resetPassword"


    property string foundUsername: "admin"
    property string favoriteAuthor: "George Orwell"

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#4B0082" }
        GradientStop { position: 1.0; color: "#2C003E" }
    }

    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width * 0.8
        spacing: 20

        Text {
            text: currentStep === "checkUser" ? "Recover Password" : "Reset Password"
            color: "#FFD700"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }


        ColumnLayout {
            visible: currentStep === "checkUser"
            Layout.fillWidth: true
            spacing: 15

            TextField {
                id: usernameInput
                placeholderText: "Enter your username"
                Layout.fillWidth: true
                background: Rectangle { radius: 10; color: "#F5F5F5" }
            }

            Text { id: userError; text: "Username not found!"; color: "#FF6B6B"; visible: false }

            Button {
                text: "Continue"
                Layout.fillWidth: true
                background: Rectangle { color: "#FFD700"; radius: 10 }
                onClicked: {
                    if (usernameInput.text === foundUsername) {
                        currentStep = "resetPassword"
                        userError.visible = false
                    } else {
                        userError.visible = true
                    }
                }
            }
        }


        ColumnLayout {
            visible: currentStep === "resetPassword"
            Layout.fillWidth: true
            spacing: 15

            TextField {
                id: authorInput
                placeholderText: "Who is your favorite author?"
                Layout.fillWidth: true
                background: Rectangle { radius: 10; color: "#F5F5F5" }
            }

            TextField {
                id: newPasswordInput
                placeholderText: "New Password"
                echoMode: TextInput.Password
                Layout.fillWidth: true
                background: Rectangle { radius: 10; color: "#F5F5F5" }
            }

            Text { id: authError; text: "Incorrect author name!"; color: "#FF6B6B"; visible: false }

            Button {
                text: "Set New Password"
                Layout.fillWidth: true
                background: Rectangle { color: "#FFD700"; radius: 10 }
                onClicked: {
                    if (authorInput.text === favoriteAuthor) {
                        console.log("Password updated successfully!")
                        stackView.pop()
                    } else {
                        authError.visible = true
                    }
                }
            }
        }
    }
}