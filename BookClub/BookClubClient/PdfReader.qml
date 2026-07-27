import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Pdf
import Qt.labs.settings 1.1

Rectangle {
    id: readerRoot
    anchors.fill: parent
    color: "#1A0F1F"

    required property var networkManager
    required property int bookId

    property int userId: 0
    property string title: "Reading"

    property bool loading: true
    property string errorMessage: ""
    property real minZoom: 0.6
    property real maxZoom: 3.0
    property real zoomStep: 0.15

    // Simple local persistence for the last page per book/user.
    property string progressKey: "pdf_reader_last_page_" + userId + "_" + bookId

    PdfDocument {
        id: pdfDoc
    }

    Settings {
        id: readerSettings
        category: "PdfReader"
    }

    function requestPdf() {
        loading = true
        errorMessage = ""

        if (networkManager) {
            networkManager.getBookPdf(userId, bookId)
        } else {
            loading = false
            errorMessage = "No network connection available."
        }
    }

    function clampPage(pageNumber) {
        if (pdfDoc.pageCount <= 0)
            return 0
        return Math.max(0, Math.min(pdfDoc.pageCount - 1, pageNumber))
    }

    function goToPage(pageNumber) {
        if (pdfDoc.status !== PdfDocument.Ready)
            return

        var targetPage = clampPage(pageNumber)
        pageView.currentPage = targetPage
        saveReadingProgress()
    }

    function nextPage() {
        goToPage(pageView.currentPage + 1)
    }

    function previousPage() {
        goToPage(pageView.currentPage - 1)
    }

    function zoomIn() {
        pageView.renderScale = Math.min(maxZoom, pageView.renderScale + zoomStep)
    }

    function zoomOut() {
        pageView.renderScale = Math.max(minZoom, pageView.renderScale - zoomStep)
    }

    function resetZoom() {
        pageView.renderScale = 1.0
    }

    function saveReadingProgress() {
        readerSettings.setValue(progressKey, pageView.currentPage)
    }

    function loadReadingProgress() {
        var savedPage = readerSettings.value(progressKey, 0)
        goToPage(Number(savedPage))
    }

    Component.onCompleted: requestPdf()

    Connections {
        target: networkManager

        function onResponseReceived(action, status, data) {
            if (action !== "getBookPdf")
                return

            var ok = status === "success" || status === "SUCCESS"
            if (!ok) {
                loading = false
                errorMessage = (data && data.message) ? data.message : "Failed to load this book."
                return
            }

            var base64 = data.pdfData || ""
            if (base64 === "") {
                loading = false
                errorMessage = "This book has no PDF file available."
                return
            }

            var localPath = networkManager.saveBase64ToCache(base64, "book_" + bookId + ".pdf")
            if (localPath === "") {
                loading = false
                errorMessage = "Failed to save the PDF locally."
                return
            }

            pdfDoc.source = localPath
        }
    }

    Connections {
        target: pdfDoc

        function onStatusChanged() {
            if (pdfDoc.status === PdfDocument.Ready) {
                loading = false
                errorMessage = ""
                loadReadingProgress()
            } else if (pdfDoc.status === PdfDocument.Error) {
                loading = false
                errorMessage = "Failed to open the PDF file."
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#2D1B33"
            border.color: "#5C3D75"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                spacing: 12

                Button {
                    text: "Back"
                    contentItem: Text {
                        text: "\u2190 " + parent.text
                        color: "#D4AF37"
                        font.pixelSize: 14
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "transparent"
                        radius: 6
                        border.color: "#5C3D75"
                        border.width: 1
                    }
                    onClicked: {
                        saveReadingProgress()
                        var sv = readerRoot.StackView ? readerRoot.StackView.view : null
                        if (sv)
                            sv.pop()
                    }
                }

                Text {
                    text: readerRoot.title
                    color: "#D4AF37"
                    font.bold: true
                    font.pixelSize: 17
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    visible: pdfDoc.status === PdfDocument.Ready
                    text: "Page " + (pageView.currentPage + 1) + " / " + pdfDoc.pageCount
                    color: "#A08EAD"
                    font.pixelSize: 13
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#140C18"

            PdfMultiPageView {
                id: pageView
                anchors.fill: parent
                anchors.margins: 12
                document: pdfDoc
                visible: !readerRoot.loading && readerRoot.errorMessage === ""
                renderScale: 1.0

                ScrollBar.vertical: ScrollBar { }
                ScrollBar.horizontal: ScrollBar { }

                onCurrentPageChanged: {
                    pageSelector.value = currentPage + 1
                    saveReadingProgress()
                }
            }

            ColumnLayout {
                anchors.centerIn: parent
                visible: readerRoot.loading
                spacing: 14

                BusyIndicator {
                    running: readerRoot.loading
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: "Loading book..."
                    color: "#A08EAD"
                    font.pixelSize: 14
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            ColumnLayout {
                anchors.centerIn: parent
                visible: !readerRoot.loading && readerRoot.errorMessage !== ""
                spacing: 14

                Text {
                    text: "!"
                    color: "#FF6666"
                    font.pixelSize: 40
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                Text {
                    text: readerRoot.errorMessage
                    color: "#FF6666"
                    font.pixelSize: 14
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    Layout.maximumWidth: 420
                    Layout.alignment: Qt.AlignHCenter
                }

                Button {
                    text: "Retry"
                    Layout.alignment: Qt.AlignHCenter
                    contentItem: Text {
                        text: parent.text
                        color: "#1A0F1F"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "#D4AF37"
                        radius: 6
                    }
                    onClicked: requestPdf()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 72
            color: "#2D1B33"
            border.color: "#5C3D75"
            border.width: 1
            visible: pdfDoc.status === PdfDocument.Ready && !readerRoot.loading && readerRoot.errorMessage === ""

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 14
                spacing: 10

                Button {
                    text: "Previous"
                    enabled: pageView.currentPage > 0
                    contentItem: Text {
                        text: parent.text
                        color: "#1A0F1F"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: parent.enabled ? "#D4AF37" : "#6E6374"
                        radius: 6
                    }
                    onClicked: previousPage()
                }

                Button {
                    text: "Next"
                    enabled: pageView.currentPage < pdfDoc.pageCount - 1
                    contentItem: Text {
                        text: parent.text
                        color: "#1A0F1F"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: parent.enabled ? "#D4AF37" : "#6E6374"
                        radius: 6
                    }
                    onClicked: nextPage()
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                    color: "#5C3D75"
                    opacity: 0.8
                }

                Text {
                    text: "Go to page"
                    color: "#D4AF37"
                    font.pixelSize: 13
                }

                SpinBox {
                    id: pageSelector
                    from: 1
                    to: Math.max(1, pdfDoc.pageCount)
                    value: 1
                    editable: true
                    Layout.preferredWidth: 90

                    onValueModified: goToPage(value - 1)

                    contentItem: TextInput {
                        text: pageSelector.textFromValue(pageSelector.value, pageSelector.locale)
                        color: "#D4AF37"
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        font.pixelSize: 13
                        selectedTextColor: "#1A0F1F"
                        selectionColor: "#D4AF37"
                        readOnly: !pageSelector.editable
                        validator: pageSelector.validator
                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                    }

                    background: Rectangle {
                        radius: 6
                        color: "#1A0F1F"
                        border.color: "#D4AF37"
                        border.width: 1
                    }
                }

                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                    color: "#5C3D75"
                    opacity: 0.8
                }

                Button {
                    text: "-"
                    Layout.preferredWidth: 38
                    contentItem: Text {
                        text: parent.text
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "#3D2B43"
                        radius: 6
                    }
                    onClicked: zoomOut()
                }

                Text {
                    text: Math.round(pageView.renderScale * 100) + "%"
                    color: "#D4AF37"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    Layout.preferredWidth: 56
                }

                Button {
                    text: "+"
                    Layout.preferredWidth: 38
                    contentItem: Text {
                        text: parent.text
                        color: "#D4AF37"
                        font.pixelSize: 18
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "#3D2B43"
                        radius: 6
                    }
                    onClicked: zoomIn()
                }

                Button {
                    text: "100%"
                    contentItem: Text {
                        text: parent.text
                        color: "#1A0F1F"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "#D4AF37"
                        radius: 6
                    }
                    onClicked: resetZoom()
                }
            }
        }
    }
}
