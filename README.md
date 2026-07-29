# 📚 BookClub - Online Book Store Application

A client-server book store application built with **Qt/C++** for the backend server and **QML** for the client UI, communicating over TCP sockets using a JSON-based request/response protocol, with a SQLite database on the server side.

---

## 🏗️ Architecture

```
QML Client  ⇄  JSON over TCP Socket  ⇄  C++ Server  ⇄  SQLite Database
```

The server is organized in layers:

- **DB (Repository) Layer** – direct database access (users, books, orders, personal library, notifications, comments, publishers, discounts, shopping cart, admin operations)
- **Services Layer** – business logic on top of the repositories (validation, rules such as "a book must be purchased before it can be marked as favorite")
- **Network Layer** – `BookClubServer` and `ClientHandler`, which parse incoming JSON actions, route them to the right service, and send back a JSON response

On the client side, `NetworkManager` sends JSON requests over the socket and exposes a `responseReceived(action, status, data)` signal that the QML views listen to.

---

## 🚀 Features

### 👤 User
- Register / login, change username and password
- Change/update a security answer (favorite author) used for account recovery
- Browse and search books, filter by genre
- Personalized home dashboard:
  - Recommended books based on the user's favorite genres
  - Books grouped by genre
  - New releases, best sellers, and free books
- Add books to:
  - Shopping cart, and check out
  - Wishlist (only allowed for books not yet purchased)
  - Favorites (only allowed for books the user has already purchased)
- Personal library view (Purchased books / Wishlist / Favorites)
- Order history with per-order date, book list, and pricing (raw price, discount, final price)
- In-app PDF reader with page navigation, zoom, and automatic saving/restoring of the last page read
- Notifications

### 📘 Publisher
- Manage published books
- Apply discounts
- View sales-related data

### 🛡️ Admin
- Manage users, publishers, books, and comments

---

## 🛠️ Tech Stack

| Component        | Technology                        |
|-------------------|------------------------------------|
| Language          | C++, QML/JavaScript                |
| Framework         | Qt (Qt Quick / QML, Qt Network, Qt SQL, Qt PDF) |
| Database          | SQLite                             |
| Communication     | TCP sockets, JSON request/response |

---

## 📁 Project Structure

```
BookClubServer/
├── DB/          Repositories (database access per entity)
├── Network/     BookClubServer & ClientHandler (socket + JSON protocol)
└── Services/    Business logic per entity

BookClubClient/
├── NetworkManager       Handles the socket connection and JSON messages
└── *.qml                Dashboard, Home, Library, Settings, PDF Reader, etc.
```

---

## 🔧 Build Instructions

### Prerequisites
- Qt (with Qt Quick, QML, Network, SQL, and PDF modules)
- A C++ compiler supported by Qt (MSVC on Windows, GCC/Clang on Linux/macOS)

### Build

Open the project in Qt Creator and build the server and client targets, or build from the command line with `qmake`/`cmake` depending on how the project is set up, followed by `make` (or `jom`/`nmake` on Windows).

---

## ▶️ Running the Application

1. **Start the server** first — it listens for incoming TCP client connections and serves requests against the SQLite database.
2. **Start one or more clients** and connect them to the server's address and port.

> **Note on deployment:** the server address/port that the client connects to should **not** be hardcoded as a raw IP if the server's IP can change (e.g. a laptop on a home/campus network). Use a configurable setting or a dynamic-DNS hostname instead, so the client doesn't need to be rebuilt whenever the server's IP changes.

---

## 👥 Team

- **Ghazal Mizbani**
- **Dina Sadeghioun**

---

## 📝 License

This project was developed for educational purposes as a university course project.
