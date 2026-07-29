import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: settingsRoot

    required property int userId
    required property string username
    required property string userRole
    required property var networkManager
    required property var booksById


    property var userGenres: []
    property bool loading: false
    property string statusMessage: ""
    property color statusColor: "#D4AF37"
    property string favoriteAuthor: ""

    property var _orderHistoryData: []

    ListModel { id: purchaseHistoryModel }

    function showMessage(message, isError) {
        statusMessage = message || ""
        statusColor = isError ? "#FF6666" : "#66FF99"
    }

    function loadData() {
        if (!networkManager)
            return

        loading = true

        if (!booksById || Object.keys(booksById).length === 0) {
            networkManager.getAllBooks()
        }

        networkManager.getOrderHistory(userId)
    }

    function resolveBookTitle(bookId) {
        var book = booksById[bookId]
        if (book && book.title)
            return book.title
        return "(book #" + bookId + ")"
    }

    function refreshPurchaseHistory() {
        purchaseHistoryModel.clear()

        if (!_orderHistoryData || _orderHistoryData.length === 0)
            return

        for (var i = 0; i < _orderHistoryData.length; i++) {
            var order = _orderHistoryData[i]

            var dateObj = new Date(order.orderDate * 1000)
            var dateStr = dateObj.toLocaleDateString()

            var entries = order.bookIds || order.books || []
            var titles = []
            for (var j = 0; j < entries.length; j++) {
                var entry = entries[j]
                var bookId = (typeof entry === "object" && entry !== null) ? entry.bookId : entry
                titles.push(resolveBookTitle(bookId))
            }

            purchaseHistoryModel.append({
                "orderId": order.id,
                "date": dateStr,
                "bookList": titles.join(", "),
                "rawPrice": order.rawPrice || 0,
                "discountAmount": order.discountAmount || 0,
                "finalPrice": order.finalPrice || 0
            })
        }
    }

    onBooksByIdChanged: {
        if (_orderHistoryData.length > 0)
            refreshPurchaseHistory()
    }

    function openGenreSelection() {
        var targetStack = settingsRoot.StackView ? settingsRoot.StackView.view : null
        if (!targetStack) {
            showMessage("StackView not found", true)
            return
        }

        var page = targetStack.push("GenreSelection.qml", {
            "username": settingsRoot.username,
            "userRole": settingsRoot.userRole,
            "userId": settingsRoot.userId,
            "isEditMode": true,
            "initialGenres": settingsRoot.userGenres,
            "networkManager": settingsRoot.networkManager
        })

        if (page && page.genresSaved) {
            page.genresSaved.connect(function(genres) {
                settingsRoot.userGenres = genres
            })
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
    }

    Connections {
        target: networkManager

        function onResponseReceived(action, status, data) {
            var ok = status === "success" || status === "SUCCESS"
            loading = false

            if (action === "changeUsername") {
                if (ok) {
                    settingsRoot.username = usernameField.text.trim()
                    showMessage("Username updated successfully", false)
                    usernamePasswordField.text = ""
                } else {
                    showMessage(data && data.message ? data.message : "Failed to update username", true)
                }
            }
            else if (action === "changePassword") {
                if (ok) {
                    showMessage("Password changed successfully", false)
                    currentPasswordField.text = ""
                    newPasswordField.text = ""
                    confirmPasswordField.text = ""
                } else {
                    showMessage(data && data.message ? data.message : "Failed to change password", true)
                }
            }
            else if (action === "updateProfile") {
                showMessage(
                    ok ? "Favorite genres updated"
                       : (data && data.message ? data.message : "Failed to update genres"),
                    !ok
                )
            }
            else if (action === "updateSecurityAnswer") {
                if (ok) {
                    settingsRoot.favoriteAuthor = favoriteAuthorField.text.trim()
                    favoriteAuthorPasswordField.text = ""
                    showMessage("Favorite author updated successfully", false)
                } else {
                    showMessage(
                        data && data.message ? data.message : "Failed to update favorite author. Check your password.",
                        true
                    )
                }
            }
            else if (action === "getOrderHistory") {
                if (ok) {
                    settingsRoot._orderHistoryData = data || []
                    refreshPurchaseHistory()
                } else {
                    settingsRoot._orderHistoryData = []
                    purchaseHistoryModel.clear()
                    showMessage(data && data.message ? data.message : "Failed to load purchase history", true)
                }
            }
            else if (action === "getAllBooks" && ok) {
                var map = {}
                if (Array.isArray(data)) {
                    for (var i = 0; i < data.length; i++) {
                        var book = data[i]
                        if (book && book.id) {
                            map[book.id] = book
                        }
                    }
                }
                settingsRoot.booksById = map

                if (settingsRoot._orderHistoryData.length > 0) {
                    refreshPurchaseHistory()
                }
            }
        }

        function onErrorOccurred(message) {
            loading = false
            showMessage(message, true)
        }
    }

    ScrollView {
        id: settingsScroll
        anchors.fill: parent
        clip: true

        contentWidth: availableWidth
        contentHeight: settingsContent.implicitHeight

        ColumnLayout {
            id: settingsContent
            width: settingsScroll.availableWidth
            spacing: 20

            Item {
                Layout.preferredHeight: 10
            }

            Text {
                text: "Settings"
                color: "#D4AF37"
                font.pixelSize: 28
                font.bold: true
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
            }

            Text {
                text: statusMessage
                visible: statusMessage.length > 0
                color: statusColor
                font.pixelSize: 13
                wrapMode: Text.Wrap
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.preferredHeight: accountCol.implicitHeight + 40
                color: "#2D1B33"
                radius: 12
                border.color: "#D4AF37"
                border.width: 1

                ColumnLayout {
                    id: accountCol
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Text {
                        text: "Account Information"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: "Role: " + settingsRoot.userRole
                        color: "#D4AF37"
                        opacity: 0.7
                        font.pixelSize: 14
                    }

                    Text {
                        text: "User ID: " + settingsRoot.userId
                        color: "#D4AF37"
                        opacity: 0.7
                        font.pixelSize: 14
                    }

                    Text {
                        text: "Username"
                        color: "#D4AF37"
                        font.pixelSize: 13
                        font.bold: true
                    }

                    TextField {
                        id: usernameField
                        Layout.fillWidth: true
                        implicitHeight: 45
                        text: settingsRoot.username
                        color: "#D4AF37"
                        leftPadding: 12
                        selectedTextColor: "#1A0F1F"
                        selectionColor: "#D4AF37"

                        background: Rectangle {
                            color: "#1A0F1F"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                        }
                    }

                    Text {
                        text: "Confirm With Password"
                        color: "#D4AF37"
                        font.pixelSize: 13
                        font.bold: true
                    }

                    TextField {
                        id: usernamePasswordField
                        Layout.fillWidth: true
                        implicitHeight: 45
                        placeholderText: "Enter your password"
                        echoMode: TextInput.Password
                        color: "#D4AF37"
                        leftPadding: 12
                        selectedTextColor: "#1A0F1F"
                        selectionColor: "#D4AF37"

                        background: Rectangle {
                            color: "#1A0F1F"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                        }
                    }

                    Button {
                        text: "Update Username"
                        Layout.alignment: Qt.AlignRight
                        Layout.preferredHeight: 42
                        enabled: !loading

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
                            if (!usernameField.text.trim()) {
                                showMessage("Username cannot be empty", true)
                                return
                            }

                            if (!usernamePasswordField.text) {
                                showMessage("Password is required to change username", true)
                                return
                            }

                            loading = true
                            networkManager.changeUsername(
                                settingsRoot.userId,
                                usernameField.text.trim(),
                                usernamePasswordField.text,
                                settingsRoot.userRole
                            )
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.preferredHeight: authorCol.implicitHeight + 40
                color: "#2D1B33"
                radius: 12
                border.color: "#D4AF37"
                border.width: 1

                ColumnLayout {
                    id: authorCol
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Text {
                        text: "Favorite Author"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Text {
                        text: "Set your favorite author (also used as your security answer)"
                        color: "#D4AF37"
                        opacity: 0.7
                        font.pixelSize: 13
                    }

                    TextField {
                        id: favoriteAuthorField
                        Layout.fillWidth: true
                        implicitHeight: 45
                        placeholderText: "Enter favorite author"
                        text: settingsRoot.favoriteAuthor
                        color: "#D4AF37"
                        leftPadding: 12
                        selectedTextColor: "#1A0F1F"
                        selectionColor: "#D4AF37"

                        background: Rectangle {
                            color: "#1A0F1F"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                        }
                    }

                    Text {
                        text: "Confirm With Password"
                        color: "#D4AF37"
                        font.pixelSize: 13
                        font.bold: true
                    }

                    TextField {
                        id: favoriteAuthorPasswordField
                        Layout.fillWidth: true
                        implicitHeight: 45
                        placeholderText: "Enter your password"
                        echoMode: TextInput.Password
                        color: "#D4AF37"
                        leftPadding: 12
                        selectedTextColor: "#1A0F1F"
                        selectionColor: "#D4AF37"

                        background: Rectangle {
                            color: "#1A0F1F"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                        }
                    }

                    Button {
                        text: "Save Favorite Author"
                        Layout.alignment: Qt.AlignRight
                        Layout.preferredHeight: 42
                        enabled: !loading

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
                            var authorName = favoriteAuthorField.text.trim()

                            if (!authorName) {
                                showMessage("Favorite author cannot be empty", true)
                                return
                            }

                            if (!favoriteAuthorPasswordField.text) {
                                showMessage("Password is required to change the favorite author", true)
                                return
                            }

                            loading = true
                            networkManager.updateSecurityAnswer(
                                settingsRoot.userId,
                                authorName,
                                favoriteAuthorPasswordField.text,
                                settingsRoot.userRole
                            )
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.preferredHeight: passCol.implicitHeight + 40
                color: "#2D1B33"
                radius: 12
                border.color: "#D4AF37"
                border.width: 1

                ColumnLayout {
                    id: passCol
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Text {
                        text: "Change Password"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    TextField {
                        id: currentPasswordField
                        Layout.fillWidth: true
                        implicitHeight: 45
                        placeholderText: "Current password"
                        echoMode: TextInput.Password
                        color: "#D4AF37"
                        leftPadding: 12
                        selectedTextColor: "#1A0F1F"
                        selectionColor: "#D4AF37"

                        background: Rectangle {
                            color: "#1A0F1F"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                        }
                    }

                    TextField {
                        id: newPasswordField
                        Layout.fillWidth: true
                        implicitHeight: 45
                        placeholderText: "New password"
                        echoMode: TextInput.Password
                        color: "#D4AF37"
                        leftPadding: 12
                        selectedTextColor: "#1A0F1F"
                        selectionColor: "#D4AF37"

                        background: Rectangle {
                            color: "#1A0F1F"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                        }
                    }

                    TextField {
                        id: confirmPasswordField
                        Layout.fillWidth: true
                        implicitHeight: 45
                        placeholderText: "Confirm new password"
                        echoMode: TextInput.Password
                        color: "#D4AF37"
                        leftPadding: 12
                        selectedTextColor: "#1A0F1F"
                        selectionColor: "#D4AF37"

                        background: Rectangle {
                            color: "#1A0F1F"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                        }
                    }

                    Button {
                        text: "Update Password"
                        Layout.alignment: Qt.AlignRight
                        Layout.preferredHeight: 42
                        enabled: !loading

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
                            if (!currentPasswordField.text || !newPasswordField.text || !confirmPasswordField.text) {
                                showMessage("Please fill all password fields", true)
                                return
                            }

                            if (newPasswordField.text !== confirmPasswordField.text) {
                                showMessage("Passwords do not match", true)
                                return
                            }

                            loading = true
                            networkManager.changePassword(
                                settingsRoot.userId,
                                currentPasswordField.text,
                                newPasswordField.text,
                                settingsRoot.userRole
                            )
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.preferredHeight: prefCol.implicitHeight + 40
                color: "#2D1B33"
                radius: 12
                border.color: "#D4AF37"
                border.width: 1

                ColumnLayout {
                    id: prefCol
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    Text {
                        text: "Favorite Genres"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                    }

                    Flow {
                        Layout.fillWidth: true
                        spacing: 8

                        Repeater {
                            model: settingsRoot.userGenres

                            delegate: Rectangle {
                                height: 30
                                width: genreLabel.implicitWidth + 24
                                radius: 14
                                color: "#1A0F1F"
                                border.color: "#D4AF37"
                                border.width: 1

                                Text {
                                    id: genreLabel
                                    anchors.centerIn: parent
                                    text: modelData
                                    color: "#D4AF37"
                                }
                            }
                        }
                    }

                    Text {
                        visible: settingsRoot.userGenres.length === 0
                        text: "No favorite genres selected yet."
                        color: "#D4AF37"
                        opacity: 0.6
                    }

                    Button {
                        text: "Change Genres"
                        Layout.alignment: Qt.AlignRight
                        Layout.preferredHeight: 42
                        enabled: !loading

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

                        onClicked: openGenreSelection()
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.preferredHeight: historyCol.implicitHeight + 40
                color: "#2D1B33"
                radius: 12
                border.color: "#D4AF37"
                border.width: 1

                ColumnLayout {
                    id: historyCol
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 15

                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: "Purchase History"
                            color: "#D4AF37"
                            font.pixelSize: 18
                            font.bold: true
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Text {
                            text: "Orders: " + purchaseHistoryModel.count
                            color: "#D4AF37"
                            font.pixelSize: 13
                        }
                    }

                    Repeater {
                        model: purchaseHistoryModel

                        delegate: Rectangle {
                            Layout.fillWidth: true
                            implicitHeight: orderCol.implicitHeight + 24
                            color: "#1A0F1F"
                            radius: 8
                            border.color: "#D4AF37"
                            border.width: 0.5

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 12

                                ColumnLayout {
                                    id: orderCol
                                    Layout.fillWidth: true
                                    spacing: 2

                                    Text {
                                        text: "Order #" + model.orderId + " — " + model.date
                                        color: "#D4AF37"
                                        font.bold: true
                                        font.pixelSize: 13
                                    }

                                    Text {
                                        text: model.bookList
                                        color: "#A08EAD"
                                        font.pixelSize: 12
                                        wrapMode: Text.WordWrap
                                        Layout.fillWidth: true
                                    }
                                }

                                ColumnLayout {
                                    spacing: 2
                                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

                                    Text {
                                        visible: model.discountAmount > 0
                                        text: "$" + model.rawPrice.toFixed(2)
                                        color: "#A08EAD"
                                        font.pixelSize: 11
                                        font.strikeout: true
                                        Layout.alignment: Qt.AlignRight
                                    }

                                    Text {
                                        text: "$" + model.finalPrice.toFixed(2)
                                        color: "#66FF99"
                                        font.bold: true
                                        font.pixelSize: 14
                                        Layout.alignment: Qt.AlignRight
                                    }
                                }
                            }
                        }
                    }

                    Text {
                        visible: purchaseHistoryModel.count === 0
                        text: "No purchases yet."
                        color: "#D4AF37"
                        opacity: 0.6
                    }
                }
            }

            Item {
                Layout.preferredHeight: 20
            }
        }
    }

    Component.onCompleted: loadData()
}