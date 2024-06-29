#include "socket.h"

void* recvPrintMsg(void* sfd_ptr_param) {
    int* sfd_ptr = (int*) sfd_ptr_param;
    char msgBuffer[512];

    while (true) {
        int bytes_recv = recv(*sfd_ptr, msgBuffer, sizeof(msgBuffer), 0);

        if (bytes_recv > 0) {
            printf("%s", msgBuffer);
            memset(msgBuffer, '\0', sizeof(msgBuffer));
        }
    }

    return NULL;
}

void startListeningAndPrintingMsg_SepThread(void* sfd_ptr) {
    pthread_t id;
    pthread_create(&id, NULL, recvPrintMsg, sfd_ptr);
}

int main() {
    int sfd = createTCPIpv4_SFD();
    struct sockaddr_in* address = createIPv4SocketAddress("127.0.0.1", 2000);
    
    int result = connect(sfd, (struct sockaddr*) address, sizeof(struct sockaddr_in));
    if (result == 0) {
        printf("Connection was successful\n");
    }

    char* name = NULL;
    size_t name_size = 0;
    printf("Please enter your name: \n");
    size_t nameCount = getline(&name, &name_size, stdin);
    name[nameCount-1] = '\0';

    char* line = NULL;
    size_t lineSize = 0;
    printf("Type msg to send (type 'exit' and press enter to quit)\n");

    startListeningAndPrintingMsg_SepThread((void*)&sfd);

    char msgBuffer[1024];

    while(true) {
        size_t charCount = getline(&line, &lineSize, stdin);
        sprintf(msgBuffer, "%s: %s", name, line);

        if (charCount > 0) {
            if (strcmp(line, "exit\n") == 0) {
                printf("Closing Stream Socket\n");
                close(sfd);
                break;
            }
            send(sfd, msgBuffer, strlen(msgBuffer), 0);
        }
    }

    free(address);
    return 0;
}