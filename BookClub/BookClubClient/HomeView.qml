import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: homeRoot

    required property string username
    required property string userRole
    required property var networkManager
    property var rootStackView: null
signal bookSelected(int bookId)

    property int userId: 0
    property var userGenres: []
    property int cartItemCount: 0
    property var booksById: ({})
    property var allBooksList: []

    property string selectedGenre: ""
    property bool loading: true

    property int bookCardWidth: 150
    property int bookCardHeight: 220
    property int bookGridSpacing: 15
    property int maxBookColumns: 7

    readonly property var genreOrder: [
        "Fiction", "NonFiction", "Mystery", "Romance",
        "SciFi", "Fantasy", "Biography", "History",
        "SelfHelp", "Poetry", "Children", "Other"
    ]

    ListModel { id: recommendedModel }
    ListModel { id: popularModel }
    ListModel { id: newReleasesModel }
    ListModel { id: bestSellersModel }
    ListModel { id: freeBooksModel }
    ListModel { id: genreBooksModel }

    function effectivePrice(book) {
        var price = book.price || 0
        var discountAmount = book.discount || 0
        return Math.max(0, price - discountAmount)
    }

    function normalizeCoverImage(rawCover) {
        if (!rawCover || rawCover === "")
            return ""

        if (typeof rawCover === "string") {
            if (rawCover.startsWith("http://")
                    || rawCover.startsWith("https://")
                    || rawCover.startsWith("file:/")
                    || rawCover.startsWith("qrc:/")
                    || rawCover.startsWith("data:image/")) {
                return rawCover
            }

            return "data:image/png;base64," + rawCover
        }

        return ""
    }

    function makeBookRow(book) {
        var finalPrice = effectivePrice(book)
        return {
            "bookId": book.id || 0,
            "title": book.title || "Unknown",
            "author": book.author || "Unknown",
            "priceLabel": finalPrice === 0 ? "Free" : ("$" + finalPrice.toFixed(2)),
            "coverImage": normalizeCoverImage(book.coverImageData)
        }
    }

    function fillModel(model, rawList) {
        model.clear()
        if (!rawList)
            return

        for (var i = 0; i < rawList.length; i++) {
            if (rawList[i].isAvailable === false)
                continue
            model.append(makeBookRow(rawList[i]))
        }
    }

    function gridColumnCount(viewWidth) {
        var columns = Math.floor((viewWidth + bookGridSpacing) / (bookCardWidth + bookGridSpacing))
        columns = Math.max(1, columns)
        return Math.min(maxBookColumns, columns)
    }

    function gridRows(modelCount, columns) {
        if (columns <= 0)
            return 0
        return Math.ceil(modelCount / columns)
    }

    function gridHeight(modelCount, viewWidth) {
        var columns = gridColumnCount(viewWidth)
        var rows = gridRows(modelCount, columns)
        if (rows <= 0)
            return 0
        return rows * bookCardHeight + Math.max(0, rows - 1) * bookGridSpacing
    }

    function loadHomeData() {
        if (networkManager && userId > 0) {
            loading = true
            networkManager.getHomeData(userId)
        }
    }

    function loadGenreBooks(genreName) {
        selectedGenre = genreName
        var idx = genreOrder.indexOf(genreName)
        if (idx < 0 || !networkManager)
            return
        networkManager.getBooksByGenre(idx)
    }

    function openBook(bookId) {
        if (!bookId || bookId <= 0)
            return

        console.log("HomeView: emitting bookSelected", bookId)
        homeRoot.bookSelected(bookId)
    }


    Component.onCompleted: loadHomeData()

    Connections {
        target: networkManager

        function onResponseReceived(action, status, data) {
            var ok = status === "success" || status === "SUCCESS"

            if (action === "getHomeData") {
                loading = false
                if (!ok)
                    return

                fillModel(recommendedModel, data.recommendedBooks)
                fillModel(popularModel, data.popularBooks)
                fillModel(newReleasesModel, data.newReleases)
                fillModel(bestSellersModel, data.bestSellers)
                fillModel(freeBooksModel, data.freeBooks)
            } else if (action === "getBooksByGenre") {
                if (ok)
                    fillModel(genreBooksModel, data)
                else
                    genreBooksModel.clear()
            }
        }
    }

    Component {
        id: bookDelegate

        Rectangle {
            width: homeRoot.bookCardWidth
            height: homeRoot.bookCardHeight
            color: "#2D1B33"
            border.color: bookMouse.containsMouse ? "#D4AF37" : "#3D2545"
            border.width: 1
            radius: 10
            scale: bookMouse.containsMouse ? 1.04 : 1.0

            Behavior on scale {
                NumberAnimation {
                    duration: 120
                }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 6

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 110
                    color: "#1A0F1F"
                    radius: 6
                    clip: true

                    Image {
                        anchors.fill: parent
                        source: model.coverImage
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true
                        cache: false
                    }

                    Text {
                        anchors.centerIn: parent
                        visible: !model.coverImage || model.coverImage === ""
                        text: "No Cover"
                        color: "#A08EAD"
                        font.pixelSize: 12
                    }
                }

                Text {
                    text: model.title
                    color: "#D4AF37"
                    font.bold: true
                    font.pixelSize: 13
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: model.author
                    color: "#CCCCCC"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: model.priceLabel
                    color: model.priceLabel === "Free" ? "#4CAF50" : "#D4AF37"
                    font.bold: true
                    font.pixelSize: 13
                }
            }

            MouseArea {
                id: bookMouse
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    parent.color = "red"

                    homeRoot.openBook(model.bookId)
                }
            }
        }
    }

    Component {
        id: bookGrid

        GridView {
            width: parent ? parent.width : 0
            height: homeRoot.gridHeight(model ? model.count : 0, width)

            cellWidth: homeRoot.bookCardWidth + homeRoot.bookGridSpacing
            cellHeight: homeRoot.bookCardHeight + homeRoot.bookGridSpacing

            interactive: false
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            flow: GridView.FlowLeftToRight
            delegate: bookDelegate
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#1A0F1F"
    }

    Text {
        anchors.centerIn: parent
        text: "Loading books..."
        color: "#A08EAD"
        font.pixelSize: 16
        visible: homeRoot.loading
    }

    Text {
        anchors.centerIn: parent
        text: "No books have been published yet. Check back soon!"
        color: "#A08EAD"
        font.pixelSize: 16
        visible: !homeRoot.loading
                 && recommendedModel.count === 0
                 && popularModel.count === 0
                 && newReleasesModel.count === 0
                 && bestSellersModel.count === 0
                 && freeBooksModel.count === 0
    }

    ScrollView {
        id: homeScroll
        anchors.fill: parent
        visible: !homeRoot.loading
        clip: true

        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        contentHeight: contentColumn.implicitHeight

        background: Rectangle {
            color: "#1A0F1F"
        }

        Column {
            id: contentColumn
            width: homeScroll.availableWidth
            topPadding: 15
            bottomPadding: 40
            leftPadding: 20
            rightPadding: 20
            spacing: 25

            RowLayout {
                width: parent.width - 40

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
                        color: "#A08EAD"
                        font.pixelSize: 14
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                Rectangle {
                    radius: 15
                    color: "#2D1B33"
                    border.color: "#D4AF37"
                    border.width: 1
                    implicitWidth: roleText.implicitWidth + 24
                    implicitHeight: 30

                    Text {
                        id: roleText
                        anchors.centerIn: parent
                        text: homeRoot.userRole.toUpperCase()
                        color: "#D4AF37"
                        font.bold: true
                    }
                }
            }

            Column {
                width: parent.width - 40
                spacing: 10
                visible: recommendedModel.count > 0

                Text {
                    text: "Recommended For You"
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "Based on your favorite genres"
                    color: "#A08EAD"
                    font.pixelSize: 12
                }

                Loader {
                    width: parent.width
                    sourceComponent: bookGrid

                    onLoaded: {
                        item.model = recommendedModel
                        item.width = width
                    }

                    height: item ? item.height : 0
                }
            }

            Column {
                width: parent.width - 40
                spacing: 10
                visible: homeRoot.userGenres.length > 0

                Text {
                    text: "Your Favorite Genres"
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Flow {
                    width: parent.width
                    spacing: 10

                    Repeater {
                        model: homeRoot.userGenres
                        Text {
                            anchors.top: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            z: 999
                            color: "yellow"
                            font.pixelSize: 20
                            text: "bookId=" + model.bookId + " stack=" + (homeRoot.rootStackView ? "OK" : "NULL")
}
                        delegate: Rectangle {
                            color: homeRoot.selectedGenre === modelData ? "#D4AF37" : "#2D1B33"
                            border.color: "#D4AF37"
                            border.width: 1
                            radius: 20
                            implicitHeight: 35
                            implicitWidth: genreName.implicitWidth + 30

                            Text {
                                id: genreName
                                anchors.centerIn: parent
                                text: modelData
                                color: homeRoot.selectedGenre === modelData ? "#1A0F1F" : "#D4AF37"
                                font.pixelSize: 13
                                font.bold: true
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (homeRoot.selectedGenre === modelData) {
                                        homeRoot.selectedGenre = ""
                                        genreBooksModel.clear()
                                    } else {
                                        homeRoot.loadGenreBooks(modelData)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Column {
                width: parent.width - 40
                spacing: 10
                visible: genreBooksModel.count > 0

                Text {
                    text: "Books in \"" + homeRoot.selectedGenre + "\""
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Loader {
                    width: parent.width
                    sourceComponent: bookGrid

                    onLoaded: {
                        item.model = genreBooksModel
                        item.width = width
                    }

                    height: item ? item.height : 0
                }
            }

            Column {
                width: parent.width - 40
                spacing: 10
                visible: popularModel.count > 0

                Text {
                    text: "Popular Books"
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "Highest rated by readers"
                    color: "#A08EAD"
                    font.pixelSize: 12
                }

                Loader {
                    width: parent.width
                    sourceComponent: bookGrid

                    onLoaded: {
                        item.model = popularModel
                        item.width = width
                    }

                    height: item ? item.height : 0
                }
            }

            Column {
                width: parent.width - 40
                spacing: 10
                visible: newReleasesModel.count > 0

                Text {
                    text: "New Releases"
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "Most recently published"
                    color: "#A08EAD"
                    font.pixelSize: 12
                }

                Loader {
                    width: parent.width
                    sourceComponent: bookGrid

                    onLoaded: {
                        item.model = newReleasesModel
                        item.width = width
                    }

                    height: item ? item.height : 0
                }
            }

            Column {
                width: parent.width - 40
                spacing: 10
                visible: bestSellersModel.count > 0

                Text {
                    text: "Best Sellers"
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "Most purchased by readers"
                    color: "#A08EAD"
                    font.pixelSize: 12
                }

                Loader {
                    width: parent.width
                    sourceComponent: bookGrid

                    onLoaded: {
                        item.model = bestSellersModel
                        item.width = width
                    }

                    height: item ? item.height : 0
                }
            }

            Column {
                width: parent.width - 40
                spacing: 10
                visible: freeBooksModel.count > 0

                Text {
                    text: "Free Books"
                    color: "#D4AF37"
                    font.pixelSize: 18
                    font.bold: true
                }

                Text {
                    text: "Price after discount is $0"
                    color: "#A08EAD"
                    font.pixelSize: 12
                }

                Loader {
                    width: parent.width
                    sourceComponent: bookGrid

                    onLoaded: {
                        item.model = freeBooksModel
                        item.width = width
                    }

                    height: item ? item.height : 0
                }
            }
        }
    }
}
