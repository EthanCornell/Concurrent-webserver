//
// client.c: A very, very primitive HTTP client.
// 
// To run, try: 
//      client hostname portnumber filename
//
// Sends one HTTP request to the specified HTTP server.
// Prints out the HTTP response.
//
// For testing your server, you will want to modify this client.  
// For example:
// You may want to make this multi-threaded so that you can 
// send many requests simultaneously to the server.
//
// You may also want to be able to request different URIs; 
// you may want to get more URIs from the command line 
// or read the list from a file. 
//
// When we test your server, we will be using modifications to this client.
//

#include "io_helper.h"

#define MAXBUF (8192)

//
// Send an HTTP request for the specified file 
//
// void client_send(int fd, char *filename) {
//     char buf[MAXBUF];
//     char hostname[MAXBUF];
    
//     gethostname_or_die(hostname, MAXBUF);
    
//     /* Form and send the HTTP request */
//     sprintf(buf, "GET %s HTTP/1.1\n", filename);
//     sprintf(buf, "%shost: %s\n\r\n", buf, hostname);
//     write_or_die(fd, buf, strlen(buf));
// }

void client_send(int fd, char *filename) {
    char buf[MAXBUF];
    char hostname[MAXBUF];
    int len; // To keep track of the length written into buf

    gethostname_or_die(hostname, MAXBUF);
    
    /* Form the HTTP request */
    // Safe because we're starting to write and know MAXBUF is the limit
    len = snprintf(buf, MAXBUF, "GET %s HTTP/1.1\n", filename);

    // Check for truncation and ensure there's space for more
    if (len < 0 || len >= MAXBUF) {
        // Handle error: output was truncated or there was an encoding error
        return;
    }

    // Append the host header safely, checking for overflow
    int available = MAXBUF - len; // Calculate available space
    int additional = snprintf(buf + len, available, "host: %s\n\r\n", hostname);

    // Check if additional content was truncated or if an error occurred
    if (additional < 0 || additional >= available) {
        // Handle error: output was truncated or there was an encoding error
        return;
    }

    write_or_die(fd, buf, strlen(buf));
}




//
// Read the HTTP response and print it out
//
void client_print(int fd) {
    char buf[MAXBUF];  
    int n;
    
    // Read and display the HTTP Header 
    n = readline_or_die(fd, buf, MAXBUF);
    while (strcmp(buf, "\r\n") && (n > 0)) {
	printf("Header: %s", buf);
	n = readline_or_die(fd, buf, MAXBUF);
	
	// If you want to look for certain HTTP tags... 
	// int length = 0;
	//if (sscanf(buf, "Content-Length: %d ", &length) == 1) {
	//    printf("Length = %d\n", length);
	//}
    }
    
    // Read and display the HTTP Body 
    n = readline_or_die(fd, buf, MAXBUF);
    while (n > 0) {
	printf("%s", buf);
	n = readline_or_die(fd, buf, MAXBUF);
    }
}

int main(int argc, char *argv[]) {
    char *host, *filename;
    int port;
    int clientfd;
    
    if (argc != 4) {
	fprintf(stderr, "Usage: %s <host> <port> <filename>\n", argv[0]);
	exit(1);
    }
    
    host = argv[1];
    port = atoi(argv[2]);
    filename = argv[3];
    
    /* Open a single connection to the specified host and port */
    clientfd = open_client_fd_or_die(host, port);
    
    client_send(clientfd, filename);
    client_print(clientfd);
    
    close_or_die(clientfd);
    
    exit(0);
}
