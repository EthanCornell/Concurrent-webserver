#include "io_helper.h"
#include "multi-thread.h"
#include "request.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

char default_root[] = "../../";

// Add these variables to the top of your file
task_queue_t queue;
thread_pool_t pool;

// Include this in your main function, before entering the main loop
int thread_count = MAX_THREADS; // Default value, adjust based on command line arguments
int queue_size = MAX_QUEUE;     // Default value, adjust based on command line arguments

// ./wserver [-d <basedir>] [-p <portnum>]
// Define default_root somewhere in your code or make sure it's passed correctly
// extern char* default_root;

void request_handle(int fd);

int main(int argc, char *argv[])
{
    int c;
    char *root_dir = default_root;
    int port = 10000; // Default port number

    // Parse command-line arguments for port and root directory
    while ((c = getopt(argc, argv, "d:p:")) != -1)
    {
        switch (c)
        {
        case 'd':
            root_dir = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            if (port <= 0)
            {
                fprintf(stderr, "Invalid port number: %s\n", optarg);
                exit(1);
            }
            break;
        default:
            fprintf(stderr, "usage: %s [-d basedir] [-p port]\n", argv[0]);
            exit(1);
        }
    }

    // Initialize thread pool and task queue here, after parsing command-line arguments
    task_queue_init(&queue, queue_size);
    thread_pool_init(&pool, thread_count, &queue);

    // Change working directory to the specified root directory
    if (chdir(root_dir) != 0)
    {
        perror("Failed to change directory");
        exit(1);
    }

    // Open a socket to listen for incoming connections on the specified port
    int listen_fd = open_listen_fd_or_die(port);

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);

        if (conn_fd < 0)
        {
            perror("Failed to accept connection");
            continue;
        }

        // Enqueue the connection file descriptor for handling by a worker thread
        task_queue_enqueue(&queue, conn_fd);
    }

    // Close the listening socket at the end of the server loop
    if (close(listen_fd) < 0)
    {
        perror("Failed to close listening socket");
    }

    return 0;
}
