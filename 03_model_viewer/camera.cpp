//
// Created by fede on 04/09/20.
//

struct ViewSetup{
    glm::vec4 position; // the position of the eye point.
    glm::vec4 target; // the position of the reference point.
    glm::vec4 upVector; // the direction of the up vector.
};
struct PerspectiveSetup{
    float fovY, // field of view angle, in degrees, in the y direction.
    aspect, // aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
    near_plane, // the distance from the viewer to the near clipping plane
    far_plane; // the distance from the viewer to the far clipping plane
};

static ViewSetup viewSetup;
static PerspectiveSetup perspectiveSetup;

/*camera structures*/
constexpr float CAMERA_ZOOM_SPEED = 0.1f;
constexpr float CAMERA_TRASLATION_SPEED = 0.01f;

bool cameraAnimation = false; // if camera animation is activated

#define CONTROL_POINTS 11
float cameraCP[CONTROL_POINTS][3]; //camera animation curve control points
float motionPortion = 0; //curve evaluation parameter (t)


void zoom(float dir) {
    // P1 = P0 + tv
    float v[3];
    float t = 0.1f * dir;

    //loop over vector components
    for (int i=0; i<3; i++) {
        v[i] = viewSetup.target[i] - viewSetup.position[i]; // v = vector from camera to focused point
        i *= t; // tv = multiply vector by direction
        viewSetup.position[i] += v[i]; // P1 = camera position + vector to origin multiplied by some value
    }
}

void horizontalPan(float dir) {
    using namespace glm;
    vec3 lookAt(viewSetup.target), position(viewSetup.position), upVector(viewSetup.upVector);

    vec3 direction = lookAt - position;
    vec3 normalToPlane = normalize(cross(direction, upVector));
    vec4 increment(normalToPlane.x * dir, normalToPlane.y * dir, normalToPlane.z * dir, 1.0);

    viewSetup.target += increment;
    viewSetup.position += increment;
}

void verticalPan(float dir) {
    using namespace glm;
    vec3 lookAt(viewSetup.target), position(viewSetup.position), upVector(viewSetup.upVector);

    vec3 direction = lookAt - position;
    vec3 normalToPlane = normalize(cross(direction, upVector));
    vec3 normalToNormalToPlane = -normalize(cross(direction, normalToPlane));
    vec4 increment(normalToNormalToPlane.x * dir, normalToNormalToPlane.y * dir, normalToNormalToPlane.z * dir, 1.0);

    viewSetup.target += increment;
    viewSetup.position += increment;
}

#include "view_utilis.cpp"

float* rotate_point_around_center(float *point, float centerX, float centerZ, double angle)
{
    float resultX, resultZ;
    point[0] = point[0] - centerX;
    point[2] = point[2] - centerZ;
    resultX =  point[0] * cos(angle) - point[2] * sin(angle);
    resultZ =  point[0] * sin(angle) + point[2] * cos(angle);
    point[0] = resultX + centerX;
    point[2] = resultZ + centerZ;

}

// Create a bezier courve moving camera around object
void create_camera_animation_path() {
    //get object positon
    GLfloat* objectPosition = objects[selected_object].model_matrix;
    float Ox = objectPosition[12];
    float Oy = objectPosition[13];
    float Oz = objectPosition[14];
    //get camera position
    float Cx = viewSetup.position[0];
    float Cz = viewSetup.position[2];
    viewSetup.target = glm::vec4(Ox,Oy,Oz,0);     //make camera point to object

    // 1. Start and end in current camera position
    cameraCP[0][0] = viewSetup.position[0];
    cameraCP[0][1] = viewSetup.position[1];
    cameraCP[0][2] = viewSetup.position[2];

    // get a middle point between camera and object
    cameraCP[1][0] = (Cx + Ox)/2;
    cameraCP[1][1] = Oy;
    cameraCP[1][2] = (Cz + Oz)/2;
    // rotate this middle point around object
    for (int i=1;i<9;i++) {
        copy(std::begin(cameraCP[i]),std::end(cameraCP[i]),std::begin(cameraCP[i+1]));
        rotate_point_around_center(cameraCP[i+1], Ox, Oz, M_PI / 4);
    }
    // 5. End in current camera position
    cameraCP[10][0] = viewSetup.position[0];
    cameraCP[10][1] = viewSetup.position[1];
    cameraCP[10][2] = viewSetup.position[2];

    cameraAnimation = true;
    motionPortion = 0;
}