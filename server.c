#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include<fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Socket API headers */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Definations */
#define DEFAULT_BUFLEN 512
#define PORT 1084

void do_job(int fd) {
int length,rcnt;
char recvbuf[DEFAULT_BUFLEN],bmsg[DEFAULT_BUFLEN];
int  recvbuflen = DEFAULT_BUFLEN;
 char titleMessage[]="Welcome to Amir's file server.\n";
    if(send(fd,titleMessage,strlen(titleMessage),0)<0){
        printf("Couldn't send welcome message");
        close(fd);
        return;
    }
    // Receive until the peer shuts down the connection
    do {
        rcnt = recv(fd, recvbuf, recvbuflen, 0);
        if (rcnt > 0) {
            printf("Bytes received: %d\n", rcnt);

        // Echo the buffer back to the sender
        rcnt = send( fd, recvbuf, rcnt, 0 );
            if (rcnt < 0) {
                printf("Send failed:\n");
                close(fd);
                break;
            }
            printf("Bytes sent: %d\n", rcnt);

        }
        else if (rcnt == 0)
            printf("Connection closing...\n");
        else  {
            printf("Receive failed:\n");
            close(fd);
            break;
        }
    } while (rcnt > 0);
}



int main(int argc, char *argv[]){
  char *directory = NULL;
  int portNumber = 0;
  char *passFile = NULL;
  int opt;
 while((opt = getopt(argc,argv,"d:p:u:"))!=-1){
  switch(opt){
   case 'd':directory = optarg;
            break;
   case 'p':portNumber = atoi(optarg);
            break;
   case 'u':passFile = optarg;
            break;
   default:
     printf("server -d directory -p port -u password\n-d: Specify running directory which files to be accessed,modified or erased\n-p: Define server port number\n-u: Password file that uses delimiter separated format which is delimiter is ':'",argv[0]);
    return 1;
  }
 }
 if directory == NULL||portNumber==0||passFile==NULL){
   printf("Some arguements are missing");
   printf("server -d directory -p port -u password\n-d: Specify running directory which files to be accessed,modified or erased\n-p: Define server port number\n-u: Password file that uses delimiter separated format which is delimiter is ':'",argv[0]);
  return 1;
 }
int server, client;
struct sockaddr_in local_addr;
struct sockaddr_in remote_addr;
int length,fd,rcnt,optval;
pid_t pid;

/* Open socket descriptor */
if ((server = socket( AF_INET, SOCK_STREAM, 0)) < 0 ) { 
    perror("Can't create socket!");
    return(1);
}
 printf("File server listening on localhost port %d\n",portNumber);


/* Fill local and remote address structure with zero */
memset( &local_addr, 0, sizeof(local_addr) );
memset( &remote_addr, 0, sizeof(remote_addr) );

/* Set values to local_addr structure */
local_addr.sin_family = AF_INET;
local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
local_addr.sin_port = htons(portNumber);

// set SO_REUSEADDR on a socket to true (1):
optval = 1;
setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

if ( bind( server, (struct sockaddr *)&local_addr, sizeof(local_addr) ) < 0 )
{
    /* could not start server */
    perror("Bind error");
    return(1);
}

if ( listen( server, SOMAXCONN ) < 0 ) {
        perror("listen");
        exit(1);
}

printf("Concurrent  socket server now starting on port %d\n",PORT);
printf("Wait for connection\n");

while(1) {  // main accept() loop
    length = sizeof remote_addr;
    if ((fd = accept(server, (struct sockaddr *)&remote_addr, \
          &length)) == -1) {
          perror("Accept Problem!");
          continue;
    }

    printf("Server: got connection from %s\n", \
            inet_ntoa(remote_addr.sin_addr));

    /* If fork create Child, take control over child and close on server side */
    if ((pid=fork()) == 0) {
        close(server);
        do_job(fd);
        printf("Child finished their job!\n");
        close(fd);
        exit(0);
    }

}

// Final Cleanup
close(server);
}

