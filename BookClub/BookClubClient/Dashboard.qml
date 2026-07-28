import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: dashboardRoot

    required property string username
    required property string userRole
    property int userId: 0
    property var userGenres: []
    property int cartItemCount: 0
    property int unreadCount: 0
    property int currentTab: 0
    property string favoriteAuthor: ""

    property var booksById: ({})
    property var allBooksList: []

    Component.onCompleted: {
        if (networkManager) {
            networkManager.getHomeData(userId)
            networkManager.getNotifications(userId)
        }
        currentTab = 0
        loadCurrentView()
    }

    onCurrentTabChanged: loadCurrentView()

    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
    }

    function loadCurrentView() {
        var props = {
            "username": username,
            "userRole": userRole,
            "userId": userId,
            "userGenres": userGenres,
            "cartItemCount": cartItemCount,
            "networkManager": networkManager,
            "booksById": booksById,
            "allBooksList": allBooksList,
            "favoriteAuthor": favoriteAuthor
        }
        switch (currentTab) {
        case 0: contentLoader.setSource("HomeView.qml", props); break
        case 1: contentLoader.setSource("LibraryView.qml", props); break
        case 2: contentLoader.setSource("SearchView.qml", props); break
        case 3: contentLoader.setSource("CartView.qml", props); break
        case 4: contentLoader.setSource("SettingsView.qml", props); break
        case 5: contentLoader.setSource("NotificationView.qml", props); break
        default: contentLoader.setSource("HomeView.qml", props); break
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: sidebar
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            color: "#2D1B33"

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
                    color: "#D4AF37"
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

                Item { Layout.preferredHeight: 20 }

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
                        onClicked: {
                            currentTab = index
                            loadCurrentView()
                        }

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

                Button {
                    id: notificationBtn
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45

                    background: Rectangle {
                        color: currentTab === 5 ? "#D4AF37" : "transparent"
                        radius: 8
                        border.color: currentTab === 5 ? "#D4AF37" : "transparent"
                    }

                    contentItem: Text {
                        text: "🔔 Notifications"
                        color: currentTab === 5 ? "#1A0F1F" : "#D4AF37"
                        font.pixelSize: 16
                        font.bold: currentTab === 5
                        leftPadding: 15
                        verticalAlignment: Text.AlignVCenter
                    }

                    Rectangle {
                        visible: unreadCount > 0
                        width: 22; height: 22; radius: 11
                        color: "#FF5555"
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter

                        Text {
                            anchors.centerIn: parent
                            text: unreadCount > 9 ? "9+" : unreadCount
                            color: "white"
                            font.pixelSize: 11
                            font.bold: true
                        }
                    }

                    onClicked: {
                        currentTab = 5
                        loadCurrentView()
                    }
                }

                Item { Layout.fillHeight: true }

                Button {
                    Layout.fillWidth: true
                    text: "Logout"
                    onClicked: {
                        var stack = StackView.view
                        if (stack) {
                            stack.pop()
                        } else {
                            console.error("Logout: StackView not found!")
                        }
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

        Loader {
            id: contentLoader
            Layout.fillWidth: true
            Layout.fillHeight: true

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
                enabled: contentLoader.item !== null

                function onBookSelected(bookId) {
                    var mainStack = dashboardRoot.StackView.view
                    if (mainStack) {
                        console.log("Dashboard: Navigating to BookPage.qml for bookId:", bookId)
                        mainStack.push("qrc:/BookPage.qml", {
                            "bookId": bookId,
                            "networkManager": networkManager,
                            "userId": userId
                        })
                    } else {
                        console.error("Dashboard: StackView not found for navigation!")
                    }
                }

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

                function onCartItemCountChanged() {
                    if (contentLoader.item && contentLoader.item.cartItemCount !== undefined) {
                        dashboardRoot.cartItemCount = contentLoader.item.cartItemCount
                    }
                }
            }

            Connections {
                target: networkManager

                function onResponseReceived(action, status, data) {
                    var ok = status === "success" || status === "SUCCESS"

                    if (action === "getNotifications" && ok) {
                        var list = Array.isArray(data) ? data : []
                        unreadCount = 0
                        for (var i = 0; i < list.length; i++) {
                            if (!list[i].isRead) unreadCount++
                        }
                    }
                    else if (action === "markNotificationAsRead" || action === "markAllNotificationsAsRead") {
                        if (ok) {
                            networkManager.getNotifications(userId)
                        }
                    }
                }

                // ===== نوتیفیکیشن لحظه‌ای =====
                function onNotificationReceived(notification) {
                    unreadCount++
                    // نمایش Toast به جای showToast از آیتم لودر
                    notificationToast.showToast(notification.message)
                }
            }
        }
    }

    // ===== کامپوننت Toast برای نمایش نوتیفیکیشن‌های لحظه‌ای =====
    NotificationToast {
        id: notificationToast
        parent: dashboardRoot
        onViewRequested: {
            currentTab = 5
            loadCurrentView()
        }
    }
}