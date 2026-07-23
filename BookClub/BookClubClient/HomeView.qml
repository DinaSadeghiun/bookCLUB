import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: homeRoot

    // Properties received from Dashboard
    required property string username
    required property string userRole
    required property var networkManager
    property int userId: 0
    property var userGenres: []
    property int cartItemCount: 0

    // Dynamic models for real data from server (No Mock Data)
    ListModel { id: recommendedModel }
    ListModel { id: trendingModel }
    ListModel { id: popularModel }
    ListModel { id: freeBooksModel }

    Component.onCompleted: {
        // Request books data from server upon loading
        if (networkManager) {
            networkManager.requestAllBooks();
        }
    }

    // Connections to handle server responses
    Connections {
        target: networkManager
        ignoreUnknownSignals: true

        function onBooksReceived(booksList) {
            // Clear previous models
            recommendedModel.clear();
            trendingModel.clear();
            popularModel.clear();
            freeBooksModel.clear();

            // Populate models with real data received from server
            for (var i = 0; i < booksList.length; i++) {
                var book = booksList[i];
                var bookData = {
                    "title": book.title,
                    "author": book.author,
                    "genre": book.genre,
                    "price": book.price === 0 ? "Free" : ("$" + book.price),
                    "rating": book.rating ? book.rating.toString() : "4.0",
                    "image": book.image || "qrc:/assets/books/default_cover.png"
                };

                // Append to general sections
                trendingModel.append(bookData);
                popularModel.append(bookData);

                if (book.price === 0) {
                    freeBooksModel.append(bookData);
                }

                // Match with user favorite genres for Recommended section
                for (var j = 0; j < userGenres.length; j++) {
                    if (book.genre === userGenres[j]) {
                        recommendedModel.append(bookData);
                        break;
                    }
                }
            }
        }
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

        Pane {
            width: homeScroll.availableWidth
            topPadding: 15
            bottomPadding: 30
            leftPadding: 20
            rightPadding: 20
            background: null

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

                // 2. Recommended Books Section
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    visible: recommendedModel.count > 0

                    Text {
                        text: "Recommended For You"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    ListView {
                        Layout.fillWidth: true
                        implicitHeight: 240
                        orientation: ListView.Horizontal
                        spacing: 15
                        model: recommendedModel
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
                        model: trendingModel
                        clip: true
                        delegate: bookDelegate
                    }
                }

                // 5. Free Books Section
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    visible: freeBooksModel.count > 0

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
                        model: freeBooksModel
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

                Text {
                    text: model.title
                    color: "#D4AF37"
                    font.pixelSize: 13
                    font.bold: true
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: model.author
                    color: "#D4AF37"
                    opacity: 0.7
                    font.pixelSize: 11
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

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
