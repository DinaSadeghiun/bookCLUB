import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ScrollView {
    id: cartView
    clip: true

    required property int cartItemCount

    ColumnLayout {
        width: parent.width - 60
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20

        Item { Layout.preferredHeight: 20 }

        Text { text: "Shopping Cart (" + cartView.cartItemCount + ")"; color: "#D4AF37"; font.pixelSize: 28; font.bold: true }

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
            visible: cartView.cartItemCount === 0
            Text { anchors.centerIn: parent; text: "Your cart is empty"; color: "#A08EAD" }
        }

        ColumnLayout {
            visible: cartView.cartItemCount > 0
            Layout.fillWidth: true
            spacing: 15

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                color: "#2D1B33"; radius: 10
                Text { anchors.centerIn: parent; text: "Cart item"; color: "white" }
            }

            Rectangle { Layout.fillWidth: true; height: 1; color: "#2D1B33" }

            RowLayout {
                Layout.fillWidth: true
                Text { text: "Subtotal"; color: "#A08EAD"; Layout.fillWidth: true }
                Text { text: "$0.00"; color: "white" }
            }
            RowLayout {
                Layout.fillWidth: true
                Text { text: "Discount"; color: "#A08EAD"; Layout.fillWidth: true }
                Text { text: "-$0.00"; color: "#D4AF37" }
            }
            RowLayout {
                Layout.fillWidth: true
                Text { text: "Total"; color: "white"; font.bold: true; Layout.fillWidth: true }
                Text { text: "$0.00"; color: "#D4AF37"; font.bold: true }
            }

            Button {
                text: "Checkout"
                Layout.fillWidth: true
                background: Rectangle { color: "#D4AF37"; radius: 8; implicitHeight: 45 }
            }
        }
    }
}
