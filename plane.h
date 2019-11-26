class Plane
{
public:
    float A,B,C,D;
    Vec3 diffuse, specular, ambient;
    float q,kr,kt,index_refraction;
    Vec3 surface_color;

    Plane(float a,
        float b,
        float c,
        float d, 
        Vec3 diff, 
        Vec3 spec, 
        float qv, 
        float krv, 
        float ktv, 
        float index,
        Vec3 amb,
        Vec3 surf) : A(a), B(b), C(c), D(d), diffuse(diff), specular(spec), q(qv), kr(krv), kt(ktv), index_refraction(index), ambient(amb), surface_color(surf) {}

    Plane() : A(0), B(0), C(0), D(0), diffuse(Vec3(0,0,0)), specular(Vec3(0,0,0)), q(0), kr(0), kt(0), index_refraction(0), ambient(Vec3(0,0,0)), surface_color(Vec3(0.5,0.5,0.5)) {}

    Vec3 getNormal(Vec3 surf_point){
        Vec3 temp(A,B,C);

        return temp;
    }

    float intersect(Vec3 rayorig, Vec3 raydir){
        Vec3 temp(A,B,C);
        float value = temp.dot(rayorig);

        float t = -(temp.dot(rayorig + (temp * (-D)))) / (temp.dot(raydir));

        if(t < 0) {
            return -1;
        }

        return t;
    }
};