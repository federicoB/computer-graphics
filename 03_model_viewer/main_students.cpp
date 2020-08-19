/******************************************************************************************
LAB 03
Gestione interattiva di una scena 3D mediante controllo da mouse e da tastiera.
I modelli geometrici in scena sono mesh poligonali in formato *.obj

CTRL+WHEEL = pan orizzontale della telecamera
SHIFT+WHEEL = pan verticale della telecamera
WHELL = ZOOM IN/OUT se si � in navigazione, altrimenti agisce sulla trasformazione dell'oggetto
g r s	per le modalit� di lavoro: traslate/rotate/scale
x y z	per l'asse di lavoro
wcs/ocs selezionabili dal menu pop-up

OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software 
based on the OpenGL Shading Language (GLSL) specifications.
*******************************************************************************************/

#define _CRT_SECURE_NO_WARNINGS // for fscanf

#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "HUD_Logger.h"

//keyboard macros
#define SHIFT_WHEEL_UP 11
#define SHIFT_WHEEL_DOWN 12
#define CTRL_WHEEL_UP 19
#define CTRL_WHEEL_DOWN 20

using namespace std;

static int WindowWidth = 1366;
static int WindowHeight = 768;
GLfloat aspect_ratio = 16.0f / 9.0f;

static glm::vec4 lightpos = {5.0f, 5.0f, 5.0f, 1.0f};

static vector<Object> objects;
static vector<Material> materials;

/*camera structures*/
constexpr float CAMERA_ZOOM_SPEED = 0.1f;
constexpr float CAMERA_TRASLATION_SPEED = 0.01f;

struct {
    glm::vec4 position;
    glm::vec4 target;
    glm::vec4 upVector;
} ViewSetup;

struct {
    float fovY, aspect, near_plane, far_plane;
} PerspectiveSetup;

enum {
    OCS, // Object Coordinate System
    WCS // World Coordinate System
} TransformMode;

static bool moving_trackball = 0;
static int last_mouse_pos_Y;
static int last_mouse_pos_X;

static unsigned int selected_object = 0;

// legge un file obj ed inizializza i vector della mesh in input
void loadObjFile(string file_path, Mesh *mesh) {
    FILE *file = fopen(file_path.c_str(), "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return;
    }
    char lineHeader[128];
    while (fscanf(file, "%s", lineHeader) != EOF) {
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(file, " %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            mesh->vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            GLuint a, b, c;
            fscanf(file, " %d %d %d\n", &a, &b, &c);
            a--;
            b--;
            c--;
            mesh->indices.push_back(a);
            mesh->indices.push_back(b);
            mesh->indices.push_back(c);
        }
    }

    //TODO calcolo normali
}

// Genera i buffer per la mesh in input e ne salva i puntatori di openGL
void generate_and_load_buffers(Mesh *mesh) {
    // Genero 1 Vertex Array Object
    glGenVertexArrays(1, &mesh->vertexArrayObjID);
    glBindVertexArray(mesh->vertexArrayObjID);

    // Genero 1 Vertex Buffer Object per i vertici
    glGenBuffers(1, &mesh->vertexBufferObjID);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(glm::vec3), &mesh->vertices[0], GL_STATIC_DRAW);
    glVertexPointer(
            3,                  // size
            GL_FLOAT,           // type
            0,                  // stride
            (void *) 0            // array buffer offset
    );

    //TODO caricamento normali in memoria

    // Genero 1 Element Buffer Object per gli indici, Nota: GL_ELEMENT_ARRAY_BUFFER
    glGenBuffers(1, &mesh->indexBufferObjID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(GLshort), mesh->indices.data(), GL_STATIC_DRAW);
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
            //TODO do something different for WCS ad OCS
            glRotatef(angle, rotation_vector.x, rotation_vector.y, rotation_vector.z);
            glScalef(scale_factor, scale_factor, scale_factor);
            glTranslatef(translation_vector.x, translation_vector.y, translation_vector.z);
            glMultMatrixf(objects[selected_object].model_matrix);
            break;
        case OCS:
            glMultMatrixf(objects[selected_object].model_matrix);
            glRotatef(angle, rotation_vector.x, rotation_vector.y, rotation_vector.z);
            glScalef(scale_factor, scale_factor, scale_factor);
            glTranslatef(translation_vector.x, translation_vector.y, translation_vector.z);
            break;
    }
    glGetFloatv(GL_MODELVIEW_MATRIX, objects[selected_object].model_matrix);
    glPopMatrix();
}


void init() {
// Default render settings
    OperationMode = NAVIGATION;

    glEnable(GL_DEPTH_TEST);    // Hidden surface removal
    glCullFace(GL_BACK);    // remove faces facing the background

    glEnable(GL_LINE_SMOOTH);
    glShadeModel(GL_FLAT);
    //FOG Setup for nice backgound transition
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    GLfloat fog_color[4] = {0.5, 0.5, 0.5, 1.0};
    glFogfv(GL_FOG_COLOR, fog_color);
    glFogf(GL_FOG_START, 50.0f);
    glFogf(GL_FOG_END, 500.0f);
    // Light Setup
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);


    // Materials setup
    materials.resize(4);
    materials[MaterialType::RED_PLASTIC].name = "Red Plastic";
    materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
    materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
    materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
    materials[MaterialType::RED_PLASTIC].shiness = red_plastic_shininess;

    materials[MaterialType::EMERALD].name = "Emerald";
    materials[MaterialType::EMERALD].ambient = emerald_ambient;
    materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
    materials[MaterialType::EMERALD].specular = emerald_specular;
    materials[MaterialType::EMERALD].shiness = emerald_shininess;

    materials[MaterialType::BRASS].name = "Brass";
    materials[MaterialType::BRASS].ambient = brass_ambient;
    materials[MaterialType::BRASS].diffuse = brass_diffuse;
    materials[MaterialType::BRASS].specular = brass_specular;
    materials[MaterialType::BRASS].shiness = brass_shininess;

    materials[MaterialType::SLATE].name = "Slate";
    materials[MaterialType::SLATE].ambient = slate_ambient;
    materials[MaterialType::SLATE].diffuse = slate_diffuse;
    materials[MaterialType::SLATE].specular = slate_specular;
    materials[MaterialType::SLATE].shiness = slate_shiness;

    // Camera Setup
    ViewSetup = {};
    ViewSetup.position = glm::vec4(10.0, 10.0, 10.0, 1.0);
    ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 1.0);
    ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);
    PerspectiveSetup = {};
    PerspectiveSetup.aspect = (GLfloat) WindowWidth / (GLfloat) WindowHeight;
    PerspectiveSetup.fovY = 45.0f;
    PerspectiveSetup.far_plane = 2000.0f;
    PerspectiveSetup.near_plane = 1.0f;

    string objectNames[] = {"sphere","cow","horse"};
    vector<glm::vec4> startingpositions = {
            {0,0,0,0},
            {0,0,-5,-5},
            {0,0,10,10}
    };

    for (unsigned int i=0; i< size(objectNames); i++) {
        string objectName = objectNames[i];
        // Mesh Loading
        Mesh mesh = {};
        loadObjFile(MeshDir + objectName + ".obj", &mesh);
        generate_and_load_buffers(&mesh);
        // Object Setup
        Object obj = {};
        obj.mesh = mesh;
        obj.material = MaterialType::RED_PLASTIC;
        obj.name = objectName;
        glLoadIdentity();
        selected_object = i;
        glGetFloatv(GL_MODELVIEW_MATRIX, obj.model_matrix);
        objects.push_back(obj);
        modifyModelMatrix(startingpositions[i],glm::vec4(1),0.f,1.0f);
    }
}

// disegna l'origine del assi
void drawAxis(float scale, int drawLetters) {
    glPushMatrix();
    glScalef(scale, scale, scale);
    glBegin(GL_LINES);

    glColor4d(1.0, 0.0, 0.0, 1.0);
    if (drawLetters) {
        glVertex3f(.8f, 0.05f, 0.0);
        glVertex3f(1.0, 0.25f, 0.0); /* Letter X */
        glVertex3f(0.8f, .25f, 0.0);
        glVertex3f(1.0, 0.05f, 0.0);
    }
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0); /* X axis      */


    glColor4d(0.0, 1.0, 0.0, 1.0);
    if (drawLetters) {
        glVertex3f(0.10f, 0.8f, 0.0);
        glVertex3f(0.10f, 0.90f, 0.0); /* Letter Y */
        glVertex3f(0.10f, 0.90f, 0.0);
        glVertex3f(0.05, 1.0, 0.0);
        glVertex3f(0.10f, 0.90f, 0.0);
        glVertex3f(0.15, 1.0, 0.0);
    }
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0); /* Y axis      */


    glColor4d(0.0, 0.0, 1.0, 1.0);
    if (drawLetters) {
        glVertex3f(0.05f, 0, 0.8f);
        glVertex3f(0.20f, 0, 0.8f); /* Letter Z*/
        glVertex3f(0.20f, 0, 0.8f);
        glVertex3f(0.05, 0, 1.0);
        glVertex3f(0.05f, 0, 1.0);
        glVertex3f(0.20, 0, 1.0);
    }
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0); /* Z axis    */

    glEnd();
    glPopMatrix();
}

// disegna la griglia del piano xz (white)
void drawGrid(float scale, int dimension) {
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glScalef(scale, scale, scale);
    glBegin(GL_LINES);
    for (int x = -dimension; x < dimension; x++) {
        glVertex3f(x, 0.0f, -dimension);
        glVertex3f(x, 0.0f, dimension);
    }
    for (int z = -dimension; z < dimension; z++) {
        glVertex3f(-dimension, 0.0f, z);
        glVertex3f(dimension, 0.0f, z);
    }
    glEnd();
    glPopMatrix();
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
    gluPerspective(PerspectiveSetup.fovY, PerspectiveSetup.aspect, PerspectiveSetup.near_plane,
                   PerspectiveSetup.far_plane);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z,
              ViewSetup.target.x, ViewSetup.target.y, ViewSetup.target.z,
              ViewSetup.upVector.x, ViewSetup.upVector.y, ViewSetup.upVector.z);
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

void display() {
    // grey background color
    glClearColor(0.5, 0.5, 0.5, 1);
    // clear the window and the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    //Static scene elements
    // update light position
    glDisable(GL_LIGHTING);
    // light position
    glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(lightpos));
    glPushMatrix();
    glTranslatef(lightpos.x, lightpos.y, lightpos.z);
    glColor4d(1, 1, 1, 1);
    glutSolidSphere(0.1, 10, 10); // Ligh ball
    glPopMatrix();
    drawAxis(3.0, 1); // The central Axis point of reference
    drawGrid(10.0, 100); // The horizontal grid
    glEnable(GL_LIGHTING);

    for (Object object : objects) {
        glPushMatrix();
        glMultMatrixf(object.model_matrix);
        glDisable(GL_LIGHTING);
        drawAxis(1.0, 0);
        glEnable(GL_LIGHTING);

        //Material Setup
        Material mat = materials[object.material];
        glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(mat.ambient));
        glMaterialfv(GL_FRONT, GL_DIFFUSE, glm::value_ptr(mat.diffuse));
        glMaterialfv(GL_FRONT, GL_SPECULAR, glm::value_ptr(mat.specular));
        glMaterialf(GL_FRONT, GL_SHININESS, mat.shiness);

        glEnableClientState(GL_VERTEX_ARRAY);
        glBindVertexArray(object.mesh.vertexArrayObjID);
        glDrawElements(GL_TRIANGLES, object.mesh.indices.size(), GL_UNSIGNED_SHORT, (void *) 0);
        glPopMatrix();
    }

    printToScreen();

    glutSwapBuffers();
}

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

void moveCameraForeward() {
    //TODO moveCameraForeward
}

void moveCameraBack() {
    //TODO moveCameraBack
}

void moveCameraLeft() {
    //TODO moveCameraLeft
}

void moveCameraRight() {
    //TODO moveCameraRight
}

void moveCameraUp() {
    //TODO moveCameraUp
}

void moveCameraDown() {
    //TODO moveCameraDown
}

void mouseClick(int button, int state, int x, int y) {
    glutPostRedisplay();
    int modifiers = glutGetModifiers();
    if (modifiers == GLUT_ACTIVE_SHIFT) {
        switch (button) {
            case SHIFT_WHEEL_UP:
                moveCameraUp();
                break;
            case SHIFT_WHEEL_DOWN:
                moveCameraDown();
                break;
        }
        return;
    }
    if (modifiers == GLUT_ACTIVE_CTRL) {
        switch (button) {
            case CTRL_WHEEL_UP:
                moveCameraRight();
                break;
            case CTRL_WHEEL_DOWN:
                moveCameraLeft();
                break;
        }
        return;
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
            modifyModelMatrix(glm::vec4(0), axis, 0.0f, 1.0f + amount);
            break;
        case NAVIGATION:
            // Wheel reports as button 3(scroll up) and button 4(scroll down)
            if (button == 3) {
                moveCameraBack();
            } else if (button == 4) {
                moveCameraForeward();
            }
            break;
        default:
            break;
    }
}

// Trackball: Effettua la rotazione del vettore posizione sulla trackball
void mouseActiveMotion(int x, int y) {
    // Spostamento su trackball del vettore posizione Camera
    if (!moving_trackball) {
        return;
    }
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
        glm::vec4 direction = ViewSetup.position - ViewSetup.target;
        // rotazione del vettore direzione w
        // determinazione della nuova posizione della camera
        ViewSetup.position =
                ViewSetup.target + glm::rotate(glm::mat4(1.0f), glm::radians(-angle), rotation_vec) * direction;
    }
    last_mouse_pos_X = x;
    last_mouse_pos_Y = y;
    glutPostRedisplay();
}

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

// gestione delle voci principali del menu
void main_menu_func(int option) {
    switch (option) {
        case MenuOption::FLAT_SHADING:
            glShadeModel(GL_FLAT);
            break;
        case MenuOption::SMOOTH_SHADING:
            glShadeModel(GL_SMOOTH);
            break;
        case MenuOption::WIRE_FRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case MenuOption::FACE_FILL:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case MenuOption::CULLING_ON:
            glEnable(GL_CULL_FACE);
            break;
        case MenuOption::CULLING_OFF:
            glDisable(GL_CULL_FACE);
            break;
        case MenuOption::CHANGE_TO_OCS:
            TransformMode = OCS;
            break;
        case MenuOption::CHANGE_TO_WCS:
            TransformMode = WCS;
            break;
        default:
            break;
    }
}

// gestione delle voci principali del sub menu per i matriali
void material_menu_function(int option) {
    switch (option) {
        case MaterialType::RED_PLASTIC:
            objects[selected_object].material = MaterialType::RED_PLASTIC;
            break;
        case MaterialType::EMERALD:
            objects[selected_object].material = MaterialType::EMERALD;
            break;
        case MaterialType::BRASS:
            objects[selected_object].material = MaterialType::BRASS;
            break;
        case MaterialType::SLATE:
            objects[selected_object].material = MaterialType::SLATE;
            break;
        default:
            break;
    }
}

// costruisce i menu openGL
void buildOpenGLMenu() {
    int materialSubMenu = glutCreateMenu(material_menu_function);

    glutAddMenuEntry(materials[MaterialType::RED_PLASTIC].name.c_str(), MaterialType::RED_PLASTIC);
    glutAddMenuEntry(materials[MaterialType::EMERALD].name.c_str(), MaterialType::EMERALD);
    glutAddMenuEntry(materials[MaterialType::BRASS].name.c_str(), MaterialType::BRASS);
    glutAddMenuEntry(materials[MaterialType::SLATE].name.c_str(), MaterialType::SLATE);


    glutCreateMenu(main_menu_func); // richiama main_menu_func() alla selezione di una voce menu
    glutAddMenuEntry("Opzioni", -1); //-1 significa che non si vuole gestire questa riga
    glutAddMenuEntry("", -1);
    glutAddMenuEntry("Wireframe", MenuOption::WIRE_FRAME);
    glutAddMenuEntry("Face fill", MenuOption::FACE_FILL);
    glutAddMenuEntry("Smooth Shading", MenuOption::SMOOTH_SHADING);
    glutAddMenuEntry("Flat Shading", MenuOption::FLAT_SHADING);
    glutAddMenuEntry("Culling: ON", MenuOption::CULLING_ON);
    glutAddMenuEntry("Culling: OFF", MenuOption::CULLING_OFF);
    glutAddSubMenu("Material", materialSubMenu);
    glutAddMenuEntry("World coordinate system", MenuOption::CHANGE_TO_WCS);
    glutAddMenuEntry("Object coordinate system", MenuOption::CHANGE_TO_OCS);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}



int main(int argc, char **argv) {

    // initialize openGL Utility Toolkit
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Model Viewer ");

    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboardDown);
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseActiveMotion);
    glutSpecialFunc(special);

    glewExperimental = GL_TRUE;

    // create a valid OpenGL rendering context
    GLenum GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult) {
        fprintf(
                stderr,
                "ERROR: %s\n",
                glewGetErrorString(GlewInitResult)
        );
        exit(EXIT_FAILURE);
    }
    fprintf(
            stdout,
            "INFO: OpenGL Version: %s\n",
            glGetString(GL_VERSION)
    );
    init();
    buildOpenGLMenu();


    glutMainLoop();

    return 1;
}
