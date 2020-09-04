//
// Created by fede on 04/09/20.
//

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