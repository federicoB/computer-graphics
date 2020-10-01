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

// control point array
std::vector<std::array<float, 3>> controlPoints;

// Window size in pixels
int WindowHeight;
int WindowWidth;
int clickedPoint;

/* Prototypes */
void addNewPoint(float x, float y);

void removeFirstPoint();

void removeLastPoint();


void keyboard_handler(unsigned char key, int x, int y) {
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
        default:
            break;
    }
}

void removeFirstPoint() {
    if (not controlPoints.empty()) {
        // Remove the first point, slide the rest
        controlPoints.erase(controlPoints.begin());
    }
}

/*
 * Get the nearest point in click range
 * @param x,y coordinate of search center point
 * @return index of the nearest point in range
 */
int getPointInRange(float x, float y) {

    // index for nearest point
    int near = -1;

    double max_distance = 0.05;

    double lowestDistance;

    double distance;

    if (!controlPoints.empty()) {
        //initialize lowest distance using first point
        lowestDistance = sqrt(pow(x - controlPoints[0][0], 2) + pow(y - controlPoints[0][1], 2));
        if (lowestDistance <= max_distance) near = 0;
        //check remaining points
        for (int i = 1; i < controlPoints.size(); i++) {
            std::array<float, 3> controlPoint = controlPoints[i];
            // euclidean distance
            distance = sqrt(pow(x - controlPoint[0], 2.0) + pow(y - controlPoint[1], 2.0));
            // if the distance is acceptable and lower than current lowest distance
            if (distance <= max_distance && distance <= lowestDistance) {
                near = i;
                lowestDistance = distance;
            }
        }
    }
    return near;
}

// macro to change reference frame from mouse to window one
#define mouse_to_windows(x, y) \
 yPos = ((float) y) / ((float) (WindowHeight - 1)); \
 xPos = ((float) x) / ((float) (WindowWidth - 1)); \
 yPos = 1.0f - yPos; //invert y coordinate

/*
 * Moves a control point
 * @param i - index of the control point
 * @param x - x mouse's coordinate
 * @param y - y mouse's coordinate
 */
void movePoint(int i, int x, int y) {
    float xPos;
    float yPos;

    mouse_to_windows(x, y);
    //set new position
    controlPoints[i][0] = xPos;
    controlPoints[i][1] = yPos;
    glutPostRedisplay();
}


void mouseHandler(int button, int state, int x, int y) {
    // Left button presses place a control point.

    float xPos;
    float yPos;

    mouse_to_windows(x, y);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        //while left mouse button is down get nearest clicked point
        clickedPoint = getPointInRange(xPos, yPos);
    } else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        if (clickedPoint >= 0) {
            movePoint(clickedPoint, x, y);
            clickedPoint = -1;
        } else {
            // if outside range of control point
            // create a new point
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
    if (!controlPoints.empty()) {
        controlPoints.pop_back();
    }
}

// Add a new point to the end of the list.
// Remove the first point in the list if too many points.
void addNewPoint(float x, float y) {
    std::array<float, 3> newPoint = {x, y, 0};
    controlPoints.push_back(newPoint);

}

/*
 * Linear interpolation
 * @param p1 - first point
 * @param p2 - second point
 * @param t - evaluates the curve in t (range 0-1)
 * @param ret - returned point
 */
void lerp(std::array<float, 3> p1, std::array<float, 3> p2, float t, std::array<float, 3> &ret) {
    ret[0] = ((1 - t) * p1[0]) + (t * p2[0]);
    ret[1] = ((1 - t) * p1[1]) + (t * p2[1]);
    ret[2] = ((1 - t) * p1[2]) + (t * p2[2]);
}

void deCasteljau(float t) {
    std::array<float, 3> temp[controlPoints.size()];
    int i;

    for (i = 0; i < controlPoints.size(); i++) {
        temp[i] = controlPoints[i];
    }


    for (i = 1; i < controlPoints.size(); i++) { //lerp step
        for (int j = 0; j < controlPoints.size() - i; j++) { //for each point of lerp step
            lerp(temp[j], temp[j + 1], t, temp[j]);

        }

    }
    glVertex3f(temp[0][0], temp[0][1], temp[0][2]);
}

/*
* Calculate the distance between points
* @param p1 - point 1
* @param p2 - point 2
*/
float point2pointDistance(std::array<float, 3> p1, std::array<float, 3> p2) {
    return sqrtf(powf(p1[0] - p2[0], 2) + powf(p1[1] - p2[1], 2));
}

/*
 * Evaluate the distance of a point from a line
 * @param p0 - point
 * @param p1, p2 - extremity of segment
 */
float point2LineDistance(std::array<float, 3> p0, std::array<float, 3> p1, std::array<float, 3> p2) {
    return fabsf(
            ((p2[0] - p1[0]) * (p1[1] - p0[1])) // (x2 - x1) * (y1 - y0)
            - ((p1[0] - p0[0]) * (p2[1] - p1[1])) // - (x1 - x0) * (p2 - p1)
    ) / point2pointDistance(p1, p2);
}

/*
 * Adaptive subdivision algorithm
 * @param cps - curve control points
 * @param tolerance - degree of smoothness
 */
void adaptiveSubdivision(std::array<float, 3> controlPoints[], unsigned long numCP, float tolerance) {
    int i, j;
    std::array<float, 3> temp[numCP];
    std::array<float, 3> curve1[numCP];
    std::array<float, 3> curve2[numCP];
    bool canApproxLine = true;

    // check if tolerance is reached and line can be approximated

    // Calculates the distance of every control point from the line between
    // the first control point and the last control point
    for (i = 1; i < numCP - 1; i++) {
        if (point2LineDistance(controlPoints[i], controlPoints[0], controlPoints[numCP - 1]) > tolerance) {
            canApproxLine = false;
        }
    }

    // Draw the line if it can be approximated
    if (canApproxLine) {
        glVertex3f(controlPoints[0][0], controlPoints[0][1], controlPoints[0][2]);
        glVertex3f(controlPoints[numCP - 1][0], controlPoints[numCP - 1][1], controlPoints[numCP - 1][2]);
    } else {

        //copy every control point in temp
        for (i = 0; i < numCP; i++) {
            temp[i] = controlPoints[i];
            curve1[i] = controlPoints[i];
            curve2[i] = controlPoints[i];
        }

        // curve 1 has first control point
        curve1[0] = temp[0];

        // curve 2 has last control point
        curve2[numCP - 1] = temp[numCP - 1];

        // Otherwise evaluate the point in 0.5 (subdivision)
        for (i = 1; i < numCP; i++) {
            for (j = 0; j < numCP - i; j++) {
                lerp(temp[j], temp[j + 1], 0.5f, temp[j]);
            }
            curve1[i] = temp[0];
            curve2[numCP - i - 1] = temp[numCP - i - 1];
        }

        // Recursive call on the 2 sub curves
        adaptiveSubdivision(curve1, numCP, tolerance);
        adaptiveSubdivision(curve2, numCP, tolerance);
    }

}

void display() {
    int i;

    glClear(GL_COLOR_BUFFER_BIT);
    glLineWidth(2);

    // Draw the straight line segments between control points
    if (controlPoints.size() > 1) {
        glColor3f(0.968, 0.113, 0.113);            // Reddish/purple lines
        // define evaluator
        glMap1f(GL_MAP1_VERTEX_3, // type of data generated
                0.0, 1.0, // evaluate between this 2 points
                3, // stride
                controlPoints.size(),  // curve order
                &controlPoints[0][0]); //pointer to array of control points
        glBegin(GL_LINE_STRIP);
        for (i = 0; i < controlPoints.size(); i++) {
            glVertex2f(controlPoints[i][0], controlPoints[i][1]);
        }
        glEnd();

        // draw curve
        glColor3f(0.270, 0.968, 0.113);            // Green lines
        glBegin(GL_LINE_STRIP);
        std::array<float, 3> cPArray[controlPoints.size()];
        std::copy(controlPoints.begin(), controlPoints.end(), cPArray);
        adaptiveSubdivision(cPArray, controlPoints.size(), 0.00005f);
        // for (i = 0; i < 100; i = i + 2) {
        //    deCasteljau((float) i / 100);
        // }
        glEnd();
    }

    // Draw the interpolated points second.
    glColor3f(0.0f, 0.0f, 0.0f);            // Draw points in black
    glBegin(GL_POINTS);
    //TODO scale points
    for (i = 0; i < controlPoints.size(); i++) {
        glVertex2f(controlPoints[i][0], controlPoints[i][1]);
    }
    glEnd();

    glFlush();
}

void initRendering() {
    //white background color
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // set point size
    glPointSize(15);


    // The following commands should induce OpenGL to create round points and
    //	antialias points and lines.  (This is implementation dependent unfortunately, and
    //  may slow down rendering considerably.)
    //  You may comment these out if you wish.
    glEnable(GL_POINT_SMOOTH); //enable point antialiasing
    glEnable(GL_LINE_SMOOTH); //enable line antialisiasing
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);    // Make round points, not square points
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);        // Antialias the lines
    glEnable(GL_MAP1_VERTEX_3); //enable opengl bezier evaluator
    glEnable(GL_BLEND); //enable object blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set blending formula
}

void resizeWindow(int new_width, int new_heigth) {
    WindowHeight = (new_heigth > 1) ? new_heigth : 2;
    WindowWidth = (new_width > 1) ? new_width : 2;
    // change viewport
    glViewport(0, 0, (GLsizei) new_width, (GLsizei) new_heigth);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);  // Always view [0,1]x[0,1].
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv) {

    glutInit(&argc, argv);
    // double buffering buffer and rgb color support
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Assignment 1 - bezier curves");

    initRendering();

    glutDisplayFunc(display);
    glutReshapeFunc(resizeWindow);
    glutKeyboardFunc(keyboard_handler);
    glutMouseFunc(mouseHandler);
    glutMotionFunc(mousemove); // detects mouse move while dragging
    //glutPassiveMotionFunc(passiveMouse)
    glutMainLoop();

    return 0;                    // This line is never reached
}
