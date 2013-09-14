#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <vector>

#define NUM_THREADS 1 

using namespace std;
std::vector<float> accels;
void doprocessing (int sock)
{
    int n;
    char buffer[256];

    bzero(buffer,256);

    n = read(sock,buffer,255);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    int len = strlen(buffer)-1;
    if(buffer[len] == '\n')
         buffer[len] = 0;
    char part[10];
    memcpy(part, buffer , 6);
    part[6] = '\0';
    float accel = atof(part) ;
    accels.push_back(accel);

    cout << accel << "...." << part << endl ;
    
    if (n < 0) 
    {
        perror("ERROR writing to socket");
        exit(1);
    }
}
 
void *socketServer(void *argument)
{
	long portno ;
	portno = (long)argument ;
	
	int sockfd, newsockfd, clilen;
    	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
    	int  n;

    /* First call to socket() function */
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    	if (sockfd < 0) 
    	{
        	perror("ERROR opening socket");
        	exit(1);
    	}
    /* Initialize socket structure */
    	bzero((char *) &serv_addr, sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = INADDR_ANY;
    	serv_addr.sin_port = htons(portno);
 
    /* Now bind the host address using bind() call.*/
    	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    	{
        	 perror("ERROR on binding");
                 exit(1);
    	}
    /* Now start listening for the clients, here 
     * process will go in sleep mode and will wait 
     * for the incoming connection
     */
    	listen(sockfd,5);
    	clilen = sizeof(cli_addr);
	
	cout << "waiting for client connection on: " << portno << endl;
	while (true)
	{
		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
        	cout << "connection accepted.." << endl ;
		if (newsockfd < 0)
        	{
            		perror("ERROR on accept");
            		exit(1);
        	}
        	/* Create child process */
        	int pid = fork();
        	if (pid < 0)
        	{
            		perror("ERROR on fork");
	    		exit(1);
        	}
        	if (pid == 0)  
        	{
            		/* This is the client process */
            		close(sockfd);
            		doprocessing(newsockfd);
            		exit(0);
        	}
        	else
        	{
            		close(newsockfd);
        	}
	
	}
}

int main(int argc, char *argv[])
{
	pthread_t threads[NUM_THREADS];
	int thread_arg = 5000;
	int rc;
	
	/* create server socket thread */
	rc = pthread_create(&threads[0], NULL, socketServer, (void *) thread_arg);
	if (rc){
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
        }
   pthread_exit(NULL);	
//	exit(EXIT_SUCCESS);
}
