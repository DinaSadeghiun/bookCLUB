import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: genreSelectionPage
    anchors.fill: parent

    // Array to store selected genres
    property var selectedGenres: []

    Rectangle {
        anchors.fill: parent
        color: "#F8F9FA" // Light background color

        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width * 0.85
            spacing: 30

            // Header Section
            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 10

                Text {
                    text: "Welcome to BookClub!"
                    font.pixelSize: 28
                    font.bold: true
                    color: "#2C3E50"
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: "Select 1 to 3 genres that you love"
                    font.pixelSize: 16
                    color: "#7F8C8D"
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            // Genre Grid
            GridLayout {
                id: genreGrid
                columns: 3
                columnSpacing: 15
                rowSpacing: 15
                Layout.alignment: Qt.AlignHCenter

                Repeater {
                    model: ["Fiction", "Science", "History", "Biography", "Fantasy",
                            "Mystery", "Horror", "Art", "Self-Help"]

                    Button {
                        id: genreButton
                        text: modelData
                        checkable: true

                        // Custom style for selection
                        contentItem: Text {
                            text: genreButton.text
                            color: genreButton.checked ? "white" : "#2C3E50"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.bold: genreButton.checked
                        }

                        background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 45
                            color: genreButton.checked ? "#3498DB" : "white"
                            border.color: genreButton.checked ? "#2980B9" : "#BDC3C7"
                            radius: 8
                        }

                        onCheckedChanged: {
                            if (checked) {
                                if (selectedGenres.length < 3) {
                                    selectedGenres.push(modelData);
                                } else {
                                    // Prevent selecting more than 3
                                    checked = false;
                                }
                            } else {
                                // Remove genre if unchecked
                                var index = selectedGenres.indexOf(modelData);
                                if (index !== -1) {
                                    selectedGenres.splice(index, 1);
                                }
                            }
                        }
                    }
                }
            }

            // Navigation Button
            Button {
                id: finishButton
                text: "Continue to Library"
                Layout.preferredWidth: 250
                Layout.preferredHeight: 50
                Layout.alignment: Qt.AlignHCenter

                // Enabled only if 1, 2, or 3 genres are selected
                enabled: selectedGenres.length >= 1 && selectedGenres.length <= 3

                background: Rectangle {
                    color: finishButton.enabled ? "#2ECC71" : "#BDC3C7"
                    radius: 25
                }

                contentItem: Text {
                    text: finishButton.text
                    color: "white"
                    font.bold: true
                    font.pixelSize: 16
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    console.log("Selected Genres:", selectedGenres)
                    // Moving to Dashboard and passing selected genres
                    stackView.push("qrc:/Dashboard.qml", {
                        "userRole": "User",
                        "preferredGenres": selectedGenres
                    })
                }
            }

            // Display current count for the user
            Text {
                text: "Selected: " + selectedGenres.length + " / 3"
                font.pixelSize: 12
                color: selectedGenres.length === 3 ? "#E74C3C" : "#7F8C8D"
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
