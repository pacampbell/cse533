#include "timecli.h"

int main(int argc, char *argv[]) {
	bool running = true;
	char response[256];
    int sock, read;
    struct sockaddr_in server;
    // Pipe to write back to main client
	int pipe_fd = atoi(argv[2]);
    // select vars
    fd_set rset;
    // Check arguements
	if(argc < 3) {
		fprintf(stderr, "Incorrect arguments provided.\n%s ipaddress parentfd", argv[0]);
		return EXIT_FAILURE;
	}
	// Create a new FD for the socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    // Set up socket for reading and writing with the server
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    debug("Attempting to connect to %s:%d\n", argv[1], PORT);
    // Attempt to connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Failed to connect");
        return EXIT_FAILURE;
    }
    debug("Connected to %s:%d\n", argv[1], PORT);
	// Start main loop
	while(running) {
		FD_ZERO(&rset);
		FD_SET(sock, &rset);
		select(sock + 1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sock, &rset)) {
			// Wait for server to send message back
	    	read = recv(sock, response, sizeof(response) - 1, 0);
	    	if(read == 0 || read < 0) {
	    		running = false;
	    		debug("Read 0 bytes; server hung up.\n");
	    		// Write to pipe
	    		write(pipe_fd, "Read 0 bytes; server hung up.\n", 34);
	        	// TODO: handle different errors
	        	debug("Press any key to continue..\n");
	        	#ifdef DEBUG
	        		getchar();
        		#endif
	    	} else {
	    		debug("Succesfully read %d bytes\n", read);
		    	// Null terminate the string
		    	response[read] = '\0';
		    	printf("> %s\n", response);
	    	}	
		}    	
	}
	// Close open FD
	close(sock);
	debug("Closed socket FD\n");

	return EXIT_SUCCESS;
}
