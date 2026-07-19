import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: homeRoot

    // Properties received from Dashboard
    required property string username
    required property string userRole
    property var userGenres: []
    property int cartItemCount: 0

    // Sample book model for populating sections dynamically
    ListModel {
        id: booksModel
        ListElement { title: "1984"; author: "George Orwell"; genre: "Fiction"; price: "Free"; rating: "4.8"; image: "qrc:/assets/books/1984.png" }
        ListElement { title: "Animal Farm"; author: "George Orwell"; genre: "Fiction"; price: "$8.99"; rating: "4.7"; image: "qrc:/assets/books/animal_farm.png" }
        ListElement { title: "The Hobbit"; author: "J.R.R. Tolkien"; genre: "Fantasy"; price: "$12.50"; rating: "4.9"; image: "qrc:/assets/books/hobbit.png" }
        ListElement { title: "Dune"; author: "Frank Herbert"; genre: "Sci-Fi"; price: "Free"; rating: "4.6"; image: "qrc:/assets/books/dune.png" }
        ListElement { title: "Sherlock Holmes"; author: "Arthur Conan Doyle"; genre: "Mystery"; price: "$5.99"; rating: "4.8"; image: "qrc:/assets/books/sherlock.png" }
        ListElement { title: "The Great Gatsby"; author: "F. Scott Fitzgerald"; rating: "4.4"; genre: "Classic"; price: "$9.99"; image: "qrc:/assets/books/gatsby.png" }
    }

    // Main background styling with dark purple theme
    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
    }

    ScrollView {
        id: homeScroll
        anchors.fill: parent
        clip: true
        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        // Pane is used here because it correctly supports individual padding properties
        Pane {
            width: homeScroll.availableWidth
            topPadding: 15
            bottomPadding: 30
            leftPadding: 20
            rightPadding: 20
            background: null // Make the background transparent

            contentItem: ColumnLayout {
                spacing: 25
                width: parent.width

                // 1. Dynamic Welcome Header
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    ColumnLayout {
                        spacing: 4
                        Text {
                            text: "Welcome back, " + homeRoot.username + "!"
                            color: "#D4AF37"
                            font.pixelSize: 24
                            font.bold: true
                        }
                        Text {
                            text: "Discover your next great read today"
                            color: "#D4AF37"
                            opacity: 0.7
                            font.pixelSize: 14
                        }
                    }

                    Item { Layout.fillWidth: true }

                    // User Role Badge
                    Rectangle {
                        color: "#2D1B33"
                        border.color: "#D4AF37"
                        border.width: 1
                        radius: 15
                        implicitWidth: roleText.implicitWidth + 24
                        implicitHeight: 30
                        Text {
                            id: roleText
                            anchors.centerIn: parent
                            text: homeRoot.userRole.toUpperCase()
                            color: "#D4AF37"
                            font.bold: true
                            font.pixelSize: 11
                        }
                    }
                }

                // 2. Recommended Books Section (based on user genres)
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    visible: homeRoot.userGenres.length > 0

                    Text {
                        text: "Recommended For You"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    ListView {
                        id: recommendedList
                        Layout.fillWidth: true
                        implicitHeight: 240
                        orientation: ListView.Horizontal
                        spacing: 15
                        model: booksModel
                        clip: true
                        delegate: bookDelegate
                    }
                }

                // 3. User Selected Favorite Genres Section
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    Text {
                        text: "Your Favorite Genres"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }
                    Flow {
                        Layout.fillWidth: true
                        spacing: 10
                        Repeater {
                            model: homeRoot.userGenres
                            delegate: Rectangle {
                                color: "#2D1B33"
                                border.color: "#D4AF37"
                                border.width: 1
                                radius: 20
                                implicitHeight: 35
                                implicitWidth: genreName.implicitWidth + 30
                                Text {
                                    id: genreName
                                    anchors.centerIn: parent
                                    text: modelData
                                    color: "#D4AF37"
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                        }
                    }
                }

                // 4. Trending & Best Sellers Section
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Text {
                        text: "Trending & Best Sellers"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        implicitHeight: 240
                        orientation: ListView.Horizontal
                        spacing: 15
                        model: booksModel
                        clip: true
                        delegate: bookDelegate
                    }
                }

                // 5. Popular & New Releases Section
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Text {
                        text: "Popular & New Releases"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        implicitHeight: 240
                        orientation: ListView.Horizontal
                        spacing: 15
                        model: booksModel
                        clip: true
                        delegate: bookDelegate
                    }
                }

                // 6. Free Books Section
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Text {
                        text: "Free Books"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        implicitHeight: 240
                        orientation: ListView.Horizontal
                        spacing: 15
                        model: booksModel
                        clip: true
                        delegate: bookDelegate
                    }
                }
            }
        }
    }

    // Common Book Card Delegate
    Component {
        id: bookDelegate
        Rectangle {
            width: 140
            height: 230
            color: "#2D1B33"
            radius: 8
            border.color: "#3d2545"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 6

                // Book Cover Container
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#1A0F1F"
                    radius: 4
                    clip: true

                    Image {
                        anchors.fill: parent
                        source: model.image
                        fillMode: Image.PreserveAspectFit
                        onStatusChanged: {
                            if (status === Image.Error) {
                                source = "qrc:/assets/books/default_cover.png"
                            }
                        }
                    }
                }

                // Title
                Text {
                    text: model.title
                    color: "#D4AF37"
                    font.pixelSize: 13
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                // Author
                Text {
                    text: model.author
                    color: "#D4AF37"
                    opacity: 0.7
                    font.pixelSize: 11
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                // Footer layout for price and rating
                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: model.price
                        color: model.price === "Free" ? "#4CAF50" : "#D4AF37"
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Item { Layout.fillWidth: true }

                    RowLayout {
                        spacing: 2
                        Text {
                            text: "⭐"
                            font.pixelSize: 10
                        }
                        Text {
                            text: model.rating
                            color: "#D4AF37"
                            font.pixelSize: 11
                        }
                    }
                }
            }

            // Hover and interaction effects
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: parent.border.color = "#D4AF37"
                onExited: parent.border.color = "#3d2545"
                onClicked: {
                    console.log("Book clicked:", model.title)
                }
            }
        }
    }
}
