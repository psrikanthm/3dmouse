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

using namespace std;


int lastFrameTime = 0;

float accels[1000];
float boxX = 0.0f;
int index1 = 0;
//clock_t begin_time = clock(), end_time = clock();
float delta = 0.0f;
float rate_factor = 0.0f;

void display(void)
{
    
 //   begin_time = clock();
 //   delta += (float (begin_time - end_time ) / CLOCKS_PER_SEC) * 10 ;
    rate_factor += 2.0f;
    if (lastFrameTime == 0)
    {
        lastFrameTime = glutGet(GLUT_ELAPSED_TIME);
    }
    float accelX = 0.0f;
    if (index1 < 41) {
        accelX = rate_factor * accels[index1];
  /*      if(delta > 0.20f)
        {
            delta = 0.0f;
            index1++;
            cout << index1 << ": " << accelX << endl;
        }
   */ }
    
    int now = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMilliseconds = now - lastFrameTime;
    float elapsedTime = elapsedMilliseconds / 1000.0f;
    lastFrameTime = now;
//    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
//    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    
    float temp_disp = (accelX * elapsedTime) + ( 0.5 * accelX * elapsedTime * elapsedTime);
    boxX += temp_disp ;
    
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
    
  //  end_time = clock();
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
}

void idle(void)
{
    glutPostRedisplay();
}

void getAccel()
{
    char str[200];
    FILE *fp;
    int i = 0;
    fp = fopen("/Users/srikanth/Documents/3dmouse/3dmouse/3dmouse/readings1.txt", "r");
    if(fp)
        while(fgets(str,sizeof(str),fp) != NULL)
        {
            // strip trailing '\n' if it exists
            int len = strlen(str)-1;
            if(str[len] == '\n')
                str[len] = 0;
            char part[10];
            memcpy(part, str , 6);
            part[6] = '\0';
            string accel(part);
            accels[i] = ::atof(accel.c_str()) ;
            i++ ;
            cout << "accel: " << i << " " << accel << endl ;
        }
    fclose(fp);
}

int main(int argc, char** argv)
{
    
    getAccel();
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    
    glutCreateWindow("GLUT Program");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    
    glutMainLoop();
    return EXIT_SUCCESS;
}