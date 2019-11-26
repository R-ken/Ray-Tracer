class Ray
{
public:
    Vec3 origin;
    Vec3 direction;

    Ray(Vec3 a, Vec3 b) : origin(a), direction(b) {}
};