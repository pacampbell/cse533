#ifndef SERVER_H
#define SERVER_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#include <sys/select.h>
#include <sys/time.h>

#include "debug.h"

#define MAX_CLIENTS 250
#define PORT_ECHO 8080
#define PORT_TIME 8089

/**
 * Function that is passed off to the pthread
 * which services client connections for echo.
 * @param socket_fd File descriptor for the new
 *                  socket that was created.
 * @param return Returns 0.
 */
void *serve_echo_client(void *socket_fd);

/**
 * Function that is passed off to the pthread
 * which services client connections for time.
 * @param socket_fd File descriptor for the new
 *                  socket that was created.
 * @param return Returns 0.
 */
void *serve_time_client(void *socket_fd);

/**
 * Function which listens for incomming socket connections
 * for the echo server.
 * @param socket_fd The echoserver socket.
 */
void *echo_accept_thread(void *socket_fd);

/**
 * Function which listens for incomming socket connections
 * for the time server.
 * @param socket_fd The timeserver socket.
 */
void *tod_accept_thread(void *socket_fd);

/**
 * Handles signals passed to the program.
 * ctrl-c, ctrl-z, etc.
 * @param signal Signal to intercept
 */
void interruptHandeler(int signal);

/**
 * Helper function for setting up a server socket.
 * @param port Port number to bind server socket to.
 */
int setupServerSocket(int port);
#endif