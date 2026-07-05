import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: signUpPage
    anchors.fill: parent

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#4B0082" }
        GradientStop { position: 1.0; color: "#2C003E" }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 15
        width: parent.width * 0.85

        Image {
            id: mascotIcon
            source: "qrc:/images/giraffe.png"
            Layout.preferredWidth: 80
            Layout.preferredHeight: 80
            Layout.alignment: Qt.AlignHCenter
            fillMode: Image.PreserveAspectFit
        }

        Text {
            text: "Create Account"
            font.pixelSize: 28
            font.bold: true
            color: "#FFD700"
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: usernameField
            placeholderText: "Username"
            Layout.fillWidth: true
            Layout.preferredHeight: 45
            color: "#2C003E"
            background: Rectangle { radius: 10; color: "#F5F5F5" }
        }


        TextField {
            id: passwordField
            placeholderText: "Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            Layout.preferredHeight: 45
            color: "#2C003E"
            background: Rectangle { radius: 10; color: "#F5F5F5" }
        }

        TextField {
            id: confirmPasswordField
            placeholderText: "Confirm Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            Layout.preferredHeight: 45
            color: "#2C003E"
            background: Rectangle { radius: 10; color: "#F5F5F5" }
        }

        ButtonGroup { id: roleGroup }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 15

            RadioButton {
                id: radioNormal
                text: "User"
                checked: true
                ButtonGroup.group: roleGroup
                contentItem: Text {
                    text: radioNormal.text
                    font.pixelSize: 14
                    color: "white"
                    leftPadding: radioNormal.indicator.width + radioNormal.spacing
                    verticalAlignment: Text.AlignVCenter
                }
            }

            RadioButton {
                id: radioPublisher
                text: "Publisher"
                ButtonGroup.group: roleGroup
                contentItem: Text {
                    text: radioPublisher.text
                    font.pixelSize: 14
                    color: "white"
                    leftPadding: radioPublisher.indicator.width + radioPublisher.spacing
                    verticalAlignment: Text.AlignVCenter
                }
            }

            RadioButton {
                id: radioAdmin
                text: "Admin"
                ButtonGroup.group: roleGroup
                contentItem: Text {
                    text: radioAdmin.text
                    font.pixelSize: 14
                    color: "white"
                    leftPadding: radioAdmin.indicator.width + radioAdmin.spacing
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        Button {
            id: registerButton
            text: "REGISTER"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            Layout.topMargin: 10

            contentItem: Text {
                text: registerButton.text
                font.pixelSize: 16
                font.bold: true
                color: "#2C003E"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                radius: 10
                color: registerButton.pressed ? "#E6C200" : "#FFD700"
                Behavior on color { ColorAnimation { duration: 100 } }
            }

            onClicked: {
                if (usernameField.text !== "" && passwordField.text !== "") {
                    let selectedRole = roleGroup.current.text;

                    if (selectedRole === "User") {
                        stackView.push("qrc:/GenreSelection.qml", {
                            "username": usernameField.text,
                            "userRole": selectedRole
                        });
                    } else {
                        stackView.push("qrc:/Dashboard.qml", {
                            "username": usernameField.text,
                            "userRole": selectedRole
                        });
                    }
                } else {
                    console.log("Validation failed: Empty fields");
                }
            }





            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    stackView.pop()
                }
            }
        }
    }
}
