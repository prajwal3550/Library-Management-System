📚 Library Management System

A simple, console-based Library Management System in C that allows users to manage book records efficiently using file handling. This program demonstrates fundamental concepts of C programming such as structures, dynamic memory allocation, string handling, and CRUD operations.

🚀 Features

Add New Book: Store new books with title, author, ISBN, and availability status. Each book gets a unique ID.

Display All Books: View all stored books in a clean table format.

Search Books: Search books by title or author using a case-insensitive keyword.

Update Book Information: Modify details like title, author, ISBN, and availability status.

Delete Book Records: Remove a book by its ID with a confirmation prompt.

🛠 How It Works

This system stores each book record in a text file called library_records.txt, with each record having the following format:

ID|TITLE|AUTHOR|ISBN|AVAILABLE


Books are loaded into memory dynamically, and all operations (add, update, search, delete) are performed on this data.

📂 Technology Used

C Programming Language

File Handling

Dynamic Memory Allocation

Structures

📁 File Structure

main.c: Contains the program logic for managing books.

library_records.txt: Database file where books are stored (created at runtime).

README.md: Project documentation.

🎯 Learning Objectives

Learn to store data persistently using text files.

Understand how to build a CRUD-based system in C.

Work with dynamic arrays and memory management functions (malloc, realloc, free).

Parse strings and implement case-insensitive search using strtok and strstr.

🧪 Example Usage

The program will present you with a menu:

Add New Book

Search Book (by title/author)

Update Book Record (by ID)

Delete Book Record (by ID)

Display All Books

Exit

🤝 Contributing

Contributions, issues, and suggestions are welcome!
Feel free to fork the repository and submit a pull request.

📜 License

This project is open-source and free to use.

🌐 Link to Project

For more details, visit
https://github.com/prajwal3550/Library-Management-System.git
