import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: dashboardRoot
    anchors.fill: parent
    color: "#0F0812"

    property string username: typeof userManager !== "undefined" ? userManager.userName : "Guest"
    property string userRole: typeof userManager !== "undefined" ? userManager.userRole : "User"
    property var selectedGenres: typeof userManager !== "undefined" ? userManager.selectedGenres : ["Classic", "Mystery", "Sci-Fi"]
    property int cartItemCount: 0
    property string currentTab: "home"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 80
            Layout.fillHeight: true
            color: "#1A0F1F"

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 20
                spacing: 25

                Image {
                    source: "qrc:/assets/images/giraffe.png"
                    Layout.preferredWidth: 50; Layout.preferredHeight: 50
                    Layout.alignment: Qt.AlignHCenter
                    fillMode: Image.PreserveAspectFit
                }

                Item { Layout.preferredHeight: 20 }

                Repeater {
                    model: [
                        {icon: "🏠", tag: "home"},
                        {icon: "📚", tag: "library"},
                        {icon: "🔍", tag: "search"},
                        {icon: "🛒", tag: "cart"},
                        {icon: "⚙️", tag: "settings"}
                    ]
                    Button {
                        Layout.alignment: Qt.AlignHCenter
                        background: Rectangle {
                            implicitWidth: 50; implicitHeight: 50; radius: 12
                            color: currentTab === modelData.tag ? "#2D1B33" : "transparent"
                        }
                        contentItem: Text {
                            text: modelData.icon; font.pixelSize: 24
                            horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: currentTab = modelData.tag
                    }
                }
                Item { Layout.fillHeight: true }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: {
                if (currentTab === "home") return 0
                if (currentTab === "library") return 1
                if (currentTab === "search") return 2
                if (currentTab === "cart") return 3
                if (currentTab === "settings") return 4
                return 0
            }

            ScrollView {
                clip: true
                ColumnLayout {
                    width: parent.width - 60
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 25

                    Item { Layout.preferredHeight: 20 }

                    RowLayout {
                        spacing: 20
                        Image {
                            source: "qrc:/assets/images/giraffe.png"
                            Layout.preferredWidth: 65; Layout.preferredHeight: 65
                        }
                        ColumnLayout {
                            Text {
                                text: "Welcome Back, " + username + "! 🦒"
                                color: "#D4AF37"; font.pixelSize: 28; font.bold: true
                            }
                            Text {
                                text: "Discover your next favorite book today"; color: "#A08EAD"; font.pixelSize: 16
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true; Layout.preferredHeight: 280
                        color: "#2D1B33"; radius: 20; border.color: "#D4AF37"; border.width: 2

                        RowLayout {
                            anchors.fill: parent; anchors.margins: 20; spacing: 25
                            Rectangle {
                                Layout.preferredWidth: 130; Layout.preferredHeight: 190
                                color: "#1A0F1F"; radius: 10; border.color: "#D4AF37"
                                Text { anchors.centerIn: parent; text: "COVER"; color: "#D4AF37" }
                            }
                            ColumnLayout {
                                Layout.fillWidth: true; spacing: 8
                                Text { text: "TRENDING NOW"; color: "#D4AF37"; font.pixelSize: 11; font.bold: true }
                                Text { text: "The Great Gatsby"; color: "white"; font.pixelSize: 26; font.bold: true }
                                Text {
                                    text: "A story of hope and tragedy in the Jazz Age."; color: "#A08EAD"
                                    wrapMode: Text.WordWrap; Layout.fillWidth: true; font.pixelSize: 14
                                }
                                Item { Layout.fillHeight: true }
                                RowLayout {
                                    spacing: 15
                                    Button {
                                        text: "Read Now"
                                        background: Rectangle { color: "#D4AF37"; radius: 8; implicitWidth: 120; implicitHeight: 40 }
                                    }
                                }
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 15
                        Text { text: "Your Interests"; color: "#D4AF37"; font.pixelSize: 20; font.bold: true }
                        Row {
                            spacing: 10
                            Repeater {
                                model: selectedGenres
                                Rectangle {
                                    width: 100; height: 35; color: "transparent"; radius: 17
                                    border.color: "#D4AF37"; border.width: 1
                                    Text { anchors.centerIn: parent; text: modelData; color: "white" }
                                }
                            }
                        }
                    }

                    Item { Layout.preferredHeight: 20 }
                }
            }

            ScrollView {
                clip: true
                ColumnLayout {
                    width: parent.width - 60
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 20

                    Item { Layout.preferredHeight: 20 }

                    Text { text: "My Library"; color: "#D4AF37"; font.pixelSize: 28; font.bold: true }

                    TabBar {
                        id: libraryTabBar
                        Layout.fillWidth: true
                        background: Rectangle { color: "transparent" }

                        TabButton {
                            text: "My Books"
                            contentItem: Text { text: parent.text; color: parent.checked ? "#D4AF37" : "#A08EAD"; horizontalAlignment: Text.AlignHCenter }
                            background: Rectangle { color: "transparent" }
                        }
                        TabButton {
                            text: "Saved / Wishlist"
                            contentItem: Text { text: parent.text; color: parent.checked ? "#D4AF37" : "#A08EAD"; horizontalAlignment: Text.AlignHCenter }
                            backgr Rectangle { color: "transparent" }
                        }
                        TabButton {
                            text: "My Shelves"
                            contentItem: Text { text: parent.text; color: parent.checked ? "#D4AF37" : "#A08EAD"; horizontalAlignment: Text.AlignHCenter }
                            background: Rectangle { color: "transparent" }
                        }
                    }

                    StackLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentIndex: libraryTabBar.currentIndex

                        Item {
                            Text { anchors.centerIn: parent; text: "Your purchased books will appear here"; color: "#A08EAD" }