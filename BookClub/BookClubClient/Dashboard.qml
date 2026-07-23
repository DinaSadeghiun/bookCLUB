import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: dashboardRoot

    required property string username
    required property string userRole
    property int userId: 0 // شناسه کاربر برای درخواست‌های شبکه
    property var userGenres: []
    property int cartItemCount: 0
    property int currentTab: 0

    // Background for the entire dashboard
    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F" // Match GenreSelection background
    }

    Component.onCompleted: loadCurrentView()
    onCurrentTabChanged: loadCurrentView()

    function loadCurrentView() {
        var props = {
            "username": username,
            "userRole": userRole,
            "userId": userId,
            "userGenres": userGenres,
            "cartItemCount": cartItemCount,
            "networkManager": networkManager // ارسال مستقیم NetworkManager به تب‌ها
        }
        switch (currentTab) {
        case 0: contentLoader.setSource("HomeView.qml", props); break
        case 1: contentLoader.setSource("LibraryView.qml", props); break
        case 2: contentLoader.setSource("SearchView.qml", props); break
        case 3: contentLoader.setSource("CartView.qml", props); break
        case 4: contentLoader.setSource("SettingsView.qml", props); break
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ---------- Sidebar ----------
        Rectangle {
            id: sidebar
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            color: "#2D1B33" // Dark Purple sidebar

            // Subtle Gold Border on the right
            Rectangle {
                width: 1
                anchors.right: parent.right
                height: parent.height
                color: "#D4AF37"
                opacity: 0.3
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 15

                // Giraffe Logo Area
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 100
                    height: 100
                    radius: 50
                    color: "#1A0F1F"
                    border.color: "#D4AF37"
                    border.width: 2

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/assets/images/giraffe.png"
                        fillMode: Image.PreserveAspectFit
                        width: 70
                        height: 70
                    }
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Welcome, " + username
                    color: "#D4AF37" // Gold text
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: userRole
                    color: "#D4AF37"
                    font.pixelSize: 12
                    opacity: 0.7
                }

                Item { Layout.preferredHeight: 20 } // Spacer

                // Navigation Buttons
                Repeater {
                    model: ["Home", "Library", "Search", "Cart", "Settings"]
                    delegate: Button {
                        id: navButton
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45

                        background: Rectangle {
                            color: currentTab === index ? "#D4AF37" : "transparent"
                            radius: 8
                            border.color: currentTab === index ? "#D4AF37" : "transparent"
                        }

                        contentItem: Text {
                            text: modelData
                            color: currentTab === index ? "#1A0F1F" : "#D4AF37"
                            font.pixelSize: 16
                            font.bold: currentTab === index
                            leftPadding: 15
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: currentTab = index

                        // Cart badge
                        Rectangle {
                            visible: index === 3 && cartItemCount > 0
                            width: 22; height: 22; radius: 11
                            color: currentTab === index ? "#1A0F1F" : "#D4AF37"
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            anchors.verticalCenter: parent.verticalCenter

                            Text {
                                anchors.centerIn: parent
                                text: cartItemCount
                                color: currentTab === index ? "#D4AF37" : "#1A0F1F"
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }
                    }
                }

                Item { Layout.fillHeight: true } // spacer

                // Logout Button
                Button {
                    Layout.fillWidth: true
                    text: "Logout"
                    onClicked: {
                        if (dashboardRoot.StackView.view)
                            dashboardRoot.StackView.view.pop()
                    }
                    background: Rectangle {
                        color: "transparent"
                        border.color: "#D4AF37"
                        border.width: 1
                        radius: 8
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#D4AF37"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        // ---------- Content area ----------
        Loader {
            id: contentLoader
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Animation for smooth tab switching
            onSourceChanged: {
                opacity = 0
                fadeIn.start()
            }

            NumberAnimation {
                id: fadeIn
                target: contentLoader
                property: "opacity"
                from: 0
                to: 1
                duration: 250
            }

            Connections {
                target: contentLoader.item
                ignoreUnknownSignals: true

                function onLogoutRequested() {
                    var sv = dashboardRoot.StackView.view
                    if (sv) {
                        sv.clear()
                        sv.push("Login.qml")
                    }
                }

                function onUserGenresChanged() {
                    if (contentLoader.item && contentLoader.item.userGenres !== undefined) {
                        dashboardRoot.userGenres = contentLoader.item.userGenres
                    }
                }
            }
        }
    }
}
