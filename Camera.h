class Camera {
public:
    Vec3 cam_position, cam_direction, cam_right, cam_down;

    Camera() : cam_position(Vec3(0,0,0)), cam_direction(Vec3(0,0,-1)), cam_right(Vec3(0,0,0)), cam_down(Vec3(0,0,0)) {}

    Camera(Vec3 a, Vec3 b, Vec3 c, Vec3 d) : cam_position(a), cam_direction(b), cam_right(c), cam_down(d) {}
};