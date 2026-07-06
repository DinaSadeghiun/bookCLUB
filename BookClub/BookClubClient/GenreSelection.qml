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
        anchors.margins: 30
        spacing: 25

        Text {
            text: "Please select 1 to 3 genres"
            font.pixelSize: 24
            font.bold: true
            color: "#FFD700"
            Layout.alignment: Qt.AlignHCenter
        }

        GridView {
            id: genreGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: genreGrid.width / 2
            cellHeight: 70
            clip: true
            model: ["Fiction", "Non-Fiction", "Science", "History", "Fantasy", "Mystery", "Biography", "Poetry"]

            delegate: Item {
                width: genreGrid.cellWidth
                height: genreGrid.cellHeight

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 8
                    radius: 12
                    color: selectedGenres.indexOf(modelData) !== -1 ? "#FFD700" : "transparent"
                    border.color: "#FFD700"
                    border.width: 2

                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: selectedGenres.indexOf(modelData) !== -1 ? "#2C003E" : "white"
                        font.bold: true
                        font.pixelSize: 16
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            let genres = [...selectedGenres];
                            let index = genres.indexOf(modelData);

                            if (index !== -1) {
                                genres.splice(index, 1);
                            } else if (genres.length < maxSelection) {
                                genres.push(modelData);
                            }
                            selectedGenres = genres;
                        }
                    }
                }
            }
        }

        Text {
            id: counterText
            text: "Selected: " + selectedGenres.length + " / " + maxSelection
            color: "white"
            font.pixelSize: 18
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            id: finishButton
            text: "Finish"
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            enabled: selectedGenres.length >= 1 && selectedGenres.length <= 3

            background: Rectangle {
                color: finishButton.enabled ? "#FFD700" : "#555555"
                radius: 10
            }

            contentItem: Text {
                text: finishButton.text
                color: "#2C003E"
                font.bold: true
                font.pixelSize: 20
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
