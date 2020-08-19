//
// Created by fede on 19/08/20.
//


#include "window.h"
#include "HUD_Logger.h"

void init_window(int argc, char **argv) {

    // initialize openGL Utility Toolkit
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Model Viewer ");
}

void resize(int w, int h) {
    if (h == 0)    // Window is minimized
        return;
    int width = h * aspect_ratio;           // width is adjusted for aspect ratio
    int left = (w - width) / 2;
    // Set Viewport to window dimensions
    glViewport(left, 0, width, h);
    WindowWidth = w;
    WindowHeight = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(perspectiveSetup.fovY, perspectiveSetup.aspect, perspectiveSetup.near_plane,
                   perspectiveSetup.far_plane);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(viewSetup.position.x, viewSetup.position.y, viewSetup.position.z,
              viewSetup.target.x, viewSetup.target.y, viewSetup.target.z,
              viewSetup.upVector.x, viewSetup.upVector.y, viewSetup.upVector.z);
}

/*Logging to screen*/
void printToScreen() {
    string axis = "Asse: ";
    string mode = "Naviga/Modifica: ";
    string obj = "Oggetto: " + objects[selected_object].name;
    string ref = "Sistema WCS/OCS: ";
    string mat = "Materiale: " + materials[objects[selected_object].material].name;
    switch (WorkingAxis) {
        case X:
            axis += "X";
            break;
        case Y:
            axis += "Y";
            break;
        case Z:
            axis += "Z";
            break;
    }
    switch (OperationMode) {
        case TRASLATING:
            mode += "Traslazione";
            break;
        case ROTATING:
            mode += "Rotazione";
            break;
        case SCALING:
            mode += "Scalatura";
            break;
        case NAVIGATION:
            mode += "Naviga";
            break;
    }
    switch (TransformMode) {
        case OCS:
            ref += "OCS";
            break;
        case WCS:
            ref += "WCS";
            break;
    }
    vector<string> lines;
    lines.push_back(mat);
    lines.push_back(obj);
    lines.push_back(axis);
    lines.push_back(mode);
    lines.push_back(ref);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0, WindowHeight * aspect_ratio, 0, WindowHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    HUD_Logger::get()->printInfo(lines);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    resize(WindowWidth, WindowHeight);
}
