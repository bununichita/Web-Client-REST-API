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
    char *cookies[10];
    int cookies_count = 0;
    char token[300] = "\0";

    int access = 0;
    bool exit = false;

    while (!exit) {
        string command;
        cin >> command;
        if (command == "exit") {
            exit = true;
            cout << "SUCCESS - Exit\n";
        } else if (command == "register") {
            string username, password;
            cout << "username=";
            cin >> username;
            cout << "password=";
            cin >> password;
            cout << "********************login = " << username << " " << password << endl;
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
            cout << response << endl << endl;
            string output;
            parse_response(command, response, output, cookies_count);
            cout << output << endl;

        } else if (command == "login") {
            string username, password;
            cout << "username=";
            cin >> username;
            cout << "password=";
            cin >> password;
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
            cout << response << endl << endl;
            string output;
            char cookie[300];
            if (parse_response(command, response, output, cookies_count) != NULL) {
                strcpy(cookie, parse_response(command, response, output, cookies_count));
                cookies[0] = cookie;
                cout << "????????cookies = " << cookies[0] << endl;
                cout << "????????cookies_count = " << cookies_count << endl;

            }
            cout << output << endl;
        } else if (command == "enter_library") {
            // sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            if (cookies_count == 0) {
                cout << "ERROR - Login first!\n";
            } else {
                sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
                message = compute_get_request(HOST, "/api/v1/tema/library/access", "application/json", cookies, cookies_count, NULL);
                cout << message << endl;
                cout << "////////////////////////////////////////\n";
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                cout << response << endl << endl;
                string output;
                strcpy(token, parse_response(command, response, output, access));
                cout << "????????token = " << token << endl;
                cout << output << endl;
            }
        } else if (command == "get_books") {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, "/api/v1/tema/library/books", "application/json", cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            cout << response << endl;
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "get_book") {
            string id;
            cout << "id=";
            cin >> id;
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            char url[300] = "/api/v1/tema/library/books/";
            strcat(url, id.c_str());
            message = compute_get_request(HOST, url, "application/json", (char **)cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            cout << response << endl;
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "add_book") {
            string title, author, genre, publisher, page_count;
            cout << "title=";
            cin >> title;
            cout << "author=";
            cin >> author;
            cout << "genre=";
            cin >> genre;
            cout << "publisher=";
            cin >> publisher;
            cout << "page_count=";
            cin >> page_count;
            bool valid = true;
            for (int i = 0; i < page_count.size(); i++) {
                if (page_count[i] < '0' || page_count[i] > '9') {
                    cout << "ERROR - Invalid data type for page_count\n";
                    valid = false;
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
            cout << response << endl;
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "delete_book") {
            string id;
            cout << "id=";
            cin >> id;
            char url[300] = "/api/v1/tema/library/books/";
            strcat(url, id.c_str());
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_delete_request(HOST, url, "application/json", cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            cout << response << endl;
            string output;
            int aux_data;
            parse_response(command, response, output, aux_data);
            cout << output << endl;
        } else if (command == "logout") {
            sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(HOST, "/api/v1/tema/auth/logout", "application/json", cookies, cookies_count, token);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            cout << response << endl;
            string output;
            int success;
            parse_response(command, response, output, success);
            if (success == 1) {
                cookies_count = 0;
                strcpy(token, "\0");
            }
            cout << output << endl;
        } else {
            cout << "Invalid command" << endl;
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