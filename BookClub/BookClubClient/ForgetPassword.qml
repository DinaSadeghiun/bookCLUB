import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: recoveryDialog
    title: "Reset Password"
    modal: true
    width: 320
    standardButtons: Dialog.Cancel

    property int recoveryStep: 1
    property string targetUser: ""
    property var usersDb: ({})

    signal passwordResetSuccess(string username, string newPassword)

    background: Rectangle {
        color: "#2D1B33"
        border.color: "#D4AF37"
        border.width: 2
        radius: 8
    }

    header: Rectangle {
        color: "#1A0F1F"
        height: 40
        radius: 8
        Text {
            text: "Password Recovery"
            color: "#D4AF37"
            font.bold: true
            anchors.centerIn: parent
        }
    }

    ColumnLayout {
        width: parent.width
        spacing: 12

        ColumnLayout {
            visible: recoveryDialog.recoveryStep === 1
            width: parent.width
            spacing: 8

            Text {
                text: "Enter your username:"
                color: "#FFFFFF"
            }
            TextField {
                id: recoveryUserField
                Layout.fillWidth: true
                color: "#FFFFFF"
                background: Rectangle {
                    color: "#1A0F1F"
                    border.color: "#5A3D66"
                    radius: 4
                }
            }
            Button {
                text: "Next"
                Layout.fillWidth: true
                onClicked: {
                    var user = recoveryUserField.text.trim();
                    if (recoveryDialog.usersDb[user] !== undefined) {
                        recoveryDialog.targetUser = user;
                        recoveryDialog.recoveryStep = 2;
                        errorText.visible = false;
                    } else {
                        errorText.text = "Username not found!";
                        errorText.visible = true;
                    }
                }
            }
        }

        ColumnLayout {
            visible: recoveryDialog.recoveryStep === 2
            width: parent.width
            spacing: 8

            Text {
                text: "Security Question:\nWho is your favorite author?"
                color: "#FFFFFF"
                horizontalAlignment: Text.AlignHCenter
            }
            TextField {
                id: recoveryAnswerField
                placeholderText: "Your answer"
                Layout.fillWidth: true
                color: "#FFFFFF"
                background: Rectangle {
                    color: "#1A0F1F"
                    border.color: "#5A3D66"
                    radius: 4
                }
            }
            Button {
                text: "Verify"
                Layout.fillWidth: true
                onClicked: {
                    var dbAnswer = recoveryDialog.usersDb[recoveryDialog.targetUser].favAuthor.toLowerCase().trim();
                    var userAnswer = recoveryAnswerField.text.toLowerCase().trim();
                    if (dbAnswer === userAnswer) {
                        recoveryDialog.recoveryStep = 3;
                        errorText.visible = false;
                    } else {
                        errorText.text = "Incorrect answer!";
                        errorText.visible = true;
                    }
                }
            }
        }

        ColumnLayout {
            visible: recoveryDialog.recoveryStep === 3
            width: parent.width
            spacing: 8

            Text {
                text: "Enter your new password:"
                color: "#FFFFFF"
            }
            TextField {
                id: newPasswordField
                echoMode: TextInput.Password
                Layout.fillWidth: true
                color: "#FFFFFF"
                background: Rectangle {
                    color: "#1A0F1F"
                    border.color: "#5A3D66"
                    radius: 4
                }
            }
            Button {
                text: "Reset & Login"
                Layout.fillWidth: true
                onClicked: {
                    if (newPasswordField.text.length > 0) {
                        recoveryDialog.passwordResetSuccess(recoveryDialog.targetUser, newPasswordField.text);
                        recoveryDialog.close();
                    }
                }
            }
        }

        Text {
            id: errorText
            color: "#FF4D4D"
            visible: false
            Layout.alignment: Qt.AlignHCenter
        }
    }

    onClosed: {
        recoveryStep = 1;
        recoveryUserField.text = "";
        recoveryAnswerField.text = "";
        newPasswordField.text = "";
        errorText.visible = false;
    }
}
