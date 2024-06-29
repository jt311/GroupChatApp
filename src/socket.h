#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

struct acceptedSocketEndpoint {
    struct sockaddr_in client_addr;
    int connectedSocketFD;
    bool acceptedSuccessfully;
    int error;
};

int num_ConnSockets = 0;
struct acceptedSocketEndpoint* connSocEndpoints[10]; 

int createTCPIpv4_SFD() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in* createIPv4SocketAddress(char* ip, int port_num) {
    struct sockaddr_in* addr = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port_num);
    if (strlen(ip) == 0) {
        addr->sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, ip, &(addr->sin_addr));
    }

    memset(addr->sin_zero, '\0', sizeof(addr->sin_zero));
    return addr;
}

struct acceptedSocketEndpoint* acceptIncomingConnection(int sfd) {
    struct acceptedSocketEndpoint* accSocket = (struct acceptedSocketEndpoint*) malloc(sizeof(struct acceptedSocketEndpoint));
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int conn_sfd = accept(sfd, (struct sockaddr *) &client_addr, &client_addr_size);

    accSocket->connectedSocketFD = conn_sfd;
    accSocket->client_addr = client_addr;
    accSocket->acceptedSuccessfully = conn_sfd > 0;

    if (!accSocket->acceptedSuccessfully) {
        accSocket->error = conn_sfd;
    }

    return accSocket;
}

void sendClientMsg(int conn_sfd, char* msg) {
    size_t msg_size = strlen(msg);
    send(conn_sfd, msg, msg_size, 0);
}

void broadcastClientMsg(char* msg, int recv_sockFD) {
    int currSFD;
    for (int i = 0; i < num_ConnSockets; i++) {
        currSFD = connSocEndpoints[i]->connectedSocketFD;
        if (currSFD != recv_sockFD) {
            sendClientMsg(currSFD, msg);
        }
    }
}

void* receiveAndPrintIncomingData(void* conn_sfd_ptr) {
    int* conn_sfd = (int*) conn_sfd_ptr;
    char incData_Buffer[512];
    while (true) {
        int bytes_recv = recv(*conn_sfd, incData_Buffer, sizeof(incData_Buffer), 0);

        if (bytes_recv > 0) {
            printf("recv()'d %d bytes of data in incData_Buffer\n", bytes_recv);
            printf("Data received -> %s", incData_Buffer);
            broadcastClientMsg(incData_Buffer, *conn_sfd);
            memset(incData_Buffer, '\0', sizeof(incData_Buffer));
        }

        if (bytes_recv == 0) {
            printf("Stream socket closed by Client\n");
            break;
        }
    }

    close(*conn_sfd);
    return NULL;
}

void receiveAndPrintData_SepThread(struct acceptedSocketEndpoint* acceptedSocket) {
    pthread_t id;
    pthread_create(&id, NULL, receiveAndPrintIncomingData, &(acceptedSocket->connectedSocketFD));
}

void startAcceptingIncomingConnections(int server_sfd) {
    while (true) {
        struct acceptedSocketEndpoint* acceptedSocket = acceptIncomingConnection(server_sfd);
        connSocEndpoints[num_ConnSockets++] = acceptedSocket;
        receiveAndPrintData_SepThread(acceptedSocket);
    }
}

    //loop through all connected socket endpoints bound to server process
    //send msg across each endpoint's stream socket except the one which sent

