import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: settingsRoot
    anchors.fill: parent
    contentWidth: availableWidth
    clip: true

    // Properties
    required property string username
    required property string userRole
    property var userGenres: ["NonFiction", "Fiction", "Mystery"]
    property string favoriteAuthor: "George Orwell"
    property int purchasedBooksCount: 3

    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F" // پس‌زمینه تیره اصلی
    }

    ColumnLayout {
        width: parent.width - 60
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 25 // فاصله بیشتر بین بخش‌ها

        // هدر اصلی با فاصله از بالا
        Item { Layout.preferredHeight: 20 }

        Text {
            text: "Account Settings & Profile"
            color: "#D4AF37"
            font.pixelSize: 28
            font.bold: true
            Layout.bottomMargin: 10
        }

        // --- بخش اول: اطلاعات کلی ---
        GroupBox {
            title: "Account Overview"
            Layout.fillWidth: true

            background: Rectangle {
                color: "#2D1B33"
                border.color: "#D4AF37"
                border.width: 1
                radius: 8
            }

            label: Text {
                text: parent.title
                color: "#D4AF37"
                font.pixelSize: 16
                font.bold: true
                y: -12 // بالا بردن لیبل برای عدم تداخل با کادر
                leftPadding: 10
                rightPadding: 10
                background: Rectangle { color: "#1A0F1F" } // ایجاد فضای خالی پشت متن لیبل
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15 // ایجاد فاصله داخلی (Padding)
                spacing: 10

                Text { text: "Current Username: " + username; color: "white"; font.pixelSize: 14 }
                Text { text: "Role: " + userRole; color: "white"; font.pixelSize: 14 }
                Text { text: "Total Purchased Books: " + purchasedBooksCount; color: "#D4AF37"; font.bold: true }
            }
        }

        // --- بخش دوم: ویرایش اطلاعات ---
        GroupBox {
            title: "Manage Account Credentials"
            Layout.fillWidth: true

            background: Rectangle {
                color: "#2D1B33"
                border.color: "#D4AF37"
                border.width: 1
                radius: 8
            }

            label: Text {
                text: parent.title
                color: "#D4AF37"
                font.pixelSize: 16
                font.bold: true
                y: -12
                leftPadding: 10
                background: Rectangle { color: "#1A0F1F" }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 12

                Text { text: "Edit Username"; color: "white"; font.pixelSize: 13 }
                TextField {
                    id: userEdit
                    text: username
                    Layout.fillWidth: true
                    selectByMouse: true
                    background: Rectangle { radius: 4; color: "white" }
                }

                Text { text: "Favorite Author"; color: "white"; font.pixelSize: 13 }
                TextField {
                    id: authorEdit
                    text: favoriteAuthor
                    Layout.fillWidth: true
                    background: Rectangle { radius: 4; color: "white" }
                }

                Button {
                    text: "Save Information"
                    Layout.alignment: Qt.AlignRight
                    Layout.topMargin: 5
                    contentItem: Text {
                        text: parent.text
                        color: "#1A0F1F"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                    }
                    background: Rectangle {
                        color: "#D4AF37"
                        radius: 5
                    }
                }
            }
        }

        // --- بخش سوم: تاریخچه خرید (رفع تداخل متنی) ---
        GroupBox {
            title: "Purchase History"
            Layout.fillWidth: true

            background: Rectangle {
                color: "#2D1B33"
                border.color: "#D4AF37"
                border.width: 1
                radius: 8
            }

            label: Text {
                text: parent.title
                color: "#D4AF37"
                font.pixelSize: 16
                font.bold: true
                y: -12
                leftPadding: 10
                background: Rectangle { color: "#1A0F1F" }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 15

                Repeater {
                    model: [
                        { t: "1984", d: "2026-05-12", p: "$9.99" },
                        { t: "Animal Farm", d: "2026-06-01", p: "$6.99" }
                    ]
                    delegate: RowLayout {
                        Layout.fillWidth: true
                        Text { text: modelData.t; color: "white"; font.bold: true; Layout.fillWidth: true }
                        Text { text: modelData.d; color: "#D4AF37"; opacity: 0.7; Layout.rightMargin: 10 }
                        Text { text: modelData.p; color: "#D4AF37"; font.bold: true }
                    }
                }
            }
        }

        Item { Layout.preferredHeight: 30 }
    }
}
