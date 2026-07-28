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
    property string selectedShelfName: ""
    property int bookToMoveId: 0
    property string moveFromShelfName: ""
    property string moveToShelfName: ""

    signal bookSelected(int bookId)

    ListModel { id: purchasedModel }
    ListModel { id: favoritesModel }
    ListModel { id: wishlistModel }
    ListModel { id: shelvesModel }
    ListModel { id: selectedShelfBooksModel }
    ListModel { id: shelfMoveTargetModel }

    function ensureList(data) {
        if (!data)
            return []

        if (Array.isArray(data))
            return data

        if (data.length !== undefined)
            return data

        console.warn("LibraryView: expected list-like data but got:", JSON.stringify(data))
        return []
    }

    function extractBookId(book) {
        if (!book)
            return 0

        var idValue = book.id
        if (idValue === undefined || idValue === null)
            idValue = book.bookId

        var numericId = Number(idValue)
        return isNaN(numericId) ? 0 : numericId
    }

    function normalizeBookRef(item) {
        if (item === undefined || item === null)
            return 0

        if (typeof item === "number")
            return item

        if (typeof item === "string") {
            var parsed = Number(item)
            return isNaN(parsed) ? 0 : parsed
        }

        if (typeof item === "object")
            return extractBookId(item)

        return 0
    }

    function resolveImage(book) {
        if (!book)
            return "qrc:/assets/books/default_cover.png"

        var raw = book.coverImageData
                || book.coverImage
                || book.image
                || book.imagePath
                || book.coverUrl
                || ""

        if (!raw || raw === "")
            return "qrc:/assets/books/default_cover.png"

        if (typeof raw === "string") {
            if (raw.startsWith("qrc:/")
                    || raw.startsWith("file:/")
                    || raw.startsWith("http://")
                    || raw.startsWith("https://")
                    || raw.startsWith("data:image/")) {
                return raw
            }

            return "data:image/png;base64," + raw
        }

        return "qrc:/assets/books/default_cover.png"
    }

    function bookRowFor(bookRef) {
        var normalizedBookId = normalizeBookRef(bookRef)

        if (normalizedBookId <= 0) {
            console.warn("bookRowFor: invalid book reference:", JSON.stringify(bookRef))
            return null
        }

        var book = booksById[normalizedBookId]

        if (!book) {
            console.warn("bookRowFor: book not found in booksById for ID:", normalizedBookId)
            return null
        }

        return {
            "bookId": extractBookId(book),
            "title": book.title || "Unknown",
            "author": book.author || "Unknown",
            "image": resolveImage(book),
            "price": Number(book.price || 0)
        }
    }

    function populateModelFromIds(modelRef, rawList, label) {
        modelRef.clear()

        var list = ensureList(rawList)
        for (var i = 0; i < list.length; i++) {
            var row = bookRowFor(list[i])
            if (row) {
                modelRef.append(row)
            } else {
                console.warn("LibraryView:", label, "missing resolved book for item:", JSON.stringify(list[i]))
            }
        }

        console.log("LibraryView:", label, "count:", modelRef.count)
    }

    function refreshLibrary() {
        if (!networkManager || userId <= 0) {
            console.warn("LibraryView: refreshLibrary skipped - invalid networkManager or userId")
            return
        }

        console.log("LibraryView: refreshLibrary called for userId:", userId)
        networkManager.getPurchasedBooks(userId)
        networkManager.getFavorites(userId)
        networkManager.getWishlist(userId)
        networkManager.getShelfNames(userId)
    }

    function openBook(bookId) {
        if (bookId > 0)
            libraryView.bookSelected(bookId)
    }

    // Opens the Move To shelf selection dialog and refreshes the shelf names
    function openMoveDialog(bookId, fromShelfName) {
        bookToMoveId = bookId
        moveFromShelfName = fromShelfName
        moveToShelfName = ""

        if (networkManager && userId > 0) {
            networkManager.getShelfNames(userId)
        }

        moveTargetDialog.open()
    }

    function refreshShelfBooks() {
        if (networkManager && userId > 0 && selectedShelfName !== "")
            networkManager.getBooksInShelf(userId, selectedShelfName)
    }

    Component.onCompleted: {
        console.log("LibraryView: Component.onCompleted - userId:", userId)

        if (networkManager && userId > 0) {
            networkManager.getAllBooks()
        } else {
            console.warn("LibraryView: networkManager or userId invalid")
        }
    }

    Connections {
        target: networkManager
        ignoreUnknownSignals: true

        function onResponseReceived(action, status, data) {
            var ok = status === "success" || status === "SUCCESS"
            console.log("LibraryView: onResponseReceived - action:", action, "status:", status, "ok:", ok)

            if (action === "getAllBooks" && ok) {
                var map = {}
                var books = ensureList(data)

                console.log("LibraryView: getAllBooks count:", books.length)

                for (var i = 0; i < books.length; i++) {
                    var book = books[i]
                    var id = extractBookId(book)

                    if (book && id > 0)
                        map[id] = book
                    else
                        console.warn("LibraryView: skipped book with invalid id at index", i, JSON.stringify(book))
                }

                libraryView.booksById = map
                libraryView.allBooksList = books
                refreshLibrary()
            }
            else if (action === "getPurchasedBooks" && ok) {
                populateModelFromIds(purchasedModel, data, "purchasedModel")
            }
            else if (action === "getFavorites" && ok) {
                populateModelFromIds(favoritesModel, data, "favoritesModel")
            }
            else if (action === "getWishlist" && ok) {
                populateModelFromIds(wishlistModel, data, "wishlistModel")
            }
            else if (action === "getShelfNames" && ok) {
                shelvesModel.clear()
                var names = ensureList(data)

                for (var s = 0; s < names.length; s++) {
                    shelvesModel.append({ "name": names[s] })
                }

                console.log("LibraryView: shelvesModel count:", shelvesModel.count)
            }
            else if (action === "getBooksInShelf" && ok) {
                populateModelFromIds(selectedShelfBooksModel, data, "selectedShelfBooksModel")
            }
            else if ((action === "addBookToShelf" || action === "removeBookFromShelf" || action === "moveBookBetweenShelves") && ok) {
                refreshShelfBooks()
                if (userId > 0)
                    networkManager.getPurchasedBooks(userId)
            }
            else if ((action === "addToFavorites" || action === "removeFromFavorites") && ok) {
                if (userId > 0)
                    networkManager.getFavorites(userId)
            }
            else if ((action === "addToWishlist" || action === "removeFromWishlist") && ok) {
                if (userId > 0)
                    networkManager.getWishlist(userId)
            }
            else if ((action === "createShelf" || action === "deleteShelf") && ok) {
                if (userId > 0)
                    networkManager.getShelfNames(userId)
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

            // 1. Purchased
            ListView {
                model: purchasedModel
                spacing: 10
                clip: true

                delegate: Rectangle {
                    width: ListView.view ? ListView.view.width : 0
                    height: 100
                    color: "#2D1B33"
                    radius: 10
                    border.color: "#D4AF37"
                    border.width: 1

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: libraryView.openBook(model.bookId)
                    }

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
                            clip: true

                            Image {
                                anchors.fill: parent
                                source: model.image
                                fillMode: Image.PreserveAspectFit
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 5

                            Text {
                                text: model.title
                                color: "white"
                                font.bold: true
                                font.pixelSize: 16
                                elide: Text.ElideRight
                            }

                            Text {
                                text: "Author: " + model.author
                                color: "#A08EAD"
                                font.pixelSize: 12
                                elide: Text.ElideRight
                            }
                        }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    visible: purchasedModel.count === 0
                    text: "You haven't purchased any books yet."
                    color: "#A08EAD"
                }
            }

            // 2. Favorites
            GridView {
                cellWidth: 160
                cellHeight: 260
                model: favoritesModel
                clip: true

                delegate: Item {
                    width: 150
                    height: 250

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 5

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 180
                            color: "#2D1B33"
                            border.color: "#FF5555"
                            border.width: 2
                            radius: 8
                            clip: true

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: libraryView.openBook(model.bookId)
                            }

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

                        Button {
                            text: "Remove"
                            Layout.alignment: Qt.AlignHCenter

                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.pixelSize: 11
                            }

                            background: Rectangle {
                                color: "#FF5555"
                                radius: 5
                            }

                            onClicked: {
                                if (networkManager && model.bookId > 0)
                                    networkManager.removeFromFavorites(userId, model.bookId)
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

            // 3. Wishlist
            ListView {
                model: wishlistModel
                spacing: 10
                clip: true

                delegate: Rectangle {
                    width: ListView.view ? ListView.view.width : 0
                    height: 80
                    color: "#2D1B33"
                    radius: 8
                    border.color: "#A08EAD"
                    border.width: 1

                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: libraryView.openBook(model.bookId)
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 15

                        Rectangle {
                            width: 40
                            height: 60
                            color: "#1A0F1F"
                            radius: 4
                            clip: true

                            Image {
                                anchors.fill: parent
                                source: model.image
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
                                elide: Text.ElideRight
                            }

                            Text {
                                text: "Author: " + model.author
                                color: "#A08EAD"
                                font.pixelSize: 11
                                elide: Text.ElideRight
                            }
                        }

                        Button {
                            text: "Remove"

                            contentItem: Text {
                                text: parent.text
                                color: "white"
                            }

                            background: Rectangle {
                                color: "#FF5555"
                                radius: 5
                            }

                            onClicked: {
                                if (networkManager && model.bookId > 0)
                                    networkManager.removeFromWishlist(userId, model.bookId)
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

            // 4. Shelves
            ColumnLayout {
                spacing: 15

                Button {
                    text: "+ Add New Shelf"
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 40

                    contentItem: Text {
                        text: parent.text
                        color: "#1A0F1F"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: "#D4AF37"
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
                    clip: true

                    delegate: Rectangle {
                        width: ListView.view ? ListView.view.width : 0
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
                                    text: parent.text
                                    color: "white"
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                background: Rectangle {
                                    color: "#5c3d75"
                                    radius: 4
                                }

                                onClicked: {
                                    libraryView.selectedShelfName = model.name
                                    if (networkManager)
                                        networkManager.getBooksInShelf(userId, model.name)
                                }
                            }

                            Button {
                                text: "Delete"

                                contentItem: Text {
                                    text: parent.text
                                    color: "white"
                                    horizontalAlignment: Text.AlignHCenter
                                }

                                background: Rectangle {
                                    color: "#FF5555"
                                    radius: 4
                                }

                                onClicked: {
                                    if (networkManager)
                                        networkManager.deleteShelf(userId, model.name)
                                }
                            }
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    visible: libraryView.selectedShelfName !== ""
                    spacing: 10

                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: "Books in \"" + libraryView.selectedShelfName + "\":"
                            color: "#D4AF37"
                            font.bold: true
                            Layout.fillWidth: true
                        }

                        Button {
                            text: "Add Book to Shelf"

                            contentItem: Text {
                                text: parent.text
                                color: "#1A0F1F"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }

                            background: Rectangle {
                                color: "#D4AF37"
                                radius: 5
                            }

                            onClicked: {
                                if (purchasedModel.count === 0) {
                                    toastText.text = "You have no purchased books to add."
                                    toastMessage.visible = true
                                    toastTimer.start()
                                    return
                                }
                                addBookDialog.open()
                            }
                        }
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 200
                        model: selectedShelfBooksModel
                        spacing: 6
                        clip: true

                        delegate: Rectangle {
                            width: ListView.view ? ListView.view.width : 0
                            height: 60
                            color: "#1A0F1F"
                            radius: 5
                            border.color: "#A08EAD"
                            border.width: 0.5

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: libraryView.openBook(model.bookId)
                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 10

                                Text {
                                    text: model.title
                                    color: "white"
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }

                                Button {
                                    text: "Move To"

                                    contentItem: Text {
                                        text: parent.text
                                        color: "white"
                                        font.pixelSize: 10
                                        horizontalAlignment: Text.AlignHCenter
                                    }

                                    background: Rectangle {
                                        color: "#7A4FD0"
                                        radius: 4
                                    }

                                    onClicked: {
                                        libraryView.openMoveDialog(model.bookId, libraryView.selectedShelfName)
                                    }
                                }

                                Button {
                                    text: "Remove"

                                    contentItem: Text {
                                        text: parent.text
                                        color: "white"
                                        font.pixelSize: 10
                                        horizontalAlignment: Text.AlignHCenter
                                    }

                                    background: Rectangle {
                                        color: "#FF5555"
                                        radius: 4
                                    }

                                    onClicked: {
                                        if (networkManager && model.bookId > 0) {
                                            networkManager.removeBookFromShelf(
                                                userId,
                                                libraryView.selectedShelfName,
                                                model.bookId
                                            )
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
                networkManager.createShelf(userId, trimmedText)
            }
        }
    }

    Dialog {
        id: addBookDialog
        title: "Add Book to \"" + (libraryView.selectedShelfName || "Shelf") + "\""
        anchors.centerIn: parent
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        width: Math.min(parent.width * 0.9, 500)
        height: Math.min(parent.height * 0.8, 500)

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
                text: addBookDialog.title
                color: "#D4AF37"
                anchors.centerIn: parent
                font.bold: true
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 10

            Text {
                text: "Select a book from your purchased books to add to this shelf:"
                color: "#D4AF37"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: purchasedModel
                spacing: 8
                clip: true

                delegate: Rectangle {
                    width: ListView.view ? ListView.view.width : 0
                    height: 70
                    color: "#1A0F1F"
                    radius: 5
                    border.color: "#A08EAD"
                    border.width: 0.5

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 10

                        Rectangle {
                            width: 40
                            height: 50
                            color: "#2D1B33"
                            radius: 4
                            clip: true

                            Image {
                                anchors.fill: parent
                                source: model.image
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
                            }

                            Text {
                                text: "by " + model.author
                                color: "#A08EAD"
                                font.pixelSize: 11
                            }
                        }

                        Button {
                            text: "Add"
                            Layout.preferredWidth: 60

                            contentItem: Text {
                                text: parent.text
                                color: "#1A0F1F"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }

                            background: Rectangle {
                                color: "#D4AF37"
                                radius: 5
                            }

                            onClicked: {
                                if (networkManager && libraryView.selectedShelfName !== "" && model.bookId > 0) {
                                    networkManager.addBookToShelf(
                                        userId,
                                        libraryView.selectedShelfName,
                                        model.bookId
                                    )
                                    addBookDialog.close()
                                }
                            }
                        }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    visible: purchasedModel.count === 0
                    text: "You have no purchased books to add."
                    color: "#A08EAD"
                    font.pixelSize: 14
                }
            }
        }
    }

    Dialog {
        id: moveTargetDialog
        title: "Move Book To"
        anchors.centerIn: parent
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        width: Math.min(parent.width * 0.85, 420)
        height: Math.min(parent.height * 0.7, 420)

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
                text: "Move Book To"
                color: "#D4AF37"
                anchors.centerIn: parent
                font.bold: true
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 10

            Text {
                text: "Select destination shelf:"
                color: "#A08EAD"
                Layout.fillWidth: true
            }

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: shelvesModel
                clip: true
                spacing: 8

                delegate: Button {
                    width: ListView.view ? ListView.view.width : 0
                    // Displays "(current)" label next to the active shelf name
                    text: model.name + (model.name === libraryView.moveFromShelfName ? " (current)" : "")

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.bold: model.name === libraryView.moveFromShelfName
                    }

                    background: Rectangle {
                        color: model.name === libraryView.moveFromShelfName ? "#3A2A40" : "#5c3d75"
                        radius: 5
                        border.color: model.name === libraryView.moveFromShelfName ? "#D4AF37" : "#8B6BA8"
                        border.width: 1
                    }

                    // Keep active so clicking it can cleanly close the dialog (No-op)
                    enabled: true

                    onClicked: {
                        if (!networkManager || bookToMoveId <= 0)
                            return

                        // If user selects the current shelf, just close the dialog (No-op)
                        if (model.name === libraryView.moveFromShelfName) {
                            moveTargetDialog.close()
                            return
                        }

                        moveToShelfName = model.name
                        networkManager.moveBookBetweenShelves(
                            userId,
                            bookToMoveId,
                            moveFromShelfName,
                            moveToShelfName
                        )
                        moveTargetDialog.close()
                    }
                }
            }
        }
    }

    Rectangle {
        id: toastMessage
        visible: false
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        width: 300
        height: 50
        radius: 10
        color: "#2D1B33"
        border.color: "#D4AF37"
        border.width: 1

        Text {
            id: toastText
            anchors.centerIn: parent
            text: ""
            color: "white"
            font.pixelSize: 13
        }
    }

    Timer {
        id: toastTimer
        interval: 2500
        onTriggered: toastMessage.visible = false
    }
}
