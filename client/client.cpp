#include "client.h"
#include <iostream>

void Client::Test() {
    nlohmann::json mes;

    //mes ["ADD"] = {10,20};
    //mes["LIST"] = "/home/karma/code";
    //mes["CREATE"] = "/home/karma/code/test.txt";
    //mes["REMOVE"] = "/home/karma/code/test.txt";
    //mes["READ"] = "/home/karma/code/test.txt";
    //mes["WRITE"] = {"/home/karma/code/test.txt", "hhhsssshhhs sgsg"};

    SendMesToServ(mes);
}

void Client::SendMesToServ(nlohmann::json mes) {
    std::string str = mes.dump();
    char msg[1024];
    strcpy(msg,str.c_str());
    std::cout << "Outgoing mes:\n";
    std::cout << msg << std::endl;

    socklen_t size_serv_addr = sizeof(serv_addr);
    auto send_mes = sendto(client_sock, reinterpret_cast<char*>(msg), sizeof(msg), NULL,
                          reinterpret_cast<sockaddr*>(&serv_addr), size_serv_addr);

    if (send_mes != sizeof(msg)) {
        std::cerr << "failed to send packet";
        exit(1);
    }
    auto received_bytes = recvfrom (client_sock, reinterpret_cast<char*>(msg), sizeof(msg), NULL,
                                   reinterpret_cast<sockaddr*>(&serv_addr), &size_serv_addr);
    if (received_bytes <= 0)
        exit(1);

    std::cout << "Incoming mes:\n";
    std::cout << msg << std::endl;
}
