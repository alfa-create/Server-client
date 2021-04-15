#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <iostream>
#include <nlohmann/json.hpp>

class Server {
public:
	Server() {

		sListen = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sListen <= 0) {
			std::cerr << "Error. Cant create socket\n";
			exit(1);
		}

        addr.sin_addr.s_addr = htonl (INADDR_ANY);
        addr.sin_port = htons(port);
		addr.sin_family = AF_INET;

		if (bind(sListen, (sockaddr*)&addr, sizeOfAddr) < 0) {
			std::cerr << "Error. Cant bind socket\n";
			exit(1);
		}

		ClientHandler();
	};
	
	~Server() {
	};

private:
	int sListen {0};
    sockaddr_in addr;
    int sizeOfAddr = sizeof(addr);
    int port = 1111;


    bool Parse (char* msg, sockaddr_in client);
	void ClientHandler();
	bool SendMes(std::string msg, sockaddr_in client);
	std::string FindPath (std::string str, bool& err);
};
