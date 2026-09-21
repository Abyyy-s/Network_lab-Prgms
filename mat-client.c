#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    int N, matrix[10][10];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    printf("Enter order of matrix (N): ");
    scanf("%d", &N);

    srand(time(0));

    printf("\nGenerated Matrix:\n");
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            matrix[i][j] = rand() % 51 ; // filling with 0 to 50
            printf("%3d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Send N
    send(sock, &N, sizeof(N), 0);

    // Send matrix
    send(sock, matrix, sizeof(matrix), 0);

    char result[50];
    recv(sock, result, sizeof(result), 0);

    printf("\nMatrix Type: %s\n", result);

    close(sock);
    return 0;
}
