#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345

void translate(char *str) {
    struct { char *abbr, *full; } dict[] = {
        {"tbh", "to be honest"},
        {"ig", "I guess"},
        {"tbf", "to be fair"},
        {"atm", "at the moment"},
        {"irl", "in real life"},
        {"lol", "laughing out loud"},
        {"asap", "as soon as possible"},
        {"omg", "oh my God"},
        {"ttyl", "talk to you later"},
        {"idk", "I don't know"},
        {"nvm", "never mind"},
        {"idc", "I don't care"}
    };

    char result[1024] = "";
    char temp[100];
    int i = 0, j;

    while (sscanf(str + i, "%s", temp) == 1) {
        int found = 0;

        for (j = 0; j < 12; j++) {
            if (strcmp(temp, dict[j].abbr) == 0) {
                strcat(result, dict[j].full);
                found = 1;
                break;
            }
        }

        if (!found) {
            strcat(result, temp);
        }

        strcat(result, " ");
        i += strlen(temp) + 1;
    }

    strcpy(str, result);
}

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("UDP Server running...\n");

    len = sizeof(client_addr);

    recvfrom(sockfd, buffer, sizeof(buffer), 0,
             (struct sockaddr *)&client_addr, &len);

    printf("Received: %s\n", buffer);

    translate(buffer);

    sendto(sockfd, buffer, strlen(buffer)+1, 0,
           (struct sockaddr *)&client_addr, len);

    close(sockfd);
    return 0;
}
