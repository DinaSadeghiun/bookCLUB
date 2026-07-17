import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: settingsRoot

    required property string username
    required property string userRole
    property var userGenres: ["NonFiction", "Fiction", "Mystery"]
    property int cartItemCount: 0

    property string favoriteAuthor: "George Orwell"
    property string userPassword: "password123"
    property var purchaseHistory: [
        "1984 - George Orwell",
        "Animal Farm - George Orwell"
    ]

    signal logoutRequested()

    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
    }

    ScrollView {
        id: settingsScroll
        anchors.fill: parent
        leftPadding: 20
        rightPadding: 20
        clip: true

        ColumnLayout {
            width: settingsScroll.availableWidth
            spacing: 20

            Item { Layout.preferredHeight: 10 }

            Text {
                text: "Settings"
                color: "#D4AF37"
                font.pixelSize: 28
                font.bold: true
                Layout.fillWidth: true
            }

            //   Account Information
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: accountCol.implicitHeight + 40
                color: "#2D1B33"
                radius: 12
                border.color: "#D4AF37"
                border.width: 1

                ColumnLayout {
                    id: accountCol
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Text {
                        text: "Account Information"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    Text {
                        text: "Role: " + settingsRoot.userRole
                        color: "#D4AF37"
                        opacity: 0.7
                        font.pixelSize: 14
                        Layout.fillWidth: true
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        Text {
                            text: "Username"
                            color: "#D4AF37"
                            font.pixelSize: 13
                            font.bold: true
                            Layout.fillWidth: true
                        }
                        TextField {
                            id: usernameField
                            Layout.fillWidth: true
                            implicitHeight: 45
                            text: settingsRoot.username
                            color: "#D4AF37"
                            verticalAlignment: TextInput.AlignVCenter
                            leftPadding: 12
                            background: Rectangle {
                                color: "#1A0F1F"
                                radius: 6
                                border.color: "#D4AF37"
                                border.width: 1
                            }
                        }
                    }

                    Button {
                        text: "Update Username"
                        Layout.alignment: Qt.AlignRight
                        implicitHeight: 40
                        implicitWidth: 150
                        background: Rectangle { color: "#D4AF37"; radius: 8 }
                        contentItem: Text {
                            text: parent.text
                            color: "#1A0F1F"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            settingsRoot.username = usernameField.text
                            console.log("Update username to:", usernameField.text)
                        }
                    }
                }
            }

            //Change Password
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: passCol.implicitHeight + 40
                color: "#2D1B33"
                radius: 12
                border.color: "#D4AF37"
                border.width: 1

                ColumnLayout {
                    id: passCol
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Text {
                        text: "Change Password"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        Text { text: "Current Password"; color: "#D4AF37"; font.pixelSize: 13; Layout.fillWidth: true }
                        TextField {
                            id: currentPasswordField
                            Layout.fillWidth: true
                            implicitHeight: 45
                            placeholderText: "Enter Current Password"
                            placeholderTextColor: "#80D4AF37"
                            echoMode: TextInput.Password
                            color: "#D4AF37"
                            verticalAlignment: TextInput.AlignVCenter
                            leftPadding: 12
                            background: Rectangle {
                                color: "#1A0F1F"; radius: 6
                                border.color: "#D4AF37"; border.width: 1
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        Text { text: "New Password"; color: "#D4AF37"; font.pixelSize: 13; Layout.fillWidth: true }
                        TextField {
                            id: newPasswordField
                            Layout.fillWidth: true
                            implicitHeight: 45
                            placeholderText: "Enter New Password"
                            placeholderTextColor: "#80D4AF37"
                            echoMode: TextInput.Password
                            color: "#D4AF37"
                            verticalAlignment: TextInput.AlignVCenter
                            leftPadding: 12
                            background: Rectangle {
                                color: "#1A0F1F"; radius: 6
                                border.color: "#D4AF37"; border.width: 1
                            }
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        Text { text: "Confirm New Password"; color: "#D4AF37"; font.pixelSize: 13; Layout.fillWidth: true }
                        TextField {
                            id: confirmPasswordField
                            Layout.fillWidth: true
                            implicitHeight: 45
                            placeholderText: "Confirm New Password"
                            placeholderTextColor: "#80D4AF37"
                            echoMode: TextInput.Password
                            color: "#D4AF37"
                            verticalAlignment: TextInput.AlignVCenter
                            leftPadding: 12
                            background: Rectangle {
                                color: "#1A0F1F"; radius: 6
                                border.color: "#D4AF37"; border.width: 1
                            }
                        }
                    }

                    Text {
                        id: passwordError
                        color: "#FF4444"
                        visible: text.length > 0
                        text: ""
                        font.pixelSize: 13
                        Layout.fillWidth: true
                    }

                    Button {
                        text: "Update Password"
                        Layout.alignment: Qt.AlignRight
                        implicitHeight: 40
                        implicitWidth: 150
                        background: Rectangle { color: "#D4AF37"; radius: 8 }
                        contentItem: Text {
                            text: parent.text
                            color: "#1A0F1F"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            if (newPasswordField.text !== confirmPasswordField.text) {
                                passwordError.text = "Passwords do not match"
                                return
                            }
                            passwordError.text = ""
                            console.log("Update password requested")
                            currentPasswordField.text = ""
                            newPasswordField.text = ""
                            confirmPasswordField.text = ""
                        }
                    }
                }
            }

            // Preferences
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: prefCol.implicitHeight + 40
                color: "#2D1B33"
                radius: 12
                border.color: "#D4AF37"
                border.width: 1

                ColumnLayout {
                    id: prefCol
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Text {
                        text: "Preferences"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    Text { text: "Favorite Genres"; color: "#D4AF37"; font.pixelSize: 13; font.bold: true; Layout.fillWidth: true }

                    Flow {
                        Layout.fillWidth: true
                        spacing: 8

                        Repeater {
                            model: settingsRoot.userGenres
                            delegate: Rectangle {
                                color: "#1A0F1F"
                                border.color: "#D4AF37"
                                border.width: 1
                                radius: 14
                                height: 30
                                width: genreTag.implicitWidth + 24

                                Text {
                                    id: genreTag
                                    anchors.centerIn: parent
                                    text: modelData
                                    color: "#D4AF37"
                                    font.pixelSize: 13
                                }
                            }
                        }
                    }

                    Button {
                        text: "Change Genres"
                        Layout.alignment: Qt.AlignRight
                        implicitHeight: 40
                        implicitWidth: 140
                        background: Rectangle { color: "transparent"; border.color: "#D4AF37"; border.width: 1; radius: 8 }
                        contentItem: Text {
                            text: parent.text
                            color: "#D4AF37"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            var targetStack = null;

                            if (typeof stackView !== "undefined" && stackView !== null) {
                                targetStack = stackView;
                            } else if (typeof mainStack !== "undefined" && mainStack !== null) {
                                targetStack = mainStack;
                            } else if (settingsRoot.StackView && settingsRoot.StackView.view) {
                                targetStack = settingsRoot.StackView.view;
                            }

                            if (targetStack) {
                                var pushedItem = null;
                                try {
                                    pushedItem = targetStack.push("GenreSelection.qml", {
                                        "username": settingsRoot.username,
                                        "userRole": settingsRoot.userRole,
                                        "isEditMode": true,
                                        "initialGenres": settingsRoot.userGenres
                                    });
                                } catch (e) {
                                    console.log("Relative path failed, trying QRC path...");
                                    try {
                                        pushedItem = targetStack.push("qrc:/GenreSelection.qml", {
                                            "username": settingsRoot.username,
                                            "userRole": settingsRoot.userRole,
                                            "isEditMode": true,
                                            "initialGenres": settingsRoot.userGenres
                                        });
                                    } catch (e2) {
                                        console.error("Error loading GenreSelection.qml:", e2.message);
                                    }
                                }

                                if (pushedItem) {
                                    pushedItem.genresSaved.connect(function(genres) {
                                        settingsRoot.userGenres = genres;
                                    });
                                }
                            } else {
                                console.error("Could not find StackView/mainStack in the component hierarchy.");
                            }
                        }
                    }


                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        Text { text: "Favorite Author"; color: "#D4AF37"; font.pixelSize: 13; font.bold: true; Layout.fillWidth: true }
                        TextField {
                            id: authorField
                            Layout.fillWidth: true
                            implicitHeight: 45
                            text: settingsRoot.favoriteAuthor
                            color: "#D4AF37"
                            verticalAlignment: TextInput.AlignVCenter
                            leftPadding: 12
                            background: Rectangle {
                                color: "#1A0F1F"; radius: 6
                                border.color: "#D4AF37"; border.width: 1
                            }
                        }
                    }

                    Button {
                        text: "Update Favorite Author"
                        Layout.alignment: Qt.AlignRight
                        implicitHeight: 40
                        implicitWidth: 180
                        background: Rectangle { color: "#D4AF37"; radius: 8 }
                        contentItem: Text {
                            text: parent.text
                            color: "#1A0F1F"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            settingsRoot.favoriteAuthor = authorField.text
                            console.log("Update favorite author to:", authorField.text)
                        }
                    }
                }
            }

            //  Purchase History
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: historyCol.implicitHeight + 40
                color: "#2D1B33"
                radius: 12
                border.color: "#D4AF37"
                border.width: 1

                ColumnLayout {
                    id: historyCol
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: "Purchase History"
                            color: "#D4AF37"
                            font.pixelSize: 18
                            font.bold: true
                        }
                        Item { Layout.fillWidth: true }
                        Text {
                            text: "Books purchased: " + settingsRoot.purchaseHistory.length
                            color: "#D4AF37"
                            font.pixelSize: 13
                        }
                    }

                    Repeater {
                        model: settingsRoot.purchaseHistory
                        delegate: Rectangle {
                            Layout.fillWidth: true
                            implicitHeight: 45
                            color: "#1A0F1F"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 0.5

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 15
                                text: modelData
                                color: "#D4AF37"
                                font.pixelSize: 13
                            }
                        }
                    }

                    Text {
                        visible: settingsRoot.purchaseHistory.length === 0
                        text: "No purchases yet."
                        color: "#D4AF37"
                        opacity: 0.6
                        font.pixelSize: 13
                        Layout.fillWidth: true
                    }
                }
            }

            Button {
                Layout.fillWidth: true
                implicitHeight: 45
                text: "Logout"
                background: Rectangle { color: "transparent"; border.color: "#D4AF37"; border.width: 1; radius: 8 }
                contentItem: Text {
                    text: parent.text
                    color: "#D4AF37"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: settingsRoot.logoutRequested()
            }

            Item { Layout.preferredHeight: 30 }
        }
    }
}
