#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "Server.h"

int main() {

	Server sr;
	sr.ClientHandler();
	return 0;

}
