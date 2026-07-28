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

    // Listen to network responses to catch the assigned userId upon successful registration
    Connections {
        target: networkManager
        function onResponseReceived(action, status, data) {
            if (action === "register" && status === "success") {
                var newUserId = data.userId || data.id || 0;
                signUpPage.StackView.view.push("GenreSelection.qml", {
                    "username": usernameField.text,
                    "userRole": "User",
                    "userId": newUserId,
                    "networkManager": networkManager
                });
            } else if (action === "register" && status === "error") {
                errorText.text = data.message || "Registration failed!";
                errorText.visible = true;
            }
            else if (action === "registerPublisher") {
                if (status === "success") {
                    var newPubId = data.id || data.userId || 0;
                    signUpPage.StackView.view.replace("PublisherDashboard.qml", {
                        "username": usernameField.text,
                        "userId": newPubId,
                        "userRole": "Publisher",
                        "favoriteAuthor": favoriteAuthorField.text,
                        "networkManager": networkManager
                    });
                } else {
                    errorText.text = data.message || "Publisher registration failed!";
                    errorText.visible = true;
                }
            }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12
        width: parent.width * 0.85

        Image {
            id: mascotIcon
            source: "qrc:/images/giraffe.png"
            Layout.preferredWidth: 70
            Layout.preferredHeight: 70
            Layout.alignment: Qt.AlignHCenter
            fillMode: Image.PreserveAspectFit
        }

        Text {
            text: "Create Account"
            font.pixelSize: 26
            font.bold: true
            color: "#FFD700"
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: usernameField
            placeholderText: "Username"
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#2C003E"
            background: Rectangle { radius: 10; color: "#F5F5F5" }
        }

        TextField {
            id: passwordField
            placeholderText: "Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#2C003E"
            background: Rectangle { radius: 10; color: "#F5F5F5" }
        }

        TextField {
            id: confirmPasswordField
            placeholderText: "Confirm Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#2C003E"
            background: Rectangle { radius: 10; color: "#F5F5F5" }
        }

        // --- Security Answer / Favorite Author Field ---
        TextField {
            id: favoriteAuthorField
            placeholderText: "Favorite Author (Security Answer)"
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#2C003E"
            background: Rectangle { radius: 10; color: "#F5F5F5" }
        }

        // --- Role Selection Section ---
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
        }

        // Error message text for UI feedback
        Text {
            id: errorText
            color: "#FF6B6B"
            font.pixelSize: 13
            visible: false
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            id: registerButton
            text: "REGISTER"
            Layout.fillWidth: true
            Layout.preferredHeight: 45
            Layout.topMargin: 5

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
                errorText.visible = false

                // 1. Validation
                if (usernameField.text.trim() === "" || passwordField.text === "") {
                    errorText.text = "Error: Fields cannot be empty!"
                    errorText.visible = true
                    return
                }
                if (passwordField.text !== confirmPasswordField.text) {
                    errorText.text = "Error: Passwords do not match!"
                    errorText.visible = true
                    return
                }

                // 2. Determine selected role
                var selectedRole = radioPublisher.checked ? "Publisher" : "User"
                var secAnswer = favoriteAuthorField.text.trim() !== "" ? favoriteAuthorField.text : "default"

                // 3. Send Request to Server via NetworkManager
                if (selectedRole === "Publisher") {
                    var companyName = "Default Publisher Co.";
                    networkManager.registerPublisher(
                        usernameField.text,
                        passwordField.text,
                        secAnswer   // ← فقط securityAnswer
                    )


                } else {
                    // Send registration request for regular user and wait for network response to get userId
                    networkManager.registerUser(
                        usernameField.text,
                        passwordField.text,
                        secAnswer,
                        []
                    )
                }

                console.log("Sent registration request for:", usernameField.text, "as", selectedRole)
            }
        }
    }
}
