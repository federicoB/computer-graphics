//
// Created by fede on 19/08/20.
//

#pragma once

static int WindowWidth = 1366;
static int WindowHeight = 768;
static GLfloat aspect_ratio = 16.0f / 9.0f;

static Object Axis, Grid;

void init_window(int argc, char **argv) {

    // initialize openGL Utility Toolkit
    glutInit(&argc, argv);
    glutSetOption(GLUT_MULTISAMPLE, 4); //activate multisampling
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Model Viewer with Shaders");
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

    // Fixed Pipeline matrices for retro compatibility
    glUseProgram(0); // Embedded openGL shader
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(perspectiveSetup.fovY, perspectiveSetup.aspect, perspectiveSetup.near_plane, perspectiveSetup.far_plane);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(viewSetup.position.x, viewSetup.position.y, viewSetup.position.z,
              viewSetup.target.x, viewSetup.target.y, viewSetup.target.z,
              viewSetup.upVector.x, viewSetup.upVector.y, viewSetup.upVector.z);

    // Programmable Pipeline matrices for object rendering
    glm::mat4 P = glm::perspective(perspectiveSetup.fovY, perspectiveSetup.aspect, perspectiveSetup.near_plane, perspectiveSetup.far_plane);
    glm::mat4 V = glm::lookAt(glm::vec3(viewSetup.position), glm::vec3(viewSetup.target), glm::vec3(viewSetup.upVector));

    for (int i = 0; i < shaders_IDs.size();i++) {
        glUseProgram(shaders_IDs[i]);
        glUniformMatrix4fv(base_uniforms[i].P_Matrix_pointer, 1, GL_FALSE, value_ptr(P));
        glUniformMatrix4fv(base_uniforms[i].V_Matrix_pointer, 1, GL_FALSE, value_ptr(V));
    }
}

#include "HUD_Logger.cpp"

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

// disegna l'origine del assi
void drawAxisAndGrid() {
    glUseProgram(shaders_IDs[Grid.shading]);
    glUniformMatrix4fv(base_uniforms[Grid.shading].M_Matrix_pointer, 1, GL_FALSE, value_ptr(Grid.model_matrix));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(Grid.mesh.vertexArrayObjID);
    glDrawArrays(GL_TRIANGLES, 0, Grid.mesh.vertices.size());
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glUseProgram(shaders_IDs[Axis.shading]);
    // Caricamento matrice trasformazione del modello
    glUniformMatrix4fv(base_uniforms[Axis.shading].M_Matrix_pointer, 1, GL_FALSE, value_ptr(Axis.model_matrix));
    glActiveTexture(GL_TEXTURE0); // this addresses the first sampler2D uniform in the shader
    glBindTexture(GL_TEXTURE_2D, Axis.textureID);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(Axis.mesh.vertexArrayObjID);
    glDrawArrays(GL_TRIANGLES, 0, Axis.mesh.vertices.size());
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
