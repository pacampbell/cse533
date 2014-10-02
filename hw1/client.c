#include "client.h"

int main(int argc, char *argv[]) {
	// Determine if ip or hostname
	char *ipaddress = NULL;
	if(argc > 1) {
		bool valid = false;
		if(isIPAddress(argv[1])) {
			// Attempt to get hostname for ipaddress
			struct hostent *he = NULL;
			struct in_addr ipv4addr;
			// Memset the structs
			memset((void*)&ipv4addr, 0, sizeof(struct in_addr));
			// Fill up the host entry
			inet_pton(AF_INET, argv[1], &ipv4addr);
			he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
			if(he != NULL) {
				printf("Host name: %s\n", he->h_name);
				valid = true;
				ipaddress = argv[1];
			} else {
				herror("Invalid ipv4 address provided");
				goto FAILURE;
			}
		} else {
			// Attempt to valid via hostname
			struct hostent *host = gethostbyname(argv[1]);
			if(host != NULL) {
				int i;
				struct in_addr **address_list = (struct in_addr **) host->h_addr_list;
				for(i = 0; address_list[i] != NULL; i++) {
					ipaddress = inet_ntoa(*address_list[i]);
					fprintf(stdout, "ip-address: %s\n", ipaddress);
					valid = true;
				}
			} else {
				fprintf(stderr, "Hostname is not valid.\n");
				goto FAILURE;
			}
		}
		// Quit if not valid
		if(!valid) {
			fprintf(stderr, "The value provided is not a valid ipaddress or hostname.\n");
			goto FAILURE;
		}
	} else {
		usage(argv[0]);
		goto FAILURE;
	}
	// Start input loop
	run(ipaddress);
	return EXIT_SUCCESS;
FAILURE:
	return EXIT_FAILURE;
}

bool isIPAddress(char *address) {
	bool valid = false;
	// xxx.xxx.xxx.xxx = 15 characters
	if(address != NULL && strlen(address) <= 15) {
		// Check to see if there is 3 '.'
		if(countOccurance(address, '.') == 3) {
			// If there is 3 dots check to make sure we have letters
			while(*address++ != '\0') {
				if(!isdigit(*address) && *address != '.') {
					// Invalid ipaddress
					break;
				}
			}
			valid = true;
		}
	}
	return valid;
}

int countOccurance(char *string, char c) {
	int occurance = 0;
	if(string != NULL) {
		while(*string++ != '\0') {
			if(*string == c) {
				occurance++;
			}
		}
	}
	return occurance;
}

void usage(char *bin) {
	fprintf(stdout, "usage: %s destination\n", bin);
}

bool run(char *ipaddress) {
	bool success = true;
	if(ipaddress != NULL) {
		// char line[256];
		int option;
		bool running = true;
		bool valid;
		char line[256];
		// Print out starting menu
		printf("1) echo\n2) time\n3) quit\n> ");
		while(running) {
			option = 0;
			fgets(line, sizeof(line), stdin);
			valid = sscanf(line, "%d", &option);
			// Check to make sure user gave us an integer
			if(!valid) {
				// Print out the invalid option.
				int len = strlen(line) - 1;
				line[len] = '\0';
				printf("Invalid option '%s' provided\n\n", line);
				// Prints menu and prompts for input again
				printf("1) echo\n2) time\n3) quit\n> ");
				continue;
			}
			// Check the option the user selected
			switch(option) {
				case 1:
					echo_client(ipaddress);
					break;
				case 2:
					time_client(ipaddress);
					break;
				case 3:
					running = false;
					break;
				case 0:
					// newline was provided so just print prompt again
					printf("> ");
					continue;
				default:
					printf("\nInvalid option '%d' provided\n\n", option);
					break;
			}
			printf("1) echo\n2) time\n3) quit\n> ");
		}
	} else {
		fprintf(stderr, "NULL ipaddress passed to run.\n");
		success = false;
	}
	return success;
}

void echo_client(char *ipaddress) {
	// int stat;
	int fd[2];
	// Create half duplex pipe
	pipe(fd);
	// Now fork the process
	switch(fork()) {
		case -1:
			// Failed fork
			fprintf(stderr, "Failed to fork child env.\n");
			break;
		case 0:
			// In child process
			close(fd[0]);
			// Start the xterm
		    char fd_str[986009];
			sprintf(fd_str, "%d", fd[1]);
			// Pass the ipaddress and FD
			if(execlp("xterm", "xterm", "-e", "./echocli", ipaddress, fd_str, NULL) < 0) {
				fprintf(stderr, "Failed to start the echo client.\n");
				exit(EXIT_FAILURE);
			}
			break;
		default:
			// In parent process
			close(fd[1]);
			char buffer[1024];
			// TODO: Write back status notifications
			while(read(fd[0], buffer, 1024) > 0) {
				printf("< %s", buffer);
			}
			// Close the input fd
			close(fd[0]);
			// wait for child to exit
			wait(NULL);
			break;
	}
}

void time_client(char *ipaddress) {
	// int stat;
	int fd[2];
	// Create half duplex pipe
	pipe(fd);
	// Now fork the process
	switch(fork()) {
		case -1:
			// Failed fork
			fprintf(stderr, "Failed to fork child env.\n");
			break;
		case 0:
			// In child process
			close(fd[0]);
			// Start the xterm
		    char fd_str[986009];
			sprintf(fd_str, "%d", fd[1]);
			// Pass the ipaddress and FD
			if(execlp("xterm", "xterm", "-e", "./timecli", ipaddress, fd_str, NULL) < 0) {
				fprintf(stderr, "Failed to start the time client.\n");
				exit(EXIT_FAILURE);
			}
			break;
		default:
			// In parent process
			close(fd[1]);
			char buffer[1024];
			// TODO: Write back status notifications
			while(read(fd[0], buffer, 1024) > 0) {
				printf("< %s", buffer);
			}
			// Close the input fd
			close(fd[0]);
			// wait for child to exit
			wait(NULL);
			break;
	}	
}
