#include <stdio.h>
#include "request.h"
#include "io_helper.h"

char default_root[] = "../../";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
// int main(int argc, char *argv[]) {
//     int c;
//     char *root_dir = default_root;
//     int port = 10000;
    
//     while ((c = getopt(argc, argv, "d:p:")) != -1)
// 	switch (c) {
// 	case 'd':
// 	    root_dir = optarg;
// 	    break;
// 	case 'p':
// 	    port = atoi(optarg);
// 	    break;
// 	default:
// 	    fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
// 	    exit(1);
// 	}

//     // run out of this directory
//     chdir_or_die(root_dir);

//     // now, get to work
//     int listen_fd = open_listen_fd_or_die(port);
//     while (1) {
// 	struct sockaddr_in client_addr;
// 	int client_len = sizeof(client_addr);
// 	int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
// 	request_handle(conn_fd);
// 	close_or_die(conn_fd);
//     }
//     return 0;
// }


// Define default_root somewhere in your code or make sure it's passed correctly
// extern char* default_root;

void request_handle(int fd);

int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root; // Ensure default_root is defined and initialized
    int port = 10000; // Default port number

    while ((c = getopt(argc, argv, "d:p:")) != -1) {
        switch (c) {
        case 'd':
            root_dir = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            if (port <= 0) {
                fprintf(stderr, "Invalid port number: %s\n", optarg);
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "usage: %s [-d basedir] [-p port]\n", argv[0]);
            exit(1);
        }
    }

    // Change working directory to the specified root directory
    if (chdir(root_dir) != 0) {
        perror("Failed to change directory");
        exit(1);
    }

    // Open a socket to listen for incoming connections on the specified port
    int listen_fd = open_listen_fd_or_die(port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);

        if (conn_fd < 0) {
            perror("Failed to accept connection");
            continue; // Optionally handle the error more gracefully
        }

        request_handle(conn_fd);

        if (close(conn_fd) < 0) {
            perror("Failed to close connection");
            // Handle error
        }
    }

    // Close the listening socket
    if (close(listen_fd) < 0) {
        perror("Failed to close listening socket");
        // Handle error
    }

    return 0;
}

    


 
