#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <limits>
#include <math.h>
#include <vector> 
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Vec3.h"
#include "sphere.h"
#include "plane.h"
#include "light.h"
#include "ray.h"
#include "Camera.h"
#include <GL/glut.h>
#include <GL/gl.h>

using namespace std;

int xsize = 400;
int ysize = 400;
float aspect_ratio = xsize/ysize;
float x_amount, y_amount;

int raydepth = 4;
int temp;
float accuracy = 0.000001;

Vec3 background(0,0,0);
Vec3 global_ambient(0.2,0.2,0.2);

Vec3 X(1,0,0);
Vec3 Y(0,1,0);
Vec3 Z(0,0,1);

Vec3 camera_position(0,0,0);

Vec3 lookat(0,0,-1);
Vec3 diff(camera_position.x - lookat.x, camera_position.y - lookat.y, camera_position.z - lookat.z);

Vec3 cam_direction = diff.negative().normalize();
Vec3 cam_r = Y.cross(cam_direction).normalize();
Vec3 cam_d = cam_r.cross(cam_direction);

Camera scene_camera(camera_position, cam_direction, cam_r, cam_d);

Light light(Vec3(7,10,-3), Vec3(1,1,1), 0, 0.3, 0.5);

Vec3 *image = new Vec3[xsize*ysize];

Vec3 trace(Vec3 rayorig, Vec3 raydir, std::vector<Sphere> spheres, Plane plane, Light light, int depth) {
    float t_near = INFINITY;
    float t_near_shadow = INFINITY;
    float t_near_reflection = INFINITY;
    float t_return = 0;
    Sphere* sphere = NULL;
    bool intersect_sphere = false;
    bool intersect_plane = false;
    Vec3 intensity, kd, ks, ka;
    Vec3 surface_color;
    float q, kr, kt, index_refraction;

    if(depth == 0)
    {
        return background;
    }

    //compute intersections
    for(int i = 0; i < spheres.size(); i++){
        t_return = spheres[i].intersect(rayorig, raydir);

        //we hit a sphere
        if(t_return != -1){
            if(t_return < t_near){
                intersect_sphere = true;
                sphere = &spheres[i];
                t_near = t_return;
            }
        }
    }

    //check same ray for plane intersection
    t_return = plane.intersect(rayorig, raydir);

    //we hit a plane with the ray
    if(t_return != -1){
        //check if the plane is closer than the sphere
        if(t_return < t_near){
            intersect_plane = true;
            intersect_sphere = false;
            t_near = t_return;
        }
    }

    //check if ray does not hit anything
    if(t_near == INFINITY){
        return background;
    }

    Vec3 point_hit;
    if(t_near > accuracy){
        point_hit = rayorig + (raydir * t_near); //point of intersection , hit
    }
    Vec3 normal_hit;  //surf_norm

    //Apply Lighting formula
    if(intersect_sphere == true){
        normal_hit = sphere->getNormal(point_hit);
        kd = sphere->diffuse;
        ks = sphere->specular;
        ka = sphere->ambient;
        q = sphere->q;
        kr = sphere->kr;
        kt = sphere->kt;
        index_refraction = sphere->index_refraction;
        surface_color = sphere->surfaceColor;
    }
    else if(intersect_plane == true){
        normal_hit = plane.getNormal(point_hit);
        kd = plane.diffuse;
        ks = plane.specular;
        ka = plane.ambient;
        q = plane.q;
        kr = plane.kr;
        kt = plane.kt;
        index_refraction = plane.index_refraction;
        surface_color = plane.surface_color;
    }
    normal_hit.normalize();

    // if it is in the object, reverse the normal vector
    if(normal_hit.dot(raydir) > 0){
        normal_hit = Vec3(0,0,0)-normal_hit;
    }

    //call phong lighting equation
    Vec3 l = light.point - point_hit;
    l.normalize();

    Vec3 v = Vec3(0,0,0) - point_hit;
    v.normalize();

    Vec3 r = (normal_hit * normal_hit.dot(l)) * 2 - l;

    float diffuse = fmaxf(normal_hit.dot(l), 0.0f);
    float specular = powf(fmaxf(r.dot(v), 0.0f), q);

    Vec3 temp = light.point - point_hit;
    float delta = temp.dot(temp);

    float div = (light.decay_a * delta * delta) + (light.decay_b * delta) + light.decay_c;

    intensity = (light.intensity * ((kd * diffuse) + (ks * specular))) / div;

    intensity += (ka * global_ambient);

    // add reflection
    Vec3 opp_dir = Vec3(0,0,0) - raydir;
    Vec3 reflect_ray = ((normal_hit * normal_hit.dot(opp_dir)) * 2) + raydir;

    intensity += trace(point_hit + reflect_ray, reflect_ray, spheres, plane, light, depth-1) * kr;

    //add refraction
    // vacuum = 1, glass = 1.52
    float ratio = 1/index_refraction;
    float c1 = -(normal_hit.dot(raydir));
    float c2 = 1 - ratio * ratio * (1 - c1 * c1);

    if(c2 > 0.0f) {
        Vec3 refract_ray = (normal_hit * (ratio * c1 - sqrtf(c2))) + (raydir * c1);
        refract_ray.normalize();
        intensity += trace(point_hit + refract_ray, refract_ray, spheres, plane, light, depth-1) * kt;
    }

    Vec3 result = (surface_color * global_ambient);

    //shadow
    Vec3 light_direction = (light.point + point_hit.negative()).normalize();

    float cos_ang = normal_hit.dot(light_direction);

    if(cos_ang > 0){
        bool shadow = false;

        Vec3 light_distance = (light.point + point_hit.negative()).normalize();
        float light_distance_magnitude = light_distance.magnitude();

        Ray shadow_ray(point_hit, light_direction);

        //compute intersections
        for(int i = 0; i < spheres.size(); i++){
            t_return = spheres[i].intersect(point_hit, light_direction);

            //we hit a sphere
            if(t_return != -1){
                if(t_return < t_near_shadow){
                    sphere = &spheres[i];
                    t_near_shadow = t_return;
                }
            }
        }

        if(t_near_shadow > accuracy){
            if(t_near_shadow <= light_distance_magnitude){
                shadow = true;
            }
        }
        
        if (shadow == false) {
            result = result + (surface_color * (light.intensity * cos_ang) * intensity);

                float d1 = normal_hit.dot(raydir.negative());
                Vec3 s1 = normal_hit * d1;
                Vec3 a1 = s1 + (raydir);
                Vec3 s2 = a1 * 2;
                Vec3 a2 = raydir.negative() + s2;
                Vec3 reflect_dir = a2.normalize();

                float specular = reflect_dir.dot(light_direction);

                if(specular > 0) {
                    specular = powf(specular, 10);
                    result = result + (light.intensity * (specular * q) * intensity); 
                }
        }

    }

    return result.clip();
}

void render(std::vector<Sphere> spheres, Plane plane, Light light) {
    int dpi_val = 72;
    int n = xsize*ysize;
    Vec3 *pixels = new Vec3[n];

    for(int x = 0; x < xsize; x++){
        for( int y = 0; y < ysize; y++){
            temp = y*xsize + x;

            // image is square
            x_amount = (x + 0.5)/xsize;
            y_amount = ((ysize - y) + 0.5) / ysize;

            Vec3 cam_origin = scene_camera.cam_position;
            Vec3 cam_ray_dir = (cam_direction + (cam_r * (x_amount - 0.5)) + (cam_d * (y_amount - 0.5))).normalize();

            Ray camera_ray(cam_origin, cam_ray_dir);

            pixels[temp] = trace(cam_origin, cam_ray_dir, spheres, plane, light, raydepth);
            image[temp] = pixels[temp];
        }
    }
}

void scene1(){
    Sphere sphere(Vec3(1.5,0,-5), 0.5, Vec3(0,0,0), Vec3(40,40,40), 0, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0.5,0.5,0.5));
    Sphere sphere2(Vec3(0,0,-5), 0.5, Vec3(40,40,40), Vec3(0,0,0), 3, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0.5,0.5,0.5));
    Sphere sphere3(Vec3(-1.5,0,-5), 0.5, Vec3(0,0,0), Vec3(0,0,0), 10, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0.5,0.5,0.5));
    Plane plane(0,1,0,-1,Vec3(1,0.5,0.8), Vec3(1,1,1), 20, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0.5,0.25,0.25));

    vector<Sphere> spheres;

    spheres.push_back(sphere);
    spheres.push_back(sphere2);
    spheres.push_back(sphere3);
    
    render(spheres,plane,light);
}

void scene2(){
    Sphere sphere(Vec3(-1.5,0,-5), 1, Vec3(0,0,0), Vec3(5,5,5), 5, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0.5,0.5,0.5));
    Sphere sphere2(Vec3(1.5,0,-5), 1, Vec3(0,0,0), Vec3(5,5,5), 5, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0.5,0.5,0.5));
    Plane plane(0,1,0,-1,Vec3(1,0.5,0.8), Vec3(1,1,1), 20, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0,1,0));

    vector<Sphere> spheres;

    spheres.push_back(sphere);
    spheres.push_back(sphere2);

    render(spheres,plane,light);
}

void scene3(){
    Sphere sphere(Vec3(0,0.5,-5), 1, Vec3(10,10,10), Vec3(10,10,10), 3, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0.75,0.75,0.75));
    Plane plane(0,1,0,-0.5,Vec3(1,0.5,0.8), Vec3(1,1,1), 100, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0,1,0));
    Light light2(Vec3(7,10,-3), Vec3(1,1,1), 0, 0.3, 0.5);

    light = light2;

    vector<Sphere> spheres;

    spheres.push_back(sphere);

    render(spheres,plane,light);
}

void scene4(){
    Sphere sphere(Vec3(0,0,-3), 0.5, Vec3(10,10,10), Vec3(10,10,10), 3, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(1,0,0));
    Sphere sphere2(Vec3(2.5,0.5,-5), 1, Vec3(10,10,10), Vec3(10,10,10), 3, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0,0,1));
    Sphere sphere3(Vec3(-2.5,0.5,-5), 1, Vec3(10,10,10), Vec3(10,10,10), 3, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0.5,0.5,0.5));
    Plane plane(0,1,0,-0.5,Vec3(1,0.5,0.8), Vec3(1,1,1), 50, 0.4, 0.4, 1.52, Vec3(0.7,0.7,0.7), Vec3(0,1,0));

    vector<Sphere> spheres;

    spheres.push_back(sphere);
    spheres.push_back(sphere2);
    spheres.push_back(sphere3);

    render(spheres,plane,light);    
}

void init() {   // places all initializations and related one-time paramter settings 
    glClearColor(0.0, 0.0, 0.0, 0.0);  //sets background color
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, (xsize), 0, (ysize), -1000.0, 1000.0); //these two commands creates a 2D plane and pastes the picture onto the window
                                        //with x values from 0 - 200 and y values 0 -150 (the size of the plane)
}

void grid() {
    glClear(GL_COLOR_BUFFER_BIT); //gets the assigned window color displayed to the values in glClearColor

    for(int x = 0; x < xsize; x++){
        for( int y = 0; y < ysize; y++){
            temp = y*xsize + x;

            glColor3f(image[temp].x, image[temp].y, image[temp].z);

            glBegin(GL_POINTS);
                glVertex2f(x,y);
            glEnd();
        }
    }

    glutSwapBuffers();
}

int main (int argc, char** argv) {
    int input = 0;
    cout << "Enter a number from 1 to 4: " << endl;
    cin >> input;

    if(input == 1)
    {
        scene1();
    }
    else if(input == 2)
    {
        scene2();
    }
    else if(input == 3)
    {
        scene3();
    }
    else if(input == 4)
    {
        scene4();
    }
    else
    {
        cout << "Error: input is not a number from 1 to 4" << endl;
        return 0;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(680, 178); // 680 pixels from the left/right of screen and 178 pixels from the top/bottom of screen
    glutInitWindowSize(xsize, ysize); //resize the window (x,y)
    glutCreateWindow("Ray Tracing");
    init();

    glutDisplayFunc(grid);
    glutMainLoop(); //It displays the initial graphics
                    //and puts the program into an infinite loop that checks for input from devices such
                    //as a mouse or keyboard


    return 0;
}
