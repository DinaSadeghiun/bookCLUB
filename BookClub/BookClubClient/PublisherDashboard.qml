import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
   id: publisherRoot
   width: parent ? parent.width : 1280
   height: parent ? parent.height : 720
   color: "#2D1B33"

   // --- State Properties ---
   property string publisherUsername: "publisher_user"
   property string publisherPassword: "password123"
   property string favoriteAuthor: "Stephen King" // For password recovery

   property real totalRevenue: 3450.75
   property int totalBooksPublished: 18 // Total books (active + inactive)

   // Editing State Variables
   property bool isEditing: false
   property int editingBookId: -1

   // --- Signals for Backend ---
   signal publishBook(string title, string author, string genre, string desc, real price, real discount, string coverPath, string pdfPath)
   signal updateBook(int bookId, string title, string author, string genre, string desc, real price, real discount, string coverPath, string pdfPath)
   signal toggleBookStatus(int bookId, bool active)
   signal updateProfile(string username, string password, string favAuthor)

   // --- Mock Data Models for UI ---
   ListModel {
       id: booksModel
       ListElement { bookId: 1; title: "The Dark Tower"; author: "Stephen King"; genre: "Fantasy"; desc: "A great epic journey."; price: 19.99; discount: 10.0; rating: 4.8; active: true; sales: 120 }
       ListElement { bookId: 2; title: "It"; author: "Stephen King"; genre: "Mystery"; desc: "A horror masterpiece."; price: 24.99; discount: 0.0; rating: 4.9; active: true; sales: 310 }
       ListElement { bookId: 3; title: "Pet Sematary"; author: "Stephen King"; genre: "Fiction"; desc: "Sometimes dead is better."; price: 14.99; discount: 15.0; rating: 4.2; active: false; sales: 45 }
       ListElement { bookId: 4; title: "Misery"; author: "Stephen King"; genre: "Fiction"; desc: "A novel about obsessed fan."; price: 12.99; discount: 5.0; rating: 4.6; active: true; sales: 15 }
       ListElement { bookId: 5; title: "The Stand"; author: "Stephen King"; genre: "Sci-Fi"; desc: "A post-apocalyptic vision."; price: 29.99; discount: 20.0; rating: 4.7; active: true; sales: 5 }
   }

   RowLayout {
       anchors.fill: parent
       spacing: 0

       // --- Left Sidebar (Navigation) ---
       Rectangle {
           Layout.fillHeight: true
           Layout.preferredWidth: 250
           color: "#1A0F1F"

           ColumnLayout {
               anchors.fill: parent
               anchors.margins: 20
               spacing: 20

               Image {
                   source: "qrc:/assets/images/giraffe.png"
                   Layout.preferredWidth: 80
                   Layout.preferredHeight: 80
                   Layout.alignment: Qt.AlignHCenter
               }

               Text {
                   text: publisherUsername
                   color: "#D4AF37" // Gold
                   font.bold: true
                   font.pixelSize: 18
                   Layout.alignment: Qt.AlignHCenter
               }

               Rectangle { Layout.preferredHeight: 2; Layout.fillWidth: true; color: "#3D2B43" }

               // Sidebar Navigation
               Repeater {
                   model: ["Statistics", "My Books", "Add New Book", "Account Settings"]
                   delegate: Button {
                       required property int index
                       required property string modelData
                       text: modelData
                       Layout.fillWidth: true
                       flat: true
                       onClicked: {
                           if (index === 2) {
                               isEditing = false;
                               clearForm();
                           }
                           mainStack.currentIndex = index;
                       }
                       contentItem: Text {
                           text: parent.text
                           color: (mainStack.currentIndex === parent.index) ? "#D4AF37" : "#FFFFFF"
                           horizontalAlignment: Text.AlignLeft
                           font.pixelSize: 16
                       }
                   }
               }

               Item { Layout.fillHeight: true } // Spacer

               Button {
                   text: "Logout"
                   Layout.fillWidth: true
                   onClicked: {
                       // Insert logout navigation logic here
                       console.log("Logged out");
                   }
               }
           }
       }

       // --- Main Content Area ---
       StackLayout {
           id: mainStack
           Layout.fillWidth: true
           Layout.fillHeight: true
           currentIndex: 0

           // 1. STATISTICS VIEW (Section 3-3)
           ScrollView {
               ColumnLayout {
                   width: parent.width - 40
                   spacing: 30
                   anchors.margins: 20

                   Text { text: "Publisher Dashboard Statistics"; color: "#D4AF37"; font.pixelSize: 28; font.bold: true }

                   RowLayout {
                       spacing: 20
                       StatCard { title: "Total Revenue"; value: "$" + totalRevenue.toFixed(2); icon: "💰" }
                       StatCard { title: "Total Published Books"; value: totalBooksPublished.toString(); icon: "📚" }
                   }

                   // Top 5 Best Sellers Chart
                   Text { text: "Top 5 Best Selling Books (Sales Count)"; color: "#FFF"; font.pixelSize: 20 }
                   RowLayout {
                       height: 200
                       spacing: 40
                       Layout.leftMargin: 20
                       // Visual representation using simulated bars from Mock data
                       Repeater {
                           model: booksModel
                           Rectangle {
                               width: 50
                               height: Math.max(20, (model.sales / 350) * 150) // Scale dynamically
                               color: "#D4AF37"
                               Layout.alignment: Qt.AlignBottom
                               Text {
                                   text: model.sales + " sales"
                                   color: "#FFF"
                                   font.pixelSize: 11
                                   anchors.bottom: parent.top
                                   anchors.horizontalCenter: parent.horizontalCenter
                               }
                               Text {
                                   text: model.title
                                   color: "#FFF"
                                   font.pixelSize: 10
                                   anchors.top: parent.bottom
                                   anchors.horizontalCenter: parent.horizontalCenter
                                   wrapMode: Text.Wrap
                                   width: 80
                                   horizontalAlignment: Text.AlignHCenter
                               }
                           }
                       }
                   }

                   Item { Layout.preferredHeight: 30 } // Space between charts

                   // Bottom 5 Low Sellers Chart
                   Text { text: "Bottom 5 Low Selling Books (Sales Count)"; color: "#FFF"; font.pixelSize: 20 }
                   RowLayout {
                       height: 200
                       spacing: 40
                       Layout.leftMargin: 20
                       Repeater {
                           model: booksModel
                           Rectangle {
                               width: 50
                               height: Math.max(20, ((350 - model.sales) / 350) * 150) // Inverted mock sales representation
                               color: "#7B1FA2" // Purple indicator for low sellers
                               Layout.alignment: Qt.AlignBottom
                               Text {
                                   text: Math.round(model.sales) + " sales"
                                   color: "#FFF"
                                   font.pixelSize: 11
                                   anchors.bottom: parent.top
                                   anchors.horizontalCenter: parent.horizontalCenter
                               }
                               Text {
                                   text: model.title
                                   color: "#FFF"
                                   font.pixelSize: 10
                                   anchors.top: parent.bottom
                                   anchors.horizontalCenter: parent.horizontalCenter
                                   wrapMode: Text.Wrap
                                   width: 80
                                   horizontalAlignment: Text.AlignHCenter
                               }
                           }
                       }
                   }
               }
           }

           // 2. MY BOOKS LIST VIEW (Section 3-2 b, c, d)
           ListView {
               model: booksModel
               clip: true
               header: Text { text: "Manage My Published Books"; color: "#D4AF37"; font.pixelSize: 24; padding: 20 }
               delegate: Rectangle {
                   width: parent.width - 40; height: 110; color: "#3D2B43"; radius: 10
                   anchors.horizontalCenter: parent.horizontalCenter
                   anchors.margins: 10

                   RowLayout {
                       anchors.fill: parent; anchors.margins: 15
                       Rectangle { width: 60; height: 80; color: "#555"; radius: 5 } // Simulated Cover

                       ColumnLayout {
                           Text { text: model.title; color: "#FFF"; font.bold: true; font.pixelSize: 16 }
                           Text { text: "Author: " + model.author + " | Genre: " + model.genre; color: "#BBB"; font.pixelSize: 12 }
                           RowLayout {
                               Text { text: "Price: $" + model.price; color: "#D4AF37" }
                               Text { text: "Discount: " + model.discount + "%"; color: "#FF5252"; visible: model.discount > 0 }
                               Text { text: "Avg Rating: ⭐ " + model.rating; color: "#FFD700" } // Individual book rating
                           }
                       }
                       Item { Layout.fillWidth: true }

                       RowLayout {
                           spacing: 10
                           // Edit Button - Transitions to Add New Book view with data
                           Button {
                               text: "Edit"
                               onClicked: {
                                   isEditing = true;
                                   editingBookId = model.bookId;
                                   titleIn.text = model.title;
                                   authorIn.text = model.author;
                                   genreIn.currentIndex = genreIn.find(model.genre);
                                   descIn.text = model.desc;
                                   priceIn.text = model.price.toString();
                                   discountIn.text = model.discount.toString();
                                   coverLabel.text = "Existing Cover Loaded";
                                   pdfLabel.text = "Existing PDF Loaded";
                                   mainStack.currentIndex = 2; // Move to Add New Book screen (index 2)
                               }
                           }
                           Button {
                               text: model.active ? "Deactivate" : "Activate"
                               onClicked: {
                                   model.active = !model.active;
                                   toggleBookStatus(model.bookId, model.active);
                               }
                           }
                       }
                   }
               }
           }

           // 3. ADD / EDIT NEW BOOK FORM (Section 3-2 a, b)
           ScrollView {
               ColumnLayout {
                   width: parent.width - 100
                   anchors.horizontalCenter: parent.horizontalCenter
                   spacing: 15

                   Text {
                       text: isEditing ? "Edit Book Details" : "Publish a New Book"
                       color: "#D4AF37"
                       font.pixelSize: 24
                       Layout.topMargin: 20
                   }

                   CustomTextField { id: titleIn; placeholder: "Book Title"; Layout.fillWidth: true }
                   CustomTextField { id: authorIn; placeholder: "Author Name"; Layout.fillWidth: true }

                   ComboBox {
                       id: genreIn
                       model: ["NonFiction", "Mystery", "Romance", "SciFi", "Fantasy", "Biography", "History", "SelfHelp", "Poetry", "Children", "Other"]
                       Layout.fillWidth: true
                   }

                   TextArea {
                       id: descIn
                       placeholderText: "Description..."
                       Layout.fillWidth: true; Layout.preferredHeight: 100
                       background: Rectangle { color: "#3D2B43"; border.color: "#D4AF37" }
                       color: "white"
                   }

                   RowLayout {
                       CustomTextField { id: priceIn; placeholder: "Price ($)"; Layout.fillWidth: true }
                       CustomTextField { id: discountIn; placeholder: "Discount (%)"; Layout.fillWidth: true }
                   }

                   RowLayout {
                       Layout.fillWidth: true
                       Button {
                           text: "Upload Cover Image"
                           onClicked: coverLabel.text = "selected_cover.jpg"
                       }
                       Text { id: coverLabel; text: "No cover selected"; color: "#BBB" }
                   }

                   RowLayout {
                       Layout.fillWidth: true
                       Button {
                           text: "Upload PDF File"
                           onClicked: pdfLabel.text = "selected_book.pdf"
                       }
                       Text { id: pdfLabel; text: "No PDF selected"; color: "#BBB" }
                   }

                   Button {
                       text: isEditing ? "SAVE CHANGES" : "PUBLISH BOOK"
                       highlighted: true
                       Layout.fillWidth: true; Layout.preferredHeight: 50
                       onClicked: {
                           if (isEditing) {
                               updateBook(editingBookId, titleIn.text, authorIn.text, genreIn.currentText, descIn.text, parseFloat(priceIn.text), parseFloat(discountIn.text), coverLabel.text, pdfLabel.text);
                               isEditing = false;
                           } else {
                               publishBook(titleIn.text, authorIn.text, genreIn.currentText, descIn.text, parseFloat(priceIn.text), parseFloat(discountIn.text), coverLabel.text, pdfLabel.text);
                           }
                           clearForm();
                           mainStack.currentIndex = 1; // Return to My Books list
                       }
                   }
               }
           }

           // 4. ACCOUNT SETTINGS (Section 3-1: Password & Favourite Author Security)
           // --- بخش تنظیمات حساب در StackLayout ---
           ColumnLayout {
               width: parent.width - 100
               anchors.centerIn: parent
               spacing: 15

               Text {
                   text: "Account Settings";
                   color: "#D4AF37";
                   font.pixelSize: 24;
                   font.bold: true;
                   Layout.alignment: Qt.AlignHCenter
               }

               // فیلد نام کاربری
               ColumnLayout {
                   spacing: 5; Layout.fillWidth: true
                   Text { text: "Username:"; color: "#BBB"; font.pixelSize: 14 }
                   CustomTextField { id: profUser; text: publisherUsername; Layout.fillWidth: true }
               }

               // فیلد نویسنده مورد علاقه (اختیاری بر اساس طراحی شما)
               ColumnLayout {
                   spacing: 5; Layout.fillWidth: true
                   Text { text: "Favorite Author:"; color: "#BBB"; font.pixelSize: 14 }
                   CustomTextField { id: profFavAuthor; text: favoriteAuthor; Layout.fillWidth: true }
               }

               Rectangle { Layout.preferredHeight: 1; Layout.fillWidth: true; color: "#3D2B43"; Layout.topMargin:10 ; Layout.bottomMargin: 10}


               Text { text: "Change Password"; color: "#D4AF37"; font.pixelSize: 18; font.bold: true }

               // فیلد رمز عبور فعلی
               ColumnLayout {
                   spacing: 5; Layout.fillWidth: true
                   Text { text: "Current Password:"; color: "#BBB"; font.pixelSize: 14 }
                   CustomTextField {
                       id: currentPassIn;
                       echoMode: TextInput.Password;
                       placeholder: "Enter current password to authorize changes";
                       Layout.fillWidth: true
                   }
               }

               // فیلد رمز عبور جدید
               ColumnLayout {
                   spacing: 5; Layout.fillWidth: true
                   Text { text: "New Password:"; color: "#BBB"; font.pixelSize: 14 }
                   CustomTextField { id: newPassIn; echoMode: TextInput.Password; placeholder: "Leave blank to keep current"; Layout.fillWidth: true }
               }

               Text {
                   id: errorMsg
                   color: "#FF4444"
                   font.pixelSize: 12
                   visible: false
               }

               Button {
                   text: "SAVE ALL CHANGES"
                   Layout.fillWidth: true
                   Layout.preferredHeight: 45
                   onClicked: {
                       // منطق امنیتی تغییر رمز
                       if (currentPassIn.text !== publisherPassword) {
                           errorMsg.text = "Error: Current password is incorrect!";
                           errorMsg.visible = true;
                       } else {
                           // تایید شد - اعمال تغییرات
                           publisherUsername = profUser.text;
                           favoriteAuthor = profFavAuthor.text;

                           if (newPassIn.text !== "") {
                               publisherPassword = newPassIn.text;
                           }

                           errorMsg.text = "Settings updated successfully!";
                           errorMsg.color = "#44FF44";
                           errorMsg.visible = true;
                           currentPassIn.text = "";
                           newPassIn.text = "";

                           // ارسال سیگنال به بک اند (اختیاری)
                           updateProfile(publisherUsername, publisherPassword, favoriteAuthor);
                       }
                   }
               }
           } // پایان ColumnLayout مربوط به Account Settings
       } // پایان StackLayout (mainStack)
   } // پایان RowLayout اصلی

   // --- Helper Functions ---
   function clearForm() {
       titleIn.text = "";
       authorIn.text = "";
       genreIn.currentIndex = 0;
       descIn.text = "";
       priceIn.text = "";
       discountIn.text = "";
       coverLabel.text = "No cover selected";
       pdfLabel.text = "No PDF selected";
   }

   // --- Reusable UI Elements ---
   component StatCard : Rectangle {
       property string title: ""
       property string value: ""
       property string icon: ""
       Layout.preferredWidth: 250; Layout.preferredHeight: 100; color: "#3D2B43"; radius: 10
       border.color: "#D4AF37"; border.width: 1
       ColumnLayout {
           anchors.centerIn: parent
           Text { text: icon + " " + title; color: "#BBB"; font.pixelSize: 14 }
           Text { text: value; color: "#D4AF37"; font.pixelSize: 22; font.bold: true }
       }
   }

   component CustomTextField : TextField {
       id: tf
       property string placeholder: ""
       placeholderText: placeholder
       color: "white"
       placeholderTextColor: "#888"
       background: Rectangle {
           color: "#3D2B43"
           border.color: tf.activeFocus ? "#D4AF37" : "#555"
           border.width: 2
           radius: 5
       }
   }
} // پایان Rectangle اصلی (publisherRoot)
