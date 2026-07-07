import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: genreSelectionPage
    anchors.fill: parent // Crucial to prevent the white gap on the right
    color: "#1A0F1F"

    property var selectedGenres: []

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // Header
        Text {
            text: "Choose Your Favorites"
            color: "#D4AF37"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Select 1 to 3 genres to personalize your feed"
            color: "#D4AF37"
            font.pixelSize: 14
            opacity: 0.8
            Layout.alignment: Qt.AlignHCenter
        }

        // Genre Grid
        GridView {
            id: genreGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: width / 2
            cellHeight: 220
            clip: true

            model: ListModel {
                ListElement { name: "Fiction"; prompt: "Imaginary stories and adventures"; img: "qrc:/assets/genres/fiction.png" }
                ListElement { name: "NonFiction"; prompt: "Real-world facts and knowledge"; img: "qrc:/assets/genres/nonFiction.png" }
                ListElement { name: "Mystery"; prompt: "Solving puzzles and crimes"; img: "qrc:/assets/genres/mystery.png" }
                ListElement { name: "Romance"; prompt: "Love stories and relationships"; img: "qrc:/assets/genres/romance.png" }
                ListElement { name: "SciFi"; prompt: "Future tech and space exploration"; img: "qrc:/assets/genres/sciFI.png" }
                ListElement { name: "Fantasy"; prompt: "Magic and mythical creatures"; img: "qrc:/assets/genres/fantasy.png" }
                ListElement { name: "Biography"; prompt: "Life stories of real people"; img: "qrc:/assets/genres/biography.jpg" }
                ListElement { name: "History"; prompt: "Exploring the events of the past"; img: "qrc:/assets/genres/history.png" }
                ListElement { name: "SelfHelp"; prompt: "Personal growth and motivation"; img: "qrc:/assets/genres/selfHelp.png" }
                ListElement { name: "Poetry"; prompt: "Rhythmic and emotional verses"; img: "qrc:/assets/genres/poetry.png" }
                ListElement { name: "Children"; prompt: "Fun stories for young minds"; img: "qrc:/assets/genres/children.png" }
                ListElement { name: "Other"; prompt: "Explore diverse categories"; img: "qrc:/assets/genres/other.png" }
            }

            delegate: Item {
                width: genreGrid.cellWidth
                height: genreGrid.cellHeight

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 8
                    radius: 15
                    color: selectedGenres.indexOf(model.name) !== -1 ? "#D4AF37" : "#2D1B33"
                    border.color: "#D4AF37"
                    border.width: 2

                    Column {
                        anchors.centerIn: parent
                        spacing: 10
                        width: parent.width - 20

                        Image {
                            source: model.img
                            width: 85
                            height: 85
                            anchors.horizontalCenter: parent.horizontalCenter
                            fillMode: Image.PreserveAspectFit
                        }

                        Text {
                            text: model.name
                            color: selectedGenres.indexOf(model.name) !== -1 ? "#2D1B33" : "#D4AF37"
                            font.bold: true
                            font.pixelSize: 16
                            anchors.horizontalCenter: parent.horizontalCenter
                        }

                        Text {
                            text: model.prompt
                            color: selectedGenres.indexOf(model.name) !== -1 ? "#2D1B33" : "#D4AF37"
                            font.pixelSize: 11
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WordWrap
                            width: parent.width
                            opacity: 0.9
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var index = selectedGenres.indexOf(model.name);
                            if (index !== -1) {
                                selectedGenres.splice(index, 1);
                            } else if (selectedGenres.length < 3) {
                                selectedGenres.push(model.name);
                            }
                            var temp = selectedGenres;
                            selectedGenres = [];
                            selectedGenres = temp;
                        }
                    }
                }
            }
        }

        // Continue Button
        Button {
            id: continueButton
            text: "Continue (" + selectedGenres.length + "/3)"
            enabled: selectedGenres.length > 0
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            Layout.bottomMargin: 10

            background: Rectangle {
                color: continueButton.enabled ? "#D4AF37" : "#444444"
                radius: 10
            }

            contentItem: Text {
                text: continueButton.text
                color: "#1A0F1F"
                font.pixelSize: 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                stackView.push("Dashboard.qml", { "userGenres": selectedGenres })
            }
        }
    }
}
