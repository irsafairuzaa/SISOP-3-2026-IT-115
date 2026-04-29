#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "protocol.h"

#define MAX_CLIENTS 30

int main(int argc, char const *argv[]) {
	int server_fd, new_socket, valread;
	int client_socket[MAX_CLIENTS];
	char client_names[MAX_CLIENTS][50];
	int max_clients = MAX_CLIENTS;
	int activity, i, sd, max_sd;
	struct sockaddr_in address;
	int opt = 1;
	socklen_t addrlen = sizeof(address);
	char buffer[1024];

fd_set readfds;

for (i = 0; i < max_clients; i++) {
client_socket[i] = 0;
memset(client_names[i], 0, 50);
}

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
		}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
		}

	if (listen(server_fd, 20) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
		}

while (1) {
	FD_ZERO(&readfds);
	FD_SET(server_fd, &readfds);
	max_sd = server_fd;
	for (i = 0; i < max_clients; i++) {
		sd = client_socket[i];
		if (sd > 0) FD_SET(sd, &readfds);
		if (sd > max_sd) max_sd = sd;
	}
	activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

	if ((activity < 0)) {
		printf("Select error\n");
		continue;
	}

	if (FD_ISSET(server_fd, &readfds)) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
			perror("accept error"); exit(EXIT_FAILURE);
			continue;
	}

	memset(buffer, 0, sizeof(buffer));
	valread = read(new_socket, buffer, sizeof(buffer) - 1);

	if (valread > 0) {
	buffer[strcspn(buffer, "\n")] = 0;

	int is_duplicate = 0;
		for (int j = 0; j < max_clients; j++) {
			if (client_socket[j] > 0 && strcmp(client_names[j], buffer) == 0) {
			is_duplicate = 1;
			break;
			}
		}

	if (is_duplicate == 1) {
		printf("[System] The identity '%s' is already synchronized in The Wired.\n", buffer);
		char *err_msg = "[System] The identity is already synchronized in The Wired.\n", buffer;
		send(new_socket, err_msg, strlen(err_msg), 0);
		close(new_socket);
	} else {
		printf("Client named %s entered.\n", buffer);

	char welcome_msg[2048];
		sprintf(welcome_msg, "--- Welcome to The Wired, %s ---", buffer);

	send(new_socket, welcome_msg, strlen(welcome_msg), 0);

	for (i = 0; i < max_clients; i++) {
		if (client_socket[i] == 0) {
			client_socket[i] = new_socket;
			strcpy(client_names[i], buffer);
			break;
		}}
	}
	} else {
		close(new_socket);
	}
	}

	for (i = 0; i < max_clients; i++) {
		sd = client_socket[i];

		if (sd > 0 && FD_ISSET(sd, &readfds)) {
			memset(buffer, 0, 1024);
			valread = read(sd, buffer, 1024);

		if (valread == 0) {
			printf("[System] Disconnecting from The Wired...\n");
			close(sd);
			client_socket[i] = 0;
			memset(client_names[i], 0, 50);
		} else if (valread > 0){
			buffer[strcspn(buffer, "\n")] = 0;
			printf("> %s\n", buffer);

	int j;
	for (j = 0; j < max_clients; j++) {
		int dest_socket = client_socket[j];

		if (dest_socket > 0 && dest_socket != sd) {
			char broadcast_msg[1100];
			sprintf(broadcast_msg, "[%s]: %s", client_names[i], buffer);
			send(dest_socket, broadcast_msg, strlen(broadcast_msg), 0);
		}
		}
	}
	}
}
}
return 0;
}
