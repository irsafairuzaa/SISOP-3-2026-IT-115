#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "protocol.h"
#include <pthread.h>

void *receive_message(void *socket_desc) {
	int sock = *(int*)socket_desc;
	char buffer[1024] = {0};
	int valread;

while ((valread = read(sock, buffer, 1024)) > 0) {
	printf("%s\n", buffer);
	memset(buffer, 0, sizeof(buffer)); // Bersihkan mangkok setelah dicetak
}
return NULL;
}

int main(int argc, char const *argv[]) {
	int sock = 0, valread;
	struct sockaddr_in serv_addr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr)<=0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}


	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

char name[50];
printf("Enter your name: ");
scanf("%s", name);
getchar();

send(sock , name , strlen(name) , 0 );

while(1) {
                printf("\n=== THE KNIGHTS CONSOLE ===\n");
                printf("1. Check Active Entites (Users)\n");
                printf("2. Check Server Uptime\n");
                printf("3. Execute Emergency Shutdown\n");
                printf("4. Disconnect\n");
                printf("Command >> ");

                char cmd[10];
                fgets(cmd, 10, stdin);
                cmd[strcspn(cmd, "\n")] = 0;

                if (strcmp(cmd, "4") == 0) {
                    printf("[System] Disconnecting from The Wired...\n");
                    break;
                }

                send(sock, cmd, strlen(cmd), 0);

pthread_t recv_thread;
	if (pthread_create(&recv_thread, NULL, receive_message, (void*)&sock) < 0) {
		perror("could not create thread");
		return 1;
	}

char message[1024];
	while (1) {
		fgets(message, 1024, stdin);
		message[strcspn(message, "\n")] = 0;
		send(sock, message, strlen(message), 0);
	}

return 0;
}
}
