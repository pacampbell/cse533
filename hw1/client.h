#ifndef CLIENT_H
#define CLIENT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "debug.h"

/**
 * Prints out the command line usage string.
 * @param bin Name of the binary being executed.
 */
void usage(char *bin);

/**
 * Checks if the string is a dotted decimal format.
 * @param address Determines if the string is a dotted decimal format string.
 * @return Returns true if the value is a dotted decimal format.
 */
bool isIPAddress(char *address);

/**
 * Checks the occurance of the a character in the string.
 * @param string String to search.
 * @param c Character to search for.
 * @return Returns the count of the character in the string.
 */
int countOccurance(char *string, char c);

/**
 * Begins the main loop of the client program.
 * @param ipaddress Address of the server to connect to.
 * @return Returns true if the client was able to enter the run loop,
 *         else it returns false.
 */
bool run(char *ipaddress);

/**
 * Starts echocli in xterm.
 * @param ipaddress Address of node to connect to.
 */
void echo(char *ipaddress);

/**
 * Starts the timecli in xterm.
 * @param ipaddress Address of node to connect to.
 */
void time(char *ipaddress);
#endif
