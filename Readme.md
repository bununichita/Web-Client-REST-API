
# Web Client: Communication with REST API

### Author: Nichita-Adrian Bunu, 323CA Facultatea de Automatica si Calculatoare UNSTPB 
**Contact:** [nichita_adrian.bunu@stud.acs.upb.ro](mailto:nichita_adrian.bunu@stud.acs.upb.ro)

---

## Overview

This project implements a web client that communicates with a REST API to manage a user's personal library of books. The client allows users to register, log in, access the library, add new books, view details of existing books, delete books, and log out. Commands are read from the keyboard and sent to the server for execution.

---

## Commands

### `register`

- Reads the username and password, then sends a packet with this information to the server at the specified URL.
- If the username contains spaces or tabs, an error message is displayed, and nothing is sent to the server.
- The program returns an error if a user with the given username already exists.

### `login`

- Reads the username and password, then sends a packet with this information to the server at the specified URL.
- If the username contains spaces or tabs, an error message is displayed, and nothing is sent to the server.
- The program returns an error if the username and password do not match.
- Cookies are saved to allow executing commands that require a logged-in user.

### `enter_library`

- Sends a packet requesting a token from the server.
- The token will be used for commands that require it and will be included in the header after the "Host" parameter.
- A user who is not logged in cannot execute this command.

### `get_books`

- Displays the ID and title of each book added by the logged-in user.
- A user who is not logged in or does not possess the token cannot execute this command.

### `get_book`

- Reads the requested book's ID and sends a packet with it to the server at the specified URL.
- The server's response contains all the details about the book in JSON format.
- An error is displayed if the ID is not a number.
- A user who is not logged in or does not possess the token cannot execute this command.

### `add_book`

- Reads the book details and sends a packet with this information in JSON format.
- After adding, the user's library will contain the new book.
- An error is displayed if the number of pages is not a number.
- A user who is not logged in or does not possess the token cannot execute this command.

### `delete_book`

- Reads the requested book's ID and sends a packet with it to the server at the specified URL.
- The server will delete the book with the given ID from the library.
- An error is displayed if the ID is not a number.
- A user who is not logged in or does not possess the token cannot execute this command.

### `logout`

- Logs out the current user, and the cookies are no longer considered (cookie_count = 0). As a result, the user will no longer be able to execute commands that require a logged-in user.
- A user who is not logged in cannot execute this command.

### `exit`

- Terminates the program execution, regardless of its current state.

---

## Implementation Details

The `requests.cpp` file contains four main functions:

- `compute_get_request`: Creates a header for a "GET" request (character string).
- `compute_post_request`: Creates a header for a "POST" request (character string).
- `compute_delete_request`: Creates a header for a "DELETE" request (character string).
- `parse_response`: Processes the server's response for each command and can return the cookie/token based on the command.  
  - The number of parsed cookies is stored in `aux_data`.

The "nlohmann" JSON library is used for creating JSON objects that are sent to the server. It was chosen for its reliability and ease of use.

---

## How to Run

1. **Build the project** using the provided Makefile:
   ```bash
   make client
   ```

2. **Run the client**:
   ```bash
   make run
   ```

3. **Clean up build files**:
   ```bash
   make clean
   ```

---
