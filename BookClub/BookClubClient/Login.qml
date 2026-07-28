
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: loginPage
    anchors.fill: parent

    property string username: ""
    property string userRole: ""
    property string forgotUsername: ""
    property string forgotRole: ""
    property string forgotSecurityAnswer: ""
    property string forgotError: ""

    property string currentState: "login"  // "login" | "forgotUsername" | "forgotSecurity" | "forgotNewPassword"

    function doLogin() {
        var user = usernameInput.text.trim()
        var pass = passwordInput.text.trim()
        if (user === "" || pass === "") {
            errorMessageText.text = "Please fill in all fields."
            errorMessageText.visible = true
            return
        }
        errorMessageText.visible = false
        loginButton.enabled = false
        loginButton.text = "CONNECTING..."
        networkManager.login(user, pass)
    }

    function goToForgotUsername() {
        currentState = "forgotUsername"
        forgotError = ""
        forgotUsernameInput.text = ""
    }

    function goToForgotSecurity(username, role) {
        forgotUsername = username
        forgotRole = role
        currentState = "forgotSecurity"
        forgotError = ""
        forgotSecurityInput.text = ""
    }

    function goToForgotNewPassword() {
        currentState = "forgotNewPassword"
        forgotError = ""
        forgotNewPasswordInput.text = ""
        forgotConfirmPasswordInput.text = ""
    }

    function goBackToLogin() {
        currentState = "login"
        forgotError = ""
    }

    Connections {
        target: networkManager

        function onResponseReceived(action, status, data) {
            var ok = status === "success" || status === "SUCCESS"
            var statusUpper = status ? status.toString().trim().toUpperCase() : ""

            // ===== ۱. لاگین =====
            if (action === "login" || action === "loginPublisher" || action === "loginAdmin" || action === "signin") {
                loginButton.enabled = true
                loginButton.text = "LOGIN"

                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    loginPage.username = usernameInput.text
                    var role = (data && data.role) ? data.role.toString().trim().toLowerCase() : "user"
                    loginPage.userRole = role

                    var userId = 0
                    if (data) {
                        userId = data.userId || data.id || 0
                    }

                    var targetDashboard = "qrc:/Dashboard.qml"
                    if (role === "admin") {
                        targetDashboard = "qrc:/AdminDashboard.qml"
                    } else if (role === "publisher") {
                        targetDashboard = "qrc:/PublisherDashboard.qml"
                    }

                    if (typeof rootStackView !== "undefined" && rootStackView !== null) {
                        rootStackView.replace(targetDashboard, {
                            "username": usernameInput.text,
                            "userRole": role,
                            "userId": userId,
                            "favoriteAuthor": data && data.securityAnswer ? data.securityAnswer : "",
                            "networkManager": networkManager,
                            "bookPageStack": rootStackView
                        })
                    }
                } else {
                    errorMessageText.text = data && data.message ? data.message : "Login failed."
                    errorMessageText.visible = true
                }
            }

            // ===== ۲. checkUsernameExists =====
            else if (action === "checkUsernameExists") {
                forgotContinueBtn.enabled = true
                forgotContinueBtn.text = "Continue"

                if (ok && data && data.role) {
                    var role = data.role.toString().toLowerCase()
                    goToForgotSecurity(forgotUsernameInput.text.trim(), role)
                } else {
                    forgotError = data && data.message ? data.message : "No account found with this username"
                }
            }

            // ===== ۳. verifySecurityAnswer =====
            else if (action === "verifySecurityAnswer") {
                forgotVerifyBtn.enabled = true
                forgotVerifyBtn.text = "Verify"

                if (ok && data && data.valid === true) {
                    forgotSecurityAnswer = forgotSecurityInput.text.trim()
                    goToForgotNewPassword()
                } else {
                    forgotError = data && data.message ? data.message : "Incorrect security answer"
                }
            }

            // ===== ۴. resetPassword =====
            else if (action === "resetPassword") {
                forgotResetBtn.enabled = true
                forgotResetBtn.text = "Reset Password"

                if (ok) {
                    successDialog.open()
                } else {
                    forgotError = data && data.message ? data.message : "Failed to reset password"
                }
            }
        }

        function onErrorOccurred(message) {
            loginButton.enabled = true
            loginButton.text = "LOGIN"
            forgotContinueBtn.enabled = true
            forgotContinueBtn.text = "Continue"
            forgotVerifyBtn.enabled = true
            forgotVerifyBtn.text = "Verify"
            forgotResetBtn.enabled = true
            forgotResetBtn.text = "Reset Password"

            if (currentState === "login") {
                errorMessageText.text = "Network Error: " + message
                errorMessageText.visible = true
            } else {
                forgotError = "Network Error: " + message
            }
        }
    }

    // ===== Dialog موفقیت =====
    Dialog {
        id: successDialog
        title: "✅ Password Reset Successful"
        anchors.centerIn: parent
        modal: true
        closePolicy: Popup.CloseOnEscape

        background: Rectangle {
            color: "#2D1B33"
            border.color: "#D4AF37"
            border.width: 2
            radius: 10
        }

        ColumnLayout {
            spacing: 15
            width: 280

            Text {
                text: "Your password has been reset successfully!"
                color: "white"
                font.pixelSize: 14
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Text {
                text: "Please login with your new password."
                color: "#A08EAD"
                font.pixelSize: 12
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }

            Button {
                text: "Go to Login"
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                contentItem: Text {
                    text: parent.text
                    color: "#1A0F1F"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: "#D4AF37"
                    radius: 8
                }
                onClicked: {
                    successDialog.close()
                    goBackToLogin()
                }
            }
        }
    }

    // ===== پس‌زمینه =====
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#4B0082" }
            GradientStop { position: 1.0; color: "#2C003E" }
        }
    }

    // =============================================
    // ===== ۱. صفحه لاگین =====
    // =============================================
    ColumnLayout {
        id: loginView
        anchors.centerIn: parent
        spacing: 15
        width: parent.width * 0.8
        visible: currentState === "login"

        Image {
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
            onAccepted: doLogin()
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
            onAccepted: doLogin()
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

            onClicked: doLogin()
        }

        Text {
            text: "Forgot Password?"
            font.pixelSize: 13
            color: "#B0A4E3"
            Layout.alignment: Qt.AlignHCenter

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: goToForgotUsername()
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
                        }
                    }
                }
            }
        }
    }

    // =============================================
    // ===== ۲. صفحه اول Forgot: Username =====
    // =============================================
    ColumnLayout {
        id: forgotUsernameView
        anchors.centerIn: parent
        spacing: 15
        width: parent.width * 0.8
        visible: currentState === "forgotUsername"

        Text {
            text: "🔐 Forgot Password"
            font.pixelSize: 28
            font.bold: true
            color: "#D4AF37"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Enter your username to verify your identity"
            color: "#A08EAD"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
        }

        TextField {
            id: forgotUsernameInput
            placeholderText: "Username"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "white"
            leftPadding: 15
            background: Rectangle {
                radius: 10
                color: "#2D1B33"
                border.color: forgotUsernameInput.activeFocus ? "#D4AF37" : "#5c3d75"
                border.width: 1
            }
            onAccepted: forgotContinueBtn.clicked()
        }

        Text {
            text: forgotError
            visible: text !== ""
            color: "#FF5555"
            font.pixelSize: 12
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            id: forgotContinueBtn
            text: "Continue"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            contentItem: Text {
                text: parent.text
                color: "#1A0F1F"
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: "#D4AF37"
                radius: 10
            }
            onClicked: {
                var user = forgotUsernameInput.text.trim()
                if (user === "") {
                    forgotError = "Please enter your username"
                    return
                }
                forgotError = ""
                forgotContinueBtn.enabled = false
                forgotContinueBtn.text = "Checking..."
                networkManager.checkUsernameExists(user)
            }
        }

        Button {
            text: "← Back to Login"
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            contentItem: Text {
                text: parent.text
                color: "#A08EAD"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: "transparent"
                border.color: "#5c3d75"
                border.width: 1
                radius: 8
            }
            onClicked: goBackToLogin()
        }
    }

    // =============================================
    // ===== ۳. صفحه دوم Forgot: Security Answer =====
    // =============================================
    ColumnLayout {
        id: forgotSecurityView
        anchors.centerIn: parent
        spacing: 15
        width: parent.width * 0.8
        visible: currentState === "forgotSecurity"

        Text {
            text: "🔐 Security Verification"
            font.pixelSize: 28
            font.bold: true
            color: "#D4AF37"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Username: " + forgotUsername
            color: "#FFFFFF"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Please answer your security question:"
            color: "#A08EAD"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Who is your favorite author?"
            color: "#D4AF37"
            font.pixelSize: 13
            font.italic: true
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: forgotSecurityInput
            placeholderText: "Your security answer"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "white"
            leftPadding: 15
            background: Rectangle {
                radius: 10
                color: "#2D1B33"
                border.color: forgotSecurityInput.activeFocus ? "#D4AF37" : "#5c3d75"
                border.width: 1
            }
            onAccepted: forgotVerifyBtn.clicked()
        }

        Text {
            text: forgotError
            visible: text !== ""
            color: "#FF5555"
            font.pixelSize: 12
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            id: forgotVerifyBtn
            text: "Verify"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            contentItem: Text {
                text: parent.text
                color: "#1A0F1F"
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: "#D4AF37"
                radius: 10
            }
            onClicked: {
                var answer = forgotSecurityInput.text.trim()
                if (answer === "") {
                    forgotError = "Please enter your security answer"
                    return
                }
                forgotError = ""
                forgotVerifyBtn.enabled = false
                forgotVerifyBtn.text = "Verifying..."
                networkManager.verifySecurityAnswer(forgotUsername, forgotRole, answer)
            }
        }

        Button {
            text: "← Back"
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            contentItem: Text {
                text: parent.text
                color: "#A08EAD"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: "transparent"
                border.color: "#5c3d75"
                border.width: 1
                radius: 8
            }
            onClicked: goToForgotUsername()
        }
    }

    // =============================================
    // ===== ۴. صفحه سوم Forgot: New Password =====
    // =============================================
    ColumnLayout {
        id: forgotNewPasswordView
        anchors.centerIn: parent
        spacing: 15
        width: parent.width * 0.8
        visible: currentState === "forgotNewPassword"

        Text {
            text: "🔐 Reset Password"
            font.pixelSize: 28
            font.bold: true
            color: "#D4AF37"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Username: " + forgotUsername
            color: "#FFFFFF"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Enter your new password"
            color: "#A08EAD"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: forgotNewPasswordInput
            placeholderText: "New Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "white"
            leftPadding: 15
            background: Rectangle {
                radius: 10
                color: "#2D1B33"
                border.color: forgotNewPasswordInput.activeFocus ? "#D4AF37" : "#5c3d75"
                border.width: 1
            }
            onAccepted: forgotResetBtn.clicked()
        }

        TextField {
            id: forgotConfirmPasswordInput
            placeholderText: "Confirm New Password"
            echoMode: TextInput.Password
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "white"
            leftPadding: 15
            background: Rectangle {
                radius: 10
                color: "#2D1B33"
                border.color: forgotConfirmPasswordInput.activeFocus ? "#D4AF37" : "#5c3d75"
                border.width: 1
            }
            onAccepted: forgotResetBtn.clicked()
        }

        Text {
            text: forgotError
            visible: text !== ""
            color: "#FF5555"
            font.pixelSize: 12
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            id: forgotResetBtn
            text: "Reset Password"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            contentItem: Text {
                text: parent.text
                color: "#1A0F1F"
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: "#D4AF37"
                radius: 10
            }
            onClicked: {
                var newPass = forgotNewPasswordInput.text
                var confirmPass = forgotConfirmPasswordInput.text

                if (newPass !== confirmPass) {
                    forgotError = "Passwords do not match"
                    return
                }
                if (newPass === "") {
                    forgotError = "Please enter a password"
                    return
                }
                forgotError = ""

                forgotResetBtn.enabled = false
                forgotResetBtn.text = "Resetting..."
                networkManager.resetPassword(forgotUsername, forgotSecurityAnswer, newPass, forgotRole)
            }
        }

        Button {
            text: "← Back"
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            contentItem: Text {
                text: parent.text
                color: "#A08EAD"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: "transparent"
                border.color: "#5c3d75"
                border.width: 1
                radius: 8
            }
            onClicked: goToForgotSecurity(forgotUsername, forgotRole)
        }
    }
}