import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: libraryView
    contentWidth: availableWidth
    clip: true

    required property string username
    required property string userRole
    required property var networkManager
    property int userId: 0
    property var booksById: ({})
    property var allBooksList: []

    property string activeAction: "Add"

    ListModel { id: purchasedModel }
    ListModel { id: favoritesModel }
    ListModel { id: wishlistModel }
    ListModel { id: shelvesModel }
    ListModel { id: selectedShelfBooksModel }
    property string selectedShelfName: ""

    function bookRowFor(bookId) {
        var book = booksById[bookId];
        if (!book) {
            console.warn("bookRowFor: book not found in booksById for ID:", bookId)
            return {
                "bookId": bookId,
                "title": "(book #" + bookId + ")",
                "author": "",
                "image": "qrc:/assets/books/default_cover.png"
            };
        }
        return {
            "bookId": book.id,
            "title": book.title || "Unknown",
            "author": book.author || "Unknown",
            "image": book.coverImageData || "qrc:/assets/books/default_cover.png",
            "price": book.price || 0
        };
    }

    function refreshLibrary() {
        if (!networkManager || userId <= 0) return;
        console.log("LibraryView: refreshLibrary called for userId:", userId)
        networkManager.getPurchasedBooks(userId);
        networkManager.getFavorites(userId);
        networkManager.getWishlist(userId);
        networkManager.getShelfNames(userId);
    }

    Component.onCompleted: {
        console.log("LibraryView: Component.onCompleted - userId:", userId)
        if (networkManager && userId > 0) {
            // اول همه کتاب‌ها رو دریافت کن تا booksById پر بشه
            networkManager.getAllBooks()
        } else {
            console.warn("LibraryView: networkManager or userId invalid")
        }
    }

    Connections {
        target: networkManager
        function onResponseReceived(action, status, data) {
            var ok = status === "success" || status === "SUCCESS"
            console.log("LibraryView: onResponseReceived - action:", action, "status:", status, "ok:", ok)

            if (action === "getAllBooks" && ok) {
                var map = {}
                if (Array.isArray(data)) {
                    for (var i = 0; i < data.length; i++) {
                        var book = data[i]
                        if (book && book.id) {
                            if (!book.coverImageData) {
                                book.coverImageData = "qrc:/assets/books/default_cover.png"
                            }
                            map[book.id] = book
                        }
                    }
                }
                libraryView.booksById = map
                libraryView.allBooksList = Array.isArray(data) ? data : []
                console.log("LibraryView: booksById loaded with", Object.keys(map).length, "books")
                // حالا که booksById پر شده، کتابخانه را رفرش کن
                refreshLibrary()
            }
            else if (action === "getPurchasedBooks" && ok) {
                purchasedModel.clear();
                console.log("LibraryView: getPurchasedBooks data:", data)
                for (var i = 0; i < data.length; i++) {
                    purchasedModel.append(bookRowFor(data[i]));
                }
                console.log("LibraryView: purchasedModel count:", purchasedModel.count)
            }
            else if (action === "getFavorites" && ok) {
                favoritesModel.clear();
                console.log("LibraryView: getFavorites response, data:", JSON.stringify(data))
                if (Array.isArray(data)) {
                    for (var i = 0; i < data.length; i++) {
                        var row = bookRowFor(data[i])
                        console.log("LibraryView: favorite book:", row.title, "ID:", data[i])
                        favoritesModel.append(row)
                    }
                } else {
                    console.warn("LibraryView: getFavorites data is not an array:", data)
                }
                console.log("LibraryView: favoritesModel count:", favoritesModel.count)
            }
            else if (action === "getWishlist" && ok) {
                wishlistModel.clear();
                console.log("LibraryView: getWishlist data:", data)
                for (var i = 0; i < data.length; i++) {
                    wishlistModel.append(bookRowFor(data[i]));
                }
                console.log("LibraryView: wishlistModel count:", wishlistModel.count)
            }
            else if (action === "getShelfNames" && ok) {
                shelvesModel.clear();
                var names = Array.isArray(data) ? data : []
                for (var i = 0; i < names.length; i++) {
                    shelvesModel.append({ "name": names[i] });
                }
                console.log("LibraryView: shelvesModel count:", shelvesModel.count)
            }
            else if (action === "getBooksInShelf" && ok) {
                selectedShelfBooksModel.clear();
                for (var i = 0; i < data.length; i++) {
                    selectedShelfBooksModel.append(bookRowFor(data[i]));
                }
                console.log("LibraryView: selectedShelfBooksModel count:", selectedShelfBooksModel.count)
            }
            else if (action === "addToFavorites" || action === "removeFromFavorites") {
                console.log("LibraryView: add/remove favorites response, status:", status)
                // بعد از اضافه/حذف، دوباره لیست فیوریت‌ها رو دریافت کن
                if (userId > 0) {
                    networkManager.getFavorites(userId);
                }
                if (!ok) console.log(action, "failed:", data.message)
            }
            else if (action === "addToWishlist" || action === "removeFromWishlist") {
                networkManager.getWishlist(userId);
                if (!ok) console.log(action, "failed:", data.message)
            }
            else if (action === "createShelf" || action === "deleteShelf") {
                networkManager.getShelfNames(userId);
                if (!ok) console.log(action, "failed:", data.message)
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
                model: ["Purchased", "Favorites", "Wishlist", "Shelves"]
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

            // 1. Purchased Books (بدون دکمه Favorite)
            ListView {
                model: purchasedModel
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
                            width: 55; height: 80; color: "#1A0F1F"; radius: 4
                            border.color: "#A08EAD"; border.width: 1
                            clip: true
                            Image {
                                anchors.fill: parent;
                                source: model.image;
                                fillMode: Image.PreserveAspectFit
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 5
                            Text {
                                text: model.title;
                                color: "white";
                                font.bold: true;
                                font.pixelSize: 16
                            }
                            Text {
                                text: "Author: " + model.author;
                                color: "#A08EAD";
                                font.pixelSize: 12
                            }
                        }
                        // دکمه ❤ Favorite حذف شد - کاربر فقط از BookPage می‌تونه اضافه کنه
                    }
                }
                Text {
                    anchors.centerIn: parent
                    visible: purchasedModel.count === 0
                    text: "You haven't purchased any books yet."
                    color: "#A08EAD"
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
                                anchors.fill: parent;
                                source: model.image;
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
                        Button {
                            text: "Remove"
                            Layout.alignment: Qt.AlignHCenter
                            contentItem: Text {
                                text: parent.text;
                                color: "white";
                                font.pixelSize: 11
                            }
                            background: Rectangle {
                                color: "#FF5555";
                                radius: 5
                            }
                            onClicked: {
                                if (networkManager) {
                                    networkManager.removeFromFavorites(userId, model.bookId);
                                }
                            }
                        }
                    }
                }
                Text {
                    anchors.centerIn: parent
                    visible: favoritesModel.count === 0
                    text: "No favorite books yet."
                    color: "#A08EAD"
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
                            clip: true
                            Image {
                                anchors.fill: parent;
                                source: model.image;
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
                            contentItem: Text {
                                text: parent.text;
                                color: "white"
                            }
                            background: Rectangle {
                                color: "#FF5555";
                                radius: 5
                            }
                            onClicked: {
                                if (networkManager) {
                                    networkManager.removeFromWishlist(userId, model.bookId);
                                }
                            }
                        }
                    }
                }
                Text {
                    anchors.centerIn: parent
                    visible: wishlistModel.count === 0
                    text: "No books in wishlist."
                    color: "#A08EAD"
                }
            }

            // 4. Shelves Management
            ColumnLayout {
                spacing: 15

                Button {
                    text: "+ Add New Shelf"
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 40
                    contentItem: Text {
                        text: parent.text;
                        color: "#1A0F1F";
                        font.bold: true;
                        horizontalAlignment: Text.AlignHCenter
                    }
                    background: Rectangle {
                        color: "#D4AF37";
                        radius: 5
                    }

                    onClicked: {
                        libraryView.activeAction = "Add"
                        shelfInputField.text = ""
                        shelfDialog.open()
                    }
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 250
                    spacing: 10
                    model: shelvesModel
                    delegate: Rectangle {
                        width: parent.width
                        height: 60
                        color: libraryView.selectedShelfName === model.name ? "#3D2B43" : "#2D1B33"
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
                                text: "View Books"
                                contentItem: Text {
                                    text: parent.text;
                                    color: "white";
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                background: Rectangle {
                                    color: "#5c3d75";
                                    radius: 4
                                }
                                onClicked: {
                                    libraryView.selectedShelfName = model.name
                                    if (networkManager) {
                                        networkManager.getBooksInShelf(userId, model.name)
                                    }
                                }
                            }

                            Button {
                                text: "Delete"
                                contentItem: Text {
                                    text: parent.text;
                                    color: "white";
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                background: Rectangle {
                                    color: "#FF5555";
                                    radius: 4
                                }
                                onClicked: {
                                    if (networkManager) {
                                        networkManager.deleteShelf(userId, model.name);
                                    }
                                }
                            }
                        }
                    }
                }

                Text {
                    visible: libraryView.selectedShelfName !== ""
                    text: "Books in \"" + libraryView.selectedShelfName + "\":"
                    color: "#D4AF37"
                    font.bold: true
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    visible: libraryView.selectedShelfName !== ""
                    model: selectedShelfBooksModel
                    spacing: 6
                    delegate: Rectangle {
                        width: parent.width
                        height: 45
                        color: "#1A0F1F"
                        radius: 5
                        border.color: "#A08EAD"
                        border.width: 0.5

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            Text {
                                text: model.title;
                                color: "white";
                                Layout.fillWidth: true
                            }
                            Button {
                                text: "Remove"
                                contentItem: Text {
                                    text: parent.text;
                                    color: "white";
                                    font.pixelSize: 10
                                }
                                background: Rectangle {
                                    color: "#FF5555";
                                    radius: 4
                                }
                                onClicked: {
                                    if (networkManager) {
                                        networkManager.removeBookFromShelf(
                                            userId,
                                            libraryView.selectedShelfName,
                                            model.bookId
                                        );
                                        networkManager.getBooksInShelf(
                                            userId,
                                            libraryView.selectedShelfName
                                        );
                                    }
                                }
                            }
                        }
                    }
                    Text {
                        anchors.centerIn: parent
                        visible: selectedShelfBooksModel.count === 0
                        text: "This shelf is empty."
                        color: "#A08EAD"
                    }
                }
            }
        }
    }

    Dialog {
        id: shelfDialog
        title: "Create New Shelf"
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
            if (trimmedText !== "" && networkManager) {
                networkManager.createShelf(userId, trimmedText);
            }
        }
    }
}