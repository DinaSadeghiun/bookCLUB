import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: notificationPage
    anchors.fill: parent

    required property var networkManager
    required property int userId
    property string username: ""
    property string userRole: ""

    property var notificationList: []
    property int unreadCount: 0

    function normalizeIsRead(value) {
        return value === true || value === 1 || value === "1"
    }

    function loadNotifications() {
        if (!networkManager) {
            console.log("NotificationView: networkManager is missing")
            return
        }

        console.log("NotificationView: loadNotifications userId =", userId)
        networkManager.getNotifications(userId)
    }

    function rebuildUnreadCount() {
        unreadCount = 0
        for (var i = 0; i < notificationModel.count; i++) {
            var item = notificationModel.get(i)
            if (!normalizeIsRead(item.isRead)) {
                unreadCount++
            }
        }
    }

    Component.onCompleted: loadNotifications()

    Connections {
        target: networkManager

        function onResponseReceived(action, status, data) {
            var ok = status === "success" || status === "SUCCESS" || status === "ok" || status === "OK"

            if (action === "getNotifications" && ok) {
                notificationList = Array.isArray(data) ? data : []
                notificationModel.clear()
                unreadCount = 0

                for (var i = 0; i < notificationList.length; i++) {
                    var item = notificationList[i] || {}

                    item.notificationId = item.notificationId !== undefined ? item.notificationId : item.id
                    item.isRead = normalizeIsRead(item.isRead)

                    if (!item.isRead) {
                        unreadCount++
                    }

                    notificationModel.append(item)
                }

                console.log("NotificationView: loaded", notificationList.length, "notifications")
                console.log("NotificationView: unreadCount =", unreadCount)
            }
            else if (action === "markNotificationAsRead" && ok) {
                console.log("NotificationView: markNotificationAsRead success -> reloading notifications")
                loadNotifications()
            }
            else if (action === "markAllNotificationsAsRead" && ok) {
                console.log("NotificationView: markAllNotificationsAsRead success -> reloading notifications")
                loadNotifications()
            }
        }

        function onNotificationReceived(notification) {
            console.log("NotificationView: new notification received:", notification.message)

            var item = notification || {}
            item.notificationId = item.notificationId !== undefined ? item.notificationId : item.id
            item.isRead = normalizeIsRead(item.isRead)

            notificationModel.insert(0, item)
            rebuildUnreadCount()
        }
    }

    ListModel {
        id: notificationModel
    }

    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "🔔 Notifications"
                font.pixelSize: 28
                font.bold: true
                color: "#D4AF37"
                Layout.fillWidth: true
            }

            Button {
                text: "Mark All Read"
                visible: unreadCount > 0

                contentItem: Text {
                    text: parent.text
                    color: "#1A0F1F"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    color: "#D4AF37"
                    radius: 5
                }

                onClicked: {
                    console.log("Mark all clicked. userId =", userId)
                    if (networkManager) {
                        networkManager.markAllNotificationsAsRead(userId)
                    }
                }
            }
        }

        Text {
            text: unreadCount > 0 ? "🔴 " + unreadCount + " unread" : "✅ All read"
            color: unreadCount > 0 ? "#FF5555" : "#4CAF50"
            font.pixelSize: 14
        }

        ListView {
            id: notificationListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            model: notificationModel
            clip: true

            delegate: Rectangle {
                width: notificationListView.width
                height: 80
                color: isRead ? "#2D1B33" : "#3D2B43"
                radius: 8
                border.color: isRead ? "#5C3D75" : "#D4AF37"
                border.width: isRead ? 1 : 2

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 12

                    Text {
                        text: {
                            var notificationType = type
                            if (notificationType === 0) return "📚"
                            if (notificationType === 1) return "💰"
                            if (notificationType === 2) return "💰"
                            if (notificationType === 3) return "⭐"
                            return "🔔"
                        }
                        font.pixelSize: 24
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            text: message || "New notification"
                            color: "white"
                            font.pixelSize: 13
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                            maximumLineCount: 2
                            elide: Text.ElideRight
                        }

                        RowLayout {
                            spacing: 10

                            Text {
                                text: {
                                    if (!createdAt) {
                                        return ""
                                    }

                                    var d = new Date(createdAt * 1000)
                                    return d.toLocaleDateString() + " " + d.toLocaleTimeString()
                                }
                                color: "#A08EAD"
                                font.pixelSize: 10
                            }

                            Rectangle {
                                visible: !isRead
                                width: 8
                                height: 8
                                radius: 4
                                color: "#D4AF37"
                            }
                        }
                    }

                    Button {
                        visible: !isRead
                        text: "✓"
                        font.pixelSize: 14

                        contentItem: Text {
                            text: parent.text
                            color: "#1A0F1F"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: "#D4AF37"
                            radius: 4
                            implicitWidth: 30
                            implicitHeight: 30
                        }

                        onClicked: {
                            console.log("Single notification button clicked. notificationId =", notificationId, "userId =", userId)
                            if (networkManager && notificationId > 0) {
                                networkManager.markNotificationAsRead(notificationId, userId)
                            }
                        }
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: notificationModel.count === 0
                text: "📭 No notifications yet"
                color: "#A08EAD"
                font.pixelSize: 16
            }
        }
    }
}
