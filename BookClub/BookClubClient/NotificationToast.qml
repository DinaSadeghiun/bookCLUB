import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: toastRoot

    signal viewRequested()

    // موقعیت‌دهی با x و y، نه anchors
    x: (parent ? parent.width : 0) / 2 - width / 2
    y: -height - 20   // شروع خارج از صفحه

    width: Math.min(440, parent ? parent.width - 40 : 440)
    height: 66
    z: 1000

    // انیمیشن برای پایین/بالا رفتن
    Behavior on y {
        NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
    }

    // تابع نمایش پیام
    function showToast(msg) {
        message = msg
        // پایین آوردن
        y = 16
        // ریستارت تایمر
        hideTimer.restart()
        // در صورت نیاز لاگ کن
        console.log("Toast shown:", msg)
    }

    // تابع مخفی‌سازی
    function hide() {
        y = -height - 20
        hideTimer.stop()
        console.log("Toast hidden")
    }

    Timer {
        id: hideTimer
        interval: 5000
        onTriggered: toastRoot.hide()
    }

    // ===== ظاهر نوار =====
    Rectangle {
        anchors.fill: parent
        radius: 12
        color: "#2D1B33"
        border.color: "#D4AF37"
        border.width: 1.5

        // سایه
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 3
            radius: 12
            color: "#000000"
            opacity: 0.25
            z: -1
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 12

            Text {
                text: "🔔"
                font.pixelSize: 22
            }

            Text {
                text: toastRoot.message
                color: "white"
                font.pixelSize: 13
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                maximumLineCount: 2
                elide: Text.ElideRight
            }

            Button {
                text: "View"
                Layout.preferredHeight: 32
                contentItem: Text {
                    text: parent.text
                    color: "#1A0F1F"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "#D4AF37"; radius: 6 }
                onClicked: {
                    toastRoot.hide()
                    toastRoot.viewRequested()
                }
            }

            Button {
                text: "✕"
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                contentItem: Text {
                    text: parent.text
                    color: "#D4AF37"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle { color: "transparent" }
                onClicked: toastRoot.hide()
            }
        }
    }
}