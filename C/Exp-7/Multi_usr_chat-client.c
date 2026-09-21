/* client.c */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 9000
#define BUFFER_SIZE 1024
#define NAME_SIZE 30

int main()
{
    int sock;

    struct sockaddr_in server;

    sock=socket(AF_INET,SOCK_STREAM,0);

    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    if(connect(sock,(struct sockaddr *)&server,sizeof(server))<0)
    {
        printf("Connection Failed\n");
        return 0;
    }

    char name[NAME_SIZE];

    printf("Enter your name : ");
    fgets(name,sizeof(name),stdin);

    name[strcspn(name,"\n")]='\0';

    send(sock,name,sizeof(name),0);

    printf("\nConnected to Chat Server\n");
    printf("Type your messages below\n\n");

    if(fork()==0)
    {
        // Sender Process

        char message[BUFFER_SIZE];

        while(1)
        {
            fgets(message,sizeof(message),stdin);

            send(sock,message,strlen(message),0);
        }
    }
    else
    {
        // Receiver Process

        char buffer[BUFFER_SIZE];

        while(1)
        {
            int n=recv(sock,buffer,sizeof(buffer)-1,0);

            if(n<=0)
            {
                printf("\nDisconnected from Server\n");
                break;
            }

            buffer[n]='\0';

            printf("%s",buffer);
        }
    }

    close(sock);

    return 0;
}
