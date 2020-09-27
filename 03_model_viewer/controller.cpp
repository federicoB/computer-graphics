//
// Created by fede on 04/09/20.
//

//all keyboard and mouse event handlers


//keyboard macros
#define WHEEL_UP 3
#define WHEEL_DOWN 4

static int last_mouse_pos_Y;
static int last_mouse_pos_X;

// Keyboard:  g traslate r rotate s scale x,y,z axis esc
void keyboardDown(unsigned char key, int x, int y) {
    switch (key) {
        // Selezione della modalit� di trasformazione
        case 'g':
            OperationMode = TRASLATING;
            break;
        case 'r':
            OperationMode = ROTATING;
            break;
        case 's':
            OperationMode = SCALING;
            break;
        case ' ':
            OperationMode = CAMERA_MOVING;
            if(!cameraAnimation) create_camera_animation_path();
            break;
        case 27:
            glutLeaveMainLoop();
            break;
            // Selezione dell'asse
        case 'x':
            WorkingAxis = X;
            break;
        case 'y':
            WorkingAxis = Y;
            break;
        case 'z':
            WorkingAxis = Z;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

// Special key arrow: select active object (arrows left,right)
void special(int key, int x, int y) {
    if (!cameraAnimation) //changing selected object is blocked during camera animation
        switch (key) {
            case GLUT_KEY_LEFT:
                    selected_object = selected_object == 0 ? objects.size() - 1 : selected_object - 1;
                break;
            case GLUT_KEY_RIGHT:
                    selected_object = (selected_object + 1) % objects.size();
                break;
            default:
                break;
        }
        glutPostRedisplay();
}

#include "view_utilis.cpp"

// Trackball: Effettua la rotazione del vettore posizione sulla trackball
void mouseActiveMotion(int x, int y) {
    // Spostamento su trackball del vettore posizione Camera
    if (moving_trackball) {
        glm::vec3 destination = getTrackBallPoint(x, y);
        glm::vec3 origin = getTrackBallPoint(last_mouse_pos_X, last_mouse_pos_Y);
        float dx, dy, dz;
        dx = destination.x - origin.x;
        dy = destination.y - origin.y;
        dz = destination.z - origin.z;
        if (dx || dy || dz) {
            // rotation angle = acos( (v dot w) / (len(v) * len(w)) ) o approssimato da ||dest-orig||;
            float pi = glm::pi<float>();
            float angle = sqrt(dx * dx + dy * dy + dz * dz) * (180.0 / pi);
            // rotation axis = (dest vec orig) / (len(dest vec orig))
            glm::vec3 rotation_vec = glm::cross(origin, destination);
            // calcolo del vettore direzione w = C - A
            glm::vec4 direction = viewSetup.position - viewSetup.target;
            // rotazione del vettore direzione w
            // determinazione della nuova posizione della camera
            viewSetup.position =
                    viewSetup.target + glm::rotate(glm::mat4(1.0f), glm::radians(-angle), rotation_vec) * direction;
        }
        last_mouse_pos_X = x;
        last_mouse_pos_Y = y;
        glutPostRedisplay();
    }
}

void mouseClick(int button, int state, int x, int y) {
    if (!cameraAnimation) { //mouse is disabled during camera animation
        glutPostRedisplay();
        int modifiers = glutGetModifiers();
        if (modifiers == GLUT_ACTIVE_SHIFT) {
            switch (button) {
                case WHEEL_UP:
                    verticalPan(CAMERA_TRASLATION_SPEED);
                    break;
                case WHEEL_DOWN:
                    verticalPan(-CAMERA_TRASLATION_SPEED);
                    break;
            }
            return;
        }
        if (modifiers == GLUT_ACTIVE_CTRL) {
            switch (button) {
                case WHEEL_UP:
                    horizontalPan(CAMERA_TRASLATION_SPEED);
                    break;
                case WHEEL_DOWN:
                    horizontalPan(-CAMERA_TRASLATION_SPEED);
                    break;
            }
            return;
        }
    }


    glm::vec4 axis;
    float amount = 0.1f;
    // Imposto il valore della trasformazione
    switch (button) {
        case 3:// scroll wheel up
            amount *= 1;
            break;
        case 4:// scroll wheel down
            amount *= -1;
            break;
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) { moving_trackball = true; }
            if (state == GLUT_UP) { moving_trackball = false; }
            OperationMode = NAVIGATION;
            last_mouse_pos_X = x;
            last_mouse_pos_Y = y;
            break;
        default:
            break;
    }
    // Selezione dell'asse per le trasformazioni
    switch (WorkingAxis) {
        case X:
            axis = glm::vec4(1.0, 0.0, 0.0, 0.0);
            break;
        case Y:
            axis = glm::vec4(0.0, 1.0, 0.0, 0.0);
            break;
        case Z:
            axis = glm::vec4(0.0, 0.0, 1.0, 0.0);
            break;
        default:
            break;
    }

    switch (OperationMode) {
        case TRASLATING:
            modifyModelMatrix(axis * amount, axis, 0.0f, 1.0f);
            break;
        case ROTATING:
            modifyModelMatrix(glm::vec4(0), axis, amount * 20.0f, 1.0f);
            break;
        case SCALING:
            //TODO fix texture scaling
            modifyModelMatrix(glm::vec4(0), axis, 0.0f, 1.0f + amount);
            break;
        case NAVIGATION:
            // Wheel reports as button 3(scroll up) and button 4(scroll down)
            if (button == 3) {
                zoom(CAMERA_ZOOM_SPEED);
            } else if (button == 4) {
                zoom(-CAMERA_ZOOM_SPEED);
            }
            break;
        default:
            break;
    }
}
