class Sphere {
public:
    Vec3 center;
    float radius;
    float radius_2;
    Vec3 diffuse, specular;
    float q,kr,kt,index_refraction;
    Vec3 ambient;
    Vec3 surfaceColor;

    Sphere(Vec3 c, 
        float r, 
        Vec3 diff, 
        Vec3 spec, 
        float qv, 
        float krv, 
        float ktv, 
        float index,
        Vec3 amb,
        Vec3 surf) : center(c), radius(r), radius_2(r*r), diffuse(diff), specular(spec), q(qv), kr(krv), kt(ktv), index_refraction(index), ambient(amb), surfaceColor(surf) {}

    Vec3 getNormal(Vec3 surf_point){
        return surf_point - center;
    }

    float intersect(Vec3 rayorig, Vec3 raydir) {
        Vec3 q = rayorig - center;
        float a = sqrtf(raydir.dot(raydir));
        float b = 2 * (q.dot(raydir));
        float c = q.dot(q) - (radius * radius);
        float delta = (b*b) - 4 *(a * c);

        if(delta < 0 ){
            return -1;
        }

        float t1 = (-b - sqrtf(delta)) / (2*a);
        float t2 = (-b + sqrtf(delta)) / (2*a);

        if(t1 < 0 && t2 < 0){
            return -1;
        }

        if(t1 < t2) {
            return t1;
        }
        else {
            return t2;
        }
    }

};