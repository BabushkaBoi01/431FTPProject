#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include<fcntl.h>
#include<stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/* Socket API headers */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Definations */
#define DEFAULT_BUFLEN 512
#define PORT 1084

bool login(const char *username,const char *password, const char *passFile){
 FILE *file = fopen(passFile,"r");
 if(file == NULL){
  printf("Error opening password file");
  return false;
 }
 char naruto[100];
 while(fgets(naruto,sizeof(naruto),file)!=NULL){
  char userN[50],passW[50];
  sscanf(naruto,"%[^:]:%s",userN,passW);
   if(strcmp(userN,username)==0 && strcmp(passW,password)==0){
     fclose(file);
     return true;
   }
 }
 fclose(file);
 return false;
}
void giveFiles(int fd,const char *directory){
 DIR *dir;
 struct dirent *ent;
 char replyMessage[DEFAULT_BUFLEN];
 
 dir = opendir(directory);
 if(dir!=NULL){
  while((ent=readdir(dir))!=NULL){
   if(ent->d_type==DT_REG){
    snprintf(replyMessage,DEFAULT_BUFLEN,"%s %ld\n",ent->d_name,ent->d_reclen);
    send(fd,replyMessage,strlen(replyMessage),0);
   }
  }
  closedir(dir);
 }
 //Terminate with dot
 char terminator[]=".\n";
 send(fd,terminator,strlen(terminator),0);
}
void userCommand(int fd,const char *username,const char *password,const char *passFile,const char *directory){
 if(login(username,password,passFile)){
  char replyMessage[]="200 User granted access.\n";
  send(fd,replyMessage,strlen(replyMessage),0);
  int rcnt;
  char recvbuf[DEFAULT_BUFLEN];
  int recvbuflen = DEFAULT_BUFLEN;
  while(true){
   rcnt = recv(fd,recvbuf,recvbuflen,0);
   if(rcnt>0){
    recvbuf[rcnt]='\0';
    printf("Received : %s",recvbuf);
    char command[50],sasuke[50];
    sscanf(recvbuf,"%s %s",command,sasuke);
    if(strcmp(command,"USER")==0){
     userCommand(fd,sasuke,password,passFile,directory);
    }
    else if (strcmp(command,"QUIT")==0){
     char replyMessage[]="Goodbye!\n";
     send(fd,replyMessage,strlen(replyMessage),0);
     break;
    }
    else if(strcmp(command,"LIST")==0){
     giveFiles(fd,directory);
    }
    else if(strcmp(command,"GET")==0){
     char pathway[100];
     snprintf(pathway,sizeof(pathway),"%s/%s",directory,sasuke);
     FILE *file = fopen(pathway,"r");
     if(file!=NULL){
      char sakura[DEFAULT_BUFLEN];
      char replyMessage[DEFAULT_BUFLEN];
      while(fgets(sakura,sizeof(sakura),file)!=NULL){
       send(fd,sakura,strlen(sakura),0);
      }
      fclose(file);
      char terminator[]="\r\n.\r\n";
      send(fd,terminator,strlen(terminator),0);
     }
     else{
      char replyMessage[DEFAULT_BUFLEN];
      snprintf(replyMessage,DEFAULT_BUFLEN,"404 File %s not found.\n",sasuke);
      send(fd,replyMessage,strlen(replyMessage),0);
     }
    }
    else{
    char replyMessage[]="Invalid command\n";
    send(fd,replyMessage,strlen(replyMessage),0);
    }
    }
   else if(rcnt ==0){
    printf("Connection is closing....\n");
    break;
   }
   else{
    printf("Receive failed.\n");
    break;
   }
  }
 }
 else{
  char replyMessage[]="400 User not found.Please try with another user.\n";
  send(fd,replyMessage,strlen(replyMessage),0);
 }
}

void do_job(int fd,const char *passFile,const char *directory) {
int length,rcnt;
char recvbuf[DEFAULT_BUFLEN],bmsg[DEFAULT_BUFLEN];
int  recvbuflen = DEFAULT_BUFLEN;
 char titleMessage[]="Welcome to Amir's file server.\n";
    if(send(fd,titleMessage,strlen(titleMessage),0)<0){
        printf("Couldn't send welcome message");
        close(fd);
        return;
    }
   
  while(true){
   rcnt=recv(fd,recvbuf,recvbuflen,0);
   if(rcnt>0){
    recvbuf[rcnt] = '\0';
    printf("Received : %s",recvbuf);
    char command[50],username[50],password[50];
    sscanf(recvbuf,"%s %s %s",command,username,password);
    if(strcmp(command,"USER")==0){
     userCommand(fd,username,password,passFile,directory);
    }
    else if(strcmp(command,"QUIT")==0){
     char replyMessage[]="Goodbye!\n";
     send(fd,replyMessage,strlen(replyMessage),0);
     break;
    }
    else{
     char replyMessage[]="Invalid command\n";
     send(fd,replyMessage,strlen(replyMessage),0);
    }
   }
   else if(rcnt == 0){
    printf("Connection is closing....\n");
    break;
   }
   else{
    printf("Receive failed.\n");
    break;
   }
  }
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
 if (directory == NULL||portNumber==0||passFile==NULL){
   printf("Some arguements are missing\n");
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

//printf("Concurrent  socket server now starting on port %d\n",PORT);
//printf("Wait for connection\n");

while(1) {  // main accept() loop
    length = sizeof remote_addr;
    if ((fd = accept(server, (struct sockaddr *)&remote_addr, \
          &length)) == -1) {
          perror("Accept Problem!");
          continue;
    }

    printf("%s connected to Amir's file server\n", \
            inet_ntoa(remote_addr.sin_addr));

    /* If fork create Child, take control over child and close on server side */
    if ((pid=fork()) == 0) {
        close(server);
        do_job(fd,passFile,directory);
        printf("Child finished their job!\n");
        close(fd);
        exit(0);
    }

}

// Final Cleanup
close(server);
return 0;
}
