#include "Server.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <vector>
#include <fstream>

void Server::ClientHandler() {
    while (true) {
        char msg[256];
        sockaddr_in client_addr;
        socklen_t size_client_addr = sizeof(client_addr);
        int received_bytes = recvfrom(sListen, (char*)msg, sizeof(msg), NULL,
                                      (sockaddr*)&client_addr, &size_client_addr);
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
        std::string str = to_string(j);


        if (j.contains("ADD")) {
            auto index = str.find('[');
            if (index == std::string::npos ) return EXIT_FAILURE;
            str.erase(0, index + 1);
            int result = 0;

            while (true) {
                std::string str2;
                index = str.find(',');
                if (index == std::string::npos) {
                    index = str.find(']');
                    if (index == std::string::npos ) return EXIT_FAILURE;
                    result += stoi(str2.assign(str, 0, index));
                    break;
                }

                result += stoi(str2.assign(str, 0, index));
                str.erase(0, index + 1);
            }
            answer["ADD"] = result;

        } else if (j.contains("LIST")) {
            bool err = false;
            std::string path = FindPath(str,err);
            if (err) return EXIT_FAILURE;

            std::vector<std::string> files;
            for (const auto & entry : std::filesystem::directory_iterator(path)) {
                std::string file = static_cast<std::string> (entry.path());
                auto index = file.find_last_of('/');
                if (index == std::string::npos ) return EXIT_FAILURE;
                file.erase(0,index+1);
                files.push_back(file);
            }
            answer["LIST"] = files;

        } else if (j.contains("CREATE")) {
            bool err = false;
            std::string path = FindPath(str,err);
            if (err) return EXIT_FAILURE;

            std::ofstream file(path);
            if (!file ) return EXIT_FAILURE;
            file.close();

            answer["CREATE"] = "SUCCESS";

        } else if (j.contains("REMOVE")) {
            bool err = false;
            std::string path = FindPath(str,err);
            if (err) return EXIT_FAILURE;

            if (remove(path.c_str())){
                return EXIT_FAILURE;
            }
            answer["REMOVE"] = "SUCCESS";

        } else if (j.contains("READ")) {
            bool err = false;
            std::string path = FindPath(str,err);
            if (err) return EXIT_FAILURE;

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
            auto index = str.find('[');
            if (index == std::string::npos ) return EXIT_FAILURE;
            str.erase(0, index + 2);
            std:: string path = str;
            index = str.find('"');
            if (index == std::string::npos ) return EXIT_FAILURE;
            path.erase(index, path.size()-1);
            str.erase(0,index +3);
            index = str.find('"');
            if (index == std::string::npos ) return EXIT_FAILURE;
            str.erase(index,str.size()-1);

            std::ifstream file(path);
            if (!file ) return EXIT_FAILURE;
            file.close();
            std::ofstream file2(path);
            file2<<str;
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

    int send_mes = sendto(sListen, (char*)mes, sizeof(mes), NULL,
                          (sockaddr*)&client, size_client);

    if (send_mes != sizeof(mes)) {
        std::cerr << "failed to send packet";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

std::string Server::FindPath(std::string str, bool& err) {
    std::string path = std::move(str);
    auto index = path.find(':');
    if (index == std::string::npos ) {
        err = true;
        return str;
    }
    path.erase(0,index +2);
    index = path.find('"');
    if (index == std::string::npos ) {
        err = true;
        return str;
    }
    path.erase(index,path.size()-1);

    return path;
}
