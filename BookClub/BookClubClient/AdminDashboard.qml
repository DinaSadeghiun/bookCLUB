import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: adminDashboard
    anchors.fill: parent
    color: "#2D1B33" // Deep purple background

    property string username: "Admin"

    // --- MOCK DATABASE MODELS ---

    // 1. Users Model (Includes Registration Date and Details)
    ListModel {
        id: usersModel
        ListElement { userId: 1; username: "ali_dev"; email: "ali@example.com"; role: "User"; regDate: "2026/02/10"; status: "Active" }
        ListElement { userId: 2; username: "tehran_publish"; email: "info@tehranpress.ir"; role: "Publisher"; regDate: "2026/01/15"; status: "Active" }
        ListElement { userId: 3; username: "reza_99"; email: "reza@gmail.com"; role: "User"; regDate: "2026/03/01"; status: "Blocked" }
        ListElement { userId: 4; username: "gilan_book"; email: "contact@gilan.com"; role: "Publisher"; regDate: "2026/04/11"; status: "Active" }
    }

    // Filtered/Searched model helper
    ListModel {
        id: filteredUsersModel
        Component.onCompleted: updateFilteredModel("")
    }

    function updateFilteredModel(filterText) {
        filteredUsersModel.clear();
        for (var i = 0; i < usersModel.count; i++) {
            var item = usersModel.get(i);
            if (filterText === "" ||
                item.username.toLowerCase().indexOf(filterText.toLowerCase()) !== -1 ||
                item.role.toLowerCase().indexOf(filterText.toLowerCase()) !== -1) {
                filteredUsersModel.append(item);
            }
        }
    }

    // 2. Books Model
    ListModel {
        id: booksModel
        ListElement { bookId: 101; title: "شاهنامه فردوسی"; author: "فردوسی"; publisher: "tehran_publish"; status: "Approved" }
        ListElement { bookId: 102; title: "اصول طراحی الگوریتم"; author: "جعفرنژاد قمی"; publisher: "gilan_book"; status: "Pending" }
        ListElement { bookId: 103; title: "راهنمای کیوت ۶"; author: "برنامه‌نویسان"; publisher: "tehran_publish"; status: "Rejected" }
    }

    // 3. Comments/Reviews Model (Missing Feature Added)
    ListModel {
        id: commentsModel
        ListElement { commentId: 1; user: "ali_dev"; bookTitle: "شاهنامه فردوسی"; content: "بسیار کتاب ارزشمندی است و کیفیت چاپ عالی دارد."; status: "Pending" }
        ListElement { commentId: 2; user: "reza_99"; bookTitle: "اصول طراحی الگوریتم"; content: "کتاب غلط‌های املایی زیادی دارد."; status: "Pending" }
    }

    // --- UI STRUCTURE ---

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ================= SIDEBAR NAVIGATOR =================
        Rectangle {
            Layout.preferredWidth: parent.width * 0.22
            Layout.fillHeight: true
            color: "#1E0F24"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 15

                ColumnLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 8
                    Image {
                        source: "qrc:/images/giraffe.png"
                        Layout.preferredWidth: 60
                        Layout.preferredHeight: 60
                        Layout.alignment: Qt.AlignHCenter
                        fillMode: Image.PreserveAspectFit
                    }
                    Text {
                        text: "Admin Panel"
                        color: "#D4AF37"
                        font.bold: true
                        font.pixelSize: 18
                        Layout.alignment: Qt.AlignHCenter
                    }
                }

                // Sidebar Navigation Tabs
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Button {
                        text: "Overview"
                        Layout.fillWidth: true
                        highlighted: stackLayout.currentIndex === 0
                        onClicked: stackLayout.currentIndex = 0
                    }
                    Button {
                        text: "Manage Users"
                        Layout.fillWidth: true
                        highlighted: stackLayout.currentIndex === 1
                        onClicked: {
                            updateFilteredModel("");
                            stackLayout.currentIndex = 1;
                        }
                    }
                    Button {
                        text: "Manage Books"
                        Layout.fillWidth: true
                        highlighted: stackLayout.currentIndex === 2
                        onClicked: stackLayout.currentIndex = 2
                    }
                    Button {
                        text: "Manage Comments"
                        Layout.fillWidth: true
                        highlighted: stackLayout.currentIndex === 3
                        onClicked: stackLayout.currentIndex = 3
                    }
                }

                Item { Layout.fillHeight: true }

                Button {
                    text: "Logout"
                    Layout.fillWidth: true
                    onClicked: {
                        adminDashboard.StackView.view.replace("qrc:/Login.qml")
                    }
                }
            }
        }

        // ================= MAIN CONTENT VIEW =================
        StackLayout {
            id: stackLayout
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: 0

            // Tab 0: Overview
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 25
                    spacing: 20

                    Text {
                        text: "Overview Dashboard"
                        color: "#D4AF37"
                        font.pixelSize: 24
                        font.bold: true
                    }

                    RowLayout {
                        spacing: 20
                        Layout.fillWidth: true

                        Rectangle {
                            Layout.fillWidth: true
                            height: 110
                            color: "#3D2744"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                            ColumnLayout {
                                anchors.centerIn: parent
                                Text { text: "Total Users / Publishers"; color: "white"; font.pixelSize: 14 }
                                Text { text: usersModel.count.toString(); color: "#D4AF37"; font.pixelSize: 28; font.bold: true }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 110
                            color: "#3D2744"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                            ColumnLayout {
                                anchors.centerIn: parent
                                Text { text: "Total Books"; color: "white"; font.pixelSize: 14 }
                                Text { text: booksModel.count.toString(); color: "#D4AF37"; font.pixelSize: 28; font.bold: true }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 110
                            color: "#3D2744"
                            radius: 6
                            border.color: "#D4AF37"
                            border.width: 1
                            ColumnLayout {
                                anchors.centerIn: parent
                                Text { text: "Pending Comments"; color: "white"; font.pixelSize: 14 }
                                Text { text: commentsModel.count.toString(); color: "#FFC107"; font.pixelSize: 28; font.bold: true }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // Tab 1: Users & Publishers Access Control
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 12

                    Text {
                        text: "User Accounts & Access Level"
                        color: "#D4AF37"
                        font.pixelSize: 22
                        font.bold: true
                    }

                    // Search and Filter Bar
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        TextField {
                            id: txtUserSearch
                            placeholderText: "Search Username or Role (User/Publisher)..."
                            Layout.fillWidth: true
                            color: "white"
                            background: Rectangle {
                                color: "#3D2744"
                                border.color: "#D4AF37"
                                radius: 4
                            }
                            onTextChanged: updateFilteredModel(text)
                        }
                    }

                    // Users Table
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "ID"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 30 }
                        Text { text: "Username"; font.bold: true; color: "#D4AF37"; Layout.fillWidth: true }
                        Text { text: "Role"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 100 }
                        Text { text: "Reg Date"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 100 }
                        Text { text: "Status"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 80 }
                        Text { text: "Actions"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 170 }
                    }

                    ListView {
                        id: usersListView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: filteredUsersModel
                        clip: true
                        delegate: Rectangle {
                            width: usersListView.width
                            height: 50
                            color: index % 2 === 0 ? "#331E3A" : "#3D2744"
                            border.color: "#2D1B33"

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 8; anchors.rightMargin: 8

                                Text { text: model.userId; color: "white"; Layout.preferredWidth: 30 }
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    Text { text: model.username; color: "white"; font.bold: true }
                                    Text { text: model.email; color: "#AAA"; font.pixelSize: 11 }
                                }
                                Text { text: model.role; color: "#D4AF37"; Layout.preferredWidth: 100 }
                                Text { text: model.regDate; color: "white"; Layout.preferredWidth: 100 }
                                Text { text: model.status; color: model.status === "Blocked" ? "#FF4D4D" : "#4DFF4D"; Layout.preferredWidth: 80 }

                                RowLayout {
                                    Layout.preferredWidth: 170
                                    spacing: 5

                                    Button {
                                        text: model.status === "Active" ? "Block" : "Unblock"
                                        Layout.preferredHeight: 30
                                        onClicked: {
                                            var targetId = model.userId;
                                            for(var i=0; i<usersModel.count; i++) {
                                                if(usersModel.get(i).userId === targetId) {
                                                    var cur = usersModel.get(i).status;
                                                    usersModel.setProperty(i, "status", cur === "Active" ? "Blocked" : "Active");
                                                    break;
                                                }
                                            }
                                            updateFilteredModel(txtUserSearch.text);
                                        }
                                    }

                                    Button {
                                        text: "Delete"
                                        Layout.preferredHeight: 30
                                        background: Rectangle { color: "#8B0000"; radius: 4 }
                                        onClicked: {
                                            var targetId = model.userId;
                                            for(var i=0; i<usersModel.count; i++) {
                                                if(usersModel.get(i).userId === targetId) {
                                                    usersModel.remove(i);
                                                    break;
                                                }
                                            }
                                            updateFilteredModel(txtUserSearch.text);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Tab 2: Books Moderation & Details Editing
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 12

                    Text {
                        text: "Book Content Moderation"
                        color: "#D4AF37"
                        font.pixelSize: 22
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Book Title"; font.bold: true; color: "#D4AF37"; Layout.fillWidth: true }
                        Text { text: "Author"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 120 }
                        Text { text: "Publisher"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 120 }
                        Text { text: "Status"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 90 }
                        Text { text: "Actions"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 200 }
                    }

                    ListView {
                        id: booksListView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: booksModel
                        clip: true
                        delegate: Rectangle {
                            width: booksListView.width
                            height: 50
                            color: index % 2 === 0 ? "#331E3A" : "#3D2744"

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 8; anchors.rightMargin: 8

                                Text { text: model.title; color: "white"; Layout.fillWidth: true }
                                Text { text: model.author; color: "white"; Layout.preferredWidth: 120 }
                                Text { text: model.publisher; color: "white"; Layout.preferredWidth: 120 }
                                Text {
                                    text: model.status
                                    color: model.status === "Approved" ? "#4DFF4D" : (model.status === "Rejected" ? "#FF4D4D" : "#FFC107")
                                    Layout.preferredWidth: 90
                                }

                                RowLayout {
                                    Layout.preferredWidth: 200
                                    spacing: 5

                                    Button {
                                        text: "Edit"
                                        Layout.preferredHeight: 30
                                        onClicked: {
                                            editBookDialog.selectedBookIndex = index;
                                            editTitleField.text = model.title;
                                            editAuthorField.text = model.author;
                                            editBookDialog.open();
                                        }
                                    }

                                    Button {
                                        text: "Approve"
                                        enabled: model.status !== "Approved"
                                        Layout.preferredHeight: 30
                                        onClicked: model.status = "Approved"
                                    }

                                    Button {
                                        text: "Reject"
                                        enabled: model.status !== "Rejected"
                                        Layout.preferredHeight: 30
                                        onClicked: model.status = "Rejected"
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Tab 3: Comments & Reviews Moderation (Added Section)
            Rectangle {
                color: "transparent"
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 12

                    Text {
                        text: "User Review & Comment Control"
                        color: "#D4AF37"
                        font.pixelSize: 22
                        font.bold: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "User"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 100 }
                        Text { text: "Book"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 130 }
                        Text { text: "Comment Content"; font.bold: true; color: "#D4AF37"; Layout.fillWidth: true }
                        Text { text: "Status"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 80 }
                        Text { text: "Action"; font.bold: true; color: "#D4AF37"; Layout.preferredWidth: 160 }
                    }

                    ListView {
                        id: commentsListView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: commentsModel
                        clip: true
                        delegate: Rectangle {
                            width: commentsListView.width
                            height: 60
                            color: index % 2 === 0 ? "#331E3A" : "#3D2744"

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 8; anchors.rightMargin: 8

                                Text { text: model.user; color: "white"; Layout.preferredWidth: 100 }
                                Text { text: model.bookTitle; color: "#D4AF37"; Layout.preferredWidth: 130 }
                                Text { text: model.content; color: "white"; Layout.fillWidth: true; wrapMode: Text.WordWrap; font.pixelSize: 12 }
                                Text { text: model.status; color: model.status === "Pending" ? "#FFC107" : "#4DFF4D"; Layout.preferredWidth: 80 }

                                RowLayout {
                                    Layout.preferredWidth: 160
                                    spacing: 5

                                    Button {
                                        text: "Approve"
                                        Layout.preferredHeight: 30
                                        onClicked: {
                                            model.status = "Approved";
                                            console.log("Approved comment ID: " + model.commentId);
                                        }
                                    }

                                    Button {
                                        text: "Delete"
                                        Layout.preferredHeight: 30
                                        background: Rectangle { color: "#8B0000"; radius: 4 }
                                        onClicked: {
                                            commentsModel.remove(index);
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

    // --- DIALOG FOR EDITING BOOK ---
    Dialog {
        id: editBookDialog
        title: "Edit Book Details"
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent
        modal: true

        property int selectedBookIndex: -1

        ColumnLayout {
            spacing: 10
            TextField {
                id: editTitleField
                placeholderText: "Book Title"
                Layout.fillWidth: true
            }
            TextField {
                id: editAuthorField
                placeholderText: "Author"
                Layout.fillWidth: true
            }
        }

        onAccepted: {
            if (selectedBookIndex !== -1) {
                booksModel.setProperty(selectedBookIndex, "title", editTitleField.text);
                booksModel.setProperty(selectedBookIndex, "author", editAuthorField.text);
            }
        }
    }
}
