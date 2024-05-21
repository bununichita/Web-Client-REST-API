#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include "helpers.hpp"
#include "requests.hpp"


char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *token)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host

    if (host) {
        memset(line, 0, LINELEN);

        strcpy(line, "Host: ");
        strcat(line, host);
        compute_message(message, line);
    }


    memset(line, 0, LINELEN);
    sprintf(line, "Authorization: Bearer %s", token);
    compute_message(message, line);

    
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    memset(line, 0, LINELEN);
    if (cookies) {
        strcpy(line, "Cookie: ");
        if (cookies != NULL) {

        for (int i = 0; i < cookies_count; i++) {
                strcat(line, cookies[i]);
                if (i != cookies_count - 1) {
                    strcat(line, "; ");
                }
        }
        }
        compute_message(message, line);

    }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char **cookies, int cookies_count, char *token)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host

    memset(line, 0, LINELEN);

    strcpy(line, "Host: ");
    strcat(line, host);
    compute_message(message, line);

    if (token) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */

    memset(line, 0, LINELEN);

    strcpy(line, "Content-Type: ");
    strcat(line, content_type);
    compute_message(message, line);

    int body_data_size = 0;

    for (int i = 0; i < body_data_fields_count; i++) {
        body_data_size += strlen(body_data[i]);
    }

    memset(line, 0, LINELEN);

    sprintf(line, "Content-Length: %d", body_data_size);

    compute_message(message, line);



    // Step 4 (optional): add cookies
    memset(line, 0, LINELEN);
    strcpy(line, "Cookie: ");
    if (cookies != NULL) {

       for (int i = 0; i < cookies_count; i++) {
              strcat(line, cookies[i]);
              if (i != cookies_count - 1) {
                strcat(line, "; ");
              }
       }
    }
    compute_message(message, line);
    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    for (int i = 0; i < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);
    }
    // memset(line, 0, LINELEN);
    strcat(message, body_data_buffer);
    free(body_data_buffer);
    free(line);
    return message;
}

char * parse_response(std::string command, char *response, std::string &output, int &aux_data)
{
    if (command == "register") {
        if (!strstr(response, "400 Bad Request")) {
            output = "SUCCESS - Utilizator înregistrat cu succes!";
        } else {
            output = "ERROR - Failed to register!";
        }
        return NULL;
    } else if (command == "login") {
        if (strstr(response, "200 OK")) {

            char *unparsed_cookies = strstr(response, "Set-Cookie: ");
            if (unparsed_cookies) {
                // char **cookies = static_cast<char**>(data);
                // char s[300];
                // int i = 0;

                strtok(unparsed_cookies, ";\r");
                unparsed_cookies += 12;
                // strcpy(((char **)data)[0], unparsed_cookies);
                // cookies[0] = s;

                // std::cout << "cookies : " << ((char **)data)[0] << std::endl;
                aux_data = 1;
            }

            output = "SUCCESS - Successfully logged in!";
            return unparsed_cookies;
        } else {
            char *error = strstr(response, "error");
            if (error) {
                char *c_output;
                c_output = error;
                c_output += 8;
                int i = 0;
                while (c_output[i] != '"') {
                    i++;
                }
                c_output[i] = '\0';
                char ouput_aux[100] = "ERROR - ";
                strcat(ouput_aux, c_output);
                std::string output_str(ouput_aux);
                output = output_str;
            } else {
                output = "ERROR - Failed to log in!";
            } 
        }
    } else if (command == "enter_library") {
        if (strstr(response, "200 OK") != NULL) {
            output = "SUCCESS - Successfully entered library!";
            char *token = strstr(response, "token");
            if (token) {
                token += 8;
                strtok(token, "\"");
                aux_data = 1;
                return token;
            }
        } else {
            output = "ERROR - Failed to enter library!";
        }
        return NULL;
    } else if (command == "get_books") {
        if (strstr(response, "200 OK") != NULL) {
            char *books = strstr(response, "timeout=");
            books = strstr(books, "[");
            if (books) {
                for (int i = 0; i < strlen(books); i++) {
                    if (books[i] == ']') {
                        books[i + 1] = '\0';
                    }
                }
            }
            output = books;
        } else {
            output = "ERROR - Failed to get books!";
        }
        return NULL;
    } else if (command == "get_book") {
        if (strstr(response, "200 OK") != NULL) {
            char *book = strstr(response, "timeout=");
            book = strstr(book, "[");
            if (book) {
                for (int i = 0; i < strlen(book); i++) {
                    if (book[i] == ']') {
                        book[i + 1] = '\0';
                    }
                }
            }
            output = book;
        } else {
            output = "ERROR - Failed to get books!";
        }
        return NULL;
    } else if (command == "add_book") {
        if (strstr(response, "200 OK") != NULL) {
            output = "Successfully added book!";
        } else {
            output = "Failed to add book!";
        }
    } else if (command == "delete_book") {
        if (strstr(response, "200 OK") != NULL) {
            output = "Successfully deleted book!";
        } else {
            output = "Failed to delete book!";
        }
    } else if (command == "logout") {
        if (strstr(response, "200 OK") != NULL) {
            output = "Successfully logged out!";
        } else {
            output = "Failed to log out!";
        }
    } else {
        output = "ERROR - Invalid command!";
    }
}
