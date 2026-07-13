import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// فایل: LibraryView.qml
ScrollView {
    id: libraryView
    contentWidth: availableWidth
    clip: true

    // Background mask to ensure it matches the dashboard
    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
        z: -1
    }

    ColumnLayout {
        width: parent.width - 40
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 25

        Item { Layout.preferredHeight: 20 }

        // Header Section
        RowLayout {
            Layout.fillWidth: true
            Text {
                text: "My Library"
                color: "#D4AF37"
                font.pixelSize: 32
                font.bold: true
                font.family: "Segoe UI" // Or your preferred comic font
            }
            Item { Layout.fillWidth: true }

            // Search in Library (Small Icon)
            Rectangle {
                width: 40; height: 40; radius: 20
                color: "#2D1B33"
                border.color: "#D4AF37"
                Text { text: "🔍"; anchors.centerIn: parent; color: "#D4AF37" }
            }
        }

        // Custom Styled TabBar
        TabBar {
            id: libraryTabBar
            Layout.fillWidth: true
            background: Rectangle { color: "transparent" }

            // یک مدل ساده برای تکرار دکمه‌ها جهت جلوگیری از تکرار کد
            Repeater {
                model: ["My Books", "Wishlist", "Shelves"]
                TabButton {
                    id: control
                    implicitWidth: 120
                    contentItem: Text {
                        text: modelData
                        font.bold: control.checked
                        font.pixelSize: 15
                        color: control.checked ? "#D4AF37" : "#A08EAD"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "transparent"
                        Rectangle {
                            anchors.bottom: parent.bottom
                            width: parent.width
                            height: control.checked ? 3 : 1
                            color: control.checked ? "#D4AF37" : "#2D1B33"
                        }
                    }
                }
            }
        }

        // Content Area based on Tabs
        StackLayout {
            id: libraryStack
            Layout.fillWidth: true
            Layout.preferredHeight: 500 // Adjust based on content
            currentIndex: libraryTabBar.currentIndex

            // Tab 0: My Books (Grid of Books)
            GridView {
                id: booksGrid
                cellWidth: 160; cellHeight: 240
                model: 5 // Placeholder for actual list model
                delegate: Column {
                    spacing: 10
                    Rectangle {
                        width: 140; height: 200
                        color: "#2D1B33"
                        border.color: "#D4AF37"
                        border.width: 2
                        radius: 5
                        Image {
                            anchors.centerIn: parent
                            source: "qrc:/assets/images/book_placeholder.png" // Create this asset
                            width: 80; height: 120; fillMode: Image.PreserveAspectFit
                        }
                    }
                    Text {
                        text: "Book Title " + (index + 1)
                        color: "white"
                        width: 140
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }
                }
            }

            // Tab 1: Wishlist
            Item {
                Column {
                    anchors.centerIn: parent
                    spacing: 10
                    Text { text: "✨"; font.pixelSize: 40; anchors.horizontalCenter: parent.horizontalCenter }
                    Text {
                        text: "Your wishlist is empty";
                        color: "#A08EAD";
                        font.italic: true
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            // Tab 2: Shelves
            Item {
                Text {
                    anchors.centerIn: parent;
                    text: "No custom shelves yet.";
                    color: "#A08EAD"
                }
            }
        }
    }
}
