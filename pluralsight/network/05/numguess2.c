/* Number-guessing server. This is an example of a
   single-process, concurrent server using TCP and select()
   This version allocated the target table dynamically
*/

#include <stdlib.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>

#define GUESS_PORT      1500    /* Our "well-known" port number */

/* Function Prototypes */
int process_request(int fd);

// This is where we store the per-client state (the number to guess)
// This array is indexed by the connection descriptor

int *target;
struct rlimit limit_info;

/* -------------------------- main() -------------------------- */
int main( int argc, char* argv[] )
{
    int sock, fd, maxfd;
    int client_len;
    struct sockaddr_in server, client;

    /* These are the socket sets used by select()  */
    fd_set test_set, ready_set;

    if(getrlimit(RLIMIT_NOFILE, &limit_info) < 0) {
        perror("getting file descriptor limit");
        exit(1);
    }

    maxfd = limit_info.rlim_cur;
    target = (int *) malloc(maxfd * sizeof (int));
    printf("maxfd = %d\n", maxfd);

    /* Mark all entries in the target table as free */
    for (fd=0; fd<maxfd; fd++)  target[fd] = 0;

    /* Create the rendezvous socket & bind the well-known port */
    /* This is much like the boiler-plate code in hangserver.c */

    sock = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);

    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(GUESS_PORT);

    if (bind(sock, (struct sockaddr *) &server, sizeof (server)) < 0) {
        fprintf(stderr, "bind failed\n");
        exit(1);
    }

    listen(sock, 5);

    /* Initially, the "test set" has in it just the rendezvous descriptor */
    FD_ZERO(&test_set);
    FD_SET(sock, &test_set);
    printf("server ready\n");

    /* Here is the head of the main service loop */
    while (1) {
        /* Because select overwrites the descriptor set, we must
           not use our "permanent" set here, we must use a copy.
        */
        memcpy(&ready_set, &test_set, sizeof test_set);
        select(maxfd, &ready_set, NULL, NULL, NULL);

        /* Did we get a new connection request? If so, we accept it,
           add the new descriptor into the read set, choose a random
           number  for this client to guess
        */
        if (FD_ISSET(sock, &ready_set)) {
            client_len = sizeof client;
            fd = accept(sock, (struct sockaddr *)&client, &client_len);
            FD_SET(fd, &test_set);
            printf("new connection on fd %d\n", fd);
            target[fd] = 1 + rand()%1000;
        }

        /* Now we must check each descriptor in the read set in turn.
           For each one which is ready, we process the client request.
        */
        for (fd = 0; fd < maxfd; fd++) {
            if (fd == sock) continue;  // Omit the rendezvous descriptor
            if (target[fd] == 0) continue;  // Empty table entry
            if (FD_ISSET(fd, &ready_set)) {
                if (process_request(fd) < 0) {
                    /* If the client has closed its end of the connection,
                       or if the dialogue with the client is complete,  we
                       close our end,  and remove the  descriptor from the
                       read set
                    */
                    close(fd);
                    FD_CLR(fd, &test_set);
                    target[fd] = 0;
                    printf("closing fd=%d\n", fd);
                }
            }
        }
    }
    return 0;           /* Keep compiler happy! */
}

/* This is the application-specific part of the code */

/* ------------------------ process_request() ----------------------- */
/* Return 0 for normal return, or -1 if encountered EOF from client or
   if number was guessed correctly
*/
int process_request(int fd)
{
    int guess;
    char inbuffer[100], *p = inbuffer;

    /* We'll read characters from the client one at a time until we get
       a newline.   This approach is needed because some telnet clients
       send characters one at a time, as they are typed. 
    */

    do {
        if (read(fd, p, 1) != 1)
            return -1;
    } while (*p++ != '\n');  // No check for buffer overflow :-(

    guess = atoi(inbuffer);
    printf("received guess %d on fd=%d\n", guess, fd);

    if (guess > target[fd]) {
        write(fd, "too big\n\r", 9);
        return 0;
    }
    if (guess < target[fd]) {
        write(fd, "too small\n\r", 11);
        return 0;
    }
    write(fd, "correct!\n\r", 10);
    return -1;
}
