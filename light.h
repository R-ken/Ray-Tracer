class Light
{
public:
    Vec3 point, intensity;
    float decay_a, decay_b, decay_c;

    Light(Vec3 p, Vec3 i, float a, float b, float c) : point(p), intensity(i), decay_a(a), decay_b(b), decay_c(c) {}
};