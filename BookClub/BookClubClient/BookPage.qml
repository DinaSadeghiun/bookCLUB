
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ScrollView {
    id: detailsRoot
    anchors.fill: parent
    clip: true

    contentWidth: availableWidth
    contentHeight: pageContent.implicitHeight

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AsNeeded

    required property var networkManager
    required property int bookId
    property int userId: 0

    property string title: "Loading..."
    property string author: ""
    property string genre: ""
    property string description: ""
    property real price: 0
    property real discountAmount: 0
    property real discountPercent: 0
    property real backendFinalPrice: -1

    property string publisherUsername: ""

    property string coverImageRaw: ""
    property string coverImageSource: ""

    property real totalRating: 0
    property int ratingCount: 0
    property bool isAvailable: true

    property bool isPurchased: false
    property bool isInCart: false
    property bool isInWishlist: false
    property bool isFavorite: false

    property int selectedStars: 5

    ListModel {
        id: commentsModel
    }

    background: Rectangle {
        color: "#1A0F1F"
    }

    function toNumber(value, fallback) {
        if (fallback === undefined)
            fallback = 0

        if (value === undefined || value === null || value === "")
            return fallback

        var n = Number(value)
        return isNaN(n) ? fallback : n
    }

    function averageRating() {
        if (ratingCount <= 0)
            return "N/A"
        return (totalRating / ratingCount).toFixed(1)
    }

    function hasDiscount() {
        return discountAmount > 0 || discountPercent > 0 || backendFinalPrice >= 0
    }

    function finalPrice() {
        if (backendFinalPrice >= 0)
            return Math.max(0, backendFinalPrice)

        if (discountAmount > 0)
            return Math.max(0, price - discountAmount)

        if (discountPercent > 0)
            return Math.max(0, price - (price * discountPercent / 100))

        return Math.max(0, price)
    }

    function resolvedDiscountAmount() {
        if (discountAmount > 0)
            return discountAmount

        if (discountPercent > 0)
            return price * discountPercent / 100

        if (backendFinalPrice >= 0 && price > backendFinalPrice)
            return price - backendFinalPrice

        return 0
    }

    function discountBadgeText() {
        if (discountPercent > 0)
            return "-" + discountPercent.toFixed(0) + "%"

        var amount = resolvedDiscountAmount()
        if (amount > 0)
            return "- $" + amount.toFixed(2)

        return ""
    }

    function resolveCoverImage(data) {
        var raw = data.coverImageData
               || data.coverImage
               || data.imagePath
               || data.coverUrl
               || data.image
               || ""

        coverImageRaw = raw

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

    function loadBook() {
        if (!networkManager)
            return

        networkManager.getBookDetails(userId, bookId)
        networkManager.getComments(bookId)
    }

    function refreshAfterAction() {
        loadBook()
    }

    Component.onCompleted: loadBook()

    Connections {
        target: networkManager

        function onResponseReceived(action, status, data) {
            var ok = (status === "success" || status === "SUCCESS")

            if (action === "getBookDetails" && ok) {
                detailsRoot.title = data.title || ""
                detailsRoot.author = data.author || ""
                detailsRoot.genre = data.genre || ""
                detailsRoot.description = data.description || ""
                detailsRoot.price = detailsRoot.toNumber(data.price, 0)

                detailsRoot.discountAmount = detailsRoot.toNumber(
                    data.discountAmount !== undefined ? data.discountAmount
                    : data.discount !== undefined ? data.discount
                    : data.discount_value !== undefined ? data.discount_value
                    : data.discountValue !== undefined ? data.discountValue
                    : data.discount_price !== undefined ? data.discount_price
                    : data.discountPrice !== undefined ? data.discountPrice
                    : 0, 0
                )

                detailsRoot.discountPercent = detailsRoot.toNumber(
                    data.discountPercent !== undefined ? data.discountPercent
                    : data.discount_percent !== undefined ? data.discount_percent
                    : data.percentDiscount !== undefined ? data.percentDiscount
                    : 0, 0
                )

                detailsRoot.backendFinalPrice = detailsRoot.toNumber(
                    data.finalPrice !== undefined ? data.finalPrice
                    : data.final_price !== undefined ? data.final_price
                    : data.discountedPrice !== undefined ? data.discountedPrice
                    : data.discounted_price !== undefined ? data.discounted_price
                    : -1, -1
                )

                detailsRoot.publisherUsername =
                        data.publisherUsername
                        || data.publisherName
                        || data.publisher_username
                        || data.publisher
                        || data.publisherUserName
                        || ""

                detailsRoot.coverImageSource = detailsRoot.resolveCoverImage(data)

                detailsRoot.totalRating = detailsRoot.toNumber(data.totalRating, 0)
                detailsRoot.ratingCount = detailsRoot.toNumber(data.ratingCount, 0)
                detailsRoot.isAvailable = data.isAvailable !== undefined ? data.isAvailable : true

                detailsRoot.isPurchased = data.isPurchased !== undefined ? data.isPurchased : false
                detailsRoot.isInCart = data.isInCart !== undefined ? data.isInCart : false
                detailsRoot.isInWishlist = data.isInWishlist !== undefined ? data.isInWishlist : false
                detailsRoot.isFavorite = data.isFavorite !== undefined ? data.isFavorite : false

                console.log("Book details data:", JSON.stringify(data))
            }
            else if (action === "getComments" && ok) {
                commentsModel.clear()

                if (data && data.length !== undefined) {
                    for (var i = 0; i < data.length; i++) {
                        var c = data[i]
                        commentsModel.append({
                            "commentId": c.commentId || c.id || 0,
                            "commentUserId": c.userId || 0,
                            "username": c.username || ("User #" + (c.userId || 0)),
                            "text": c.text || "",
                            "rating": c.rating || 0
                        })
                    }
                }
            }
            else if (action === "addComment") {
                if (ok) {
                    commentInput.text = ""
                    detailsRoot.selectedStars = 5
                    formStatus.text = "Comment posted successfully."
                    formStatus.color = "#4CAF50"
                    formStatus.visible = true
                    refreshAfterAction()
                } else {
                    formStatus.text = (data && data.message ? data.message : "Failed to post comment.")
                    formStatus.color = "#FF5555"
                    formStatus.visible = true
                }
            }
            else if (action === "editComment") {
                if (ok) {
                    formStatus.text = "Comment edited successfully."
                    formStatus.color = "#4CAF50"
                    formStatus.visible = true
                    refreshAfterAction()
                } else {
                    formStatus.text = (data && data.message ? data.message : "Failed to edit comment.")
                    formStatus.color = "#FF5555"
                    formStatus.visible = true
                }
            }
            else if (action === "removeComment" || action === "deleteComment") {
                if (ok) {
                    formStatus.text = "Comment removed successfully."
                    formStatus.color = "#4CAF50"
                    formStatus.visible = true
                    refreshAfterAction()
                } else {
                    formStatus.text = (data && data.message ? data.message : "Failed to remove comment.")
                    formStatus.color = "#FF5555"
                    formStatus.visible = true
                }
            }
            else if (action === "addToCart") {
                if (ok) {
                    actionStatus.text = "Book added to cart. Purchase will be completed after cart confirmation."
                    actionStatus.color = "#4CAF50"
                    actionStatus.visible = true
                    refreshAfterAction()
                } else {
                    actionStatus.text = (data && data.message ? data.message : "Failed to add book to cart.")
                    actionStatus.color = "#FF5555"
                    actionStatus.visible = true
                }
            }
            else if (action === "addToWishlist") {
                if (ok) {
                    actionStatus.text = "Book added to wishlist."
                    actionStatus.color = "#4CAF50"
                    actionStatus.visible = true
                    refreshAfterAction()
                } else {
                    actionStatus.text = (data && data.message ? data.message : "Failed to add book to wishlist.")
                    actionStatus.color = "#FF5555"
                    actionStatus.visible = true
                }
            }
            else if (action === "addToFavorites") {
                if (ok) {
                    actionStatus.text = "Book added to favorites."
                    actionStatus.color = "#4CAF50"
                    actionStatus.visible = true
                    refreshAfterAction()
                } else {
                    actionStatus.text = (data && data.message ? data.message : "Failed to add book to favorites.")
                    actionStatus.color = "#FF5555"
                    actionStatus.visible = true
                }
            }
        }
    }

    Column {
        id: pageContent
        width: Math.min(detailsRoot.availableWidth - 40, 920)
        x: (detailsRoot.availableWidth - width) / 2
        spacing: 20

        Item {
            width: 1
            height: 15
        }

        Button {
            id: backButton
            text: "← Back"
            width: 90
            height: 36

            contentItem: Text {
                text: parent.text
                color: "#D4AF37"
                font.pixelSize: 14
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: "transparent"
                border.color: "#5c3d75"
                border.width: 1
                radius: 8
            }

            onClicked: {
                var sv = detailsRoot.StackView.view
                if (sv)
                    sv.pop()
            }
        }

        Rectangle {
            width: parent.width
            color: "#2D1B33"
            radius: 14
            border.color: "#5c3d75"
            border.width: 1
            implicitHeight: headerRow.implicitHeight + 30

            RowLayout {
                id: headerRow
                anchors.fill: parent
                anchors.margins: 15
                spacing: 25

                Rectangle {
                    Layout.preferredWidth: 220
                    Layout.preferredHeight: 320
                    color: "#1A0F1F"
                    radius: 12
                    border.color: "#D4AF37"
                    border.width: 1
                    clip: true

                    Image {
                        id: coverImageItem
                        anchors.fill: parent
                        anchors.margins: 6
                        source: detailsRoot.coverImageSource !== "" ? detailsRoot.coverImageSource : "qrc:/assets/books/default_cover.png"
                        fillMode: Image.PreserveAspectFit
                        asynchronous: true
                        cache: false

                        onStatusChanged: {
                            if (status === Image.Error) {
                                console.log("Image load error. Source:", source)
                                source = "qrc:/assets/books/default_cover.png"
                            }
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignTop
                    spacing: 10

                    Text {
                        text: detailsRoot.title
                        color: "#D4AF37"
                        font.pixelSize: 28
                        font.bold: true
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignRight
                    }

                    Text {
                        text: "by " + detailsRoot.author
                        color: "white"
                        font.pixelSize: 16
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignLeft
                    }

                    RowLayout {
                        spacing: 10

                        Rectangle {
                            visible: detailsRoot.genre !== ""
                            color: "#1A0F1F"
                            border.color: "#D4AF37"
                            border.width: 1
                            radius: 12
                            implicitHeight: 28
                            implicitWidth: genreText.implicitWidth + 24

                            Text {
                                id: genreText
                                anchors.centerIn: parent
                                text: detailsRoot.genre
                                color: "#D4AF37"
                                font.pixelSize: 12
                                font.bold: true
                            }
                        }

                        Rectangle {
                            visible: !detailsRoot.isAvailable
                            color: "#FF5555"
                            radius: 12
                            implicitHeight: 28
                            implicitWidth: unavailableText.implicitWidth + 24

                            Text {
                                id: unavailableText
                                anchors.centerIn: parent
                                text: "Currently Unavailable"
                                color: "white"
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }

                        Rectangle {
                            visible: detailsRoot.isPurchased
                            color: "#4CAF50"
                            radius: 12
                            implicitHeight: 28
                            implicitWidth: purchasedText.implicitWidth + 24

                            Text {
                                id: purchasedText
                                anchors.centerIn: parent
                                text: "Purchased"
                                color: "white"
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }
                    }

                    RowLayout {
                        spacing: 6

                        Text {
                            text: "⭐"
                            font.pixelSize: 18
                        }

                        Text {
                            text: detailsRoot.averageRating() + " (" + detailsRoot.ratingCount + " ratings)"
                            color: "#D4AF37"
                            font.pixelSize: 14
                        }

                        Text {
                            text: "•"
                            color: "#A08EAD"
                            font.pixelSize: 14
                        }

                        Text {
                            text: commentsModel.count + " comments"
                            color: "#A08EAD"
                            font.pixelSize: 14
                        }
                    }

                    Text {
                        visible: detailsRoot.publisherUsername !== ""
                        text: "Publisher: " + detailsRoot.publisherUsername
                        color: "#A08EAD"
                        font.pixelSize: 13
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        spacing: 12
                        Layout.topMargin: 4

                        Text {
                            text: detailsRoot.hasDiscount()
                                  ? "$" + detailsRoot.finalPrice().toFixed(2)
                                  : (detailsRoot.price === 0 ? "Free" : "$" + detailsRoot.price.toFixed(2))
                            color: "#4CAF50"
                            font.pixelSize: 22
                            font.bold: true
                        }

                        Text {
                            visible: detailsRoot.hasDiscount() && detailsRoot.price > detailsRoot.finalPrice()
                            text: "$" + detailsRoot.price.toFixed(2)
                            color: "#A08EAD"
                            font.pixelSize: 15
                            font.strikeout: true
                        }

                        Rectangle {
                            visible: detailsRoot.hasDiscount() && detailsRoot.discountBadgeText() !== ""
                            color: "#FF5555"
                            radius: 10
                            implicitHeight: 22
                            implicitWidth: discountText.implicitWidth + 16

                            Text {
                                id: discountText
                                anchors.centerIn: parent
                                text: detailsRoot.discountBadgeText()
                                color: "white"
                                font.pixelSize: 11
                                font.bold: true
                            }
                        }
                    }

                    Text {
                        id: actionStatus
                        visible: false
                        color: "#4CAF50"
                        font.pixelSize: 12
                        font.bold: true
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.topMargin: 6
                        spacing: 10

                        Button {
                            text: "📖 Read"
                            Layout.preferredWidth: 120
                            Layout.preferredHeight: 42
                            enabled: detailsRoot.isPurchased

                            contentItem: Text {
                                text: parent.text
                                color: "#1A0F1F"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            background: Rectangle {
                                color: parent.enabled ? "#D4AF37" : "#7A6A35"
                                radius: 8
                            }

                            onClicked: {
                                if (!detailsRoot.isPurchased) {
                                    actionStatus.text = "You must complete the purchase from cart before reading this book."
                                    actionStatus.color = "#FF5555"
                                    actionStatus.visible = true
                                    return
                                }

                                var sv = detailsRoot.StackView.view
                                if (sv) {
                                    sv.push("PdfReader.qml", {
                                        "networkManager": networkManager,
                                        "bookId": bookId,
                                        "userId": userId,
                                        "title": detailsRoot.title
                                    })
                                }
                            }
                        }

                        Button {
                            text: detailsRoot.isInCart ? "🛒 In Cart" : "🛒 Add to Cart"
                            Layout.preferredHeight: 42
                            enabled: !detailsRoot.isPurchased && !detailsRoot.isInCart && detailsRoot.isAvailable

                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            background: Rectangle {
                                color: parent.enabled ? "#5c3d75" : "#4A3A52"
                                radius: 8
                            }

                            onClicked: {
                                if (detailsRoot.isPurchased) {
                                    actionStatus.text = "This book has already been purchased."
                                    actionStatus.color = "#FF5555"
                                    actionStatus.visible = true
                                    return
                                }

                                if (networkManager)
                                    networkManager.addToCart(userId, bookId)
                            }
                        }

                        Button {
                            text: detailsRoot.isInWishlist ? "🔖 In Wishlist" : "🔖 Wishlist"
                            Layout.preferredHeight: 42
                            enabled: !detailsRoot.isPurchased && !detailsRoot.isInWishlist

                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            background: Rectangle {
                                color: parent.enabled ? "#5c3d75" : "#4A3A52"
                                radius: 8
                            }

                            onClicked: {
                                if (detailsRoot.isPurchased) {
                                    actionStatus.text = "Purchased books cannot be added to wishlist."
                                    actionStatus.color = "#FF5555"
                                    actionStatus.visible = true
                                    return
                                }

                                if (networkManager)
                                    networkManager.addToWishlist(userId, bookId)
                            }
                        }

                        Button {
                            text: detailsRoot.isFavorite ? "❤ Favorited" : "❤ Favorite"
                            Layout.preferredHeight: 42
                            enabled: !detailsRoot.isFavorite

                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }

                            background: Rectangle {
                                color: parent.enabled ? "#5c3d75" : "#4A3A52"
                                radius: 8
                            }

                            onClicked: {
                                if (networkManager)
                                    networkManager.addToFavorites(userId, bookId)
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            width: parent.width
            color: "#2D1B33"
            radius: 12
            border.color: "#5c3d75"
            border.width: 1
            implicitHeight: descriptionColumn.implicitHeight + 30

            Column {
                id: descriptionColumn
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 15
                spacing: 10

                Text {
                    text: "Description"
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    width: parent.width
                    text: detailsRoot.description !== "" ? detailsRoot.description : "No description provided."
                    color: "white"
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignRight
                }
            }
        }

        Rectangle {
            width: parent.width
            color: "#2D1B33"
            radius: 12
            border.color: "#5c3d75"
            border.width: 1
            implicitHeight: commentsSection.implicitHeight + 30

            Column {
                id: commentsSection
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 15
                spacing: 15

                Text {
                    text: "Comments (" + commentsModel.count + ")"
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Column {
                    width: parent.width
                    spacing: 8

                    Text {
                        text: "Your rating:"
                        color: "#A08EAD"
                        font.pixelSize: 13
                    }

                    Row {
                        spacing: 4

                        Repeater {
                            model: 5

                            Text {
                                text: index < detailsRoot.selectedStars ? "★" : "☆"
                                color: "#D4AF37"
                                font.pixelSize: 26

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: detailsRoot.selectedStars = index + 1
                                }
                            }
                        }
                    }

                    TextArea {
                        id: commentInput
                        width: parent.width
                        height: 80
                        placeholderText: "Write a comment about this book..."
                        placeholderTextColor: "#807090"
                        color: "white"
                        wrapMode: TextArea.Wrap

                        background: Rectangle {
                            color: "#1A0F1F"
                            border.color: "#5c3d75"
                            border.width: 1
                            radius: 8
                        }
                    }

                    Text {
                        id: formStatus
                        visible: false
                        font.pixelSize: 12
                        font.bold: true
                    }

                    Button {
                        text: "Post Comment"
                        width: 150
                        height: 38

                        contentItem: Text {
                            text: parent.text
                            color: "#1A0F1F"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: "#D4AF37"
                            radius: 6
                        }

                        onClicked: {
                            if (commentInput.text.trim() === "") {
                                formStatus.text = "Comment can't be empty."
                                formStatus.color = "#FF5555"
                                formStatus.visible = true
                                return
                            }

                            if (networkManager) {
                                // ✅ اصلاح: userId, bookId, text, rating
                                networkManager.addComment(
                                    userId,                     // userId اول
                                    bookId,                     // bookId دوم
                                    commentInput.text.trim(),   // text سوم
                                    detailsRoot.selectedStars   // rating چهارم
                                )
                            }
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: "#5c3d75"
                }

                Column {
                    width: parent.width
                    spacing: 12

                    Repeater {
                        model: commentsModel

                        delegate: Rectangle {
                            width: commentsSection.width
                            color: "#1A0F1F"
                            radius: 8
                            border.color: "#3d2545"
                            border.width: 1
                            implicitHeight: commentBody.implicitHeight + 24

                            property bool isOwner: model.commentUserId === detailsRoot.userId

                            Column {
                                id: commentBody
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.margins: 12
                                spacing: 6

                                RowLayout {
                                    width: parent.width

                                    Row {
                                        spacing: 8
                                        Layout.alignment: Qt.AlignLeft

                                        Text {
                                            text: model.username
                                            color: "#D4AF37"
                                            font.pixelSize: 13
                                            font.bold: true
                                        }

                                        Text {
                                            text: "•"
                                            color: "#A08EAD"
                                            font.pixelSize: 13
                                        }

                                        Text {
                                            text: "★".repeat(model.rating) + "☆".repeat(5 - model.rating)
                                            color: "#D4AF37"
                                            font.pixelSize: 13
                                        }
                                    }

                                    // Action buttons for comment owner
                                    Row {
                                        spacing: 10
                                        Layout.alignment: Qt.AlignRight
                                        visible: isOwner

                                        Button {
                                            text: "Edit"
                                            implicitWidth: 50
                                            implicitHeight: 24

                                            contentItem: Text {
                                                text: parent.text
                                                color: "#D4AF37"
                                                font.pixelSize: 11
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            background: Rectangle {
                                                color: "transparent"
                                                border.color: "#D4AF37"
                                                border.width: 1
                                                radius: 4
                                            }

                                            onClicked: {
                                                editCommentDialog.targetCommentId = model.commentId
                                                editCommentDialog.tempText = model.text
                                                editCommentDialog.tempRating = model.rating
                                                editCommentDialog.open()
                                            }
                                        }

                                        Button {
                                            text: "Delete"
                                            implicitWidth: 60
                                            implicitHeight: 24

                                            contentItem: Text {
                                                text: parent.text
                                                color: "#FF5555"
                                                font.pixelSize: 11
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            background: Rectangle {
                                                color: "transparent"
                                                border.color: "#FF5555"
                                                border.width: 1
                                                radius: 4
                                            }

                                            onClicked: {
                                                if (networkManager) {
                                                    // Standard C++ side removeComment(commentId)
                                                    networkManager.removeComment(model.commentId)
                                                }
                                            }
                                        }
                                    }
                                }

                                Text {
                                    width: parent.width
                                    text: model.text
                                    color: "white"
                                    font.pixelSize: 13
                                    wrapMode: Text.WordWrap
                                    horizontalAlignment: Text.AlignRight
                                }
                            }
                        }
                    }

                    Text {
                        visible: commentsModel.count === 0
                        text: "No comments yet. Be the first to share your thoughts!"
                        color: "#A08EAD"
                        font.pixelSize: 13
                    }
                }
            }
        }

        Item {
            width: 1
            height: 30
        }
    }

    // Dialog for editing existing comment
    Dialog {
        id: editCommentDialog
        title: "Edit Comment"
        x: (detailsRoot.availableWidth - width) / 2
        y: (parent.height - height) / 2
        modal: true
        parent: Overlay.overlay

        property int targetCommentId: 0
        property string tempText: ""
        property int tempRating: 5

        background: Rectangle {
            color: "#2D1B33"
            border.color: "#5c3d75"
            border.width: 1
            radius: 12
        }

        header: Label {
            text: "Edit your comment"
            color: "#D4AF37"
            font.pixelSize: 16
            font.bold: true
            padding: 12
            background: Rectangle {
                color: "#1A0F1F"
                radius: 12
            }
        }

        contentItem: ColumnLayout {
            spacing: 12
            width: 320

            Text {
                text: "Your Rating:"
                color: "#A08EAD"
                font.pixelSize: 12
            }

            Row {
                spacing: 4
                Layout.alignment: Qt.AlignLeft

                Repeater {
                    model: 5

                    Text {
                        text: index < editCommentDialog.tempRating ? "★" : "☆"
                        color: "#D4AF37"
                        font.pixelSize: 26

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: editCommentDialog.tempRating = index + 1
                        }
                    }
                }
            }

            TextArea {
                id: editTextInput
                Layout.fillWidth: true
                implicitHeight: 80
                text: editCommentDialog.tempText
                placeholderText: "Edit your comment..."
                placeholderTextColor: "#807090"
                color: "white"
                wrapMode: TextArea.Wrap

                background: Rectangle {
                    color: "#1A0F1F"
                    border.color: "#5c3d75"
                    border.width: 1
                    radius: 8
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: 10
                spacing: 10

                Button {
                    text: "Save"
                    Layout.fillWidth: true
                    implicitHeight: 36

                    contentItem: Text {
                        text: parent.text
                        color: "#1A0F1F"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: "#D4AF37"
                        radius: 6
                    }

                    onClicked: {
                        if (editTextInput.text.trim() === "")
                            return

                        if (networkManager) {
                            networkManager.editComment(
                                editCommentDialog.targetCommentId,
                                userId,
                                bookId,
                                editTextInput.text.trim(),
                                editCommentDialog.tempRating
                            )
                        }
                        editCommentDialog.close()
                    }

                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    implicitHeight: 36

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: "#5c3d75"
                        radius: 6
                    }

                    onClicked: editCommentDialog.close()
                }
            }
        }
    }
}}