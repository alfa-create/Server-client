#include <iostream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <vector>
#include <fstream>
#include "Server.h"

void Server::ClientHandler() {
    while (true) {
        char msg[256];
        sockaddr_in client_addr;
        socklen_t size_client_addr = sizeof(client_addr);

        auto received_bytes = recvfrom(sListen, reinterpret_cast<char*> (msg), sizeof(msg), NULL,
                reinterpret_cast<sockaddr*>(&client_addr), &size_client_addr);

        if (received_bytes <= 0)
            break;
        else{
            unsigned int client_address = (client_addr.sin_addr.s_addr);
            unsigned int client_port = (client_addr.sin_port);
            std::cout << "Connected " << client_address <<":"<< client_port << std::endl;
            if (Parse(msg,client_addr)){
                try {
                    nlohmann::json j = nlohmann::json::parse(msg);
                    nlohmann::json answer;
                    if (j.contains("ADD")) answer["ADD"] = "ERROR";
                    else if (j.contains("LIST")) answer["LIST"] = "ERROR";
                    else if (j.contains("CREATE")) answer["CREATE"] = "ERROR";
                    else if (j.contains("REMOVE")) answer["REMOVE"] = "ERROR";
                    else if (j.contains("WRITE")) answer["WRITE"] = "ERROR";
                    else if (j.contains("READ")) answer["READ"] = "ERROR";
                    else answer["INCORRECT COMMAND"] = "ERROR";
                    if (SendMes(answer.dump(), client_addr)) {
                        std::cerr << "Error in sending message\n";
                        exit(1);
                    }
                    }catch(...){
                        nlohmann::json answer;
                        answer["INCORRECT COMMAND"] = "ERROR";
                        if (SendMes(answer.dump(), client_addr)) {
                            std::cerr << "Error in sending message\n";
                            exit(1);
                    }
                }
            }
        }
    }

}

bool Server::Parse(char* msg, sockaddr_in client) {
    try {
        nlohmann::json j = nlohmann::json::parse(msg);
        nlohmann::json answer;

        if (j.contains("ADD")) {
            int result = 0;
            std::vector<int> nums = j.at("ADD");

            for (auto &i : nums){
                result+= i;
            }
            answer["ADD"] = result;

        } else if (j.contains("LIST")) {
            std:: string path = j.at("LIST");
            std::vector<std::string> files;

            for (const auto & entry : std::filesystem::directory_iterator(path)) {
                files.push_back(std::filesystem::path(entry).filename());
            }
            answer["LIST"] = files;

        } else if (j.contains("CREATE")) {
            std:: string path = j.at("CREATE");

            std::ofstream file(path);
            if (!file ) return EXIT_FAILURE;
            file.close();

            answer["CREATE"] = "SUCCESS";

        } else if (j.contains("REMOVE")) {
            std:: string path = j.at("REMOVE");

            if (remove(path.c_str())){
                return EXIT_FAILURE;
            }
            answer["REMOVE"] = "SUCCESS";

        } else if (j.contains("READ")) {
            std:: string path = j.at("READ");

            std::ifstream file(path);
            if (!file ) return EXIT_FAILURE;
            std::string text;
            while (file){
                std::string s;
                file >> s;
                text += s;
                text += " ";
            }
            file.close();
            answer["READ"] = text;

        } else if (j.contains("WRITE")) {

            std::vector<std::string> vec = j.at("WRITE");

            std::ifstream file(vec[0]);
            if (!file ) return EXIT_FAILURE;
            file.close();
            std::ofstream file2(vec[0]);
            file2<<vec[1];
            file2.close();

            answer["WRITE"] = "SUCCESS";
        } else {
            std::cerr << "Incorrect type of command";
            return EXIT_FAILURE;
        }
        std::string answ = answer.dump();
        if (SendMes(answ, client)) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }catch (...){
        return EXIT_FAILURE;
    }
}

bool Server::SendMes(std::string msg, sockaddr_in client) {
    char mes[1024];
    strcpy(mes,msg.c_str());

    int size_client = sizeof(client);

    auto send_mes = sendto(sListen, reinterpret_cast<char*>(mes), sizeof(mes), NULL,
                          reinterpret_cast<sockaddr*>(&client), size_client);

    if (send_mes != sizeof(mes)) {
        std::cerr << "failed to send packet";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}