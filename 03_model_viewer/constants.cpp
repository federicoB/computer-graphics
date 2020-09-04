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


#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    std::vector<GLushort> indices;
    GLuint vertexArrayObjID;
    GLuint vertexBufferObjID;
    GLuint normalBufferObjID;
    GLuint indexBufferObjID;
} Mesh;

typedef enum {
    RED_PLASTIC,
    EMERALD,
    BRASS,
    SLATE
} MaterialType;

typedef struct {
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    GLfloat shiness;
} Material;

typedef struct {
    Mesh mesh;
    MaterialType material;
    GLfloat model_matrix[16];
    string name;
} Object;


using namespace std;

static glm::vec4 lightpos = {5.0f, 5.0f, 5.0f, 1.0f};

static bool moving_trackball = 0;

vector<Object> objects;
vector<Material> materials;
unsigned int selected_object = 0;