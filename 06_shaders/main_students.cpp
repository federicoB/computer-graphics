/******************************************************************************************
LAB 06
Gestione interattiva di una scena 3D mediante controllo da mouse e da tastiera.
I modelli geometrici in scena sono mesh poligonali caricati da file in formato *.obj,  
con associata una parametrizzazione (sfere e cubo), oggetti poligonali creati in modo procedurale
(toroide).  
W/w		incrementa/decrementa NumWrap toro
N/n		incrementa/decrementa NumPerWrap toro

CTRL+WHEEL = pan orizzontale della telecamera
SHIFT+WHEEL = pan verticale della telecamera
WHELL = ZOOM IN/OUT se si è in navigazione, altrimenti agisce sulla trasformazione dell'oggetto
g r s	per le modalità di lavoro: traslate/rotate/scale
x y z	per l'asse di lavoro
wcs/ocs selezionabili dal menu pop-up

OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software
based on the OpenGL Shading Language (GLSL) specifications.
***********************************com  ********************************************************/

#define _CRT_SECURE_NO_WARNINGS // for fscanf

#include "constants.cpp"
vector<GLuint> shaders_IDs; //Pointers to the shader programs
#include "HUD_Logger.cpp"
#include "display_object.cpp"
#include "camera.cpp"
#include "window.cpp"
#include "mesh_loading.cpp"
#include "menu.cpp"
#include "objects.cpp"
#include "controller.cpp"
#include "shaders_utils.cpp"
#include "init_setup.cpp"


void display() {


    // grey background color
    glClearColor(0.5, 0.5, 0.5, 1);
    // clear the window and the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Draw the central Axis point of reference and the grid
    drawAxisAndGrid(); 

    for (Object object : objects) {
        display_object(object);
    }


    if(cameraAnimation) { //perform camera animation
        camera_animation_step();
    }

    // OLD fixed pipeline for Text and symbols
    glUseProgram(0);
    printToScreen();

    glutSwapBuffers();


}

void init() {
// Default render settings
    OperationMode = NAVIGATION;

    glEnable(GL_DEPTH_TEST);    // Hidden surface removal

    glEnable(GL_LINE_SMOOTH); //enable line antialiasing
    glShadeModel(GL_FLAT); //start with default flat shading

    //Light initialization
    light.position = {5.0,5.0,-5.0};
    light.color = {1.0,1.0,1.0};
    light.power = 1.f;

    material_setup();

    shaders_setup();

    // Camera Setup
    viewSetup = {};
    viewSetup.position = glm::vec4(-10.0, 5.0, -10.0, 1.0);
    viewSetup.target = glm::vec4(0.0, 0.0, 0.0, 1.0);
    viewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);
    perspectiveSetup = {};
    perspectiveSetup.aspect = (GLfloat) WindowWidth / (GLfloat) WindowHeight;
    perspectiveSetup.fovY = 45.0f;
    perspectiveSetup.far_plane = 2000.0f;
    perspectiveSetup.near_plane = 1.0f;

    init_sphere_FLAT();
    init_sphere_SMOOTH();
    init_textured_plane();
    init_cube();
    init_light_object();
    init_axis();
    init_grid();
    init_torus();

    // call display directly to show every object in the scene
    display();
}

int main(int argc, char **argv) {

    init_window(argc,argv);

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
