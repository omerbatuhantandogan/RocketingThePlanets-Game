/*********
   CTIS164 - Template Source Program
----------
STUDENT : Omer Batuhan Tandogan
SECTION : 1
HOMEWORK: 4
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:

       Two light sources, their effects are merged.
       Rockets lock itself to the planet that is nearer than the other when creation.
       'P' key gives random colors to the planets



*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  10 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

typedef struct {
    float r, g, b;  //Used for color calculation
}rgb_t;

typedef struct {
    vec_t p;
    int angle;    //Used for keeping planet data
    rgb_t color;
}planet_t;

typedef struct{
    vec_t p;
    int angle;  //Used for keeping rocket data
    bool on = false;
    int target;
}rocket_t;





planet_t planetArr[2];
rocket_t rocketArr[20];  //Arrays
int ir = 0; 




//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}



//Calculating color for a point according to selected planetand outputting on given color
rgb_t calcColor(rgb_t inColor, vec_t p, planet_t* pl) {
    vec_t len = subV(pl->p,p );
    vec_t uLen = unitV(len);
    float get = dotP(uLen, { 0,1 }) * ((-1./700)*magV(len)+1);
    return { inColor.r*get,inColor.g * get,inColor.b * get };
}

//Calculating color for a point by summing two different light effects.
void applyColor(vec_t p) {
    rgb_t p1 = calcColor(planetArr[0].color, p, &planetArr[0]);
    rgb_t p2 = calcColor(planetArr[1].color, p, &planetArr[1]);

    glColor3f(p1.r + p2.r, p1.g + p2.g, p1.b + p2.b);
}



//Earth plane with two parts, different slopes
void mountains() {

    


    glBegin(GL_LINES);
    for (float i = -400; i <= 0; i++)
    {
        applyColor({ i, -i / 5 - 300 });
        glVertex2f(i, -i/5-300);
        glColor3f(0, 0, 0);
        glVertex2f(i, -400);
    }
    glEnd();

    glBegin(GL_LINES);
    for (float i = 1; i <= 400; i++) {
        applyColor({ i, i / 5 - 300 });
        glVertex2f(i, i/5-300);
        glColor3f(0, 0, 0);
        glVertex2f(i, -400);
    }
    glEnd();
}



//Planets
void planets() {
    glColor3f(0, 1, 1);
    circle(planetArr[0].p.x, planetArr[0].p.y, 65);
    circle(planetArr[1].p.x, planetArr[1].p.y, 65);
    
    glColor3f(planetArr[0].color.r, planetArr[0].color.g, planetArr[0].color.b);
    circle(planetArr[0].p.x, planetArr[0].p.y, 60);
    glColor3f(planetArr[1].color.r, planetArr[1].color.g, planetArr[1].color.b);
    circle(planetArr[1].p.x, planetArr[1].p.y, 60);
}


//Trigonometric rotation formulas
int sx(int x, int y, int ang) {
    return x * cos(ang * D2R) - y * sin(ang * D2R);
}

int sy(int x, int y, int ang) {
    return x * sin(ang * D2R) + y * cos(ang * D2R);
}


//Rockets rotated to its angle
void rockets() {
    glColor3f(0, 1, 0);
    for (int i = 0; i < 20; i++) {
        if (rocketArr[i].on)
        {
            
            glColor3f(1, 0, 1);
            glBegin(GL_TRIANGLES);
            glVertex2f(sx(0, 10, rocketArr[i].angle)+rocketArr[i].p.x,sy(0, 10, rocketArr[i].angle)+rocketArr[i].p.y);
            glVertex2f(sx(10, 0, rocketArr[i].angle)+rocketArr[i].p.x,sy(10, 0, rocketArr[i].angle)+rocketArr[i].p.y);
            glVertex2f(sx(0, -10, rocketArr[i].angle)+rocketArr[i].p.x,sy(0, -10, rocketArr[i].angle)+rocketArr[i].p.y);
            glColor3f(0, 0, 0);
            glVertex2f(sx(-40, 15, rocketArr[i].angle)+rocketArr[i].p.x,sy(-40, 15, rocketArr[i].angle)+rocketArr[i].p.y);
            glVertex2f(sx(-25, 0, rocketArr[i].angle)+rocketArr[i].p.x,sy(-25, 0, rocketArr[i].angle)+rocketArr[i].p.y);
            glVertex2f(sx(-40, -15, rocketArr[i].angle)+rocketArr[i].p.x,sy(-40, -15, rocketArr[i].angle)+rocketArr[i].p.y);
            glColor3f(0.2, 0.8, 0.5);
            glEnd();
            glBegin(GL_POLYGON);
            glVertex2f(sx(0, 10, rocketArr[i].angle)+rocketArr[i].p.x,sy(0, 10, rocketArr[i].angle)+rocketArr[i].p.y);
            glVertex2f(sx(-30, 15, rocketArr[i].angle)+rocketArr[i].p.x,sy(-30, 15, rocketArr[i].angle)+rocketArr[i].p.y);
            glVertex2f(sx(-30, -15, rocketArr[i].angle)+rocketArr[i].p.x,sy(-30, -15, rocketArr[i].angle)+rocketArr[i].p.y);
            glVertex2f(sx(0, -10, rocketArr[i].angle)+rocketArr[i].p.x,sy(0, -10, rocketArr[i].angle)+rocketArr[i].p.y);
            glEnd();



        }

    }
}





//
// To display onto window using OpenGL commands
//
void display() {
    //
    // clear window to black
    //
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0, 0.4, 1);
    glRectf(-400, 400, 400, -400);
    planets();                                       //All functions neeeded
    mountains();
    rockets();


    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    if (key == 'p' || key == 'P') {
        
        planetArr[0].color = { (rand() % 101) / (float)100.,(rand() % 101) / (float)100. ,(rand() % 101) / (float)100. };
        planetArr[1].color = { (rand() % 101) / (float)100.,(rand() % 101) / (float)100. ,(rand() % 101) / (float)100. };

    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
        int mx = x - winWidth / 2;
        int my = winHeight / 2 - y;
        vec_t clicked = { mx,my };

        rocketArr[ir].p = clicked;
                                                  //Creating an instance of rocket and locking it to the nearest planet
        if (magV(subV(clicked, planetArr[0].p)) < magV(subV(clicked, planetArr[1].p)))
            rocketArr[ir].target = 0;
        else
            rocketArr[ir].target = 1;

        rocketArr[ir].on = true;
        ir = (ir + 1) % 20;
    }

    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    planetArr[0].angle = (planetArr[0].angle + 1) % 361;
    planetArr[1].angle = (planetArr[1].angle - 1) % 361;
    planetArr[0].p.x = cos(planetArr[0].angle * D2R) * 250;
    planetArr[1].p.x = cos(planetArr[1].angle * D2R) * 250;          //Trigonometric orbital movement
    planetArr[0].p.y = sin(planetArr[0].angle * D2R) * 250;
    planetArr[1].p.y = sin(planetArr[1].angle * D2R) * 250;


    for (int i = 0; i < 20; i++) {
        if (rocketArr[i].on) {                             //Moving rocket, changing its angle and checking collisions
            rocketArr[i].p = addV(rocketArr[i].p, mulV(5,unitV(subV(planetArr[rocketArr[i].target].p, rocketArr[i].p))));
            rocketArr[i].angle = angleV(subV(planetArr[rocketArr[i].target].p, rocketArr[i].p));



            if ((rocketArr[i].p.x - planetArr[0].p.x) * (rocketArr[i].p.x - planetArr[0].p.x) + (rocketArr[i].p.y - planetArr[0].p.y) * (rocketArr[i].p.y - planetArr[0].p.y) < 2000 || (rocketArr[i].p.x - planetArr[1].p.x) * (rocketArr[i].p.x - planetArr[1].p.x) + (rocketArr[i].p.y - planetArr[1].p.y) * (rocketArr[i].p.y - planetArr[1].p.y) < 2000)
                rocketArr[i].on = false;


        }
    }
    




    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    planetArr[0].angle = 180;
    planetArr[0].color = { 1,0,0 };
    planetArr[1].color = { 0,1,0 };

}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("Rocketing the Planets<>HW-4<>Omer Batuhan Tandogan");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}
