class Vec3 
{
public:
    float x;
    float y;
    float z;

    Vec3(): x(0), y(0), z(0) {}

    Vec3(float a, float b, float c): x(a), y(b), z(c) {}

    Vec3 operator +(Vec3 vec) {
        return Vec3(x+vec.x,y+vec.y,z+vec.z);
    }
    Vec3 operator -(Vec3 vec) {
        return Vec3(x-vec.x,y-vec.y,z-vec.z);
    }
    Vec3 operator *(float scalar) {
        return Vec3(scalar*x,scalar*y,scalar*z);
    }
    Vec3 operator*(Vec3 vec) {
        return Vec3(x*vec.x,y*vec.y,z*vec.z);
    }
    Vec3 operator/(float scalar) {
        return Vec3(x/scalar, y/scalar, z/scalar);
    }
    Vec3 operator +=(Vec3 vec) {
        x += vec.x;
        y += vec.y;
        z += vec.z;

        return *this;
    }
    float magnitude () {
        return sqrt((x*x) + (y*y) + (z*z));
    }
    float dot(Vec3 vec) {
        return x*vec.x+y*vec.y+z*vec.z;
    }
    Vec3 normalize() {
        float dot_val = x * x + y * y + z * z;
        if (dot_val > 0) {
            float norm = 1 / sqrt(dot_val);
            x *= norm, y *= norm, z *= norm;
        }
        return *this;
    }
    Vec3 negative () {
        return Vec3(-x,-y,-z);
    }
    Vec3 cross(Vec3 vec) {
        return Vec3(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
    }
    Vec3 clip() {
        float a = x+y+z;
        float b = a - 3;
        if(b > 0) {
            x = x + b*(x/a);
            y = y + b*(y/a);
            z = z + b*(z/a);
        }
        if(x > 1) {x = 1;}
        if(y > 1) {y = 1;}
        if(z > 1) {z = 1;}
        if(x < 0) {x = 0;}
        if(y < 0) {y = 0;}
        if(z < 0) {z = 0;}

        return Vec3(x,y,z);
    }

    float getx(){
        return x;
    }

    float gety(){
        return y;
    }

    float getz(){
        return z;
    }
};