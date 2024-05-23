#include <iostream>
#include <string>
#include <sys/socket.h>
#include <fstream>

#include "helpers.hpp"
#include "requests.hpp"
#include "json.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    char copy_cookie[300] = "\0";
    char **cookies;
    cookies = new char*[10];
    for (int i = 0; i < 10; i++) {
        cookies[i] = new char[300];
    }
    int cookies_count = 0;
    char token[300] = "\0";

    int access = 0;
    bool exit = false;

    while (!exit) {
        char *com = new char[100];
        string command;
        fgets(com, 99, stdin);
        com[strlen(com) - 1] = '\0';
        command = com;
        strcpy(cookies[0], copy_cookie);
        if (command == "exit") {
            exit = true;
        } else if (command == "register") {
            char username[100];
            char password[100];
            cout << "username=";
            fgets(username, 99, stdin);
            username[strlen(username) - 1] = '\0';
            cout << "password=";
            fgets(password, 99, stdin);
            password[strlen(password) - 1] = '\0';
            bool invalid = false;
            for (int i = 0; i < strlen(username); i++) {
                if (username[i] == ' ' || username[i] == '\t') {
                    invalid = true;
                    break;
                }
            }
            if (invalid) {
                cout << "ERROR - Bad credentials\n";
                continue;
            }
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            nlohmann::json j;
            j["username"] = username;
            j["password"] = password;
            string json = j.dump();
            char *c_json = (char *)json.c_str();
            char *data[100];
            data[0] = c_json;
            message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json", data, 1, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            string output;
            parse_response(command, response, output, cookies_count);
            cout << output << endl;

        } else if (command == "login") {
            char username[100], password[100];
            cout << "username=";
            fgets(username, 99, stdin);
            username[strlen(username) - 1] = '\0';
            cout << "password=";
            fgets(password, 99, stdin);
            password[strlen(password) - 1] = '\0';
            bool invalid = false;
            for (int i = 0; i < strlen(username); i++) {
                if (username[i] == ' ' || username[i] == '\t') {
                    invalid = true;
                    break;
                }
            }
            if (invalid) {
                cout << "ERROR - Bad credentials\n";
                continue;
            }
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            nlohmann::json j;
            j["username"] = username;
            j["password"] = password;
            string json = j.dump();
            char *c_json = (char *)json.c_str();
            char *data[100];
            data[0] = c_json;
            message = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json", data, 1, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            string output;
            char cookie[300];
            if (parse_response(command, response, output, cookies_count) != NULL) {
                strcpy(cookie, parse_response(command, response, output, cookies_count));
                strcpy(copy_cookie, cookie);

            }
            cout << output << endl;
        } else if (command == "enter_library") {
            if (cookies_count == 0) {
                cout << "ERROR - Login first!\n";
            } else {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request(HOST, "/api/v1/tema/library/access", "application/json", cookies, cookies_count, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                string output;
                strcpy(token, parse_response(command, response, output, access));
                cout << output << endl;
            }
        } else if (command == "get_books") {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, "/api/v1/tema/library/books", "application/json", cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);\
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "get_book") {
            char id[100];
            cout << "id=";
            fgets(id, 99, stdin);
            id[strlen(id) - 1] = '\0';
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            char url[300] = "/api/v1/tema/library/books/";
            strcat(url, id);
            message = compute_get_request(HOST, url, "application/json", (char **)cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "add_book") {
            char title[300], author[300], genre[300], publisher[300], page_count[10];
            cout << "title=";
            fgets(title, 299, stdin);
            title[strlen(title) - 1] = '\0';
            cout << "author=";
            fgets(author, 299, stdin);
            author[strlen(author) - 1] = '\0';
            cout << "genre=";
            fgets(genre, 299, stdin);
            genre[strlen(genre) - 1] = '\0';
            cout << "publisher=";
            fgets(publisher, 299, stdin);
            publisher[strlen(publisher) - 1] = '\0';
            cout << "page_count=";
            fgets(page_count, 9, stdin);
            page_count[strlen(page_count) - 1] = '\0';
            bool valid = true;
            for (int i = 0; i < strlen(page_count); i++) {
                if (page_count[i] < '0' || page_count[i] > '9') {
                    cout << "ERROR - Invalid data type for page_count\n";
                    valid = false;
                    break;
                }
            }
            if (!valid) {
                continue;
            }
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            nlohmann::json j;
            j["title"] = title;
            j["author"] = author;
            j["genre"] = genre;
            j["page_count"] = page_count;
            j["publisher"] = publisher;
            string json = j.dump();
            char *c_json = (char *)json.c_str();
            char *data[100];
            data[0] = c_json;
            message = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json", data, 1, cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "delete_book") {
            char id[100];
            cout << "id=";
            fgets(id, 99, stdin);
            id[strlen(id) - 1] = '\0';
            char url[300] = "/api/v1/tema/library/books/";
            strcat(url, id);
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_delete_request(HOST, url, "application/json", cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "logout") {
            if (cookies_count == 0) {
                cout << "ERROR - Not logged in\n";
                continue;
            }
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            string output;
            int success;
            parse_response(command, response, output, success);
            if (success == 1) {
                cookies_count = 0;
                strcpy(token, "\0");
            }
            cout << output << endl;
        } else {
            cout << "ERROR - Invalid command\n";
        }
    }

    return 0;
}
