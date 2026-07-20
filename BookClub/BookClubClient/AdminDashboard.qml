import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: adminDashboard
    anchors.fill: parent
    color: "#2D1B33" // Deep purple background

    // --- MOCK DATABASE MODELS ---
    ListModel {
        id: usersModel
        ListElement { userId: 1; username: "ali_dev"; password: "123"; role: "User"; favAuthor: "J.K. Rowling"; regDate: "2026/02/10"; status: "Active" }
        ListElement { userId: 2; username: "tehran_publish"; password: "456"; role: "Publisher"; favAuthor: "N/A"; regDate: "2026/01/15"; status: "Active" }
        ListElement { userId: 3; username: "reza_99"; password: "789"; role: "User"; favAuthor: "Victor Hugo"; regDate: "2026/03/01"; status: "Blocked" }
    }

    ListModel {
        id: filteredUsersModel
        Component.onCompleted: updateFilteredModel("")
    }

    function updateFilteredModel(filterText) {
        filteredUsersModel.clear();
        for (var i = 0; i < usersModel.count; i++) {
            var item = usersModel.get(i);
            if (filterText === "" || item.username.toLowerCase().indexOf(filterText.toLowerCase()) !== -1) {
                filteredUsersModel.append(item);
            }
        }
    }

    ListModel {
        id: booksModel
        ListElement { bookId: 101; title: "Shahnameh"; author: "Ferdowsi"; publisher: "tehran_publish"; genre: "Epic"; price: "150"; description: "Persian epic poetry." }
        ListElement { bookId: 102; title: "Qt 6 Guide"; author: "Qt Team"; publisher: "gilan_book"; genre: "Education"; price: "200"; description: "C++ Framework guide." }
    }

    ListModel {
        id: commentsModel
        ListElement { commentId: 1; user: "ali_dev"; bookTitle: "Shahnameh"; content: "Excellent quality!" }
        ListElement { commentId: 2; user: "reza_99"; bookTitle: "Qt 6 Guide"; content: "Very helpful for beginners." }
    }

    // --- UI STRUCTURE ---
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // SIDEBAR
        Rectangle {
            Layout.preferredWidth: parent.width * 0.22
            Layout.fillHeight: true
            color: "#1E0F24"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 20

                Text {
                    text: "ADMIN PANEL"
                    color: "#D4AF37"
                    font.bold: true
                    font.pixelSize: 22
                    Layout.alignment: Qt.AlignHCenter
                }

                Button { text: "Manage Users"; Layout.fillWidth: true; onClicked: stackLayout.currentIndex = 0 }
                Button { text: "Manage Books"; Layout.fillWidth: true; onClicked: stackLayout.currentIndex = 1 }
                Button { text: "Manage Comments"; Layout.fillWidth: true; onClicked: stackLayout.currentIndex = 2 }

                Item { Layout.fillHeight: true }
            }
        }

        // MAIN CONTENT
        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true

            // TAB 0: USERS
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 20
                    Text { text: "User Management"; color: "#D4AF37"; font.pixelSize: 22; font.bold: true }
                    TextField {
                        placeholderText: "Search Username..."; Layout.fillWidth: true
                        onTextChanged: updateFilteredModel(text)
                    }
                    // Users Table Header
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "User/Pass"; color: "#D4AF37"; Layout.preferredWidth: 150; font.bold: true }
                        Text { text: "Role"; color: "#D4AF37"; Layout.preferredWidth: 80; font.bold: true }
                        Text { text: "Fav Author"; color: "#D4AF37"; Layout.fillWidth: true; font.bold: true }
                        Text { text: "Reg Date"; color: "#D4AF37"; Layout.preferredWidth: 100; font.bold: true }
                        Text { text: "Status"; color: "#D4AF37"; Layout.preferredWidth: 80; font.bold: true }
                        Text { text: "Actions"; color: "#D4AF37"; Layout.preferredWidth: 100; font.bold: true }
                    }
                    ListView {
                        Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                        model: filteredUsersModel
                        delegate: Rectangle {
                            width: parent.width; height: 60; color: index % 2 === 0 ? "#331E3A" : "#3D2744"
                            RowLayout {
                                anchors.fill: parent; anchors.margins: 10
                                Column { Layout.preferredWidth: 150; Text { text: model.username; color: "white"; font.bold: true } Text { text: "PW: " + model.password; color: "#AAA"; font.pixelSize: 10 } }
                                Text { text: model.role; color: "#D4AF37"; Layout.preferredWidth: 80 }
                                Text { text: model.favAuthor; color: "white"; Layout.fillWidth: true }
                                Text { text: model.regDate; color: "white"; Layout.preferredWidth: 100 }
                                Text { text: model.status; color: model.status === "Blocked" ? "#FF4D4D" : "#4DFF4D"; Layout.preferredWidth: 80 }
                                Button {
                                    text: model.status === "Active" ? "Block" : "Activate"
                                    Layout.preferredWidth: 100
                                    onClicked: {
                                        for(var i=0; i<usersModel.count; i++) {
                                            if(usersModel.get(i).userId === model.userId) {
                                                usersModel.setProperty(i, "status", model.status === "Active" ? "Blocked" : "Active");
                                                break;
                                            }
                                        }
                                        updateFilteredModel("");
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // TAB 1: BOOKS
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 20
                    Text { text: "Book Management"; color: "#D4AF37"; font.pixelSize: 22; font.bold: true }

                    // Books Table Header
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.bottomMargin: 5
                        Text { text: "Book Info (Title/Author/Genre)"; color: "#D4AF37"; Layout.fillWidth: true; font.bold: true }
                        Text { text: "Publisher"; color: "#D4AF37"; Layout.preferredWidth: 120; font.bold: true }
                        Text { text: "Price"; color: "#D4AF37"; Layout.preferredWidth: 80; font.bold: true }
                        Text { text: "Actions"; color: "#D4AF37"; Layout.preferredWidth: 150; font.bold: true }
                    }

                    ListView {
                        Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                        model: booksModel
                        delegate: Rectangle {
                            width: parent.width; height: 60; color: index % 2 === 0 ? "#331E3A" : "#3D2744"
                            RowLayout {
                                anchors.fill: parent; anchors.margins: 10
                                Column {
                                    Layout.fillWidth: true
                                    Text { text: model.title; color: "white"; font.bold: true }
                                    Text { text: model.author + " | " + model.genre; color: "#AAA"; font.pixelSize: 11 }
                                }
                                Text { text: model.publisher; color: "white"; Layout.preferredWidth: 120 }
                                Text { text: model.price + " $"; color: "#D4AF37"; Layout.preferredWidth: 80 }
                                Row {
                                    spacing: 5
                                    Button { text: "Edit"; onClicked: { editBookDialog.targetIndex = index; editTitle.text = model.title; editGenre.text = model.genre; editPrice.text = model.price; editDesc.text = model.description; editBookDialog.open() } }
                                    Button { text: "Delete"; background: Rectangle { color: "#8B0000" } onClicked: booksModel.remove(index) }
                                }
                            }
                        }
                    }
                }
            }

            // TAB 2: COMMENTS
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 20
                    Text { text: "Comment Moderation"; color: "#D4AF37"; font.pixelSize: 22; font.bold: true }
                    ListView {
                        Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                        model: commentsModel
                        delegate: Rectangle {
                            width: parent.width; height: 70; color: index % 2 === 0 ? "#331E3A" : "#3D2744"
                            RowLayout {
                                anchors.fill: parent; anchors.margins: 10
                                Column { Layout.fillWidth: true; Text { text: model.user + " on " + model.bookTitle; color: "#D4AF37"; font.bold: true } Text { text: model.content; color: "white"; wrapMode: Text.WordWrap; width: 400 } }
                                Button { text: "Delete"; background: Rectangle { color: "#8B0000" } onClicked: commentsModel.remove(index) }
                            }
                        }
                    }
                }
            }
        }
    }

    // EDIT BOOK DIALOG
    Dialog {
        id: editBookDialog
        property int targetIndex: -1
        title: "Edit Book Information"; anchors.centerIn: parent; modal: true
        standardButtons: Dialog.Save | Dialog.Cancel

        ColumnLayout {
            spacing: 10
            width: 300

            Label { text: "Book Title:"; color: "#D4AF37"; font.bold: true }
            TextField { id: editTitle; Layout.fillWidth: true }

            Label { text: "Genre:"; color: "#D4AF37"; font.bold: true }
            TextField { id: editGenre; Layout.fillWidth: true }

            Label { text: "Price ($):"; color: "#D4AF37"; font.bold: true }
            TextField { id: editPrice; Layout.fillWidth: true }

            Label { text: "Description:"; color: "#D4AF37"; font.bold: true }
            TextArea { id: editDesc; Layout.fillWidth: true; implicitHeight: 100; wrapMode: Text.WordWrap }
        }

        onAccepted: {
            if (targetIndex !== -1) {
                booksModel.setProperty(targetIndex, "title", editTitle.text);
                booksModel.setProperty(targetIndex, "genre", editGenre.text);
                booksModel.setProperty(targetIndex, "price", editPrice.text);
                booksModel.setProperty(targetIndex, "description", editDesc.text);
            }
        }
    }
}
