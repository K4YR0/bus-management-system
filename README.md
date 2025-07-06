# 🚌 Bus Management System

A comprehensive C-based bus management system with user authentication, data persistence, and interactive games. This project demonstrates advanced C programming concepts including linked lists, file I/O, memory management, and modular programming.

## 📋 Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [Usage](#usage)
- [System Requirements](#system-requirements)
- [Technical Details](#technical-details)
- [Contributing](#contributing)
- [License](#license)

## ✨ Features

### 🔐 User Authentication
- User registration and login system
- Password-protected access
- User data persistence in files

### 🚍 Bus Management
- Add, modify, and delete bus records
- View all buses with detailed information
- Search buses by license plate
- Data validation for all inputs
- File-based data storage and retrieval

### 👥 Client Management
- Complete client information management
- Add clients at beginning or end of list
- Modify and delete client records
- Search functionality
- Export/import client data

### 🎮 Interactive Games
- **Tic-Tac-Toe**: Classic game with AI opponent
- **Rock Paper Scissors**: Simple strategy game
- Colorful console interface

### 💾 Data Persistence
- File-based storage for all data
- Load and save functionality
- Data integrity validation

## 📁 Project Structure

```
BusManagementSystem/
├── src/
│   └── main.c                  # Main application source code
├── include/
│   └── bus_management_system.h # Header file with declarations
└── README.md                  # Project documentation
```

## 🚀 Installation

### Prerequisites
- GCC compiler (MinGW for Windows)
- C99 standard support
- Terminal/Command Prompt

### Build Instructions

1. **Clone the repository:**
   ```bash
   git clone https://github.com/K4YR0/bus-management-system.git
   cd bus-management-system
   ```

2. **Compile the project:**
   ```bash
   # Windows (MinGW)
   gcc -Wall -Wextra -std=c99 -o bus_system src/main.c

   # Linux/Mac
   gcc -Wall -Wextra -std=c99 -o bus_system src/main.c
   ```

3. **Run the application:**
   ```bash
   # Windows
   ./bus_system.exe

   # Linux/Mac
   ./bus_system
   ```

## 🎯 Usage

### Getting Started

1. **First Run:**
   - The system will prompt you to register a new user
   - Enter your desired username and password
   - Your credentials will be saved for future sessions

2. **Login:**
   - Use your registered credentials to access the system
   - The main menu will display available options

3. **Navigation:**
   - Use number keys to navigate menus
   - Follow on-screen prompts for data entry
   - Press any key to continue when prompted

### Main Features

#### Bus Management
- **Add Bus**: Enter license plate, price, purchase date, and seat count
- **View Buses**: Display all registered buses in a formatted table
- **Search**: Find specific buses by license plate number
- **Modify**: Update bus information
- **Delete**: Remove buses from the system

#### Client Management
- **Add Client**: Enter personal and contact information
- **View Clients**: Display all registered clients
- **Search**: Find clients by ID
- **Modify**: Update client information
- **Delete**: Remove clients from the system

#### Games
- **Tic-Tac-Toe**: Play against the computer
- **Rock Paper Scissors**: Quick strategy game

## 🖥️ System Requirements

### Minimum Requirements
- **OS**: Windows 7+, Linux, macOS
- **Compiler**: GCC with C99 support
- **Memory**: 50MB RAM
- **Storage**: 10MB free space

### Supported Platforms
- ✅ Windows (10/11)
- ✅ Linux (Ubuntu, Fedora, etc.)
- ✅ macOS (10.12+)

## 🔧 Technical Details

### Programming Concepts Demonstrated
- **Dynamic Memory Management**: Malloc/free for linked lists
- **File I/O Operations**: Reading/writing structured data
- **Data Structures**: Linked lists for scalable data storage
- **Input Validation**: Safe input handling with error checking
- **Modular Programming**: Separation of concerns with header files
- **Cross-platform Compatibility**: Windows/Unix conditional compilation

### Key Data Structures
```c
typedef struct Bus {
    int license_plate;
    float price;
    PurchaseDate purchase_date;
    int seat_count;
    struct Bus *next;
} Bus;

typedef struct Client {
    int client_id;
    char first_name[MAX_STRING_LENGTH];
    char last_name[MAX_STRING_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    char city[MAX_STRING_LENGTH];
    char province[MAX_STRING_LENGTH];
    int postal_code;
    struct Client *next;
} Client;
```

### Memory Management
- Dynamic allocation for scalable data structures
- Proper cleanup to prevent memory leaks
- Error handling for allocation failures

## 🎨 User Interface

The application features a colorful console interface with:
- Color-coded menus and headers
- Formatted data display tables
- Interactive prompts and validation
- Cross-platform console color support

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📝 Development Notes

This project was developed as part of a computer science curriculum, demonstrating:
- Advanced C programming techniques
- Software engineering principles
- Data structure implementation
- File system interaction
- User interface design

## 🏆 Skills Demonstrated

- **Programming Languages**: C
- **Concepts**: Data Structures, Algorithms, Memory Management
- **Tools**: GCC, Git, File I/O
- **Practices**: Modular Design, Code Documentation, Version Control

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

---

⭐ **Star this repository if you found it helpful!** ⭐
