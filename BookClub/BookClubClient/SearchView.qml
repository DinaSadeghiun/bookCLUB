import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: searchRoot
    anchors.fill: parent
    color: "#1A0F1F"

    required property var networkManager
    signal bookSelected(int bookId)
    property int userId: 0
    property var booksById: ({})
    property var allBooksList: []

    property string selectedFilter: "All"

    ListModel { id: resultsModel }

    function passesFilter(book, query) {
        if (query === "") return true;
        var q = query.toLowerCase();
        var titleMatch = book.title && book.title.toLowerCase().indexOf(q) !== -1;
        var authorMatch = book.author && book.author.toLowerCase().indexOf(q) !== -1;
        var publisherMatch = book.publisherUsername && book.publisherUsername.toLowerCase().indexOf(q) !== -1;

        if (selectedFilter === "All") {
            return titleMatch || authorMatch || publisherMatch;
        }
        if (selectedFilter === "Book Title") {
            return titleMatch;
        }
        if (selectedFilter === "Author") {
            return authorMatch;
        }
        if (selectedFilter === "Publisher") {
            return publisherMatch;
        }
        return true;
    }

    function rebuildResults(rawList) {
        resultsModel.clear();
        var query = searchInput.text.trim();

        for (var i = 0; i < rawList.length; i++) {
            var book = rawList[i];
            if (!book.isAvailable) continue;

            var fullBook = searchRoot.booksById[book.id] || book;
            if (!passesFilter(fullBook, query)) continue;

            resultsModel.append({
                "bookId": book.id,
                "title": book.title,
                "author": book.author,
                "genre": book.genre,
                "price": book.price,
                "image": book.coverImageData || "qrc:/assets/images/giraffe.png",
                "publisherUsername": fullBook.publisherUsername || ""
            });
        }
    }

    property var lastRawResults: []

    Connections {
        target: networkManager
        function onResponseReceived(action, status, data) {
            if (action === "searchBooks") {
                if (status === "success" || status === "SUCCESS") {
                    searchRoot.lastRawResults = data;
                    rebuildResults(data);
                } else {
                    resultsModel.clear();
                }
            }
        }
    }

    Timer {
        id: searchDebounce
        interval: 350
        repeat: false
        onTriggered: {
            if (searchInput.text.trim() === "") {
                rebuildResults(searchRoot.allBooksList);
            } else if (networkManager) {
                networkManager.searchBooks(searchInput.text.trim());
            }
        }
    }

    Component.onCompleted: rebuildResults(allBooksList)
    onSelectedFilterChanged: {
        rebuildResults(searchInput.text.trim() === "" ? allBooksList : lastRawResults)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#2d1b4e"
            radius: 10
            border.color: "#FFD700"
            border.width: 2

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 15
                anchors.rightMargin: 15

                Text {
                    text: "🔍"
                    font.pixelSize: 20
                    color: "#FFD700"
                }

                TextField {
                    id: searchInput
                    Layout.fillWidth: true
                    placeholderText: "Search by title, author, or publisher..."
                    color: "white"
                    placeholderTextColor: "#aaaaaa"
                    background: Rectangle { color: "transparent" }
                    font.pixelSize: 16
                    verticalAlignment: TextInput.AlignVCenter
                    onTextChanged: searchDebounce.restart()
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                text: "Search In:"
                color: "#FFD700"
                font.bold: true
                font.pixelSize: 14
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Repeater {
                    model: ["All", "Book Title", "Author", "Publisher"]
                    Button {
                        Layout.preferredHeight: 35
                        Layout.fillWidth: true

                        contentItem: Text {
                            text: modelData
                            color: searchRoot.selectedFilter === modelData ? "#1A0F1F" : "#FFD700"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: searchRoot.selectedFilter === modelData ? "#FFD700" : "#2d1b4e"
                            border.color: "#FFD700"
                            border.width: 1
                            radius: 5
                        }

                        onClicked: searchRoot.selectedFilter = modelData
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: searchInput.text !== "" ? "Results for: \"" + searchInput.text + "\"" : "Browse All Books"
                color: "#FFD700"
                font.bold: true
                font.pixelSize: 18
                Layout.fillWidth: true
            }
            Text {
                text: resultsModel.count + " result(s)"
                color: "#A08EAD"
                font.pixelSize: 12
            }
        }

        GridView {
            id: resultsGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: 150
            cellHeight: 260
            clip: true
            model: resultsModel

            delegate: Item {
                width: 140
                height: 250

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: searchRoot.bookSelected(model.bookId)
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 5

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 170
                        color: "#2D1B33"
                        radius: 8
                        border.color: "#FFD700"
                        border.width: 1
                        clip: true

                        Image {
                            anchors.fill: parent
                            anchors.margins: 5
                            source: model.image
                            fillMode: Image.PreserveAspectFit
                            onStatusChanged: {
                                if (status === Image.Error) source = "qrc:/assets/images/giraffe.png"
                            }
                        }
                    }

                    Text {
                        text: model.title
                        color: "white"
                        font.bold: true
                        font.pixelSize: 12
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }

                    Text {
                        text: model.author
                        color: "#A08EAD"
                        font.pixelSize: 10
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: model.price === 0 ? "Free" : ("$" + model.price)
                            color: model.price === 0 ? "#4CAF50" : "#FFD700"
                            font.bold: true
                            font.pixelSize: 11
                            Layout.alignment: Qt.AlignHCenter
                            Layout.fillWidth: true
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
    }
}