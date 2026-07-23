import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: loginPage
    anchors.fill: parent

    property string username: ""
    property string userRole: ""

    Connections {
        target: networkManager

        function onResponseReceived(action, status, data) {
            if (action === "login" || action === "loginPublisher" || action === "loginAdmin" || action === "signin") {
                loginButton.enabled = true
                loginButton.text = "LOGIN"

                var statusUpper = status ? status.toUpperCase() : ""

                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    loginPage.username = usernameInput.text

                    var role = data.role ? data.role.toLowerCase() : "user"
                    loginPage.userRole = role

                    var userId = data.userId || data.id || 0
                    var targetDashboard = "qrc:/Dashboard.qml"

                    if (role === "admin") {
                        targetDashboard = "qrc:/AdminDashboard.qml"
                    } else if (role === "publisher") {
                        targetDashboard = "qrc:/PublisherDashboard.qml"
                    } else {
                        targetDashboard = "qrc:/Dashboard.qml"
                    }

                    if (typeof rootStackView !== "undefined" && rootStackView !== null) {
                        rootStackView.replace(targetDashboard, {
                            "username": usernameInput.text,
                            "userRole": role,
                            "userId": userId,
                            "networkManager": networkManager
                        })
                    }
                } else {
                    errorMessageText.text = data.message || "Invalid credentials or account does not exist."
                    errorMessageText.visible = true
                }
            }
        }

        function onErrorOccurred(message) {
            loginButton.enabled = true
            loginButton.text = "LOGIN"
            errorMessageText.text = "Network Error: " + message
            errorMessageText.visible = true
        }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#4B0082" }
            GradientStop { position: 1.0; color: "#2C003E" }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 15
        width: parent.width * 0.8

        Image {
            id: mascotIcon
            source: "qrc:/images/giraffe.png"
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

        Item { Layout.preferredHeight: 5 }

        Text {
            id: errorMessageText
            visible: false
            color: "#FF5252"
            font.pixelSize: 13
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.maximumWidth: parent.width
        }

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
                errorMessageText.visible = false

                if (usernameInput.text.trim() === "" || passwordInput.text.trim() === "") {
                    errorMessageText.text = "Please fill in all fields."
                    errorMessageText.visible = true
                    return
                }

                loginButton.enabled = false
                loginButton.text = "CONNECTING..."

                networkManager.login(usernameInput.text.trim(), passwordInput.text.trim())
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
                    if (typeof rootStackView !== "undefined" && rootStackView !== null) {
                        rootStackView.push("qrc:/ForgotPassword.qml")
                    }
                }
            }
        }

        Item { Layout.preferredHeight: 5 }

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
