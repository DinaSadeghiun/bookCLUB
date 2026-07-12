import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: dashboardRoot

    required property string username
    required property string userRole
    property var userGenres: []
    property int cartItemCount: 0

    property int currentTab: 0

    Component.onCompleted: loadCurrentView()
    onCurrentTabChanged: loadCurrentView()

    function loadCurrentView() {
        var props = {
            "username": username,
            "userRole": userRole,
            "userGenres": userGenres,
            "cartItemCount": cartItemCount
        }
        switch (currentTab) {
        case 0: contentLoader.setSource("HomeView.qml", props); break
        case 1: contentLoader.setSource("LibraryView.qml", props); break
        case 2: contentLoader.setSource("SearchView.qml", props); break
        case 3: contentLoader.setSource("CartView.qml", props); break
        case 4: contentLoader.setSource("SettingsView.qml", props); break
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ---------- Sidebar ----------
        Rectangle {
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            color: "#1E1E1E"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Image {
                    Layout.alignment: Qt.AlignHCenter
                    source: "qrc:/assets/images/giraffe.png"
                    fillMode: Image.PreserveAspectFit
                    Layout.preferredWidth: 80
                    Layout.preferredHeight: 80
                }

                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: username
                    color: "white"
                    font.bold: true
                }

                Repeater {
                    model: ["Home", "Library", "Search", "Cart", "Settings"]
                    delegate: Button {
                        Layout.fillWidth: true
                        text: modelData
                        highlighted: currentTab === index
                        onClicked: currentTab = index

                        // Cart badge
                        Rectangle {
                            visible: index === 3 && cartItemCount > 0
                            width: 20; height: 20; radius: 10
                            color: "#D4AF37"
                            anchors.right: parent.right
                            anchors.top: parent.top
                            Text {
                                anchors.centerIn: parent
                                text: cartItemCount
                                color: "black"
                                font.pixelSize: 11
                            }
                        }
                    }
                }

                Item { Layout.fillHeight: true } // spacer
            }
        }

        // ---------- Content area ----------
        Loader {
            id: contentLoader
            Layout.fillWidth: true
            Layout.fillHeight: true

            Connections {
                target: contentLoader.item
                function onLogoutRequested() {
                    var sv = dashboardRoot.StackView.view
                    if (sv) {
                        sv.clear()
                        sv.push("Login.qml")
                    }
                }
            }
        }
    }
}
