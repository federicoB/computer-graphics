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

#include "constants.cpp"

// TODO implement camera animation along path

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

#include "HUD_Logger.cpp"
#include "camera.cpp"
#include "window.cpp"
#include "mesh_loading.cpp"
#include "menu.cpp"
#include "controller.cpp"

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
    glutSolidSphere(0.1, 10, 10); // Light ball
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
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindVertexArray(object.mesh.vertexArrayObjID);
        glDrawElements(GL_TRIANGLES, object.mesh.indices.size(), GL_UNSIGNED_SHORT, (void *) 0);
        glPopMatrix();
    }

    printToScreen();

    glutSwapBuffers();
}

void init() {
// Default render settings
    OperationMode = NAVIGATION;

    glEnable(GL_DEPTH_TEST);    // Hidden surface removal

    glEnable(GL_LINE_SMOOTH); //enable line antialiasing
    glShadeModel(GL_FLAT); //start with default flat shading
    //FOG Setup for nice background transition
    glEnable(GL_FOG); // enable shader placeholder "fog"
    glFogi(GL_FOG_MODE, GL_LINEAR);  // type of fog
    GLfloat fog_color[4] = {0.5, 0.5, 0.5, 1.0};
    glFogfv(GL_FOG_COLOR, fog_color);
    glFogf(GL_FOG_START, 50.0f);
    glFogf(GL_FOG_END, 500.0f);
    // Light Setup
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); //enable light 0 in the evaluation of the lighting equation


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
    viewSetup = {};
    viewSetup.position = glm::vec4(10.0, 10.0, 10.0, 1.0);
    viewSetup.target = glm::vec4(0.0, 0.0, 0.0, 1.0);
    viewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);
    perspectiveSetup = {};
    perspectiveSetup.aspect = (GLfloat) WindowWidth / (GLfloat) WindowHeight;
    perspectiveSetup.fovY = 45.0f;
    perspectiveSetup.far_plane = 2000.0f;
    perspectiveSetup.near_plane = 1.0f;

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
