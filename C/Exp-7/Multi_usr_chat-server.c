/* server.c */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/select.h>

#define PORT 9000
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define NAME_SIZE 30

typedef struct
{
    int socket;
    char name[NAME_SIZE];
}Client;

Client clients[MAX_CLIENTS];

int main()
{
    int server_fd,new_socket;
    struct sockaddr_in address;
    socklen_t addrlen=sizeof(address);

    fd_set readfds;

    char buffer[BUFFER_SIZE];

    // Initialize client list
    for(int i=0;i<MAX_CLIENTS;i++)
    {
        clients[i].socket=0;
        strcpy(clients[i].name,"");
    }

    // Create socket
    server_fd=socket(AF_INET,SOCK_STREAM,0);

    address.sin_family=AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port=htons(PORT);

    bind(server_fd,(struct sockaddr *)&address,sizeof(address));

    listen(server_fd,5);

    printf("=====================================\n");
    printf(" Multi User Chat Server Started\n");
    printf(" Port : %d\n",PORT);
    printf("=====================================\n");

    while(1)
    {
        FD_ZERO(&readfds);

        FD_SET(server_fd,&readfds);

        int max_sd=server_fd;

        for(int i=0;i<MAX_CLIENTS;i++)
        {
            if(clients[i].socket>0)
            {
                FD_SET(clients[i].socket,&readfds);

                if(clients[i].socket>max_sd)
                    max_sd=clients[i].socket;
            }
        }

        select(max_sd+1,&readfds,NULL,NULL,NULL);

        // New client connection
        if(FD_ISSET(server_fd,&readfds))
        {
            new_socket=accept(server_fd,
                              (struct sockaddr *)&address,
                              &addrlen);

            char name[NAME_SIZE];

            recv(new_socket,name,sizeof(name),0);

            for(int i=0;i<MAX_CLIENTS;i++)
            {
                if(clients[i].socket==0)
                {
                    clients[i].socket=new_socket;
                    strcpy(clients[i].name,name);

                    printf("%s Connected\n",name);

                    break;
                }
            }
        }

        // Existing clients
        for(int i=0;i<MAX_CLIENTS;i++)
        {
            int sd=clients[i].socket;

            if(sd>0 && FD_ISSET(sd,&readfds))
            {
                int valread=recv(sd,buffer,sizeof(buffer)-1,0);

                if(valread<=0)
                {
                    printf("%s Disconnected\n",clients[i].name);

                    close(sd);

                    clients[i].socket=0;

                    strcpy(clients[i].name,"");
                }
                else
                {
                    buffer[valread]='\0';

                    char message[1200];

                    sprintf(message,"%s : %s",
                            clients[i].name,
                            buffer);

                    printf("%s",message);

                    // Broadcast to all other clients
                    for(int j=0;j<MAX_CLIENTS;j++)
                    {
                        if(clients[j].socket!=0 &&
                            clients[j].socket!=sd)
                        {
                            send(clients[j].socket,
                                 message,
                                 strlen(message),
                                 0);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);

    return 0;
}
