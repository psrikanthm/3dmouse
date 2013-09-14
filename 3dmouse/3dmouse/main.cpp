//
//  main.c
//  3dmouse
//
//  Created by srikanth on 20/04/13.
//  Copyright (c) 2013 srikanth. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <GLUT/glut.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <time.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <unistd.h>
//#include <mutex>


int lastFrameTime = 0;

float accelaration = 0.0f;
//std::mutex accelaration_mutex;

float boxX = 0.0f;
float rate_factor = 2.0f;
FILE *fp;

pthread_mutex_t mutex_for_accelaration = PTHREAD_MUTEX_INITIALIZER;

using namespace std;

void display(void)
{
    if (lastFrameTime == 0)
    {
        lastFrameTime = glutGet(GLUT_ELAPSED_TIME);
    }
    int now = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMilliseconds = now - lastFrameTime;
    float elapsedTime = elapsedMilliseconds / 1000.0f;
    lastFrameTime = now;
    
    pthread_mutex_lock( &mutex_for_accelaration );
    float accelX = rate_factor * accelaration ;
    float meter_disp = (accelaration * elapsedTime) + ( 0.5 * accelaration * elapsedTime * elapsedTime);
    pthread_mutex_unlock( &mutex_for_accelaration );
    
    float pixel_disp = (accelX * elapsedTime) + ( 0.5 * accelX * elapsedTime * elapsedTime);

    boxX += pixel_disp ;
    
    if (boxX > 1.0f)
    {
        boxX -= 2.0f;
    }
    if (boxX < -1.0f)
    {
        boxX += 2.0f;
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    
    glTranslatef(boxX, 0.0f, 0.0f);
    
    glBegin(GL_QUADS);
    glVertex2f(-0.25f, -0.25f);
    glVertex2f( 0.25f, -0.25f);
    glVertex2f( 0.25f,  0.25f);
    glVertex2f(-0.25f,  0.25f);
    glEnd();
    glPopMatrix();
    glutSwapBuffers();
    
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

void idle(void)
{
    glutPostRedisplay();
}

void *doprocessing (void* argument)
{
    int n;
    char buffer[256];
    
    long sock ;
    sock = (long)argument ;
    
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
    string str(buffer);

    pthread_mutex_lock( &mutex_for_accelaration );
    accelaration = atof(str.c_str()) ;
    cout << accelaration << endl; 
    pthread_mutex_unlock( &mutex_for_accelaration );
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
 //       cout << "Connection Accepted" << endl;
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        /* Create child thread */
        pthread_t threads[1];
        int thread_arg = newsockfd;
        int rc1;
        
        /* create server socket thread */
        rc1 = pthread_create(&threads[0], NULL, doprocessing, (void *) thread_arg);
        
        if (rc1){
            cout << "Error:unable to create thread," << rc1 << endl;
            close(sockfd);
            close(newsockfd);
            exit(-1);
        }
     }
}

void startReadings()
{
//       clock_t begin_time = clock();
//       while ((float (clock() - begin_time ) / CLOCKS_PER_SEC) < 120);
    std::string filename = "/Users/srikanth/Documents/3dmouse/readings/reading_01.txt" ;
    fp=fopen(filename.c_str(), "w+");
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    
    glutCreateWindow("GLUT Program");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    
    startReadings();

    pthread_t threads[1];
    int thread_arg = 5000;
    int rc1;
    
    /* create server socket thread */
    rc1 = pthread_create(&threads[0], NULL, socketServer, (void *) thread_arg);

    if (rc1){
        cout << "Error:unable to create thread," << rc1 << endl;
        exit(-1);
    }
    glutMainLoop();
}