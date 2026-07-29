import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: cartRoot
    anchors.fill: parent
    color: "#1A0F1F"

    required property var networkManager
    property int userId: 0
    property var booksById: ({})
    property var allBooksList: []

    property int cartItemCount: 0
    property real originalTotal: 0.0
    property real discountAmount: 0.0
    property real finalTotal: 0.0

    property var lastCartIds: []
    property var lastCartBooks: []

    ListModel {
        id: cartModel
    }

    function refreshCart() {
        if (networkManager && userId > 0) {
            console.log("CartView: requesting cart for userId =", userId)
            networkManager.getCart(userId)
        }
    }

    function formatPrice(price) {
        var numericPrice = Number(price)
        if (isNaN(numericPrice))
            numericPrice = 0
        return numericPrice.toFixed(2)
    }

    function normalizeId(value) {
        if (value === undefined || value === null)
            return ""
        return String(value)
    }

    function findBookById(bookId) {
        var targetId = normalizeId(bookId)

        if (targetId === "")
            return null

        if (booksById) {
            if (booksById[targetId])
                return booksById[targetId]
            if (booksById[bookId])
                return booksById[bookId]

            for (var key in booksById) {
                var item = booksById[key]
                if (!item)
                    continue

                if (normalizeId(key) === targetId)
                    return item

                if (normalizeId(item.bookId) === targetId)
                    return item

                if (normalizeId(item.id) === targetId)
                    return item
            }
        }

        if (allBooksList && allBooksList.length !== undefined) {
            for (var i = 0; i < allBooksList.length; i++) {
                var book = allBooksList[i]
                if (!book)
                    continue

                if (normalizeId(book.bookId) === targetId)
                    return book

                if (normalizeId(book.id) === targetId)
                    return book
            }
        }

        return null
    }

    function resolveBookId(book, fallbackId) {
        if (!book)
            return fallbackId

        return book.bookId !== undefined ? book.bookId
             : book.id !== undefined ? book.id
             : fallbackId
    }

    function resolveBookTitle(book, fallbackId) {
        if (!book || (!book.title && !book.name && !book.bookTitle))
            return "Book #" + fallbackId

        return book.title
            || book.name
            || book.bookTitle
            || ("Book #" + fallbackId)
    }

    function resolveBookAuthor(book) {
        if (!book)
            return ""

        return book.author
            || book.writer
            || book.authorName
            || ""
    }

    function resolveBookPrice(book) {
        if (!book)
            return 0

        var price = book.price !== undefined ? Number(book.price) : 0
        return isNaN(price) ? 0 : price
    }

    function resolveBookDiscount(book) {
        if (!book)
            return 0

        var discount = book.discountValue !== undefined ? Number(book.discountValue) : 0
        return isNaN(discount) ? 0 : discount
    }

    function resolveCoverImage(book) {
        if (!book)
            return ""

        var raw = book.image
               || book.coverImageData
               || book.coverImage
               || book.imagePath
               || book.coverUrl
               || book.imageUrl
               || ""

        if (!raw || raw === "")
            return ""

        if (raw.indexOf("data:image") === 0)
            return raw

        if (raw.indexOf("qrc:/") === 0
                || raw.indexOf("file:/") === 0
                || raw.indexOf("http://") === 0
                || raw.indexOf("https://") === 0) {
            return raw
        }

        if (raw.length > 100)
            return "data:image/png;base64," + raw

        return raw
    }

    function appendBookToModel(book, fallbackId) {
        var resolvedId = resolveBookId(book, fallbackId)

        cartModel.append({
            "bookId": resolvedId,
            "title": resolveBookTitle(book, resolvedId),
            "author": resolveBookAuthor(book),
            "price": resolveBookPrice(book),
            "discountValue": resolveBookDiscount(book),
            "image": resolveCoverImage(book)
        })
    }

    function rebuildCartModelFromBooks(books) {
        cartModel.clear()

        if (!books || books.length === undefined) {
            cartItemCount = 0
            return
        }

        cartItemCount = books.length

        for (var i = 0; i < books.length; i++) {
            appendBookToModel(books[i], i + 1)
        }
    }

    function rebuildCartModelFromIds(ids) {
        cartModel.clear()

        if (!ids || ids.length === undefined) {
            cartItemCount = 0
            return
        }

        cartItemCount = ids.length

        for (var i = 0; i < ids.length; i++) {
            var idValue = ids[i]
            var book = findBookById(idValue)

            if (!book) {
                book = { "bookId": idValue }
                console.log("CartView: book not found for id =", idValue, ". Using placeholder.")
            } else {
                console.log("CartView: matched book for id =", idValue,
                            "title =", resolveBookTitle(book, idValue))
            }

            appendBookToModel(book, idValue)
        }
    }

    function rebuildCartIfPossible() {
        if (lastCartBooks && lastCartBooks.length > 0) {
            console.log("CartView: rebuilding cart model from cached books")
            rebuildCartModelFromBooks(lastCartBooks)
            return
        }

        if (lastCartIds && lastCartIds.length > 0) {
            console.log("CartView: rebuilding cart model from cached ids")
            rebuildCartModelFromIds(lastCartIds)
        }
    }

    onBooksByIdChanged: {
        console.log("CartView: booksById changed")
        rebuildCartIfPossible()
    }

    onAllBooksListChanged: {
        console.log("CartView: allBooksList changed, count =",
                    allBooksList && allBooksList.length !== undefined ? allBooksList.length : 0)
        rebuildCartIfPossible()
    }

    Component.onCompleted: refreshCart()

    Connections {
        target: networkManager

        function onResponseReceived(action, status, data) {
            var ok = status === "success" || status === "SUCCESS"

            if (action === "getCart") {
                console.log("CartView getCart status =", status)
                console.log("CartView getCart data =", JSON.stringify(data))

                if (!ok) {
                    console.log("CartView: getCart failed")
                    checkoutMessage.text = data && data.message ? data.message : "Could not load cart."
                    checkoutMessage.color = "#FF5555"
                    checkoutMessage.visible = true
                    return
                }

                checkoutMessage.visible = false

                originalTotal = Number(data.rawTotalPrice || 0.0)
                discountAmount = Number(data.totalDiscountAmount || 0.0)
                finalTotal = Number(data.finalPriceToPay || 0.0)

                var books = data.books || []
                var ids = data.bookIds || []

                lastCartBooks = books
                lastCartIds = ids

                console.log("CartView: received books =", books.length)
                console.log("CartView: received ids =", JSON.stringify(ids))

                if (books.length > 0) {
                    rebuildCartModelFromBooks(books)
                } else {
                    rebuildCartModelFromIds(ids)
                }
            } else if (action === "removeFromCart") {
                if (ok) {
                    refreshCart()
                } else {
                    console.log("removeFromCart failed:",
                                data && data.message ? data.message : "")
                }
            } else if (action === "checkout") {
                if (ok) {
                    cartModel.clear()
                    cartItemCount = 0
                    lastCartBooks = []
                    lastCartIds = []
                    originalTotal = 0.0
                    discountAmount = 0.0
                    finalTotal = 0.0

                    checkoutMessage.text = "Books moved to Purchased Library successfully."
                    checkoutMessage.color = "#FFD700"
                    checkoutMessage.visible = true

                    refreshCart()
                } else {
                    checkoutMessage.text = data && data.message
                        ? data.message
                        : "Could not complete checkout."
                    checkoutMessage.color = "#FF5555"
                    checkoutMessage.visible = true
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        RowLayout {
            Layout.fillWidth: true

            Text {
                text: "My Cart"
                color: "#FFD700"
                font.bold: true
                font.pixelSize: 22
                Layout.fillWidth: true
            }

            Rectangle {
                width: 110
                height: 30
                color: "#2D1B33"
                border.color: "#FFD700"
                border.width: 1
                radius: 15

                Text {
                    anchors.centerIn: parent
                    text: cartModel.count + " books"
                    color: "white"
                    font.bold: true
                    font.pixelSize: 12
                }
            }
        }

        Text {
            id: checkoutMessage
            visible: false
            font.pixelSize: 13
            font.bold: true
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
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
                height: 100
                color: "#2D1B33"
                radius: 8
                border.color: "#FFD700"
                border.width: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 15

                    Rectangle {
                        Layout.preferredWidth: 60
                        Layout.preferredHeight: 80
                        color: "#1A0F1F"
                        radius: 6
                        border.color: "#FFD700"
                        border.width: 1
                        clip: true

                        Image {
                            id: coverImage
                            anchors.fill: parent
                            anchors.margins: 3
                            source: model.image
                            fillMode: Image.PreserveAspectFit
                            asynchronous: true
                            cache: false
                            visible: source !== ""

                            onStatusChanged: {
                                if (status === Image.Error) {
                                    console.log("CartView: image load error for",
                                                model.bookId, "source =", source)
                                    source = ""
                                }
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            visible: coverImage.source === ""
                                     || coverImage.status === Image.Error
                                     || coverImage.status === Image.Null
                            text: "No Cover"
                            color: "#A08EAD"
                            font.pixelSize: 10
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4

                        Text {
                            text: model.title
                            color: "white"
                            font.bold: true
                            font.pixelSize: 15
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            maximumLineCount: 1
                        }

                        Text {
                            text: model.author !== "" ? ("by " + model.author) : ""
                            color: "#A08EAD"
                            font.pixelSize: 12
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            visible: text !== ""
                        }

                        Text {
                            text: "Price: " + formatPrice(model.price)
                            color: "#FFD700"
                            font.pixelSize: 12
                            Layout.fillWidth: true
                        }

                        Text {
                            visible: Number(model.discountValue) > 0
                            text: "Discount: " + formatPrice(model.discountValue)
                            color: "#A08EAD"
                            font.pixelSize: 12
                            Layout.fillWidth: true
                        }

                        Text {
                            text: "Ready to add to Purchased Library"
                            color: "#FFD700"
                            font.pixelSize: 12
                            Layout.fillWidth: true
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
                            if (networkManager && userId > 0)
                                networkManager.removeFromCart(userId, model.bookId)
                        }
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: cartModel.count === 0
                text: "Your cart is empty."
                color: "#A08EAD"
                font.pixelSize: 14
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 130
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

                    Text {
                        text: "Selected Books:"
                        color: "#A08EAD"
                        font.pixelSize: 13
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Text {
                        text: cartModel.count.toString()
                        color: "white"
                        font.pixelSize: 13
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "Original Total:"
                        color: "#A08EAD"
                        font.pixelSize: 13
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Text {
                        text: formatPrice(originalTotal)
                        color: "white"
                        font.pixelSize: 13
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "Discount:"
                        color: "#A08EAD"
                        font.pixelSize: 13
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Text {
                        text: formatPrice(discountAmount)
                        color: "white"
                        font.pixelSize: 13
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#A08EAD"
                }

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: "Final Total:"
                        color: "#FFD700"
                        font.bold: true
                        font.pixelSize: 15
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Text {
                        text: formatPrice(finalTotal)
                        color: "#FFD700"
                        font.bold: true
                        font.pixelSize: 15
                    }
                }
            }
        }

        Button {
            id: checkoutButton
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            enabled: cartModel.count > 0

            contentItem: Text {
                text: "CONFIRM & ADD TO PURCHASED LIBRARY"
                color: "#1A0F1F"
                font.bold: true
                font.pixelSize: 15
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
                if (networkManager && userId > 0) {
                    console.log("CartView checkout userId =", userId)
                    networkManager.checkout(userId)
                }
            }
        }
    }
}

