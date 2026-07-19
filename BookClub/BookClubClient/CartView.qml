import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: cartRoot
    anchors.fill: parent
    color: "#1A0F1F"

    ListModel {
        id: cartModel
        ListElement { bookId: 1; title: "The Great Gatsby"; author: "F. Scott Fitzgerald"; price: 15.99; discountPrice: 12.99; imagePath: "qrc:/assets/images/giraffe.png" }
        ListElement { bookId: 2; title: "1984"; author: "George Orwell"; price: 12.50; discountPrice: 12.50; imagePath: "qrc:/assets/images/giraffe.png" }
        ListElement { bookId: 3; title: "To Kill a Mockingbird"; author: "Harper Lee"; price: 18.00; discountPrice: 14.40; imagePath: "qrc:/assets/images/giraffe.png" }
    }

    function calculateOriginalTotal() {
        var total = 0.0;
        for (var i = 0; i < cartModel.count; ++i) {
            total += cartModel.get(i).price;
        }
        return total;
    }

    function calculateFinalTotal() {
        var total = 0.0;
        for (var i = 0; i < cartModel.count; ++i) {
            total += cartModel.get(i).discountPrice;
        }
        return total;
    }

    function calculateDiscountSaved() {
        return calculateOriginalTotal() - calculateFinalTotal();
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: "My Shopping Cart"
                color: "#FFD700"
                font.bold: true
                font.pixelSize: 22
                Layout.fillWidth: true
            }
            Rectangle {
                width: 70
                height: 30
                color: "#2D1B33"
                border.color: "#FFD700"
                border.width: 1
                radius: 15
                Text {
                    anchors.centerIn: parent
                    text: cartModel.count + " items"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 12
                }
            }
        }

        ListView {
            id: cartListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: cartModel
            clip: true
            spacing: 10

            delegate: Rectangle {
                width: cartListView.width
                height: 90
                color: "#2D1B33"
                radius: 8
                border.color: "#FFD700"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 15

                    Rectangle {
                        Layout.preferredWidth: 50
                        Layout.preferredHeight: 70
                        color: "#1A0F1F"
                        radius: 4
                        border.color: "#FFD700"
                        border.width: 1
                        Image {
                            anchors.fill: parent
                            anchors.margins: 3
                            source: model.imagePath
                            fillMode: Image.PreserveAspectFit
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text {
                            text: model.title
                            color: "white"
                            font.bold: true
                            font.pixelSize: 14
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                        Text {
                            text: model.author
                            color: "#A08EAD"
                            font.pixelSize: 12
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }

                    // اصلاح این بخش با استفاده از Layout.alignment به صورت Attached
                    ColumnLayout {
                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                        spacing: 2

                        Text {
                            text: "$" + model.price.toFixed(2)
                            color: "#A08EAD"
                            font.pixelSize: 11
                            font.strikeout: model.price > model.discountPrice
                            visible: model.price > model.discountPrice
                            Layout.alignment: Qt.AlignRight
                        }
                        Text {
                            text: "$" + model.discountPrice.toFixed(2)
                            color: "#FFD700"
                            font.bold: true
                            font.pixelSize: 14
                            Layout.alignment: Qt.AlignRight
                        }
                    }

                    Button {
                        Layout.preferredWidth: 35
                        Layout.preferredHeight: 35

                        contentItem: Text {
                            text: "✕"
                            color: "#FFD700"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: "transparent"
                            border.color: "#FFD700"
                            border.width: 1
                            radius: 17.5
                        }

                        onClicked: {
                            cartModel.remove(index)
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 140
            color: "#2D1B33"
            radius: 8
            border.color: "#FFD700"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "Original Total:"; color: "#A08EAD"; font.pixelSize: 13 }
                    Text { text: "$" + calculateOriginalTotal().toFixed(2); color: "white"; font.pixelSize: 13; Layout.alignment: Qt.AlignRight }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "Discount Applied:"; color: "#FFD700"; font.pixelSize: 13 }
                    Text { text: "-$" + calculateDiscountSaved().toFixed(2); color: "#FFD700"; font.pixelSize: 13; Layout.alignment: Qt.AlignRight }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#A08EAD"
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "Grand Total:"; color: "#FFD700"; font.bold: true; font.pixelSize: 16 }
                    Text { text: "$" + calculateFinalTotal().toFixed(2); color: "#FFD700"; font.bold: true; font.pixelSize: 16; Layout.alignment: Qt.AlignRight }
                }
            }
        }

        Button {
            id: checkoutButton
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            enabled: cartModel.count > 0

            contentItem: Text {
                text: "CONFIRM & PURCHASE"
                color: "#1A0F1F"
                font.bold: true
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: checkoutButton.enabled ? "#FFD700" : "#554A63"
                radius: 8
                border.color: "#FFD700"
                border.width: 1
            }

            onClicked: {
                console.log("Purchase confirmed. Transferring books to Personal Library...")
                cartModel.clear()
            }
        }
    }
}
