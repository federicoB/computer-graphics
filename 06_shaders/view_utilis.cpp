//
// Created by fede on 04/09/20.
//

#pragma once

#include "window.cpp"

// Trackball: Converte un punto 2D sullo schermo in un punto 3D sulla trackball
glm::vec3 getTrackBallPoint(float x, float y) {
    float zTemp;
    glm::vec3 point;
    //map to [-1;1]
    point.x = (2.0f * x - WindowWidth) / WindowWidth;
    point.y = (WindowHeight - 2.0f * y) / WindowHeight;

    zTemp = 1.0f - pow(point.x, 2.0) - pow(point.y, 2.0);
    point.z = (zTemp > 0.0f) ? sqrt(zTemp) : 0.0;
    return point;
}

void deCasteljau(float curveCP[][3], float t) {
    float temp[CONTROL_POINTS][3];
    int i;

    //copy Cp array in a temp array to not overwrite it
    for (i = 0; i < CONTROL_POINTS; i++) {
        temp[i][0] = curveCP[i][0];
        temp[i][1] = curveCP[i][1];
        temp[i][2] = curveCP[i][2];
    }

    //lerp
    for (i = 1; i < CONTROL_POINTS; i++)
        for (int j = 0; j < CONTROL_POINTS - i; j++) { // Interpolazione lineare (lerp)
            temp[j][0] = t * temp[j+1][0] + (1-t) * temp[j][0];
            temp[j][1] = t * temp[j+1][1] + (1-t) * temp[j][1];
            temp[j][2] = t * temp[j+1][2] + (1-t) * temp[j][2];
        }

    for (i = 0; i < 3; i++)
        viewSetup.position[i] = temp[0][i];
}

/*
	Crea ed applica la matrice di trasformazione alla matrice dell'oggeto discriminando tra WCS e OCS.
	La funzione � gia invocata con un input corretto, � sufficiente concludere la sua implementazione.
*/
void modifyModelMatrix(glm::vec4 translation_vector, glm::vec4 rotation_vector, GLfloat angle, GLfloat scale_factor) {
    glPushMatrix();
    glLoadIdentity();
    // Usare glMultMatrix per moltiplicare la matrice attiva in openGL con una propria matrice.
    // In alternativa si pu� anche usare glm per creare e manipolare le matrici.

    switch (TransformMode) {
        case WCS:
            glRotatef(angle, rotation_vector.x, rotation_vector.y, rotation_vector.z);
            glScalef(scale_factor, scale_factor, scale_factor);
            glTranslatef(translation_vector.x, translation_vector.y, translation_vector.z);
            // multiply for matrix that defines object position / rotation wrt world
            glMultMatrixf(value_ptr(objects[selected_object].model_matrix));
            break;
        case OCS:
            glMultMatrixf(value_ptr(objects[selected_object].model_matrix));
            glRotatef(angle, rotation_vector.x, rotation_vector.y, rotation_vector.z);
            glScalef(scale_factor, scale_factor, scale_factor);
            glTranslatef(translation_vector.x, translation_vector.y, translation_vector.z);
            break;
    }
    glGetFloatv(GL_MODELVIEW_MATRIX, value_ptr(objects[selected_object].model_matrix));
    glPopMatrix();
}