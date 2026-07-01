import QtQuick
import QtQuick.Controls

Window {
    width: 400
    height: 500
    visible: true
    title: "Book Club 📚"

    Rectangle {
        anchors.fill: parent
        color: "#4B0082" // بنفش تیره حرفه‌ای
    }

    Column {
        anchors.centerIn: parent
        spacing: 25
               Text {
                  text:  "Welcome"
            font.pixelSize:28
            color:"#FFD700"
        }
        Text {
                           text:  "to"
                    font.pixelSize: 28
                    color:   "#FFD700"
                }
        Text {


            text: " BOOK CLUB "
            font.family: "Segoe UI"
            font.pixelSize: 37
            font.bold: true
            color: "#FFD700" // طلایی
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // استایل‌دهی حرفه‌ای به فیلدها
        TextField {
            id: usernameInput
            placeholderText: "Username"
            width: 280
            height: 45
            background: Rectangle {
                radius: 10
                color: "#FFFFFF"
                border.color: "#FFD700"
            }
        }

        TextField {
            id: passwordInput
            placeholderText: "Password"
            echoMode: TextInput.Password
            width: 280
            height: 45
            background: Rectangle {
                radius: 10
                color: "#FFFFFF"
                border.color: "#FFD700"
            }
        }

        // طراحی دکمه حرفه‌ای
        Button {
            text: "Login"
            width: 280
            height: 45
            contentItem: Text {
                text: parent.text
                color: "#4B0082"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
            }
            background: Rectangle {
                radius: 10
                color: parent.hovered ? "#FFC107" : "#FFD700" // تغییر رنگ هنگام بردن موس
            }
        }
    }
}