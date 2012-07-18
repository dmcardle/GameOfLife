#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif



/* Option: Represents an option such as "gravity on" */
typedef struct {
    char* label;
    int value;
} Option;

enum option {
    OPT_GRAV,
    OPT_INTER_OBJ_GRAV,
    OPT_INTER_OBJ_BOUNCE,
    OPT_MOUSE_ATTR,
    OPT_AIR_RESIST,
    OPT_WALL_BOUNCE
};
const Option options[] = {
    /*        {"[g] gravity", false},
              {"[i] inter-obj gravity", false},
              {"[b] inter-obj bounce", false},
              {"[m] mouse attraction", true},
              {"[d] air resistance", false},
              {"[w] wall bounce", false}
     */
};
const char* otherCommands[] = {
    /*    "[r] add object",
          "[r] remove object",
          "[c] clear objects"
     */
};

/* ------------------------------------------------------------------- */


const char* HELP_STR =
"ARGUMENTS\n\n"
;


int width = 1000;
int height = 600;
int numCols, numRows;
int pixelsPerSquareSide = 10;

int xMousePos = -1;
int yMousePos = -1;
char rotateMode = 'x';

char** grid;
char** gridBack;
char running = 0;
char drawNeighborCounts = 0;

int lastUpdate = 0;

void init();
void display();
void reshape(int w, int h); 
void renderScene();
void mouse(int x, int y);
bool processArgs(int argc, char* argv[]);
void drawString(char* str, int x, int y);
void keyPressed(unsigned char key, int x, int y);
void specialKeyPressed(int key, int x, int y);
float min(float a, float b);
float max(float a, float b);
void runGameOfLife();
char** initMatrix(int n, int m);
void zeroMatrix(int n, int m, char** a); 
void copyMatrix(int n, int m, char** a, char** b);

int countNeighbors(int c, int r) {
    int count = 0;
    int cc,rr;
    int thisC, thisR;

    /*
      X X X
      X O X
      X X X
    */

    for (cc=-1; cc<=1; cc++) {
        thisC = c + cc;
        if (thisC >= 0 && thisC < numCols) {
            for (rr=-1; rr<=1; rr++) {
                if (cc != 0 || rr != 0) {
                    thisR = r + rr;
                    if (thisR >= 0 && thisR < numRows) {
                        count += grid[thisC][thisR];
                    }
                }
            }
        }
    }

    return count;
}

void runGameOfLife() {
    int r,c;
    int neighbors;

    //if (!running)
    //    return;

    //int now = glutGet(GLUT_ELAPSED_TIME);
    //int since = now - lastUpdate;
    //if (since <= 10)
    //    return;
    
    //lastUpdate = now;

    for (r=0;r<numRows;r++) {
        for (c=0;c<numCols;c++) {

            neighbors = countNeighbors(c,r);
          
            // reset gridBack
            gridBack[c][r] = 0;
           
            // Any live cell... 
            if (grid[c][r]) {
                // with fewer than two live neighbors dies (underpopulation)
                if (neighbors < 2) {
                    gridBack[c][r] = 0;  
                }
                // with three neighbors lives
                else if (neighbors < 4) {
                    gridBack[c][r] = 1;
                }
                // with more than three dies (overcrowding)
                else {
                    gridBack[c][r] = 0;
                }
            }
            // Any dead cell...
            else {
                // with exactly three neighbors lives (reproduction)
                if (neighbors == 3)
                    gridBack[c][r] = 1;
            }
        }
    }

    copyMatrix(numRows, numCols, gridBack, grid);

    glutPostRedisplay();
}


/* Creates a 2D array of ints and returns a pointer. */
char** initMatrix(int n, int m) {
    int i;

    // reserve space for n pointers to int arrays
    char** a = (char**) malloc(sizeof(char*)*n);

    // make each pointer to a long array point to an array of m elements
    for (i=0; i<n; i++) {
        a[i] = (char*) malloc(sizeof(char)*m);
    }

    return a;
}

/* Fills matrix a with zeroes */
void zeroMatrix(int n, int m, char** a) {
    int i, j;
    for (i=0; i<n; i++) {
        for (j=0; j<m; j++) {
            a[i][j] = 0;
        }
    }
}

/* copies matrix a to matrix b */
void copyMatrix(int n, int m, char** a, char** b) {
    int i, j;

    for (i=0; i<n; i++) {
        for (j=0; j<m; j++) {
            b[i][j] = a[i][j];
        }
    }
}


int main(int argc, char* argv[]) {

    int validArgs = processArgs(argc, argv);
    if (validArgs == false) {
        printf("\nQuitting...\n");
        return 0;
    }

    // initialize both front and back grids
    numCols = width / pixelsPerSquareSide;
    numRows = height / pixelsPerSquareSide;

    grid = initMatrix(numCols, numRows);
    zeroMatrix(numCols, numRows, grid);

    gridBack = initMatrix(numCols, numRows);
    zeroMatrix(numCols, numRows, gridBack);


    // GLUT setup stuff
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(width, height);
    glutCreateWindow("Game of Life");

    // register display callback
    glutDisplayFunc(display);

    // when mouse moves with button down
    //glutMotionFunc(mouse);

    // when mouse moves without button down
    glutPassiveMotionFunc(mouse);

    // keyboard functions
    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(specialKeyPressed);

    // redisplay
    //glutIdleFunc(runGameOfLife);

    // reshape
    glutReshapeFunc(reshape);

    init();

    glutMainLoop();
}



/*
   Set up screen stuff
 */
void init() {

    // enable z axis
    glEnable(GL_DEPTH_TEST);

    //glDepthMask(GL_TRUE);

    glShadeModel(GL_SMOOTH);

    glViewport(0,0,width,height);

    // choose background color
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // set point size
    glPointSize(1.0);

    // load matrix mode
    glMatrixMode (GL_PROJECTION);

    // load identity matrix
    glLoadIdentity();
    
    glOrtho(0, width, 0, height, -max(width,height), max(width,height));
    //glFrustum(0, width, 0, height, -max(width,height), max(width,height));
    
    //         eye      at      up
    gluLookAt(0,0,0,  0,0,-1,  0,1,0);

}



/*
   Display the state of the game visually
 */
void display() {
    
    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //glLoadIdentity();

    //glRotatef(10, 0, 1, 1);

    //glTranslatef(0, 0, -1);

    // draw the current state of the game
    renderScene();

    // swap front and back buffers
    glutSwapBuffers();

}



void renderScene() {

    int r,c;

    int x1,y1;
    int x2,y2;
    int neighbors;

    // foreground r,g,b
    float rf,gf,bf;
    // background r,g,b
    float rb,gb,bb;


    for (r=0; r<numRows; r++) {
        for (c=0; c<numCols; c++) {

            if (grid[c][r]) {
                rf=gf=bf=1;
                rb=gb=bb=0;
            }
            else {
                rf=gf=bf=0;
                rb=gb=bb=1;
            }
            glColor3f(rf,gf,bf);
            //glColor3f(1.*r / numRows, 1.*c / numCols, 0);

            x1 = c*pixelsPerSquareSide;
            x2 = x1 + pixelsPerSquareSide;
            y1 = r*pixelsPerSquareSide;
            y2 = y1 + pixelsPerSquareSide;

            glBegin(GL_QUADS);
            glVertex2f(x1, y1);
            glVertex2f(x1, y2);
            glVertex2f(x2, y2);
            glVertex2f(x2, y1);
            glEnd();
           
            if (drawNeighborCounts) { 
                // count neighbors
                glColor3f(rb,gb,bb);
                neighbors = countNeighbors(c,r);
                glRasterPos2f(x1+4.5,y1+7.5);
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, neighbors + '0');
            }
        }
    }


    // flushes all unfinished drawing commands
    glFlush();

}



float max(float a, float b) {
    return a>b ? a:b;
}
float min(float a, float b) {
    return a<b ? a:b;
}

/*
float* RGBToHSV(float r, float g, float b) {
    float* ans = malloc(sizeof(float)*3);

    float M = max(max(r,g),b);
    float m = min(min(r,g),b);
    float C = M-m;
}
*/

void reshape(int w, int h) {
    width = w;
    height = h;
    glViewport(0,0,w,h);
    glutPostRedisplay();
}

void mouse(int x, int y) {
    xMousePos = x;
    yMousePos = height - y;
}

void timer() {
    glutPostRedisplay();
}

bool processArgs(int argc, char* argv[]) {
    int argNum;
    char* argLabel, *argVal;
   
    // read in all arguments
    argNum=1;
    while (argNum < argc) {
        argLabel = argv[argNum];
        argVal = argv[argNum+1];

        // strcmp(a,b) returns 0 if the strings are equal, so my shorthand
        // for checking if two strings are equal is !strcmp(a,b)
        //if (!strcmp(argLabel, "--nVal") || !strcmp(argLabel, "-n")) {    
        //}

        argNum++;
    }

    return true;
}



void drawString(char* str, int x, int y) {
    int i;
    int len;
    
    // set color
    glColor3f(0.0, 1.0, 0.0);
    
    // set position
    glRasterPos2i(x, y);

    // draw each character
    len = strlen(str);
    for (i=0; i<len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
    }

}

void specialKeyPressed(int key, int x, int y) {

    float rx, ry, rz;
    rx = ry = rz = 0;
    switch (rotateMode) {
        case 'x':
            rx = 1;
            break;
        case 'y':
            ry = 1;
            break;
        case 'z':
            rz = 1;
            break;
    }

    float angle = 5;

    glPopMatrix();

    glTranslatef(width/2, height/2, 0);

    switch(key) {
        case GLUT_KEY_LEFT:
            glRotatef(-angle, rx, ry, rz);
            break;
        case GLUT_KEY_RIGHT:
            glRotatef(angle, rx, ry, rz);
            break;
        case GLUT_KEY_UP:
            glScalef(1.1, 1.1, 1.1);
            break;
        case GLUT_KEY_DOWN: 
            glScalef(.9, .9, .9);
            break;
    }

    glTranslatef(-width/2, -height/2, 0);

    glutPostRedisplay();
    glPushMatrix();

}


void keyPressed(unsigned char key, int x, int y) {
    int c,r;
    int turnSquareOn;

    if (key == 'q')
        turnSquareOn = 1;
    else if (key == 'w')
        turnSquareOn = 0;

    y = height - y;

    

    switch(key) {
        case 'q':
        case 'w':
            c = x / pixelsPerSquareSide;
            r = y / pixelsPerSquareSide;
            grid[c][r] = turnSquareOn; 
            glutPostRedisplay();   
            break;
        case 's':
            running = !running;
            runGameOfLife();
            glutPostRedisplay();
            break;
        case 'c':
            zeroMatrix(numCols, numRows, grid);
            zeroMatrix(numCols, numRows, gridBack);
            glutPostRedisplay();
            break;
        case 'n':
            drawNeighborCounts = !drawNeighborCounts;
            glutPostRedisplay();
            break;
        case 'r':
            glutPostRedisplay();
            break;
        case 'x':
        case 'y':
        case 'z':
            rotateMode = key;
            break;
    }

}

