#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

class Client {
public:
	Client() {
		client_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (client_sock <= 0) {
			std::cerr << "Error. Cant create socket\n";
			system("pause");
			exit(1);
		}

		socklen_t size_serv_addr = sizeof(serv_addr);

		serv_addr.sin_port = htons(1111);
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(std::stoi(ip_serv));

		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(0);
		client_addr.sin_addr.s_addr = INADDR_ANY;
		int size_client_addr = sizeof(client_addr);

		int rc = bind(client_sock, (sockaddr*)&client_addr, size_client_addr);
		if (rc < 0) {
			std::cerr << "Error. Cant bind socket\n";
			system("pause");
			exit(1);
		}
		Test();
    };
	~Client()= default;
private:
	std::string ip_serv = "0.0.0.0";
	int client_sock{0};
    sockaddr_in serv_addr;
    sockaddr_in client_addr;
    void Test();
    void SendMesToServ(nlohmann::json mes);
};