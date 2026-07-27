import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1

Rectangle {
    id: publisherRoot
    width: parent ? parent.width : 1280
    height: parent ? parent.height : 720
    color: "#2D1B33"

    // ========== PROPERTIES ==========
    property int userId: 0
    property string username: ""
    property string userRole: ""
    property string publisherPassword: ""
    property string favoriteAuthor: ""

    property real totalRevenue: 0.0
    property int totalBooksPublished: 0
    property int totalActiveBooks: 0

    property bool isEditing: false
    property int editingBookId: -1

    property string coverImagePath: ""
    property string pdfFilePath: ""
    property double pendingDiscount: 0.0

    property string successMessageText: ""
    property bool showSuccess: false
    property bool showError: false

    // ===== فلگ‌های جدید برای تشخیص تغییر فایل =====
    property bool isCoverChanged: false
    property bool isPdfChanged: false

    // ========== MODELS ==========
    ListModel {
        id: booksModel
    }

    // ========== FILE DIALOGS ==========
    FileDialog {
        id: coverFileDialog
        title: "Select Book Cover Image"
        nameFilters: ["Image files (*.jpg *.png *.jpeg)"]
        onAccepted: {
            coverImagePath = coverFileDialog.file
            coverLabel.text = coverFileDialog.file.toString().split('/').pop()
            isCoverChanged = true
        }
    }

    FileDialog {
        id: pdfFileDialog
        title: "Select Book PDF File"
        nameFilters: ["PDF files (*.pdf)"]
        onAccepted: {
            pdfFilePath = pdfFileDialog.file
            pdfLabel.text = pdfFileDialog.file.toString().split('/').pop()
            isPdfChanged = true
        }
    }

    // ========== NETWORK CONNECTIONS ==========
    Connections {
        target: networkManager

        function onResponseReceived(action, status, data) {
            var statusUpper = status ? status.toUpperCase() : ""

            if (action === "getAllBooks" || action === "getPublisherBooks") {
                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    booksModel.clear()

                    var booksArray = []
                    if (Array.isArray(data)) {
                        booksArray = data
                    } else if (data.books) {
                        booksArray = data.books
                    } else if (data.data) {
                        booksArray = data.data
                    }

                    totalBooksPublished = booksArray.length || 0
                    totalActiveBooks = 0
                    var rev = 0.0

                    for (var i = 0; i < booksArray.length; i++) {
                        var book = booksArray[i]
                        var isActive = book.isAvailable !== undefined ? book.isAvailable : true

                        if (isActive) totalActiveBooks++

                        booksModel.append({
                            bookId: book.id || book.bookId || 0,
                            title: book.title || "",
                            author: book.author || "",
                            genre: book.genre || "Unknown",
                            desc: book.description || "",
                            price: book.price || 0.0,
                            discount: book.discountValue || 0.0,
                            rating: book.rating || 0.0,
                            active: isActive,
                            sales: book.sales || book.salesCount || 0,
                            coverData: book.coverImageData || book.coverData || "",
                            coverImagePath: book.coverImagePath || book.coverPath || "",
                            pdfFilePath: book.pdfFilePath || book.pdfPath || ""
                        })
                        rev += (book.price * (book.sales || book.salesCount || 0))
                    }
                    totalRevenue = rev

                    showSuccessMessage("✅ " + totalBooksPublished + " books loaded")
                } else {
                    showErrorMessage("❌ Failed to load books: " + status)
                }
            }

            else if (action === "addBook") {
                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    var newBookId = data.id || data.bookId || 0

                    if (pendingDiscount > 0 && newBookId > 0) {
                        var now = Math.floor(Date.now() / 1000)
                        var oneYearLater = now + 365 * 24 * 60 * 60
                        networkManager.applyDiscountToBook(userId, newBookId, pendingDiscount, 0, now, oneYearLater)
                    }

                    pendingDiscount = 0
                    networkManager.getPublisherBooks(userId)
                    clearForm()
                    isEditing = false
                    mainStack.currentIndex = 1
                    showSuccessMessage("✅ Book published successfully!")
                } else {
                    showErrorMessage("❌ Failed to add book: " + status)
                }
            }

            else if (action === "updateBookDetails" || action === "updateBook") {
                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    networkManager.getPublisherBooks(userId)
                    clearForm()
                    isEditing = false
                    isCoverChanged = false
                    isPdfChanged = false
                    mainStack.currentIndex = 1
                    showSuccessMessage("✅ Book details updated successfully!")
                } else {
                    showErrorMessage("❌ Failed to update book: " + status)
                }
            }

            else if (action === "deactivateBook") {
                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    networkManager.getPublisherBooks(userId)
                    showSuccessMessage("✅ Book deactivated")
                } else {
                    showErrorMessage("❌ Failed to deactivate book: " + status)
                }
            }

            else if (action === "activateBook") {
                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    networkManager.getPublisherBooks(userId)
                    showSuccessMessage("✅ Book activated")
                } else {
                    showErrorMessage("❌ Failed to activate book: " + status)
                }
            }

            else if (action === "deleteBookPermanent") {
                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    networkManager.getPublisherBooks(userId)
                    showSuccessMessage("✅ Book permanently deleted")
                } else {
                    showErrorMessage("❌ Failed to delete book: " + status)
                }
            }

            else if (action === "updateBookPrice" || action === "applyDiscountToBook" || action === "removeDiscountFromBook") {
                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    networkManager.getPublisherBooks(userId)
                } else {
                    console.log(action, "FAILED. Server status:", status)
                }
            }

            else if (action === "updateProfile") {
                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    showSuccessMessage("✅ Profile updated successfully!")
                   console.log("SettingsView - userId:", userId)
                } else {
                    showErrorMessage("❌ Failed to update profile: " + status)
                }
            }
            else if (action === "changeUsername" || action === "updateSecurityAnswer" || action === "changePassword") {
                if (statusUpper === "SUCCESS" || statusUpper === "OK") {
                    if (action === "changeUsername") username = profUser.text.trim()
                    if (action === "updateSecurityAnswer") favoriteAuthor = profFavAuthor.text.trim()
                    showSuccessMessage("✅ " + (data.message || "Settings updated successfully!"))
                } else {
                    showErrorMessage("❌ " + (data.message || (action + " failed.")))
                }
            }
        }
    }

    // ========== COMPONENT LIFECYCLE ==========
    Component.onCompleted: {
        if (typeof networkManager !== "undefined" && networkManager !== null && userId > 0) {
            networkManager.getPublisherBooks(userId)
        }
        if (favoriteAuthor === "") {
            favoriteAuthor = "Not set"
        }
    }

    // ========== UI ==========
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ===== SIDEBAR =====
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 250
            color: "#1A0F1F"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                Image {
                    source: "qrc:/images/giraffe.png"
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 80
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: username !== "" ? username : "Publisher"
                    color: "#D4AF37"
                    font.bold: true
                    font.pixelSize: 18
                    Layout.alignment: Qt.AlignHCenter
                }

                Rectangle {
                    Layout.preferredHeight: 2
                    Layout.fillWidth: true
                    color: "#3D2B43"
                }

                Repeater {
                    model: ["Statistics", "My Books", "Add New Book", "Account Settings"]
                    delegate: Button {
                        required property int index
                        required property string modelData
                        text: modelData
                        Layout.fillWidth: true
                        flat: true
                        onClicked: {
                            if (index === 2) {
                                isEditing = false;
                                clearForm();
                            }
                            mainStack.currentIndex = index;
                        }
                        contentItem: Text {
                            text: parent.text
                            color: (mainStack.currentIndex === parent.index) ? "#D4AF37" : "#FFFFFF"
                            horizontalAlignment: Text.AlignLeft
                            font.pixelSize: 16
                        }
                    }
                }

                Item { Layout.fillHeight: true }

                Button {
                    text: "Logout"
                    Layout.fillWidth: true
                    onClicked: {
                        if (typeof rootStackView !== "undefined" && rootStackView !== null) {
                            rootStackView.pop()
                        }
                    }
                }
            }
        }

        // ===== MAIN CONTENT =====
        StackLayout {
            id: mainStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: 0

            // ===== TAB 0: STATISTICS =====
            ScrollView {
                contentWidth: availableWidth
                ColumnLayout {
                    width: parent ? parent.width - 40 : 800
                    spacing: 30
                    Layout.margins: 20

                    Text {
                        text: "Publisher Dashboard Statistics"
                        color: "#D4AF37"
                        font.pixelSize: 28
                        font.bold: true
                    }

                    RowLayout {
                        spacing: 20
                        StatCard {
                            title: "Total Revenue"
                            value: "$" + totalRevenue.toFixed(2)
                            icon: "💰"
                        }
                        StatCard {
                            title: "Total Published Books"
                            value: totalBooksPublished.toString()
                            icon: "📚"
                        }
                        StatCard {
                            title: "Active Books"
                            value: totalActiveBooks.toString()
                            icon: "✅"
                        }
                    }

                    Text {
                        text: "Best Selling Books (Sales Count)"
                        color: "#FFF"
                        font.pixelSize: 20
                        Layout.topMargin: 20
                    }

                    RowLayout {
                        height: 200
                        spacing: 40
                        Layout.leftMargin: 20
                        Repeater {
                            model: booksModel
                            Rectangle {
                                width: 50
                                height: Math.max(20, (model.sales / 350) * 150)
                                color: model.active ? "#D4AF37" : "#666666"
                                Layout.alignment: Qt.AlignBottom
                                Text {
                                    text: model.sales + " sales"
                                    color: "#FFF"
                                    font.pixelSize: 11
                                    anchors.bottom: parent.top
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                                Text {
                                    text: model.title
                                    color: "#FFF"
                                    font.pixelSize: 10
                                    anchors.top: parent.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    wrapMode: Text.Wrap
                                    width: 80
                                    horizontalAlignment: Text.AlignHCenter
                                }
                            }
                        }
                    }
                }
            }

            // ===== TAB 1: MY BOOKS =====
            ListView {
                id: booksListView
                model: booksModel
                clip: true

                header: ColumnLayout {
                    width: booksListView.width
                    spacing: 10

                    Text {
                        text: "Manage My Published Books (" + totalBooksPublished + " total)"
                        color: "#D4AF37"
                        font.pixelSize: 24
                        padding: 20
                        Layout.fillWidth: true
                    }

                    Rectangle {
                        visible: showSuccess
                        Layout.fillWidth: true
                        Layout.margins: 10
                        height: 40
                        radius: 5
                        color: "#2E7D32"

                        Text {
                            anchors.centerIn: parent
                            text: successMessageText
                            color: "white"
                            font.pixelSize: 14
                            font.bold: true
                        }

                        Timer {
                            running: showSuccess
                            interval: 3000
                            onTriggered: showSuccess = false
                        }
                    }

                    Rectangle {
                        visible: showError
                        Layout.fillWidth: true
                        Layout.margins: 10
                        height: 40
                        radius: 5
                        color: "#C62828"

                        Text {
                            anchors.centerIn: parent
                            text: successMessageText
                            color: "white"
                            font.pixelSize: 14
                            font.bold: true
                        }

                        Timer {
                            running: showError
                            interval: 3000
                            onTriggered: showError = false
                        }
                    }
                }

                delegate: Rectangle {
                    width: booksListView.width - 40
                    height: 130
                    color: model.active ? "#3D2B43" : "#2A1A30"
                    radius: 10
                    x: 20
                    anchors.margins: 10
                    opacity: model.active ? 1.0 : 0.7

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 15

                        Rectangle {
                            width: 75
                            height: 100
                            color: "#555"
                            radius: 5
                            clip: true

                            Image {
                                id: coverImage
                                anchors.fill: parent
                                source: model.coverData !== "" ? model.coverData : (model.coverImagePath !== "" ? "file:///" + model.coverImagePath : "")
                                fillMode: Image.PreserveAspectCrop
                                visible: source !== ""
                            }

                            Text {
                                anchors.centerIn: parent
                                text: "📖"
                                font.pixelSize: 35
                                visible: model.coverData === "" && model.coverImagePath === ""
                            }
                        }

                        ColumnLayout {
                            spacing: 3
                            Layout.fillWidth: true

                            RowLayout {
                                spacing: 10
                                Text {
                                    text: model.title
                                    color: "#FFF"
                                    font.bold: true
                                    font.pixelSize: 16
                                }
                                Text {
                                    text: model.active ? "● Active" : "● Deactivated"
                                    color: model.active ? "#44FF44" : "#FF4444"
                                    font.pixelSize: 11
                                    font.bold: true
                                }
                            }

                            Text {
                                text: "Author: " + model.author + " | Genre: " + model.genre
                                color: "#BBB"
                                font.pixelSize: 12
                            }

                            Text {
                                text: "Description: " + (model.desc.length > 60 ? model.desc.substring(0, 60) + "..." : model.desc)
                                color: "#999"
                                font.pixelSize: 11
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }

                            RowLayout {
                                spacing: 15
                                Text {
                                    text: "Price: $" + model.price
                                    color: "#D4AF37"
                                    font.pixelSize: 13
                                }
                                Text {
                                    text: "Discount: " + model.discount + "%"
                                    color: "#FF5252"
                                    font.pixelSize: 13
                                    visible: model.discount > 0
                                }
                                Text {
                                    text: "⭐ " + model.rating
                                    color: "#FFD700"
                                    font.pixelSize: 13
                                }
                                Text {
                                    text: "Sales: " + model.sales
                                    color: "#AAA"
                                    font.pixelSize: 12
                                }
                            }
                        }

                        Item { Layout.fillWidth: true }

                        ColumnLayout {
                            spacing: 8

                            Button {
                                text: "Edit"
                                Layout.preferredWidth: 100
                                Layout.preferredHeight: 32
                                background: Rectangle {
                                    color: "#D4AF37"
                                    radius: 5
                                }
                                contentItem: Text {
                                    text: "Edit"
                                    color: "#1A0F1F"
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    isEditing = true;
                                    editingBookId = model.bookId;
                                    titleIn.text = model.title;
                                    authorIn.text = model.author;
                                    var genreIndex = genreIn.find(model.genre);
                                    genreIn.currentIndex = genreIndex !== -1 ? genreIndex : 0;
                                    descIn.text = model.desc;
                                    priceIn.text = model.price.toString();
                                    discountIn.text = model.discount.toString();

                                    coverImagePath = model.coverImagePath || "";
                                    pdfFilePath = model.pdfFilePath || "";

                                    isCoverChanged = false;
                                    isPdfChanged = false;

                                    coverLabel.text = model.coverImagePath ? "Current: " + model.coverImagePath.split('/').pop() : "No cover";
                                    pdfLabel.text = model.pdfFilePath ? "Current: " + model.pdfFilePath.split('/').pop() : "No PDF";
                                    mainStack.currentIndex = 2;
                                }
                            }

                            Button {
                                text: model.active ? "Deactivate" : "Activate"
                                Layout.preferredWidth: 100
                                Layout.preferredHeight: 32
                                background: Rectangle {
                                    color: model.active ? "#FF6B35" : "#44BB44"
                                    radius: 5
                                }
                                contentItem: Text {
                                    text: parent.text
                                    color: "white"
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: {
                                    if (typeof networkManager === "undefined") return;

                                    if (model.active) {
                                        networkManager.deactivateBook(userId, model.bookId);
                                    } else {
                                        networkManager.activateBook(userId, model.bookId);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // ===== TAB 2: ADD NEW BOOK =====
            ScrollView {
                contentWidth: availableWidth
                ColumnLayout {
                    width: parent ? parent.width - 100 : 800
                    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                    spacing: 15

                    Text {
                        text: isEditing ? "Edit Book Details" : "Publish a New Book"
                        color: "#D4AF37"
                        font.pixelSize: 24
                        Layout.topMargin: 20
                    }

                    CustomTextField { id: titleIn; placeholder: "Book Title"; Layout.fillWidth: true }
                    CustomTextField { id: authorIn; placeholder: "Author Name"; Layout.fillWidth: true }

                    ComboBox {
                        id: genreIn
                        model: ["Fiction", "NonFiction", "Mystery", "Romance", "SciFi", "Fantasy",
                                "Biography", "History", "SelfHelp", "Poetry", "Children", "Other"]
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: "#3D2B43"
                            border.color: genreIn.activeFocus ? "#D4AF37" : "#555"
                            border.width: 2
                            radius: 5
                        }
                        contentItem: Text {
                            text: genreIn.displayText
                            color: "white"
                            font.pixelSize: 14
                            leftPadding: 10
                            verticalAlignment: Text.AlignVCenter
                        }
                        indicator: Text {
                            text: "▼"
                            color: "#D4AF37"
                            font.pixelSize: 16
                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    TextArea {
                        id: descIn
                        placeholderText: "Description..."
                        Layout.fillWidth: true
                        Layout.preferredHeight: 100
                        background: Rectangle {
                            color: "#3D2B43"
                            border.color: descIn.activeFocus ? "#D4AF37" : "#555"
                            border.width: 2
                            radius: 5
                        }
                        color: "white"
                        placeholderTextColor: "#888"
                    }

                    RowLayout {
                        spacing: 15
                        Layout.fillWidth: true

                        CustomTextField {
                            id: priceIn
                            placeholder: "Price ($)"
                            Layout.fillWidth: true
                            validator: DoubleValidator { bottom: 0; decimals: 2 }
                        }

                        CustomTextField {
                            id: discountIn
                            placeholder: "Discount (%)"
                            Layout.fillWidth: true
                            validator: DoubleValidator { bottom: 0; top: 100; decimals: 2 }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Button {
                            text: "Upload Cover Image"
                            Layout.preferredHeight: 35
                            background: Rectangle {
                                color: "#D4AF37"
                                radius: 5
                            }
                            contentItem: Text {
                                text: parent.text
                                color: "#1A0F1F"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            onClicked: coverFileDialog.open()
                        }
                        Text {
                            id: coverLabel
                            text: "No cover selected"
                            color: "#BBB"
                            font.pixelSize: 12
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Button {
                            text: "Upload PDF File"
                            Layout.preferredHeight: 35
                            background: Rectangle {
                                color: "#D4AF37"
                                radius: 5
                            }
                            contentItem: Text {
                                text: parent.text
                                color: "#1A0F1F"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            onClicked: pdfFileDialog.open()
                        }
                        Text {
                            id: pdfLabel
                            text: "No PDF selected"
                            color: "#BBB"
                            font.pixelSize: 12
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }
                    }

                    Button {
                        text: isEditing ? "SAVE CHANGES" : "PUBLISH BOOK"
                        highlighted: true
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        background: Rectangle {
                            color: "#D4AF37"
                            radius: 5
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#1A0F1F"
                            font.bold: true
                            font.pixelSize: 16
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            if (typeof networkManager === "undefined") return;

                            var priceVal = parseFloat(priceIn.text) || 0.0
                            var discountVal = parseFloat(discountIn.text) || 0.0
                            var now = Math.floor(Date.now() / 1000)
                            var oneYearLater = now + 365 * 24 * 60 * 60

                            if (isEditing) {
                                var finalCoverPath = isCoverChanged ? coverImagePath : ""
                                var finalPdfPath = isPdfChanged ? pdfFilePath : ""

                                networkManager.updateBookDetails(
                                    editingBookId,
                                    titleIn.text,
                                    authorIn.text,
                                    priceVal,
                                    genreIn.currentText,
                                    descIn.text,
                                    finalCoverPath,
                                    finalPdfPath,
                                    "publisher",
                                    userId
                                );

                                if (discountVal > 0) {
                                    networkManager.applyDiscountToBook(userId, editingBookId, discountVal, 0, now, oneYearLater);
                                } else {
                                    networkManager.removeDiscountFromBook(userId, editingBookId);
                                }
                            } else {
                                pendingDiscount = discountVal;
                                networkManager.addBook(
                                    userId,
                                    titleIn.text,
                                    authorIn.text,
                                    priceVal,
                                    genreIn.currentText,
                                    descIn.text,
                                    coverImagePath,
                                    pdfFilePath
                                );
                            }
                        }
                    }
                }
            }

            // ===== TAB 3: ACCOUNT SETTINGS =====
            ScrollView {
                contentWidth: availableWidth
                ColumnLayout {
                    width: parent ? parent.width - 100 : 800
                    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
                    spacing: 15
                    Layout.topMargin: 20

                    Text {
                        text: "Account Settings"
                        color: "#D4AF37"
                        font.pixelSize: 24
                        font.bold: true
                        Layout.alignment: Qt.AlignHCenter
                    }

                    ColumnLayout {
                        spacing: 5
                        Layout.fillWidth: true
                        Text {
                            text: "Username:"
                            color: "#BBB"
                            font.pixelSize: 14
                        }
                        CustomTextField {
                            id: profUser
                            text: username
                            Layout.fillWidth: true
                        }
                    }

                    ColumnLayout {
                        spacing: 5
                        Layout.fillWidth: true
                        Text {
                            text: "Favorite Author:"
                            color: "#BBB"
                            font.pixelSize: 14
                        }
                        CustomTextField {
                            id: profFavAuthor
                            text: favoriteAuthor
                            Layout.fillWidth: true
                            placeholder: "Enter your favorite author"
                        }
                    }

                    Rectangle {
                        Layout.preferredHeight: 1
                        Layout.fillWidth: true
                        color: "#3D2B43"
                        Layout.topMargin: 10
                        Layout.bottomMargin: 10
                    }

                    Text {
                        text: "Change Password"
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                        Layout.alignment: Qt.AlignHCenter
                    }

                    ColumnLayout {
                        spacing: 5
                        Layout.fillWidth: true
                        Text {
                            text: "Current Password:"
                            color: "#BBB"
                            font.pixelSize: 14
                        }
                        CustomTextField {
                            id: currentPassIn
                            echoMode: TextInput.Password
                            placeholder: "Enter current password to authorize changes"
                            Layout.fillWidth: true
                        }
                    }

                    ColumnLayout {
                        spacing: 5
                        Layout.fillWidth: true
                        Text {
                            text: "New Password:"
                            color: "#BBB"
                            font.pixelSize: 14
                        }
                        CustomTextField {
                            id: newPassIn
                            echoMode: TextInput.Password
                            placeholder: "Leave blank to keep current"
                            Layout.fillWidth: true
                        }
                    }

                    Text {
                        id: errorMsg
                        color: "#FF4444"
                        font.pixelSize: 12
                        visible: false
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Button {
                        text: "SAVE ALL CHANGES"
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45
                        background: Rectangle {
                            color: "#D4AF37"
                            radius: 5
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#1A0F1F"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        onClicked: {
                            if (typeof networkManager === "undefined") return;

                            errorMsg.visible = false;

                            var wantsUsernameChange = profUser.text.trim() !== "" && profUser.text !== username;
                            var wantsFavAuthorChange = profFavAuthor.text.trim() !== "" && profFavAuthor.text !== favoriteAuthor;
                            var wantsPasswordChange = newPassIn.text !== "";

                            if ((wantsUsernameChange || wantsFavAuthorChange || wantsPasswordChange) && currentPassIn.text === "") {
                                errorMsg.text = "Error: Enter current password to save any changes!";
                                errorMsg.color = "#FF4444";
                                errorMsg.visible = true;
                                return;
                            }

                            if (wantsUsernameChange) {
                                networkManager.changeUsername(userId, profUser.text.trim(), currentPassIn.text, "publisher");
                            }

                            if (wantsFavAuthorChange) {
                                networkManager.updateSecurityAnswer(userId, profFavAuthor.text.trim(), currentPassIn.text, "publisher");
                            }

                            if (wantsPasswordChange) {
                                networkManager.changePassword(userId, currentPassIn.text, newPassIn.text, "publisher");
                            }

                            currentPassIn.text = "";
                            newPassIn.text = "";
                        }
                    }
                }
            }
        }
    }

    // ========== HELPER FUNCTIONS ==========
    function clearForm() {
        titleIn.text = ""
        authorIn.text = ""
        genreIn.currentIndex = 0
        descIn.text = ""
        priceIn.text = ""
        discountIn.text = ""
        coverImagePath = ""
        pdfFilePath = ""
        coverLabel.text = "No cover selected"
        pdfLabel.text = "No PDF selected"
        pendingDiscount = 0
        isCoverChanged = false
        isPdfChanged = false
    }

    function showSuccessMessage(msg) {
        successMessageText = msg
        showSuccess = true
        showError = false
    }

    function showErrorMessage(msg) {
        successMessageText = msg
        showError = true
        showSuccess = false
    }

    // ========== COMPONENTS ==========
    component StatCard : Rectangle {
        property string title: ""
        property string value: ""
        property string icon: ""
        Layout.preferredWidth: 250
        Layout.preferredHeight: 100
        color: "#3D2B43"
        radius: 10
        border.color: "#D4AF37"
        border.width: 1

        ColumnLayout {
            anchors.centerIn: parent
            Text {
                text: icon + " " + title
                color: "#BBB"
                font.pixelSize: 14
            }
            Text {
                text: value
                color: "#D4AF37"
                font.pixelSize: 22
                font.bold: true
            }
        }
    }

    component CustomTextField : TextField {
        id: tf
        property string placeholder: ""
        placeholderText: placeholder
        color: "white"
        placeholderTextColor: "#888"
        background: Rectangle {
            color: "#3D2B43"
            border.color: tf.activeFocus ? "#D4AF37" : "#555"
            border.width: 2
            radius: 5
        }
    }
}
