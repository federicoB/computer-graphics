//
// Created by fede on 04/09/20.
//

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <glm/vec3.hpp>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>


#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define NUM_SHADERS 8

using namespace std;

// Materiali disponibili
const glm::vec3 red_plastic_ambient = {0.0, 0.0, 0.0},
        red_plastic_diffuse = {0.5, 0.0, 0.0},
        red_plastic_specular = {0.7, 0.6, 0.6};

const GLfloat red_plastic_shininess = 32.0;
const glm::vec3 brass_ambient = {0.33, 0.22, 0.03},
        brass_diffuse = {0.78, 0.57, 0.11},
        brass_specular = {0.99, 0.91, 0.81};

const GLfloat brass_shininess = 27.8;
const glm::vec3 emerald_ambient = {0.0215, 0.1745, 0.0215},
        emerald_diffuse = {0.07568, 0.61424, 0.07568}, emerald_specular = {0.633, 0.727811, 0.633};

const GLfloat emerald_shininess = 78.8;
const glm::vec3 slate_ambient = {0.02, 0.02, 0.02}, slate_diffuse = {0.02, 0.01, 0.01}, slate_specular{0.4, 0.4, 0.4};
const GLfloat slate_shiness = .78125f;

enum {
    NAVIGATION,
    CAMERA_MOVING,
    TRASLATING,
    ROTATING,
    SCALING
} OperationMode ;

enum {
    OCS, // Object Coordinate System
    WCS // World Coordinate System
} TransformMode;

enum {
    X,
    Y,
    Z
} WorkingAxis;

typedef struct {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    GLuint vertexArrayObjID;
    GLuint vertexBufferObjID;
    GLuint normalBufferObjID;
    GLuint uvBufferObjID;
} Mesh;

typedef enum {
    RED_PLASTIC,
    EMERALD,
    BRASS,
    SLATE,
    NO_MATERIAL
} MaterialType;

typedef struct {
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    GLfloat shininess;
} Material;

typedef enum { // used also as index, don't modify order
    GOURAUD,
    PHONG,
    BLINN,
    TOON,
    TEXTURE_ONLY,
    TEXTURE_PHONG,
    PASS_THROUGH,
    WAVE
} ShadingType;

string shaders_names[] = {
        "GOURAUD",
        "PHONG",
        "BLINN",
        "TOON",
        "TEXTURE_ONLY",
        "TEXTURE_PHONG",
        "PASS_THROUGH",
        "WAVE"
};

typedef struct {
    Mesh mesh;
    MaterialType material;
    ShadingType shading;
    int textureID;
    glm::mat4 model_matrix;
    string name;
} Object;

typedef struct {
    GLuint light_position_pointer;
    GLuint light_color_pointer;
    GLuint light_power_pointer;
    GLuint material_diffuse;
    GLuint material_ambient;
    GLuint material_specular;
    GLuint material_shininess;
} LightShaderUniform;

typedef struct {
    GLuint P_Matrix_pointer;
    GLuint V_Matrix_pointer;
    GLuint M_Matrix_pointer;
} BaseShaderUniform;


const string TextureDir = "Textures/";
const string ShaderDir = "Shaders/";

using namespace std;

typedef struct {
    glm::vec3 position;
    glm::vec3 color;
    GLfloat power;
} point_light;

static point_light light;

vector<Object> objects;
vector<Material> materials;
unsigned int selected_object = 0;