/*
 *
 *     This program draws straight lines connecting dots placed with mouse clicks.
 *
 * Usage:
 *   Left click to place a control point.
 *		Maximum number of control points allowed is currently set at 64.
 *	 Press "f" to remove the first control point
 *	 Press "l" to remove the last control point.
 *	 Press escape to exit.
 */

#include <stdlib.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <vector>
#include <array>
#include <math.h>

std::vector<std::array<float, 3>> PointArray;
int NumPts = 0;

// Window size in pixels
int WindowHeight;
int WindowWidth;
int clickedPoint;

/* Prototypes */
void addNewPoint(float x, float y);

void removeFirstPoint();

void removeLastPoint();


void myKeyboardFunc(unsigned char key, int x, int y) {
    switch (key) {
        case 'f':
            removeFirstPoint();
            glutPostRedisplay();
            break;
        case 'l':
            removeLastPoint();
            glutPostRedisplay();
            break;
        case 27:            // Escape key
            exit(0);
            break;
    }
}

void removeFirstPoint() {
    int i;
    if (NumPts > 0) {
        // Remove the first point, slide the rest down
        NumPts--;
        for (i = 0; i < NumPts; i++) {
            PointArray[i][0] = PointArray[i + 1][0];
            PointArray[i][1] = PointArray[i + 1][1];
            PointArray[i][2] = 0;
        }
    }
}

/*
 * Get the nearest point in click range
 * @return index of the nearest point in range
 */
int getPointInRange(float x, float y) {
    int near;
    double lowerDistance;
    double r;
    double distance;

    near = -1;
    r = 0.005;
    if (NumPts > 0) {
        //check if the first point is near click
        lowerDistance = sqrt(pow(x - PointArray[0][0], 2) + pow(y - PointArray[0][1], 2));
        if (lowerDistance <= r) near = 0;

        //check other points
        for (int i = 1; i < NumPts; i++) {
            distance = sqrt(pow(x - PointArray[i][0], 2.0) + pow(y - PointArray[i][1], 2.0));
            if (distance <= r && distance <= lowerDistance) {
                near = i;
                lowerDistance = distance;
            }
        }
    }


    return near;
}

/*
 * Moves a control point
 * @param i - index of the control point
 * @param x - x mouse's coordinate
 * @param y - y moouse's coordinate
 */
void movePoint(int i, int x, int y) {
    float xPos;
    float yPos;

    yPos = ((float) y) / ((float) (WindowHeight - 1));
    xPos = ((float) x) / ((float) (WindowWidth - 1));
    yPos = 1.0f - yPos;               // Flip value since y position is from top row.

    PointArray[i][0] = xPos;
    PointArray[i][1] = yPos;
    glutPostRedisplay();
}

// Left button presses place a control point.
void myMouseFunc(int button, int state, int x, int y) {
    float xPos;
    float yPos;

    yPos = ((float) y) / ((float) (WindowHeight - 1));
    xPos = ((float) x) / ((float) (WindowWidth - 1));
    yPos = 1.0f - yPos;               // Flip value since y position is from top row.

    // [NEW] check if point in range
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        clickedPoint = getPointInRange(xPos, yPos);
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (clickedPoint >= 0) {
            movePoint(clickedPoint, x, y);
            clickedPoint = -1;
        } else {
            addNewPoint(xPos, yPos);
            glutPostRedisplay();
        }
    }
}

/*
 * Handles mouse move events
 * @param x - mouse x position
 * @param y - mouse y position
 */
void mousemove(int x, int y) {
    if (clickedPoint >= 0) {
        movePoint(clickedPoint, x, y);
    }
}

// Add a new point to the end of the list.
// Remove the first point in the list if too many points.
void removeLastPoint() {
    if (NumPts > 0) {
        NumPts--;
    }
}

// Add a new point to the end of the list.
// Remove the first point in the list if too many points.
void addNewPoint(float x, float y) {
    std::array<float, 3> newPoint = {x, y, 0};
    PointArray.push_back(newPoint);
    NumPts++;

}

/*
 * Copies a point
 * @param var - destination point
 * @param value - original point
 */
void pointCopy(float var[], std::array<float, 3> value) {
    var[0] = value[0];
    var[1] = value[1];
    var[2] = value[2];
}

/*
 * Linear interpolation
 * @param p1 - first point
 * @param p2 - second point
 * @param t - evaluates the curve in t (range 0-1)
 * @param ret - returned point
 */
void lerp(float *p1, float *p2, float t, float *ret) {
    ret[0] = ((1 - t) * p1[0]) + (t * p2[0]);
    ret[1] = ((1 - t) * p1[1]) + (t * p2[1]);
    ret[2] = ((1 - t) * p1[2]) + (t * p2[2]);
}

void deCasteljau(float t) {
    float temp[NumPts][3];
    int i;

    for (i = 0; i < NumPts; i++) {
        pointCopy(temp[i], PointArray[i]);
    }

    for (i = 1; i < NumPts; i++) {
        for (int j = 0; j < NumPts - i; j++) {
            lerp(temp[j], temp[j + 1], t, temp[j]);

        }

    }
    glVertex3f(temp[0][0], temp[0][1], temp[0][2]);
}

void display(void) {
    int i;

    glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(2);

    // Draw the line segments

    if (NumPts > 1) {
        glColor3f(0.968, 0.113, 0.113);            // Reddish/purple lines
        glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, NumPts, &PointArray[0][0]);
        glBegin(GL_LINE_STRIP);
        for (i = 0; i < NumPts; i++) {
            glVertex2f(PointArray[i][0], PointArray[i][1]);
        }
        glEnd();
        glColor3f(0.270, 0.968, 0.113);            // Green lines
        glBegin(GL_LINE_STRIP);
        for (i = 0; i < 100; i = i + 2) {
            deCasteljau((float) i / 100);
        }
        glEnd();
    }

    // Draw the interpolated points second.
    glColor3f(0.0f, 0.0f, 0.0f);            // Draw points in black
    glBegin(GL_POINTS);

    for (i = 0; i < NumPts; i++) {
        glVertex2f(PointArray[i][0], PointArray[i][1]);
    }
    glEnd();

    glFlush();
}

void initRendering() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Make big points and wide lines.  (This may be commented out if desired.)
    glPointSize(8);


    // The following commands should induce OpenGL to create round points and
    //	antialias points and lines.  (This is implementation dependent unfortunately, and
    //  may slow down rendering considerably.)
    //  You may comment these out if you wish.
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);    // Make round points, not square points
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);        // Antialias the lines
    glEnable(GL_MAP1_VERTEX_3);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void resizeWindow(int w, int h) {
    WindowHeight = (h > 1) ? h : 2;
    WindowWidth = (w > 1) ? w : 2;
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);  // Always view [0,1]x[0,1].
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Assignment 1 - bezier curves");

    initRendering();

    glutDisplayFunc(display);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(myKeyboardFunc);
    glutMouseFunc(myMouseFunc);
    glutMotionFunc(mousemove); // detects mouse move while dragging
    //glutPassiveMotionFunc(passiveMouse)
    glutMainLoop();

    return 0;                    // This line is never reached
}
