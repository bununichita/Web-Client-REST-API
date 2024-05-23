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
    // char the_cookie[300] = "\0";
    // cookies[0] = the_cookie;
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
        // cin >> command;
        if (command == "exit") {
            exit = true;
            cout << "SUCCESS - Exit\n";
        } else if (command == "register") {
            char username[100];
            char password[100];
            cout << "username=";
            fgets(username, 99, stdin);
            username[strlen(username) - 1] = '\0';
            cout << "password=";
            fgets(password, 99, stdin);
            password[strlen(password) - 1] = '\0';
            // cout << "********************register = " << username << " " << password << endl;
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
            // cout << response << endl << endl;
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
            // cout << response << endl << endl;
            string output;
            char cookie[300];
            if (parse_response(command, response, output, cookies_count) != NULL) {
                strcpy(cookie, parse_response(command, response, output, cookies_count));
                strcpy(copy_cookie, cookie);
                // cookies[0] = cookie;
                // strcpy(cookies[0], cookie);
                // cout << "????????cookies = " << cookies[0] << endl;
                // cout << "????????cookies_count = " << cookies_count << endl;

            }
            cout << output << endl;
        } else if (command == "enter_library") {
            // sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            if (cookies_count == 0) {
                cout << "ERROR - Login first!\n";
            } else {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request(HOST, "/api/v1/tema/library/access", "application/json", cookies, cookies_count, NULL);
                // cout << message << endl;
                // cout << "////////////////////////////////////////\n";
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                // cout << response << endl << endl;
                string output;
                strcpy(token, parse_response(command, response, output, access));
                // cout << "????????token = " << token << endl;
                cout << output << endl;
            }
        } else if (command == "get_books") {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, "/api/v1/tema/library/books", "application/json", cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            // cout << response << endl;
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
            // cout << response << endl;
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "add_book") {
            char title[300], author[300], genre[300], publisher[300], page_count[10];
            // fgets(title, 299, stdin);
            // cout << "cokies: " << cookies[0] << endl;
            cout << "title=";
            // cin.getline(title, 299);
            fgets(title, 299, stdin);
            // cout << "cokies: " << cookies[0] << endl;
            title[strlen(title) - 1] = '\0';
            // cout << "cokies: " << cookies[0] << endl;
            
            cout << "author=";
            // cin.getline(author, 299);
            fgets(author, 299, stdin);
            // cout << "cokies: " << cookies[0] << endl;
            author[strlen(author) - 1] = '\0';
            // cout << "cokies: " << cookies[0] << endl;
            
            cout << "genre=";
            // cin.getline(genre, 299);
            fgets(genre, 299, stdin);
            // cout << "cokies: " << cookies[0] << endl;
            genre[strlen(genre) - 1] = '\0';
            // cout << "cokies: " << cookies[0] << endl;
            
            cout << "publisher=";
            // cin.getline(publisher, 299);
            fgets(publisher, 299, stdin);
            // cout << "cokies: " << cookies[0] << endl;
            publisher[strlen(publisher) - 1] = '\0';
            // cout << "cokies: " << cookies[0] << endl;
            
            cout << "page_count=";
            // cin.getline(page_count, 299);
            fgets(page_count, 9, stdin);
            // cout << "cokies: " << cookies[0] << endl;
            // cout << "page_count: " << page_count << endl;
            page_count[strlen(page_count) - 1] = '\0';
            // cout << "cokies: " << cookies[0] << endl;
            // cout << "page_count: " << page_count << endl;
            // cout << page_count <<endl;
            // cout << "title = " << title << "\n";
            // cout << "author = " << author << "\n";
            // cout << "genre = " << genre << "\n";
            // cout << "publisher = " << publisher << "\n";
            // cout << "page_count = " << page_count << "\n";
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
            // cout << response << endl;
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
            // cout << response << endl;
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "logout") {
            if (cookies_count == 0) {
                cout << "ERROR - Not logged in\n";
                continue;
            }
            // cout << "????????cookies = " << cookies[0] << endl;
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, "/api/v1/tema/auth/logout", NULL, cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            // cout << response << endl;
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

        
    // // Ex 1.1: GET dummy from main server
    // sockfd = open_connection("54.170.241.232", 8080, AF_INET, SOCK_STREAM, 0);
    // // Ex 1.2: POST dummy and print response from main server
    // message = compute_get_request("54.170.241.232", "/api/v1/dummy", NULL, NULL, 0);
    // // Ex 2: Login into main server
    // send_to_server(sockfd, message);
    // // Ex 3: GET weather key from main server
    // response = receive_from_server(sockfd);
    // // Ex 4: GET weather data from OpenWeather API
    // printf("%s\n", response);
    // // Ex 5: POST weather data for verification to main server
    // char *body_data[10];
    // char s[100] = "temperature=10";
    // body_data[0] = s;

    // char *cookies[10];
    // char c[100] = "__utmt=1";
    // cookies[0] = c;

    // message = compute_post_request("54.170.241.232", "/api/v1/dummy", "application/x-www-form-urlencoded", body_data, 1, cookies, 1);
    // send_to_server(sockfd, message);
    // response = receive_from_server(sockfd);
    // printf("%s\n", response);
    // // Ex 6: Logout from main server

    // // BONUS: make the main server return "Already logged in!"

    // // free the allocated data at the end!

    return 0;
}