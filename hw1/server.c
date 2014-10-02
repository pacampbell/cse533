#include "server.h"

static pthread_t echo_server_thread, tod_server_thread;

int main(int argc, char *argv[]) {
	int echo_socket, time_socket;

	// Set up the echo socket
	echo_socket = setupServerSocket(PORT_ECHO);
	if(echo_socket < 0) {
		fprintf(stderr, "Failed to start echo server on port %d\n", PORT_ECHO);
		return EXIT_FAILURE;
	}

	// Set up the time socket
	time_socket = setupServerSocket(PORT_TIME);
	if(time_socket < 0) {
		fprintf(stderr, "Failed to start time server on port %d\n", PORT_TIME);
		return EXIT_FAILURE;
	}

	// Start up echo accept thread
	pthread_create(&echo_server_thread, NULL, echo_accept_thread, (void*)&echo_socket);
	
	// Start of time server accept thread
	pthread_create(&tod_server_thread, NULL, tod_accept_thread, (void*)&time_socket);

	// Wait for the accept threads to come back
	pthread_join(echo_server_thread, NULL);
	pthread_join(tod_server_thread, NULL);

	// Close open sockets
	close(echo_socket);
	close(time_socket);

	// Exit the server
	return EXIT_SUCCESS;
}

void *echo_accept_thread(void *socket_fd) {
	// pthread_t client_threads[MAX_CLIENTS];
	struct sockaddr_in client;
	int new_socket, c;
	// Convert the server socket back to an integer
	int server_sock = *(int*)socket_fd;
	// Print out a message saying the server started
	debug("Echo server is waiting for incomming connections on port %d\n", PORT_ECHO);
	// Create space for new struct
	c = sizeof(struct sockaddr_in);
	// Wait for incomming connections
	while(true) {
        // Make a new socket connection
    	if((new_socket = accept(server_sock, (struct sockaddr*)&client, (socklen_t*)&c))) {
    		// Create a new thread to serve the client
			pthread_t client_thread;
			#ifdef DEBUG
			char *address = inet_ntoa(*((struct in_addr*)&(client.sin_addr.s_addr)));
			#endif
			debug("Connecting to client on @ port %s:%d\n", address, client.sin_port);
	    	if(pthread_create(&client_thread, NULL, serve_echo_client, (void*)&new_socket) < 0) {
	    		perror("Failed to create thread");
	    		return NULL;
	    	}
    	} else if(new_socket < 0) {
    		// Socket bind failed
			perror("Failed to accept an incomming connection.");
    	}
	}
	debug("Exiting echo_accept_thread\n");
	return 0;
}

void *tod_accept_thread(void *socket_fd) {
	// pthread_t client_threads[MAX_CLIENTS];
	struct sockaddr_in client;
	int new_socket, c;
	// Convert the server socket back to an integer
	int server_sock = *(int*)socket_fd;
	// Print out a message saying the server started
	debug("Time server is waiting for incomming connections on port %d\n", PORT_TIME);
	// Create space for new struct
	c = sizeof(struct sockaddr_in);
	// Wait for incomming connections
	while(true) {
        // Make a new socket connection
    	if((new_socket = accept(server_sock, (struct sockaddr*)&client, (socklen_t*)&c))) {
    		// Create a new thread to serve the client
			pthread_t client_thread;
			#ifdef DEBUG
			char *address = inet_ntoa(*((struct in_addr*)&(client.sin_addr.s_addr)));
			#endif
			debug("Connecting to client on @ port %s:%d\n", address, client.sin_port);
	    	if(pthread_create(&client_thread, NULL, serve_time_client, (void*)&new_socket) < 0) {
	    		perror("Failed to create thread");
	    		return NULL;
	    	}
    	} else if(new_socket < 0) {
    		// Socket bind failed
			perror("Failed to accept an incomming connection.");
    	}
	}
	debug("Exiting echo_accept_thread\n");
	return NULL;
}

void *serve_echo_client(void *socket_fd) {
	//Get the socket file descriptor
    int sockfd = *(int*)socket_fd;
    char message[2049];
    int read;
    bool running = true;
    fd_set rset;

    while(running) {
    	FD_ZERO(&rset);
		FD_SET(sockfd, &rset);
		select(sockfd + 1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(sockfd, &rset)) {
			read = recv(sockfd, message, sizeof(message) - 1, 0);
			if(read == 0) {
				// The socket is closed so quit the loop
				debug("Client disconnected\n");
				running = false;	
			} else if(read < 0) {
				debug("Read failed in echo client\n");
				running = false;
			} else {
				message[read] = '\0';
    			debug("> %s", message);
    			write(sockfd, message, strlen(message));
			}
		}
    }

    // flush the buffer
    if(read == 0) {
    	fflush(stdout);
    } else if(read < 0) {
    	// There was a bad read
    	perror("Failed to recv bytes");	
    }

    // We are done, close the socket
    close(sockfd);
    debug("Closed echo client socket fd\n");
    return 0;
}

void *serve_time_client(void *socket_fd) {
	bool running = true;
	time_t now;
	struct tm timestruct;
	char buffer[128];
	int fd = *(int*)socket_fd;
	struct timeval tv = {.tv_sec = 5, .tv_usec = 0};
	fd_set rset;
	// Send the time back to the client
	while(running) {
		FD_ZERO(&rset);
		FD_SET(fd, &rset);
		select(fd + 1, &rset, NULL, NULL, &tv);
		if(FD_ISSET(fd, &rset)) {
			// The socket is closed so quit the loop
			debug("Client disconnected\n");
			running = false;	
		} else {
			// Get the time right NOW
			now = time(0);
			timestruct = *localtime(&now);
			// Format the buffer into the current time for the local area
			strftime(buffer, sizeof(buffer), "%Y-%m-%d.%X", &timestruct);
			// Write the time to the client
			write(fd, buffer, strlen(buffer));
			// reset the time
			tv.tv_sec = 5;
			tv.tv_usec = 0;
		}

	}
	// Close the socket
	close(fd);
	debug("Closed time client fd\n");
	return NULL;
}

int setupServerSocket(int port) {
	int server_sock = socket(AF_INET , SOCK_STREAM , 0);
	struct sockaddr_in server;

	// Create socket
	if(server_sock < 0) {
		perror("Failed to open socket");
		return -1;
	}

	// Set Server socket listening options
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	// Set address reuse
	int optval = 1;
	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	// Create the server socket
	if(bind(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		perror("Failed to bind on port.");
		return -1;
	}

	// Finally start listening for connections
	listen(server_sock, 3); // Start trashing incomming connections after 3

	// Return the server socket.
	return server_sock;
}
