import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 150
    height: 260

    // Core book data (from backend)
    property string title: "Book Title"
    property string author: "Author"
    property string coverSource: ""
    property real price: 0
    property real discountPercent: 0
    property bool isFree: false
    property real averageRating: 0
    property int ratingCount: 0

    // Determines which section the card is used in
    // Possible values: "store" | "wishlist" | "shelf" | "myBooks"
    property string mode: "store"

    // Signals
    signal cardClicked()
    signal addToCartClicked()
    signal addToWishlistClicked()
    signal removeFromWishlistClicked()
    signal readClicked()
    signal moveToShelfClicked()

    readonly property real finalPrice: isFree ? 0
        : price - (price * discountPercent / 100)

    Rectangle {
        anchors.fill: parent
        color: "#2D1B4E"
        radius: 10
        border.color: "#FFD700"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 4

            Image {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 100
                Layout.preferredHeight: 130
                source: root.coverSource
                fillMode: Image.PreserveAspectCrop
                layer.enabled: true
            }

            Text {
                text: root.title
                color: "white"
                font.bold: true
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            Text {
                text: root.author
                color: "#FFD700"
                font.pixelSize: 10
                Layout.fillWidth: true
                elide: Text.ElideRight
            }

            RowLayout {
                spacing: 2
                Text {
                    text: "★ " + root.averageRating.toFixed(1)
                    color: "#FFD700"
                    font.pixelSize: 10
                }
                Text {
                    text: "(" + root.ratingCount + ")"
                    color: "#CCCCCC"
                    font.pixelSize: 9
                }
            }

            RowLayout {
                visible: root.mode === "store"
                spacing: 6
                Text {
                    visible: root.isFree
                    text: "Free"
                    color: "#00E676"
                    font.bold: true
                }
                Text {
                    visible: !root.isFree && root.discountPercent > 0
                    text: root.price.toFixed(0)
                    color: "#FF6B6B"
                    font.strikeout: true
                    font.pixelSize: 10
                }
                Text {
                    visible: !root.isFree
                    text: root.finalPrice.toFixed(0)
                    color: "white"
                    font.bold: true
                }
            }

            Button {
                Layout.fillWidth: true
                text: {
                    switch (root.mode) {
                        case "store": return "Add to Cart"
                        case "wishlist": return "Remove from Wishlist"
                        case "myBooks": return "Read"
                        case "shelf": return "Move to Shelf"
                        default: return ""
                    }
                }
                onClicked: {
                    switch (root.mode) {
                        case "store": root.addToCartClicked(); break
                        case "wishlist": root.removeFromWishlistClicked(); break
                        case "myBooks": root.readClicked(); break
                        case "shelf": root.moveToShelfClicked(); break
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            z: -1
            onClicked: root.cardClicked()
        }
    }
}
