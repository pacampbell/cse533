#include "echocli.h"

int main(int argc, char *argv[]) {
	bool running = true, waiting = false;
	char line[256], response[256];
	// SOcket variables
    int sock, read;
    struct sockaddr_in server;
    // select vars
    fd_set rset;
    
    // check arguments
    if(argc < 3) {
		fprintf(stderr, "Incorrect arguments provided.\n%s ipaddress parentfd", argv[0]);
		return EXIT_FAILURE;
	}

    // Create a new FD for the socket
    sock = socket(AF_INET , SOCK_STREAM , 0);

    // Set up socket for reading and writing with the server
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    // Attempt to connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Failed to connect");
        return EXIT_FAILURE;
    }

	// int output_fd = atoi(argv[2]);
	
	fprintf(stdout, "> ");
	fflush(stdout);
	while(running) {
		FD_ZERO(&rset);
		FD_SET(sock, &rset);
		FD_SET(STDIN_FILENO, &rset);
		select(sock + 1, &rset, NULL, NULL, NULL);
		// Handle stdin
		if(!waiting && FD_ISSET(STDIN_FILENO, &rset)) {
			if(fgets(line, sizeof(line), stdin) > 0) {
				// If we read in something, send it out!
				if(send(sock, line, strlen(line), 0) < 0) {
					perror("Failed to send data.");
					running = false;
					break;
				}
			} else {
				running = false;
				break;
			}
		}
		// Get the message sent back
		if(FD_ISSET(sock, &rset)) {
			// Wait for the response
			read = recv(sock, response, sizeof(response) - 1, 0);
			if(read <= 0) {
				running = false;
				debug("Read 0 bytes; server hung up\n");
				// TODO: Handle pipe info
				// write(output_fd, line, 256);
				debug("Press any key to continue..\n");
				#ifdef DEBUG
	        		getchar();
        		#endif
			} else {
				debug("Succesfully read %d bytes\n", read);
				// Null terminate the string
        		response[read] = '\0';
   				// Print out echo content
				printf("< %s", response);
				// Silly things to force print
				fprintf(stdout, "> ");
				fflush(stdout);
			}
		}
	}

	// Close open socket
	close(sock);
	debug("Closed socket FD\n");

	return EXIT_SUCCESS;
}
