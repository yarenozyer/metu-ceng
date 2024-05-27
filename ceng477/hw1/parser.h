#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <iostream>
namespace parser
{
    // Notice that all the structures are as simple as possible
    // so that you are not enforced to adopt any style or design.

    struct Vec3f
    {
        float x, y, z;

        // addition with another vecf3
        Vec3f operator+(const Vec3f &other) const
        {
            Vec3f result;
            result.x = this->x + other.x;
            result.y = this->y + other.y;
            result.z = this->z + other.z;
            return result;
        }

        // subtraction  with another vecf3
        Vec3f operator-(const Vec3f &other) const
        {
            Vec3f result;
            result.x = this->x - other.x;
            result.y = this->y - other.y;
            result.z = this->z - other.z;
            return result;
        }

        // multiplication with a scalar
        Vec3f operator*(const float &other) const
        {
            Vec3f result;
            result.x = this->x * other;
            result.y = this->y * other;
            result.z = this->z * other;
            return result;
        }

        Vec3f operator/(const float &other) const
        {
            Vec3f result;
            result.x = this->x / other;
            result.y = this->y / other;
            result.z = this->z / other;
            return result;
        }

        float operator*(const Vec3f &other) const
        {
            return (this->x * other.x + this->y * other.y + this->z * other.z);
        }
    };

    struct ParametricLine
    {
        Vec3f e, d;
    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
    };

    struct Material
    {
        bool is_mirror;
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
    };

    struct Triangle
    {
        int material_id;
        Face indices;
    };

    struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;
    };

    struct Scene
    {
        // Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;

        // Functions
        void loadFromXml(const std::string &filepath);
    };
    void printParamLine(ParametricLine eye_ray);
    Vec3f crossProduct(const Vec3f &A, const Vec3f &B);
}

#endif
