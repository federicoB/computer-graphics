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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "common.h"

//TODO decide where to move these
//Shaders Uniforms
static vector<LightShaderUniform> light_uniforms; // for shaders with light
static vector<BaseShaderUniform> base_uniforms; // for ALL shaders
vector<GLuint> shaders_IDs; //Pointers to the shader programs
static Object Axis, Grid;
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
            //TODO fix with method that support glm::mat4
            //glMultMatrixf(objects[selected_object].model_matrix);
            break;
        case OCS:
            //TODO fix with method that support glm::mat4
            //glMultMatrixf(objects[selected_object].model_matrix);
            glRotatef(angle, rotation_vector.x, rotation_vector.y, rotation_vector.z);
            glScalef(scale_factor, scale_factor, scale_factor);
            glTranslatef(translation_vector.x, translation_vector.y, translation_vector.z);
            break;
    }
    //TODO fix with method that support glm::mat4
    //glGetFloatv(GL_MODELVIEW_MATRIX, objects[selected_object].model_matrix);
    glPopMatrix();
}

#include "HUD_Logger.cpp"
#include "camera.cpp"
#include "window.cpp"
#include "mesh_loading.cpp"
#include "menu.cpp"
#include "objects.cpp"
#include "controller.cpp"

void display() {


    // grey background color
    glClearColor(0.5, 0.5, 0.5, 1);
    // clear the window and the depth buffer
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // Draw the central Axis point of reference and the grid
    drawAxisAndGrid();

    //Static scene elements
    // update light position
    glDisable(GL_LIGHTING);
    // light position
    glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(light.position));
    glPushMatrix();
    glTranslatef(light.position.x, light.position.y, light.position.z);
    glColor4d(1, 1, 1, 1);
    glutSolidSphere(0.1, 10, 10); // Light ball
    glPopMatrix();
    drawAxisAndGrid(); // The central Axis point of reference
    drawGrid(10.0, 100); // The horizontal grid
    glEnable(GL_LIGHTING);



    for (Object object : objects) {
        //Shader selection
        switch (object.shading) {
            case ShadingType::GOURAUD:
                glUseProgram(shaders_IDs[GOURAUD]);
                // Caricamento matrice trasformazione del modello
                glUniformMatrix4fv(base_uniforms[GOURAUD].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
                //Material loading
                glUniform3fv(light_uniforms[GOURAUD].material_ambient, 1, glm::value_ptr(materials[object.material].ambient));
                glUniform3fv(light_uniforms[GOURAUD].material_diffuse, 1, glm::value_ptr(materials[object.material].diffuse));
                glUniform3fv(light_uniforms[GOURAUD].material_specular, 1, glm::value_ptr(materials[object.material].specular));
                glUniform1f(light_uniforms[GOURAUD].material_shininess, materials[object.material].shininess);
                break;
            case ShadingType::PHONG:
                //TODO
                break;
            case ShadingType::BLINN:
                glUseProgram(shaders_IDs[BLINN]);
                // Caricamento matrice trasformazione del modello
                glUniformMatrix4fv(base_uniforms[BLINN].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
                //Material loading
                glUniform3fv(light_uniforms[BLINN].material_ambient, 1, glm::value_ptr(materials[object.material].ambient));
                glUniform3fv(light_uniforms[BLINN].material_diffuse, 1, glm::value_ptr(materials[object.material].diffuse));
                glUniform3fv(light_uniforms[BLINN].material_specular, 1, glm::value_ptr(materials[object.material].specular));
                glUniform1f(light_uniforms[BLINN].material_shininess, materials[object.material].shininess);
                break;
            case ShadingType::TEXTURE_ONLY:
                glUseProgram(shaders_IDs[TEXTURE_ONLY]);
                // Caricamento matrice trasformazione del modello
                glUniformMatrix4fv(base_uniforms[TEXTURE_ONLY].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
                glActiveTexture(GL_TEXTURE0); // this addresses the first sampler2D uniform in the shader
                glBindTexture(GL_TEXTURE_2D, object.textureID);
                break;
            case ShadingType::PASS_THROUGH:
                glUseProgram(shaders_IDs[PASS_THROUGH]);
                // Caricamento matrice trasformazione del modello
                glUniformMatrix4fv(base_uniforms[PASS_THROUGH].M_Matrix_pointer, 1, GL_FALSE, value_ptr(object.model_matrix));
                break;
            case ShadingType::TOON:
                break;
            case ShadingType::WAVE:
                break;
            default:
                break;
        }
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindVertexArray(object.mesh.vertexArrayObjID);
        glDrawArrays(GL_TRIANGLES, 0, object.mesh.vertices.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }


    if(cameraAnimation) { //perform camera animation
        if(motionPortion > 200) { // finish animation
                cameraAnimation = false;
                OperationMode = NAVIGATION;
        } else // Move the camera along Bezier curve
            motionPortion += 1;
            deCasteljau(cameraCP, motionPortion / 200.0); // scale motionPortion from 0-200 to 0-1
        glutPostRedisplay();
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

    //TODO keep fog?
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
    materials.resize(5);
    materials[MaterialType::RED_PLASTIC].name = "Red Plastic";
    materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
    materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
    materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
    materials[MaterialType::RED_PLASTIC].shininess = red_plastic_shininess;

    materials[MaterialType::EMERALD].name = "Emerald";
    materials[MaterialType::EMERALD].ambient = emerald_ambient;
    materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
    materials[MaterialType::EMERALD].specular = emerald_specular;
    materials[MaterialType::EMERALD].shininess = emerald_shininess;

    materials[MaterialType::BRASS].name = "Brass";
    materials[MaterialType::BRASS].ambient = brass_ambient;
    materials[MaterialType::BRASS].diffuse = brass_diffuse;
    materials[MaterialType::BRASS].specular = brass_specular;
    materials[MaterialType::BRASS].shininess = brass_shininess;

    materials[MaterialType::SLATE].name = "Slate";
    materials[MaterialType::SLATE].ambient = slate_ambient;
    materials[MaterialType::SLATE].diffuse = slate_diffuse;
    materials[MaterialType::SLATE].specular = slate_specular;
    materials[MaterialType::SLATE].shininess = slate_shiness;

    materials[MaterialType::NO_MATERIAL].name = "NO_MATERIAL";
    materials[MaterialType::NO_MATERIAL].ambient = glm::vec3(1,1,1);
    materials[MaterialType::NO_MATERIAL].diffuse = glm::vec3(0, 0, 0);
    materials[MaterialType::NO_MATERIAL].specular = glm::vec3(0, 0, 0);
    materials[MaterialType::NO_MATERIAL].shininess = 1.f;



    // SHADERS configuration section
    shaders_IDs.resize(NUM_SHADERS);
    light_uniforms.resize(4); // allocate space for uniforms of PHONG, BLINN and GOURAND + TOON
    base_uniforms.resize(NUM_SHADERS); // allocate space for uniforms of PHONG,BLINN,GOURAND,TOON,WAVE,TEXTURE_ONLY

    //Gourand Shader loading
    shaders_IDs[GOURAUD] = initShader(ShaderDir + "v_gouraud.glsl", ShaderDir + "f_gouraud.glsl");
    BaseShaderUniform base_unif = {};
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "M");
    base_uniforms[ShadingType::GOURAUD] = base_unif;
    LightShaderUniform light_unif = {};
    light_unif.material_ambient = glGetUniformLocation(shaders_IDs[GOURAUD], "material.ambient");
    light_unif.material_diffuse = glGetUniformLocation(shaders_IDs[GOURAUD], "material.diffuse");
    light_unif.material_specular = glGetUniformLocation(shaders_IDs[GOURAUD], "material.specular");
    light_unif.material_shininess = glGetUniformLocation(shaders_IDs[GOURAUD], "material.shininess");
    light_unif.light_position_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "light.position");
    light_unif.light_color_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "light.color");
    light_unif.light_power_pointer = glGetUniformLocation(shaders_IDs[GOURAUD], "light.power");
    light_uniforms[ShadingType::GOURAUD] = light_unif;
    //Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[GOURAUD]);
    //Shader uniforms initialization
    glUniform3f(light_uniforms[GOURAUD].light_position_pointer, light.position.x, light.position.y, light.position.z);
    glUniform3f(light_uniforms[GOURAUD].light_color_pointer, light.color.r, light.color.g, light.color.b);
    glUniform1f(light_uniforms[GOURAUD].light_power_pointer, light.power);

    //Phong Shader loading
    // TODO

    //Texture_Phong Shader loading
    // TODO

    //Blinn Shader loading
    shaders_IDs[BLINN] = initShader(ShaderDir + "v_blinn.glsl", ShaderDir + "f_blinn.glsl");
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[BLINN], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[BLINN], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[BLINN], "M");
    base_uniforms[ShadingType::BLINN] = base_unif;
    light_unif.material_ambient = glGetUniformLocation(shaders_IDs[BLINN], "material.ambient");
    light_unif.material_diffuse = glGetUniformLocation(shaders_IDs[BLINN], "material.diffuse");
    light_unif.material_specular = glGetUniformLocation(shaders_IDs[BLINN], "material.specular");
    light_unif.material_shininess = glGetUniformLocation(shaders_IDs[BLINN], "material.shininess");
    light_unif.light_position_pointer = glGetUniformLocation(shaders_IDs[BLINN], "light.position");
    light_unif.light_color_pointer = glGetUniformLocation(shaders_IDs[BLINN], "light.color");
    light_unif.light_power_pointer = glGetUniformLocation(shaders_IDs[BLINN], "light.power");
    light_uniforms[ShadingType::BLINN] = light_unif;
    //Rendiamo attivo lo shader
    glUseProgram(shaders_IDs[BLINN]);
    //Shader uniforms initialization
    glUniform3f(light_uniforms[BLINN].light_position_pointer, light.position.x, light.position.y, light.position.z);
    glUniform3f(light_uniforms[BLINN].light_color_pointer, light.color.r, light.color.g, light.color.b);
    glUniform1f(light_uniforms[BLINN].light_power_pointer, light.power);

    //Texture Shader loading
    shaders_IDs[TEXTURE_ONLY] = initShader(ShaderDir + "v_texture.glsl", ShaderDir + "f_texture.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[TEXTURE_ONLY], "M");
    base_uniforms[ShadingType::TEXTURE_ONLY] = base_unif;

    //Pass-Through Shader loading
    shaders_IDs[PASS_THROUGH] = initShader(ShaderDir + "v_passthrough.glsl", ShaderDir + "f_passthrough.glsl");
    //Otteniamo i puntatori alle variabili uniform per poterle utilizzare in seguito
    base_unif.P_Matrix_pointer = glGetUniformLocation(shaders_IDs[PASS_THROUGH], "P");
    base_unif.V_Matrix_pointer = glGetUniformLocation(shaders_IDs[PASS_THROUGH], "V");
    base_unif.M_Matrix_pointer = glGetUniformLocation(shaders_IDs[PASS_THROUGH], "M");
    base_uniforms[ShadingType::PASS_THROUGH] = base_unif;
    glUseProgram(shaders_IDs[PASS_THROUGH]);
    glUniform4fv(glGetUniformLocation(shaders_IDs[PASS_THROUGH], "Color"), 1, value_ptr(glm::vec4(1.0, 1.0, 1.0, 1.0)));

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
