import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: libraryView
    contentWidth: availableWidth
    clip: true

    // Required properties received from Dashboard
    required property string username
    required property string userRole
    required property var networkManager
    property int userId: 0

    // Internal state properties to handle Add/Edit action and the target index
    property string activeAction: "Add"
    property int selectedShelfIndex: -1

    // Dynamic models for real data from server (No Mock Data)
    ListModel { id: allBooksModel }
    ListModel { id: favoritesModel }
    ListModel { id: wishlistModel }
    ListModel { id: shelvesModel }

    Component.onCompleted: {
        // Request user library data from server upon loading
        if (networkManager && userId > 0) {
            networkManager.requestUserLibrary(userId);
            networkManager.requestUserShelves(userId);
        }
    }

    // Connections to handle server responses
    Connections {
        target: networkManager
        ignoreUnknownSignals: true

        function onUserLibraryReceived(libraryData) {
            allBooksModel.clear();
            favoritesModel.clear();
            wishlistModel.clear();

            for (var i = 0; i < libraryData.length; i++) {
                var book = libraryData[i];
                var bookData = {
                    "bookId": book.id,
                    "title": book.title,
                    "author": book.author,
                    "year": book.year || "N/A",
                    "image": book.image || "qrc:/assets/books/default_cover.png"
                };

                allBooksModel.append(bookData);

                if (book.isFavorite) {
                    favoritesModel.append(bookData);
                }
                if (book.isWishlist) {
                    wishlistModel.append(bookData);
                }
            }
        }

        function onUserShelvesReceived(shelvesData) {
            shelvesModel.clear();
            for (var i = 0; i < shelvesData.length; i++) {
                shelvesModel.append({
                    "shelfId": shelvesData[i].id,
                    "name": shelvesData[i].name
                });
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
        z: -1
    }

    ColumnLayout {
        width: parent.width - 40
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 25

        Text {
            text: "Personal Library"
            color: "#D4AF37"
            font.pixelSize: 32
            font.bold: true
        }

        TabBar {
            id: libraryTabBar
            Layout.fillWidth: true
            background: Rectangle { color: "transparent" }
            Repeater {
                model: ["All Books", "Favorites", "Wishlist", "Shelves"]
                TabButton {
                    text: modelData
                    contentItem: Text {
                        text: modelData
                        color: parent.checked ? "#D4AF37" : "#A08EAD"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: parent.checked ? "#2D1B33" : "transparent"
                        border.color: parent.checked ? "#D4AF37" : "transparent"
                        border.width: 1
                        radius: 5
                    }
                }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 600
            currentIndex: libraryTabBar.currentIndex

            // 1. All Books List
            ListView {
                model: allBooksModel
                spacing: 10
                delegate: Rectangle {
                    width: parent.width
                    height: 100
                    color: "#2D1B33"
                    radius: 10
                    border.color: "#D4AF37"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 15

                        Rectangle {
                            width: 55
                            height: 80
                            color: "#1A0F1F"
                            radius: 4
                            border.color: "#A08EAD"
                            border.width: 1

                            Image {
                                anchors.fill: parent
                                source: model.image
                                fillMode: Image.PreserveAspectFit
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 5
                            Text { text: model.title; color: "white"; font.bold: true; font.pixelSize: 16 }
                            Text { text: "Author: " + model.author + " | Year: " + model.year; color: "#A08EAD"; font.pixelSize: 12 }
                        }

                        Button {
                            text: "Details"
                            font.pixelSize: 12
                            contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter }
                            background: Rectangle { color: "#D4AF37"; radius: 5 }
                            onClicked: {
                                console.log("View details for book:", model.bookId)
                            }
                        }
                    }
                }
            }

            // 2. Favorites Grid
            GridView {
                cellWidth: 160
                cellHeight: 230
                model: favoritesModel
                clip: true
                delegate: Item {
                    width: 150
                    height: 220
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 5
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "#2D1B33"
                            border.color: "#FF5555"
                            border.width: 2
                            radius: 8
                            clip: true

                            Image {
                                anchors.fill: parent
                                source: model.image
                                fillMode: Image.PreserveAspectFit
                            }
                        }
                        Text {
                            text: model.title
                            color: "white"
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 14
                            elide: Text.ElideRight
                        }
                    }
                }
            }

            // 3. Wishlist List
            ListView {
                model: wishlistModel
                spacing: 10
                delegate: Rectangle {
                    width: parent.width
                    height: 80
                    color: "#2D1B33"
                    radius: 8
                    border.color: "#A08EAD"
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 15

                        Rectangle {
                            width: 40; height: 60; color: "#1A0F1F"; radius: 4
                            Image {
                                anchors.fill: parent
                                source: model.image
                                fillMode: Image.PreserveAspectFit
                            }
                        }

                        Text {
                            text: model.title
                            color: "white"
                            Layout.fillWidth: true
                            font.bold: true
                        }

                        Button {
                            text: "Remove"
                            contentItem: Text { text: parent.text; color: "white" }
                            background: Rectangle { color: "#FF5555"; radius: 5 }
                            onClicked: {
                                if (networkManager) {
                                    networkManager.removeFromWishlist(userId, model.bookId);
                                    wishlistModel.remove(index);
                                }
                            }
                        }
                    }
                }
            }

            // 4. Shelves Management
            ColumnLayout {
                spacing: 15

                Button {
                    text: "+ Add New Shelf"
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 40
                    contentItem: Text { text: parent.text; color: "#1A0F1F"; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                    background: Rectangle { color: "#D4AF37"; radius: 5 }

                    onClicked: {
                        libraryView.activeAction = "Add"
                        shelfInputField.text = ""
                        shelfDialog.open()
                    }
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 10
                    model: shelvesModel
                    delegate: Rectangle {
                        width: parent.width
                        height: 60
                        color: "#2D1B33"
                        radius: 5
                        border.color: "#D4AF37"
                        border.width: 1

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 10

                            Text {
                                text: model.name
                                color: "white"
                                font.bold: true
                                Layout.fillWidth: true
                            }

                            Button {
                                text: "Edit Name"
                                contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter }
                                background: Rectangle { color: "#5c3d75"; radius: 4 }
                                onClicked: {
                                    libraryView.activeAction = "Edit"
                                    libraryView.selectedShelfIndex = index
                                    shelfInputField.text = model.name
                                    shelfDialog.open()
                                }
                            }

                            Button {
                                text: "Delete"
                                contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter }
                                background: Rectangle { color: "#FF5555"; radius: 4 }
                                onClicked: {
                                    if (networkManager) {
                                        networkManager.deleteShelf(userId, model.shelfId);
                                        shelvesModel.remove(index);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Modal popup dialog for handling Shelf input operations safely
    Dialog {
        id: shelfDialog
        title: libraryView.activeAction === "Add" ? "Create New Shelf" : "Edit Shelf Name"
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {
            color: "#2D1B33"
            border.color: "#D4AF37"
            border.width: 2
            radius: 8
        }

        header: Rectangle {
            color: "#1A0F1F"
            implicitHeight: 40
            radius: 8
            Text {
                text: shelfDialog.title
                color: "#D4AF37"
                anchors.centerIn: parent
                font.bold: true
            }
        }

        ColumnLayout {
            spacing: 10
            width: 260

            Text {
                text: "Shelf Name:"
                color: "#A08EAD"
                font.pixelSize: 13
            }

            TextField {
                id: shelfInputField
                Layout.fillWidth: true
                color: "white"
                placeholderText: "Enter shelf name..."
                placeholderTextColor: "#807090"
                background: Rectangle {
                    color: "#1A0F1F"
                    border.color: parent.activeFocus ? "#D4AF37" : "#5c3d75"
                    border.width: 1
                    radius: 4
                }
            }
        }

        onAccepted: {
            var trimmedText = shelfInputField.text.trim()
            if (trimmedText !== "") {
                if (libraryView.activeAction === "Add") {
                    if (networkManager) {
                        networkManager.createShelf(userId, trimmedText);
                    }
                } else if (libraryView.activeAction === "Edit" && libraryView.selectedShelfIndex !== -1) {
                    var shelfId = shelvesModel.get(libraryView.selectedShelfIndex).shelfId;
                    if (networkManager) {
                        networkManager.updateShelf(userId, shelfId, trimmedText);
                    }
                    shelvesModel.setProperty(libraryView.selectedShelfIndex, "name", trimmedText)
                }
            }
        }
    }
}
