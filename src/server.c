#include "socket.h"

int main() {
    int server_sfd = createTCPIpv4_SFD();
    struct sockaddr_in* server_addr = createIPv4SocketAddress("", 2000);
    int bind_result = bind(server_sfd, (struct sockaddr *) server_addr, sizeof(struct sockaddr_in));
    
    if (bind_result == 0) {
        printf("Socket bound successfully to Port: %d \n", ntohs(server_addr->sin_port));
    }

    int listen_result = listen(server_sfd, 10);

    if (listen_result == 0) {
        printf("Listening for incoming connections on Port: %d \n", ntohs(server_addr->sin_port));
    }

    startAcceptingIncomingConnections(server_sfd);
    
    shutdown(server_sfd, SHUT_RDWR);
    free(server_addr);
    return 0;
}