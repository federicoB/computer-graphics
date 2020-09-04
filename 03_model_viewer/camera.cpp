//
// Created by fede on 04/09/20.
//

struct ViewSetup{
    glm::vec4 position;
    glm::vec4 target;
    glm::vec4 upVector;
};
struct PerspectiveSetup{
    float fovY, aspect, near_plane, far_plane;
};

static ViewSetup viewSetup;
static PerspectiveSetup perspectiveSetup;

/*camera structures*/
constexpr float CAMERA_ZOOM_SPEED = 0.1f;
constexpr float CAMERA_TRASLATION_SPEED = 0.01f;

void zoom(float dir) {
    // P1 = P0 + tv
    float v[3];
    float t = 0.1f * dir;

    for (int i = 0; i < 3; i++) {
        v[i] = viewSetup.target[i] - viewSetup.position[i]; // v = vector from camera to focused point
        v[i] *= t; // tv = multiply vector by direction
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