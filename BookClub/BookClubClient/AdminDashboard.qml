import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: dashboardRoot

    required property string username
    required property string userRole
    property int userId: 0
    property int currentTab: 0

    Component.onCompleted: currentTab = 0

    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: sidebar
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            color: "#2D1B33"

            Rectangle {
                width: 1
                anchors.right: parent.right
                height: parent.height
                color: "#D4AF37"
                opacity: 0.3
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 15

                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 100
                    height: 100
                    radius: 50
                    color: "#1A0F1F"
                    border.color: "#D4AF37"
                    border.width: 2

                    Image {
                        anchors.centerIn: parent
                        source: "qrc:/assets/images/giraffe.png"
                        fillMode: Image.PreserveAspectFit
                        width: 70
                        height: 70
                    }
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "Welcome, " + username
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: userRole
                    color: "#D4AF37"
                    font.pixelSize: 12
                    opacity: 0.7
                }

                Item { Layout.preferredHeight: 20 }

                Repeater {
                    model: ["Users", "Books", "Comments"]
                    delegate: Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45

                        background: Rectangle {
                            color: currentTab === index ? "#D4AF37" : "transparent"
                            radius: 8
                            border.color: currentTab === index ? "#D4AF37" : "transparent"
                        }

                        contentItem: Text {
                            text: modelData
                            color: currentTab === index ? "#1A0F1F" : "#D4AF37"
                            font.pixelSize: 16
                            font.bold: currentTab === index
                            leftPadding: 15
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: currentTab = index
                    }
                }

                Item { Layout.fillHeight: true }

                Button {
                    Layout.fillWidth: true
                    text: "Logout"
                    onClicked: {
                        if (dashboardRoot.StackView.view) {
                            dashboardRoot.StackView.view.clear()
                            dashboardRoot.StackView.view.push("Login.qml")
                        }
                    }
                    background: Rectangle {
                        color: "transparent"
                        border.color: "#D4AF37"
                        border.width: 1
                        radius: 8
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#D4AF37"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        Loader {
            id: contentLoader
            Layout.fillWidth: true
            Layout.fillHeight: true

            sourceComponent: currentTab === 0 ? usersViewComponent
                              : currentTab === 1 ? booksViewComponent
                              : commentsViewComponent

            onSourceComponentChanged: {
                opacity = 0
                fadeIn.start()
            }

            NumberAnimation {
                id: fadeIn
                target: contentLoader
                property: "opacity"
                from: 0
                to: 1
                duration: 250
            }
        }
    }

    Component {
        id: usersViewComponent

        Item {
            id: usersRoot

            property string activeType: "users"
            property string searchQuery: ""

            ListModel { id: usersModel }

            function fillModel(list, isPublisher) {
                usersModel.clear()
                if (!list) return
                for (var i = 0; i < list.length; i++) {
                    var u = list[i]
                    usersModel.append({
                        "userId": u.id || u.userId || 0,
                        "username": u.username || "Unknown",
                        "registeredAt": u.createdAt || u.registrationDate || u.registeredAt || u.joinDate || "",
                        "isBlocked": u.isBlocked !== undefined ? u.isBlocked : (u.blocked !== undefined ? u.blocked : false),
                        "isPublisher": isPublisher
                    })
                }
            }

            function loadUsers() {
                if (!networkManager) return
                if (activeType === "users")
                    networkManager.getAllUsers()
                else
                    networkManager.getAllPublishers()
            }

            function doSearch() {
                if (!networkManager) return
                if (searchQuery === "") { loadUsers(); return }
                if (activeType === "users")
                    networkManager.searchUsers(searchQuery)
                else
                    networkManager.searchPublishers(searchQuery)
            }

            Component.onCompleted: loadUsers()

            Connections {
                target: networkManager
                function onResponseReceived(action, status, data) {
                    var ok = (status === "success" || status === "SUCCESS")

                    if (action === "getAllUsers" || action === "getAllPublishers"
                            || action === "searchUsers" || action === "searchPublishers") {
                        console.log("AdminUsersView - " + action + ":", JSON.stringify(data))
                    }

                    if (!ok) return

                    if (action === "getAllUsers" || action === "searchUsers") {
                        fillModel(data, false)
                    } else if (action === "getAllPublishers" || action === "searchPublishers") {
                        fillModel(data, true)
                    } else if (action === "blockUser" || action === "unblockUser" || action === "deleteUser") {
                        loadUsers()
                    }
                }
            }

            Rectangle { anchors.fill: parent; color: "#1A0F1F" }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15

                Text {
                    text: "User Management"
                    color: "#D4AF37"
                    font.pixelSize: 22
                    font.bold: true
                }

                RowLayout {
                    spacing: 10

                    Button {
                        text: "Users"
                        background: Rectangle {
                            color: usersRoot.activeType === "users" ? "#D4AF37" : "#2D1B33"
                            radius: 8; border.color: "#D4AF37"; border.width: 1
                        }
                        contentItem: Text {
                            text: parent.text
                            color: usersRoot.activeType === "users" ? "#1A0F1F" : "#D4AF37"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                        }
                        onClicked: {
                            usersRoot.activeType = "users"
                            usersRoot.searchQuery = ""
                            searchField.text = ""
                            usersRoot.loadUsers()
                        }
                    }

                    Button {
                        text: "Publishers"
                        background: Rectangle {
                            color: usersRoot.activeType === "publishers" ? "#D4AF37" : "#2D1B33"
                            radius: 8; border.color: "#D4AF37"; border.width: 1
                        }
                        contentItem: Text {
                            text: parent.text
                            color: usersRoot.activeType === "publishers" ? "#1A0F1F" : "#D4AF37"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                        }
                        onClicked: {
                            usersRoot.activeType = "publishers"
                            usersRoot.searchQuery = ""
                            searchField.text = ""
                            usersRoot.loadUsers()
                        }
                    }

                    Item { Layout.fillWidth: true }

                    TextField {
                        id: searchField
                        Layout.preferredWidth: 220
                        placeholderText: "Search by username..."
                        color: "white"
                        placeholderTextColor: "#807090"
                        background: Rectangle {
                            color: "#2D1B33"; border.color: "#5c3d75"; border.width: 1; radius: 8
                        }
                        onTextChanged: usersRoot.searchQuery = text
                        onAccepted: usersRoot.doSearch()
                    }

                    Button {
                        text: "Search"
                        onClicked: usersRoot.doSearch()
                        background: Rectangle { color: "#5c3d75"; radius: 8 }
                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#2D1B33"
                    radius: 12
                    border.color: "#5c3d75"
                    border.width: 1
                    clip: true

                    Text {
                        anchors.centerIn: parent
                        visible: usersModel.count === 0
                        text: "No " + usersRoot.activeType + " found."
                        color: "#A08EAD"
                        font.pixelSize: 14
                    }

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        ListView {
                            id: listView
                            model: usersModel
                            spacing: 8

                            delegate: Rectangle {
                                width: listView.width
                                height: 60
                                color: "#1A0F1F"
                                radius: 8
                                border.color: "#3d2545"
                                border.width: 1

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 10

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 2

                                        Text {
                                            text: model.username + (model.isPublisher ? " (Publisher)" : "")
                                            color: "#D4AF37"
                                            font.bold: true
                                            font.pixelSize: 14
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }

                                        Text {
                                            text: model.registeredAt !== "" ? ("Joined: " + model.registeredAt) : ""
                                            color: "#A08EAD"
                                            font.pixelSize: 11
                                        }
                                    }

                                    Rectangle {
                                        width: 90
                                        height: 26
                                        radius: 13
                                        color: model.isBlocked ? "#FF5555" : "#4CAF50"

                                        Text {
                                            anchors.centerIn: parent
                                            text: model.isBlocked ? "Blocked" : "Active"
                                            color: "white"
                                            font.pixelSize: 11
                                            font.bold: true
                                        }
                                    }

                                    Button {
                                        text: model.isBlocked ? "Unblock" : "Block"
                                        Layout.preferredWidth: 90
                                        background: Rectangle { color: "#5c3d75"; radius: 6 }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "white"
                                            font.pixelSize: 12
                                            horizontalAlignment: Text.AlignHCenter
                                        }
                                        onClicked: {
                                            if (!networkManager) return
                                            if (model.isBlocked)
                                                networkManager.unblockUser(model.userId)
                                            else
                                                networkManager.blockUser(model.userId)
                                        }
                                    }

                                    Button {
                                        text: "Delete"
                                        Layout.preferredWidth: 80
                                        background: Rectangle { color: "#8B2C2C"; radius: 6 }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "white"
                                            font.pixelSize: 12
                                            horizontalAlignment: Text.AlignHCenter
                                        }
                                        onClicked: {
                                            deleteConfirmDialog.targetUserId = model.userId
                                            deleteConfirmDialog.targetUsername = model.username
                                            deleteConfirmDialog.open()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Dialog {
                id: deleteConfirmDialog
                property int targetUserId: 0
                property string targetUsername: ""
                title: "Confirm Delete"
                modal: true
                anchors.centerIn: parent
                standardButtons: Dialog.Yes | Dialog.No

                contentItem: Text {
                    text: "Are you sure you want to delete \"" + deleteConfirmDialog.targetUsername + "\"? This cannot be undone."
                    color: "white"
                    wrapMode: Text.WordWrap
                }

                background: Rectangle {
                    color: "#2D1B33"; border.color: "#D4AF37"; border.width: 1; radius: 10
                }

                onAccepted: {
                    if (networkManager)
                        networkManager.deleteUserByAdmin(deleteConfirmDialog.targetUserId)
                }
            }
        }
    }

    Component {
        id: booksViewComponent

        Item {
            id: booksRoot

            ListModel { id: booksModel }

            function fillModel(list) {
                booksModel.clear()
                if (!list) return
                for (var i = 0; i < list.length; i++) {
                    var b = list[i]
                    booksModel.append({
                        "bookId": b.id || 0,
                        "title": b.title || "Unknown",
                        "author": b.author || "Unknown",
                        "genre": b.genre || "",
                        "description": b.description || "",
                        "price": b.price || 0,
                        "isAvailable": b.isAvailable !== undefined ? b.isAvailable : true,
                        "publisherId": b.publisherId || 0
                    })
                }
            }

            function loadBooks() {
                if (networkManager) networkManager.getAllBooks()
            }

            Component.onCompleted: loadBooks()

            Connections {
                target: networkManager
                function onResponseReceived(action, status, data) {
                    var ok = (status === "success" || status === "SUCCESS")
                    if (!ok) return
                    if (action === "getAllBooks") fillModel(data)
                    else if (action === "removeBook") loadBooks()
                    else if (action === "adminUpdateBook") {
                        if (ok) {
                            editBookDialog.close()
                            loadBooks()
                        }
                    }
                }
            }

            Rectangle { anchors.fill: parent; color: "#1A0F1F" }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15

                Text {
                    text: "Book & Content Management"
                    color: "#D4AF37"
                    font.pixelSize: 22
                    font.bold: true
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#2D1B33"
                    radius: 12
                    border.color: "#5c3d75"
                    border.width: 1
                    clip: true

                    Text {
                        anchors.centerIn: parent
                        visible: booksModel.count === 0
                        text: "No books found."
                        color: "#A08EAD"
                        font.pixelSize: 14
                    }

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        ListView {
                            id: listView
                            model: booksModel
                            spacing: 8

                            delegate: Rectangle {
                                width: listView.width
                                height: 80
                                color: "#1A0F1F"
                                radius: 8
                                border.color: "#3d2545"
                                border.width: 1

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 10

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 2

                                        Text {
                                            text: model.title
                                            color: "#D4AF37"
                                            font.bold: true
                                            font.pixelSize: 14
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }
                                        Text {
                                            text: model.author + (model.genre !== "" ? " • " + model.genre : "")
                                            color: "#A08EAD"
                                            font.pixelSize: 11
                                            elide: Text.ElideRight
                                            Layout.fillWidth: true
                                        }
                                    }

                                    Text {
                                        text: model.price === 0 ? "Free" : ("$" + model.price.toFixed(2))
                                        color: "#4CAF50"
                                        font.bold: true
                                    }

                                    Rectangle {
                                        width: 90
                                        height: 26
                                        radius: 13
                                        color: model.isAvailable ? "#4CAF50" : "#FF5555"
                                        Text {
                                            anchors.centerIn: parent
                                            text: model.isAvailable ? "Available" : "Unavailable"
                                            color: "white"
                                            font.pixelSize: 11
                                            font.bold: true
                                        }
                                    }

                                    Button {
                                        text: "Edit"
                                        Layout.preferredWidth: 70
                                        background: Rectangle { color: "#D4AF37"; radius: 6 }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "#1A0F1F"
                                            font.pixelSize: 12
                                            horizontalAlignment: Text.AlignHCenter
                                        }
                                        onClicked: {
                                            editBookDialog.targetBookId = model.bookId
                                            editBookDialog.targetPublisherId = model.publisherId
                                            editBookDialog.titleText = model.title
                                            editBookDialog.authorText = model.author
                                            editBookDialog.genreText = model.genre
                                            editBookDialog.descText = model.description || ""
                                            editBookDialog.priceText = model.price.toString()
                                            editBookDialog.availableChecked = model.isAvailable
                                            editBookDialog.open()
                                        }
                                    }

                                    Button {
                                        text: "Remove"
                                        Layout.preferredWidth: 90
                                        background: Rectangle { color: "#8B2C2C"; radius: 6 }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "white"
                                            font.pixelSize: 12
                                            horizontalAlignment: Text.AlignHCenter
                                        }
                                        onClicked: {
                                            removeConfirmDialog.targetBookId = model.bookId
                                            removeConfirmDialog.targetPublisherId = model.publisherId
                                            removeConfirmDialog.targetTitle = model.title
                                            removeConfirmDialog.open()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Dialog {
                id: removeConfirmDialog
                property int targetBookId: 0
                property int targetPublisherId: 0
                property string targetTitle: ""
                title: "Confirm Removal"
                modal: true
                anchors.centerIn: parent
                standardButtons: Dialog.Yes | Dialog.No

                contentItem: Text {
                    text: "Remove \"" + removeConfirmDialog.targetTitle + "\" from the store? This cannot be undone."
                    color: "white"
                    wrapMode: Text.WordWrap
                }
                background: Rectangle { color: "#2D1B33"; border.color: "#D4AF37"; border.width: 1; radius: 10 }

                onAccepted: {
                    if (networkManager)
                        networkManager.removeBook(removeConfirmDialog.targetPublisherId, removeConfirmDialog.targetBookId)
                }
            }

            Dialog {
                id: editBookDialog
                property int targetBookId: 0
                property int targetPublisherId: 0
                property string titleText: ""
                property string authorText: ""
                property string genreText: ""
                property string descText: ""
                property string priceText: "0"
                property bool availableChecked: true

                title: "Edit Book Details"
                modal: true
                anchors.centerIn: parent
                standardButtons: Dialog.Ok | Dialog.Cancel
                width: Math.min(parent.width * 0.9, 500)

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
                        text: "Edit Book Details"
                        color: "#D4AF37"
                        anchors.centerIn: parent
                        font.bold: true
                    }
                }

                contentItem: ColumnLayout {
                    spacing: 12
                    width: parent.width

                    ColumnLayout {
                        spacing: 4
                        Layout.fillWidth: true
                        Text { text: "Title:"; color: "#A08EAD"; font.pixelSize: 13 }
                        TextField {
                            id: editTitle
                            text: editBookDialog.titleText
                            Layout.fillWidth: true
                            color: "white"
                            background: Rectangle { color: "#1A0F1F"; border.color: "#5c3d75"; border.width: 1; radius: 5 }
                        }
                    }

                    ColumnLayout {
                        spacing: 4
                        Layout.fillWidth: true
                        Text { text: "Author:"; color: "#A08EAD"; font.pixelSize: 13 }
                        TextField {
                            id: editAuthor
                            text: editBookDialog.authorText
                            Layout.fillWidth: true
                            color: "white"
                            background: Rectangle { color: "#1A0F1F"; border.color: "#5c3d75"; border.width: 1; radius: 5 }
                        }
                    }

                    ColumnLayout {
                        spacing: 4
                        Layout.fillWidth: true
                        Text { text: "Genre:"; color: "#A08EAD"; font.pixelSize: 13 }
                        ComboBox {
                            id: editGenre
                            model: ["Fiction", "NonFiction", "Mystery", "Romance", "SciFi", "Fantasy",
                                    "Biography", "History", "SelfHelp", "Poetry", "Children", "Other"]
                            Layout.fillWidth: true
                            currentIndex: {
                                var idx = model.indexOf(editBookDialog.genreText)
                                return idx !== -1 ? idx : 0
                            }
                            background: Rectangle {
                                color: "#1A0F1F"
                                border.color: editGenre.activeFocus ? "#D4AF37" : "#5c3d75"
                                border.width: 1
                                radius: 5
                            }
                            contentItem: Text {
                                text: editGenre.displayText
                                color: "white"
                                font.pixelSize: 13
                                leftPadding: 10
                                verticalAlignment: Text.AlignVCenter
                            }
                            indicator: Text {
                                text: "▼"
                                color: "#D4AF37"
                                font.pixelSize: 14
                                anchors.right: parent.right
                                anchors.rightMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 4
                        Layout.fillWidth: true
                        Text { text: "Description:"; color: "#A08EAD"; font.pixelSize: 13 }
                        TextArea {
                            id: editDesc
                            text: editBookDialog.descText
                            Layout.fillWidth: true
                            implicitHeight: 80
                            color: "white"
                            wrapMode: TextArea.Wrap
                            background: Rectangle {
                                color: "#1A0F1F"
                                border.color: editDesc.activeFocus ? "#D4AF37" : "#5c3d75"
                                border.width: 1
                                radius: 5
                            }
                        }
                    }

                    RowLayout {
                        spacing: 15
                        Layout.fillWidth: true
                        ColumnLayout {
                            spacing: 4
                            Layout.fillWidth: true
                            Text { text: "Price:"; color: "#A08EAD"; font.pixelSize: 13 }
                            TextField {
                                id: editPrice
                                text: editBookDialog.priceText
                                Layout.fillWidth: true
                                color: "white"
                                validator: DoubleValidator { bottom: 0; decimals: 2 }
                                background: Rectangle { color: "#1A0F1F"; border.color: "#5c3d75"; border.width: 1; radius: 5 }
                            }
                        }

                        ColumnLayout {
                            spacing: 4
                            Layout.alignment: Qt.AlignBottom
                            Text { text: "Available:"; color: "#A08EAD"; font.pixelSize: 13 }
                            CheckBox {
                                id: editAvailable
                                checked: editBookDialog.availableChecked
                                indicator: Rectangle {
                                    width: 24; height: 24; radius: 4
                                    color: editAvailable.checked ? "#D4AF37" : "#2D1B33"
                                    border.color: "#D4AF37"; border.width: 2
                                    Text {
                                        anchors.centerIn: parent
                                        text: editAvailable.checked ? "✓" : ""
                                        color: "#1A0F1F"
                                        font.bold: true
                                    }
                                }
                                contentItem: Text {
                                    text: ""
                                }
                            }
                        }
                    }
                }

                onAccepted: {
                    if (!networkManager) return
                    var title = editTitle.text.trim()
                    var author = editAuthor.text.trim()
                    var genre = editGenre.currentText
                    var desc = editDesc.text.trim()
                    var price = parseFloat(editPrice.text)
                    if (isNaN(price) || price < 0) price = 0
                    var available = editAvailable.checked

                    if (title === "" || author === "") {
                        editBookDialog.close()
                        return
                    }

                    networkManager.updateBookDetails(
                        editBookDialog.targetBookId,
                        title,
                        author,
                        price,
                        genre,
                        desc,
                        "",  // coverPath
                        "",  // pdfPath
                        "admin",
                        editBookDialog.targetPublisherId
                    )
                }
            }
        }
    }

    Component {
        id: commentsViewComponent

        Item {
            id: commentsRoot

            property int selectedBookId: 0
            property string selectedBookTitle: ""

            ListModel { id: booksModel2 }
            ListModel { id: commentsModel }

            function fillBooks(list) {
                booksModel2.clear()
                if (!list) return
                for (var i = 0; i < list.length; i++) {
                    var b = list[i]
                    booksModel2.append({ "bookId": b.id || 0, "title": b.title || "Unknown" })
                }
            }

            function fillComments(list) {
                commentsModel.clear()
                if (!list) return
                for (var i = 0; i < list.length; i++) {
                    var c = list[i]
                    commentsModel.append({
                        "commentId": c.id || c.commentId || 0,
                        "username": c.username || ("User #" + (c.userId || 0)),
                        "text": c.text || "",
                        "rating": c.rating || 0
                    })
                }
            }

            function loadBooks() {
                if (networkManager) networkManager.getAllBooks()
            }

            function selectBook(bookId, title) {
                selectedBookId = bookId
                selectedBookTitle = title
                commentsModel.clear()
                if (networkManager) networkManager.getComments(bookId)
            }

            Component.onCompleted: loadBooks()

            Connections {
                target: networkManager
                function onResponseReceived(action, status, data) {
                    var ok = (status === "success" || status === "SUCCESS")

                    if (action === "getComments")
                        console.log("AdminCommentsView - getComments raw:", JSON.stringify(data))

                    if (!ok) return

                    if (action === "getAllBooks") fillBooks(data)
                    else if (action === "getComments") fillComments(data)
                    else if (action === "adminRemoveComment") {
                        if (commentsRoot.selectedBookId > 0 && networkManager)
                            networkManager.getComments(commentsRoot.selectedBookId)
                    }
                }
            }

            Rectangle { anchors.fill: parent; color: "#1A0F1F" }

            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15

                Rectangle {
                    Layout.preferredWidth: 260
                    Layout.fillHeight: true
                    color: "#2D1B33"
                    radius: 12
                    border.color: "#5c3d75"
                    border.width: 1
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 10

                        Text {
                            text: "Select a Book"
                            color: "#D4AF37"
                            font.bold: true
                            font.pixelSize: 16
                        }

                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true

                            ListView {
                                model: booksModel2
                                spacing: 6

                                delegate: Rectangle {
                                    width: ListView.view ? ListView.view.width : 220
                                    height: 40
                                    radius: 8
                                    color: commentsRoot.selectedBookId === model.bookId ? "#D4AF37" : "#1A0F1F"

                                    Text {
                                        anchors.fill: parent
                                        anchors.margins: 8
                                        text: model.title
                                        color: commentsRoot.selectedBookId === model.bookId ? "#1A0F1F" : "white"
                                        elide: Text.ElideRight
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: commentsRoot.selectBook(model.bookId, model.title)
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#2D1B33"
                    radius: 12
                    border.color: "#5c3d75"
                    border.width: 1
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        spacing: 10

                        Text {
                            text: commentsRoot.selectedBookId > 0
                                  ? "Comments on \"" + commentsRoot.selectedBookTitle + "\" (" + commentsModel.count + ")"
                                  : "Select a book to view its comments"
                            color: "#D4AF37"
                            font.bold: true
                            font.pixelSize: 16
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }

                        Text {
                            visible: commentsRoot.selectedBookId > 0 && commentsModel.count === 0
                            text: "No comments on this book."
                            color: "#A08EAD"
                            font.pixelSize: 13
                        }

                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true

                            ListView {
                                id: commentsList
                                model: commentsModel
                                spacing: 8

                                delegate: Rectangle {
                                    width: commentsList.width
                                    color: "#1A0F1F"
                                    radius: 8
                                    border.color: "#3d2545"
                                    border.width: 1
                                    implicitHeight: commentBody.implicitHeight + 24

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 12
                                        spacing: 10

                                        ColumnLayout {
                                            id: commentBody
                                            Layout.fillWidth: true
                                            spacing: 4

                                            RowLayout {
                                                spacing: 8
                                                Text {
                                                    text: model.username
                                                    color: "#D4AF37"
                                                    font.bold: true
                                                    font.pixelSize: 13
                                                }
                                                Text {
                                                    text: "★".repeat(model.rating) + "☆".repeat(5 - model.rating)
                                                    color: "#D4AF37"
                                                    font.pixelSize: 13
                                                }
                                            }

                                            Text {
                                                text: model.text
                                                color: "white"
                                                font.pixelSize: 13
                                                wrapMode: Text.WordWrap
                                                Layout.fillWidth: true
                                            }
                                        }

                                        Button {
                                            text: "Delete"
                                            background: Rectangle { color: "#8B2C2C"; radius: 6 }
                                            contentItem: Text {
                                                text: parent.text
                                                color: "white"
                                                font.pixelSize: 12
                                                horizontalAlignment: Text.AlignHCenter
                                            }
                                            onClicked: {
                                                if (networkManager && model.commentId > 0)
                                                    networkManager.adminRemoveComment(model.commentId)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}