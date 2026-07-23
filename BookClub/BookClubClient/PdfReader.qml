import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtPdf 6.5

Rectangle {
    id: pdfReaderRoot
    width: 1024
    height: 768
    color: "#2D1B33"

    // --- C++ Integration Properties ---
    property string pdfUrl: (typeof bookManager !== "undefined") ? bookManager.currentPdfPath : "file:///C:/sample.pdf"
    property int bookId: (typeof bookManager !== "undefined") ? bookManager.currentBookId : 0

    signal exitReader()

    // Timer to debounce saving the last read page to prevent overloading the database/network
    Timer {
        id: saveDelayTimer
        interval: 1000
        repeat: false
        onTriggered: {
            if (typeof bookManager !== "undefined") {
                bookManager.saveLastReadPage(pdfReaderRoot.bookId, pdfView.currentPage);
            } else {
                console.log("C++ Interface: Saving page " + pdfView.currentPage + " for book " + pdfReaderRoot.bookId);
            }
        }
    }

    PdfDocument {
        id: pdfDocument
        source: pdfReaderRoot.pdfUrl

        onStatusChanged: {
            if (status === PdfDocument.Ready) {
                // Restore last read page from backend settings
                if (typeof bookManager !== "undefined") {
                    var lastPage = bookManager.getLastReadPage(pdfReaderRoot.bookId);
                    pdfView.currentPage = Math.min(lastPage, pdfDocument.pageCount - 1);
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 1. Top Control Bar (Flat Comic Theme)
        Rectangle {
            Layout.fillWidth: true
            height: 60
            color: "#1E1222"
            border.color: "#D4AF37"
            border.width: 2

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 15
                anchors.rightMargin: 15
                spacing: 15

                // Close Button
                Button {
                    id: closeBtn
                    text: "✖ Close"
                    onClicked: {
                        if (typeof bookManager !== "undefined") {
                            bookManager.saveLastReadPage(pdfReaderRoot.bookId, pdfView.currentPage);
                        }
                        pdfReaderRoot.exitReader();
                    }

                    contentItem: Text {
                        text: closeBtn.text
                        color: "#D4AF37"
                        font.bold: true
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        implicitWidth: 80
                        implicitHeight: 35
                        color: closeBtn.down ? "#1E1222" : "#2D1B33"
                        border.color: "#D4AF37"
                        border.width: 2
                        radius: 4
                    }
                }

                // Page Navigation (Prev / Next & Direct Jump)
                Row {
                    spacing: 5
                    Layout.alignment: Qt.AlignVCenter

                    Button {
                        id: prevBtn
                        text: "◀"
                        enabled: pdfView.currentPage > 0
                        onClicked: pdfView.currentPage--

                        background: Rectangle {
                            implicitWidth: 35
                            implicitHeight: 35
                            color: !parent.enabled ? "#444" : (parent.down ? "#1E1222" : "#2D1B33")
                            border.color: "#D4AF37"
                            border.width: 2
                            radius: 4
                        }
                        contentItem: Text {
                            text: prevBtn.text
                            color: prevBtn.enabled ? "#D4AF37" : "#888"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    TextField {
                        id: pageInput
                        width: 55
                        height: 35
                        text: (pdfView.currentPage + 1).toString()
                        color: "#D4AF37"
                        font.bold: true
                        horizontalAlignment: TextInput.AlignHCenter
                        selectByMouse: true
                        validator: IntValidator { bottom: 1; top: pdfDocument.pageCount }

                        background: Rectangle {
                            color: "#1E1222"
                            border.color: "#D4AF37"
                            border.width: 2
                            radius: 4
                        }

                        onAccepted: {
                            var targetPage = parseInt(text) - 1;
                            if (targetPage >= 0 && targetPage < pdfDocument.pageCount) {
                                pdfView.currentPage = targetPage;
                            } else {
                                text = (pdfView.currentPage + 1).toString();
                            }
                        }
                    }

                    Text {
                        text: "/  " + pdfDocument.pageCount
                        color: "#FFF"
                        font.bold: true
                        font.pixelSize: 14
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Button {
                        id: nextBtn
                        text: "▶"
                        enabled: pdfView.currentPage < pdfDocument.pageCount - 1
                        onClicked: pdfView.currentPage++

                        background: Rectangle {
                            implicitWidth: 35
                            implicitHeight: 35
                            color: !parent.enabled ? "#444" : (parent.down ? "#1E1222" : "#2D1B33")
                            border.color: "#D4AF37"
                            border.width: 2
                            radius: 4
                        }
                        contentItem: Text {
                            text: nextBtn.text
                            color: nextBtn.enabled ? "#D4AF37" : "#888"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                Item { Layout.fillWidth: true }

                // Zoom Level Controls
                Row {
                    spacing: 10
                    Layout.alignment: Qt.AlignVCenter

                    Button {
                        id: zoomOutBtn
                        text: "Zoom -"
                        onClicked: pdfView.zoomFactor = Math.max(0.5, pdfView.zoomFactor - 0.1)

                        background: Rectangle {
                            implicitWidth: 75
                            implicitHeight: 35
                            color: zoomOutBtn.down ? "#1E1222" : "#2D1B33"
                            border.color: "#D4AF37"
                            border.width: 2
                            radius: 4
                        }
                        contentItem: Text {
                            text: zoomOutBtn.text
                            color: "#D4AF37"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Text {
                        text: Math.round(pdfView.zoomFactor * 100) + "%"
                        color: "white"
                        font.bold: true
                        font.pixelSize: 14
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Button {
                        id: zoomInBtn
                        text: "Zoom +"
                        onClicked: pdfView.zoomFactor = Math.min(3.0, pdfView.zoomFactor + 0.1)

                        background: Rectangle {
                            implicitWidth: 75
                            implicitHeight: 35
                            color: zoomInBtn.down ? "#1E1222" : "#2D1B33"
                            border.color: "#D4AF37"
                            border.width: 2
                            radius: 4
                        }
                        contentItem: Text {
                            text: zoomInBtn.text
                            color: "#D4AF37"
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }

        // 2. Main PDF View Area
        PdfMultiPageView {
            id: pdfView
            Layout.fillWidth: true
            Layout.fillHeight: true
            document: pdfDocument
            renderStrategy: PdfPageView.NoAntialiasing

            onCurrentPageChanged: {
                pageInput.text = (pdfView.currentPage + 1).toString();
                saveDelayTimer.restart();
            }
        }
    }
}
