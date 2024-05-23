#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
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

    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    if (host) {
        memset(line, 0, LINELEN);

        strcpy(line, "Host: ");
        strcat(line, host);
        compute_message(message, line);
    }

    memset(line, 0, LINELEN);
    sprintf(line, "Authorization: Bearer %s", token);
    compute_message(message, line);

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

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    strcpy(line, "Host: ");
    strcat(line, host);
    compute_message(message, line);

    if (token) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

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

    compute_message(message, "");

    for (int i = 0; i < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);
    }
    strcat(message, body_data_buffer);
    free(body_data_buffer);
    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                            char **cookies, int cookies_count, char *token)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    if (host) {
        memset(line, 0, LINELEN);

        strcpy(line, "Host: ");
        strcat(line, host);
        compute_message(message, line);
    }

    memset(line, 0, LINELEN);
    sprintf(line, "Authorization: Bearer %s", token);
    compute_message(message, line);

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
    compute_message(message, "");

    free(line);
    return message;
}

char *parse_response(std::string command, char *response, std::string &output, int &aux_data)
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
                strtok(unparsed_cookies, ";\r");
                unparsed_cookies += 12;
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
            return NULL;
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
                        break;
                    }
                }
            }
            char aux_return[BUFLEN];
            strcpy(aux_return, "SUCCESS\n");
            strcat(aux_return, books);
            output = aux_return;
        } else {
            if (!strstr(response, "error\":\"Error when decoding tokenn!")) {
                output = "ERROR - Invalid token!";
            } else {
                output = "ERROR - Failed to get books!";
            }
        }
        return NULL;
    } else if (command == "get_book") {
        if (strstr(response, "200 OK") != NULL) {
            char *book = strstr(response, "timeout=");
            book = strstr(book, "{");
            if (book) {
                for (int i = 0; i < strlen(book); i++) {
                    if (book[i] == '}') {
                        book[i + 1] = '\0';
                    }
                }
            }
            output = book;
        } else {
            if (strstr(response, "error\":\"Error when decoding tokenn!")) {
                output = "ERROR - Invalid token!";
            } else {
                output = "ERROR - Failed to get books!";
            }
        }
        return NULL;
    } else if (command == "add_book") {
        if (strstr(response, "200 OK") != NULL) {
            output = "SUCCESS - Successfully added book!";
        } else {
            if (strstr(response, "error\":\"Error when decoding tokenn!")) {
                output = "ERROR - Invalid token!";
            } else {
                output = "ERROR - Failed to add book!";
            }
        }
        return NULL;
    } else if (command == "delete_book") {
        if (strstr(response, "200 OK") != NULL) {
            output = "SUCCESS - Successfully deleted book!";
        } else {
            if (strstr(response, "error\":\"Error when decoding tokenn!")) {
                output = "ERROR - Invalid token!";
            } else {
                output = "ERROR - Failed to delete book!";
            }
        }
        return NULL;
    } else if (command == "logout") {
        if (strstr(response, "200 OK") != NULL) {
            output = "SUCCESS - Successfully logged out!";
            aux_data = 1;
        } else {
            output = "ERROR - Failed to log out!";
            aux_data = 0;
        }
        return NULL;
    } else {
        output = "ERROR - Invalid command!";
        return NULL;
    }
}
