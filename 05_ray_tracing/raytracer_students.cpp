#include "raytracer.h"
#include "material.h"
#include "vectors.h"
#include "argparser.h"
#include "raytree.h"
#include "utils.h"
#include "mesh.h"
#include "face.h"
#include "sphere.h"

/**
 * casts a single ray through the scene geometry and finds the closest hit
 * @param ray
 * @param h
 * @param use_sphere_patches: if the sphere is considered as a patch or analytically
 * @return
 */
bool RayTracer::CastRay(Ray &ray, Hit &h, bool use_sphere_patches) const {
    bool answer = false;
    Hit nearest;
    nearest = Hit();

    // intersect each of the quads
    for (int i = 0; i < mesh->numQuadFaces(); i++) {
        Face *f = mesh->getFace(i);
        // checks if some objects are intesected
        if (f->intersect(ray, h, args->intersect_backfacing)) {
            // check if its the nearest intersection
            if (h.getT() < nearest.getT()) {
                answer = true;
                nearest = h;
            }
        }
    }

    // intersect each of the spheres (either the patches, or the original spheres)
    if (use_sphere_patches) {
        for (int i = mesh->numQuadFaces(); i < mesh->numFaces(); i++) {
            Face *f = mesh->getFace(i);
            if (f->intersect(ray, h, args->intersect_backfacing)) {
                if (h.getT() < nearest.getT()) {
                    answer = true;
                    nearest = h;
                }
            }
        }
    } else {
        const vector<Sphere *> &spheres = mesh->getSpheres();
        for (unsigned int i = 0; i < spheres.size(); i++) {
            if (spheres[i]->intersect(ray, h)) {
                if (h.getT() < nearest.getT()) {
                    answer = true;
                    nearest = h;
                }
            }
        }
    }

    h = nearest;
    return answer;
}

Vec3f
RayTracer::TraceRay(Ray &ray, Hit &hit, int bounce_count) const {

    hit = Hit();
    bool intersect = CastRay(ray, hit, false);

    if (bounce_count == args->num_bounces)
        RayTree::SetMainSegment(ray, 0, hit.getT());
    else
        RayTree::AddReflectedSegment(ray, 0, hit.getT());

    Vec3f answer = args->background_color;

    // get the material of the hit point (for instance for checking if the ray has hit a light)
    Material *m = hit.getMaterial();
    if (intersect == true) {

        assert (m != NULL);
        Vec3f normal = hit.getNormal();
        Vec3f point = ray.pointAtParameter(hit.getT());

        // ----------------------------------------------
        // ambient light
        answer = args->ambient_light * m->getDiffuseColor();


        // ----------------------------------------------
        // get reflective component to check if the surface is shiny...
        Vec3f reflectiveColor = m->getReflectiveColor();

        // ==========================================
        // ASSIGNMENT:  ADD REFLECTIVE LOGIC
        // ==========================================
        // TODO norm?
        if (reflectiveColor.Length() > 0 and bounce_count > 0) {
            // calculate reflection ray  R=2<n,l>n -l
            Vec3f ReflectionRay = ray.getDirection() - 2 * (ray.getDirection().Dot3(normal)) * normal;
            // instance the reflection ray ray
            Ray reflected = Ray(point, ReflectionRay);
            //	   call TraceRay(ReflectionRay, hit,bounce_count-1)
            Hit h;
            Vec3f recursive_answer = TraceRay(reflected, h, bounce_count - 1);
            //     aggiungere ad answer il contributo riflesso
            answer += m->getReflectiveColor() * recursive_answer;
        }

        // ----------------------------------------------
        // add each light
        int num_lights = mesh->getLights().size();
        for (int i = 0; i < num_lights; i++) {
            // ==========================================
            // ASSIGNMENT:  ADD SHADOW LOGIC
            // ==========================================
            Face *f = mesh->getLights()[i];
            Vec3f pointOnLight = f->computeCentroid();
            // difference between point on light and hit point
            Vec3f dirToLight = pointOnLight - point;
            dirToLight.Normalize();

            // creare shadow ray verso il punto luce
            Ray light_ray = Ray(point, dirToLight);

            // controllare il primo oggetto colpito da tale raggio
            Hit light_hit = Hit();
            bool intersect_light = CastRay(light_ray, light_hit, false);

            // se e' la sorgente luminosa i-esima allora
            if (intersect_light) {
                //	calcolare e aggiungere ad answer il contributo luminoso
                if (normal.Dot3(dirToLight) > 0 && light_hit.getMaterial() == f->getMaterial()) {
                    Vec3f lightColor = 0.2 * f->getMaterial()->getEmittedColor() * f->getArea();
                    answer += m->Shade(ray, hit, dirToLight, lightColor, args);
                }
            }
            // altrimenti
            //    la luce i non contribuisce alla luminosita' di point.

            if (normal.Dot3(dirToLight) > 0) {
                Vec3f lightColor = 0.2 * f->getMaterial()->getEmittedColor() * f->getArea();
                answer += m->Shade(ray, hit, dirToLight, lightColor, args);
            }
        }

    }

    return answer;
}
