import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: genrePage
    anchors.fill: parent

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#4B0082" }
        GradientStop { position: 1.0; color: "#2C003E" }
    }

    property var selectedGenres: []
    property int maxSelection: 3
    property string username: ""

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Text {
            text: "Choose Your Genres"
            font.pixelSize: 26
            font.bold: true
            color: "#FFD700"
            Layout.alignment: Qt.AlignHCenter
        }

        GridView {
            id: genreGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: genreGrid.width / 3
            cellHeight: cellWidth + 40
            clip: true

            model: ListModel {
                ListElement { name: "Fiction"; img: "qrc:/assets/genres/fiction.jpg" }
                ListElement { name: "NonFiction"; img: "qrc:/assets/genres/nonfiction.jpg" }
                ListElement { name: "Mystery"; img: "qrc:/assets/genres/mystery.jpg" }
                ListElement { name: "Romance"; img: "qrc:/assets/genres/romance.jpg" }
                ListElement { name: "SciFi"; img: "qrc:/assets/genres/scifi.jpg" }
                ListElement { name: "Fantasy"; img: "qrc:/assets/genres/fantasy.jpg" }
                ListElement { name: "Biography"; img: "qrc:/assets/genres/biography.jpg" }
                ListElement { name: "History"; img: "qrc:/assets/genres/history.jpg" }
                ListElement { name: "SelfHelp"; img: "qrc:/assets/genres/selfhelp.jpg" }
                ListElement { name: "Poetry"; img: "qrc:/assets/genres/poetry.jpg" }
                ListElement { name: "Children"; img: "qrc:/assets/genres/children.jpg" }
                ListElement { name: "Other"; img: "qrc:/assets/genres/other.jpg" }
            }

            delegate: Item {
                width: genreGrid.cellWidth
                height: genreGrid.cellHeight

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 5
                    spacing: 8

                    Rectangle {
                        Layout.preferredWidth: parent.width - 10
                        Layout.preferredHeight: Layout.preferredWidth
                        Layout.alignment: Qt.AlignHCenter
                        radius: 12
                        clip: true
                        border.color: selectedGenres.indexOf(name) !== -1 ? "#FFD700" : "transparent"
                        border.width: 3
                        color: "#3B0062"

                        Image {
                            anchors.fill: parent
                            source: img
                            fillMode: Image.PreserveAspectCrop
                            smooth: true
                        }

                        Rectangle {
                            anchors.fill: parent
                            color: "#4B0082"
                            opacity: selectedGenres.indexOf(name) !== -1 ? 0.0 : 0.3
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                let genres = [...selectedGenres];
                                let index = genres.indexOf(name);
                                if (index !== -1) {
                                    genres.splice(index, 1);
                                } else if (genres.length < maxSelection) {
                                    genres.push(name);
                                }
                                selectedGenres = genres;
                            }
                        }
                    }

                    Text {
                        text: name
                        color: selectedGenres.indexOf(name) !== -1 ? "#FFD700" : "white"
                        font.bold: true
                        font.pixelSize: 14
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }
        }

        Button {
            id: finishButton
            text: "Continue (" + selectedGenres.length + "/" + maxSelection + ")"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            enabled: selectedGenres.length >= 1

            background: Rectangle {
                color: finishButton.enabled ? "#FFD700" : "#555555"
                radius: 10
            }

            contentItem: Text {
                text: finishButton.text
                color: "#2C003E"
                font.bold: true
                font.pixelSize: 18
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                stackView.push("qrc:/Dashboard.qml", {
                    "username": username,
                    "userRole": "User",
                    "preferredGenres": selectedGenres
                });
            }
        }
    }
}
