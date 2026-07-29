import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Pdf

Rectangle {
    id: readerRoot
    anchors.fill: parent
    color: "#140C18"

    // --- Required Properties ---
    required property var networkManager
    required property int bookId
    required property int userId
    property string title: "Reading"

    // --- Internal State ---
    property bool loading: true
    property string errorMessage: ""
    property real minZoom: 0.6
    property real maxZoom: 3.0
    property real zoomStep: 0.15
    property bool isFullyLoaded: false
    property int savedPage: 0

    // --- Document ---
    PdfDocument {
        id: pdfDoc
    }

    // --- Helpers ---

    // Request the PDF and the saved page from the server
    function requestPdf() {
        loading = true
        errorMessage = ""
        readerRoot.isFullyLoaded = false
        if (networkManager) {
            // First, fetch the last saved page for this book
            networkManager.getReadingProgress(userId, bookId)
        } else {
            loading = false
            errorMessage = "No network connection available."
        }
    }

    // Clamp page number to valid range
    function clampPage(pageNumber) {
        if (pdfDoc.pageCount <= 0) return 0
        return Math.max(0, Math.min(pdfDoc.pageCount - 1, pageNumber))
    }

    // Navigate to a specific page and trigger save (if fully loaded)
    function goToPage(pageNumber) {
        if (pdfDoc.status !== PdfDocument.Ready) return
        var target = clampPage(pageNumber)
        pageView.goToPage(target)
        pageSelector.value = target + 1
        if (readerRoot.isFullyLoaded) {
            saveDebounce.restart()
        }
    }

    // Debounced save to avoid frequent database writes during fast scrolling
    Timer {
        id: saveDebounce
        interval: 1000
        repeat: false
        onTriggered: {
            if (pdfDoc.status === PdfDocument.Ready && readerRoot.isFullyLoaded) {
                networkManager.saveReadingProgress(userId, bookId, pageView.currentPage)
                console.log("Progress saved to DB: book", bookId, "page", pageView.currentPage)
            }
        }
    }

    // Load the PDF from Base64 data after receiving the saved page
    function loadPdfFromBase64(base64Data) {
        var localPath = networkManager.saveBase64ToCache(base64Data, "book_" + bookId + ".pdf")
        if (localPath === "") {
            loading = false
            errorMessage = "Failed to save the PDF locally."
            return
        }
        pdfDoc.source = localPath
    }

    // --- Network Connections ---

    Component.onCompleted: requestPdf()

    // Save progress when the reader is closed (app exit or popping from StackView)
    Component.onDestruction: {
        if (pdfDoc.status === PdfDocument.Ready && readerRoot.isFullyLoaded) {
            networkManager.saveReadingProgress(userId, bookId, pageView.currentPage)
        }
    }

    Connections {
        target: networkManager

        // Handle response for getReadingProgress and getBookPdf
        function onResponseReceived(action, status, data) {
            var ok = (status === "success" || status === "SUCCESS")

            if (action === "getReadingProgress" && ok) {
                var page = data.page || 0
                readerRoot.savedPage = page
                console.log("Retrieved saved page from DB:", page)
                // Now fetch the actual PDF
                networkManager.getBookPdf(userId, bookId)
            }
            else if (action === "getBookPdf" && ok) {
                var base64 = data.pdfData || ""
                if (base64 === "") {
                    loading = false
                    errorMessage = "This book has no PDF file available."
                    return
                }
                loadPdfFromBase64(base64)
            }
            else if (action === "getReadingProgress" && !ok) {
                // On failure, fallback to page 0
                readerRoot.savedPage = 0
                networkManager.getBookPdf(userId, bookId)
            }
            else if (action === "getBookPdf" && !ok) {
                loading = false
                errorMessage = (data && data.message) ? data.message : "Failed to load this book."
            }
            else if (action === "saveReadingProgress") {
                if (!ok) console.warn("Failed to save reading progress to DB")
            }
        }
    }

    Connections {
        target: pdfDoc

        // When PDF is ready, restore the saved page
        function onStatusChanged() {
            if (pdfDoc.status === PdfDocument.Ready) {
                loading = false
                errorMessage = ""
                var target = clampPage(readerRoot.savedPage)
                pageView.goToPage(target)
                pageSelector.value = target + 1
                readerRoot.isFullyLoaded = true
                console.log("PDF loaded, restored to page:", target)
            } else if (pdfDoc.status === PdfDocument.Error) {
                loading = false
                errorMessage = "Failed to open the PDF file."
            }
        }
    }

    // --- UI Components (unchanged from original) ---

    component IconButton: Button {
        id: iconBtn
        property string glyph: "?"
        property bool prominent: false
        implicitWidth: 42
        implicitHeight: 42

        contentItem: Text {
            text: iconBtn.glyph
            color: iconBtn.prominent ? "#1A0F1F" : "#D4AF37"
            font.pixelSize: 18
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        background: Rectangle {
            radius: 21
            color: iconBtn.prominent
                   ? (iconBtn.pressed ? "#B8952E" : "#D4AF37")
                   : (iconBtn.hovered ? "#3D2B4A" : "#2D1B33")
            border.color: "#5C3D75"
            border.width: iconBtn.prominent ? 0 : 1
            Behavior on color { ColorAnimation { duration: 120 } }
        }
        scale: pressed ? 0.92 : 1.0
        Behavior on scale { NumberAnimation { duration: 90 } }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        //  Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#241530"
            gradient: Gradient {
                orientation: Gradient.Horizontal
                GradientStop { position: 0.0; color: "#241530" }
                GradientStop { position: 1.0; color: "#2D1B3D" }
            }
            Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: "#5C3D75"; opacity: 0.6 }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14; anchors.rightMargin: 18; spacing: 14

                IconButton {
                    glyph: "\u2190"
                    onClicked: {
                        if (pdfDoc.status === PdfDocument.Ready && readerRoot.isFullyLoaded) {
                            networkManager.saveReadingProgress(userId, bookId, pageView.currentPage)
                        }
                        var sv = readerRoot.StackView ? readerRoot.StackView.view : null
                        if (sv) sv.pop()
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true; spacing: 1
                    Text {
                        text: readerRoot.title; color: "#D4AF37"; font.bold: true
                        font.pixelSize: 16; elide: Text.ElideRight; Layout.fillWidth: true
                    }
                    Text {
                        visible: pdfDoc.status === PdfDocument.Ready
                        text: "Page " + (pageView.currentPage + 1) + " of " + pdfDoc.pageCount
                        color: "#A08EAD"; font.pixelSize: 11
                    }
                }

                Rectangle {
                    visible: pdfDoc.status === PdfDocument.Ready
                    color: "#1A0F1F"; radius: 14; implicitHeight: 28; implicitWidth: zoomPct.implicitWidth + 20
                    border.color: "#5C3D75"; border.width: 1
                    Text { id: zoomPct; anchors.centerIn: parent; text: Math.round(pageView.renderScale * 100) + "%"; color: "#D4AF37"; font.pixelSize: 12; font.bold: true }
                }
            }
        }

        //  Reader View Area
        Rectangle {
            Layout.fillWidth: true; Layout.fillHeight: true; color: "#0F090F"

            PdfMultiPageView {
                id: pageView
                anchors.fill: parent; anchors.margins: 16
                document: pdfDoc
                visible: !readerRoot.loading && readerRoot.errorMessage === ""
                renderScale: 1.0

                ScrollBar.vertical: ScrollBar { }
                ScrollBar.horizontal: ScrollBar { }

                onCurrentPageChanged: {
                    if (readerRoot.isFullyLoaded) {
                        pageSelector.value = currentPage + 1
                        saveDebounce.restart()
                    }
                }
            }

            // Loading State
            ColumnLayout {
                anchors.centerIn: parent; visible: readerRoot.loading; spacing: 16
                BusyIndicator { running: readerRoot.loading; Layout.alignment: Qt.AlignHCenter }
                Text { text: "Loading your book..."; color: "#A08EAD"; font.pixelSize: 14; Layout.alignment: Qt.AlignHCenter }
            }

            // Error State
            ColumnLayout {
                anchors.centerIn: parent; visible: !readerRoot.loading && readerRoot.errorMessage !== ""; spacing: 16
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter; width: 64; height: 64; radius: 32; color: "#3A1E1E"; border.color: "#FF6666"; border.width: 1
                    Text { anchors.centerIn: parent; text: "!"; color: "#FF6666"; font.pixelSize: 30; font.bold: true }
                }
                Text {
                    text: readerRoot.errorMessage; color: "#FF8888"; font.pixelSize: 14; wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter; Layout.maximumWidth: 420; Layout.alignment: Qt.AlignHCenter
                }
                Button {
                    text: "Retry"; Layout.alignment: Qt.AlignHCenter; Layout.preferredWidth: 140; Layout.preferredHeight: 42
                    contentItem: Text { text: parent.text; color: "#1A0F1F"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { color: "#D4AF37"; radius: 8 }
                    onClicked: requestPdf()
                }
            }
        }

        //  Footer Controls
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 76; color: "#241530"
            visible: pdfDoc.status === PdfDocument.Ready && !readerRoot.loading && readerRoot.errorMessage === ""
            Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: "#5C3D75"; opacity: 0.6 }

            RowLayout {
                anchors.fill: parent; anchors.leftMargin: 18; anchors.rightMargin: 18; spacing: 14

                IconButton {
                    glyph: "\u276E"; enabled: pageView.currentPage > 0; opacity: enabled ? 1.0 : 0.4
                    onClicked: goToPage(pageView.currentPage - 1)
                }

                Text { text: "Page"; color: "#A08EAD"; font.pixelSize: 12 }

                SpinBox {
                    id: pageSelector; from: 1; to: Math.max(1, pdfDoc.pageCount); value: 1; editable: true; Layout.preferredWidth: 70
                    contentItem: TextInput {
                        text: pageSelector.textFromValue(pageSelector.value, pageSelector.locale)
                        color: "#D4AF37"; horizontalAlignment: Qt.AlignHCenter; verticalAlignment: Qt.AlignVCenter
                        font.pixelSize: 13; selectedTextColor: "#1A0F1F"; selectionColor: "#D4AF37"
                        readOnly: !pageSelector.editable; validator: pageSelector.validator; inputMethodHints: Qt.ImhFormattedNumbersOnly
                    }
                    background: Rectangle { radius: 8; color: "#1A0F1F"; border.color: "#5C3D75"; border.width: 1 }
                }

                Button {
                    text: "Go"; Layout.preferredWidth: 50; Layout.preferredHeight: 36
                    contentItem: Text { text: parent.text; color: "#1A0F1F"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                    background: Rectangle { color: "#D4AF37"; radius: 8 }
                    onClicked: goToPage(pageSelector.value - 1)
                }

                IconButton {
                    glyph: "\u276F"; enabled: pageView.currentPage < pdfDoc.pageCount - 1; opacity: enabled ? 1.0 : 0.4
                    onClicked: goToPage(pageView.currentPage + 1)
                }

                Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; Layout.topMargin: 10; Layout.bottomMargin: 10; color: "#5C3D75"; opacity: 0.6 }

                IconButton { glyph: "\u2212"; onClicked: pageView.renderScale = Math.max(minZoom, pageView.renderScale - zoomStep) }
                IconButton { glyph: "\u21BB"; onClicked: pageView.renderScale = 1.0 }
                IconButton { glyph: "\uFF0B"; onClicked: pageView.renderScale = Math.min(maxZoom, pageView.renderScale + zoomStep) }

                Item { Layout.fillWidth: true }
            }
        }
    }
}