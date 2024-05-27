#include <iostream>
#include <cmath>
#include "parser.h"
#include "ppm.h"
#include "misc.h"
#include <pthread.h>

typedef unsigned char RGB[3];
bool checker = false;

void findNormals(parser::Scene &scene, std::vector<std::vector<parser::Vec3f>> &meshNormals,
                 std::vector<parser::Vec3f> &triangleNormals);

misc::IntersectingRayData triangleIntersection(parser::Scene &scene, parser::Face face, parser::ParametricLine eye_ray);

misc::IntersectingRayData sphereIntersection(parser::Scene &scene, parser::Sphere sphere, parser::ParametricLine eye_ray);

parser::Vec3i computeColors(parser::Scene &scene, unsigned char *image, parser::ParametricLine eye_ray, parser::Camera &camera, std::vector<std::vector<parser::Vec3f>> &meshNormals,
                            std::vector<parser::Vec3f> &triangleNormals, int depth, int i, int j);

void *threadFn(void *ptr)
{
    misc::ThreadArgs *threadArgs = (misc::ThreadArgs *)ptr;
    parser::Scene scene = *(threadArgs->scene);
    parser::Camera camera = threadArgs->camera;
    int width = camera.image_width, height = camera.image_height;
    int i_start = threadArgs->i_start, i_end = threadArgs->i_end;
    int j_start = threadArgs->j_start, j_end = threadArgs->j_end;
    unsigned char* image = threadArgs->image;
    std::vector<std::vector<parser::Vec3f>> meshNormals;
    std::vector<parser::Vec3f> triangleNormals;

    findNormals(scene, meshNormals, triangleNormals);
    int image_size = camera.image_width * camera.image_height * 3;
    parser::Vec3f u = parser::crossProduct(camera.gaze, camera.up);
    parser::Vec3f m = camera.position + (camera.gaze * camera.near_distance);

    parser::Vec3f q = m + u * camera.near_plane.x + camera.up * camera.near_plane.w;

    float u_constant = (camera.near_plane.y - camera.near_plane.x) / (float)width;
    float v_constant = (camera.near_plane.w - camera.near_plane.z) / (float)height;

    for (int i = i_start; i < i_end; i++)
    {
        
        float s_u = (i + 0.5) * u_constant;

        for (int j = j_start; j < j_end; j++)
        {

            float s_v = (j + 0.5) * v_constant;

            parser::Vec3f s = q + u * s_u - camera.up * s_v;

            parser::ParametricLine eye_ray = {camera.position, s - camera.position};

            parser::Vec3i color = computeColors(scene, image, eye_ray, camera, meshNormals, triangleNormals, 0, i, j);
            int pixelIndex = (j * width + i) * 3;

            if(pixelIndex < image_size)
            {
            image[pixelIndex] = color.x > 255 ? 255 : color.x;
            image[pixelIndex + 1] = color.y > 255 ? 255 : color.y;
            image[pixelIndex + 2] = color.z > 255 ? 255 : color.z;

            }
        }
    }

    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

    for (int cam_index = 0; cam_index < scene.cameras.size(); cam_index++)
    {
        parser::Camera camera = scene.cameras[cam_index];
        unsigned char *image = new unsigned char[camera.image_width * camera.image_height * 3];

        int i_values[3] = {0, camera.image_width / 2, camera.image_width};
        int j_values[3] = {0, camera.image_height / 2, camera.image_height};

        pthread_t tid[4];
        for (int t_index = 0; t_index < 4; t_index++)
        {
            int i_start = -1, i_end = -1, j_start = -1, j_end = -1;

            if (t_index < 2)
            {
                i_start = i_values[0];
                i_end = i_values[1];
            }
            else
            {
                i_start = i_values[1];
                i_end = i_values[2];
            }

            if (t_index % 2 == 0)
            {
                j_start = j_values[0];
                j_end = j_values[1];
            }
            else
            {
                j_start = j_values[1];
                j_end = j_values[2];
            }
            misc::ThreadArgs *threadArgs = new misc::ThreadArgs{camera, &scene, i_start, i_end, j_start, j_end, image};
            pthread_create(&(tid[t_index]), NULL, threadFn, threadArgs);
        }

        for (int t_index = 0; t_index < 4; t_index++)
            pthread_join(tid[t_index], NULL);

        const char *image_name = camera.image_name.c_str();
        write_ppm(image_name, image, camera.image_width, camera.image_height);
    }

    exit(0);
}

parser::Vec3i computeColors(parser::Scene &scene, unsigned char *image, parser::ParametricLine eye_ray, parser::Camera &camera, std::vector<std::vector<parser::Vec3f>> &meshNormals,
                            std::vector<parser::Vec3f> &triangleNormals, int depth, int i, int j)
{
    if (depth > scene.max_recursion_depth)
        return {0, 0, 0};

    float t_min = __FLT_MAX__;
    float epsilon = 0.001;
    misc::HitObjectData hitObjectData = {misc::ObjectType::NONE, -1, -1, -1};

    for (int mesh_index = 0; mesh_index < scene.meshes.size(); mesh_index++)
    {
        for (int face_index = 0; face_index < scene.meshes[mesh_index].faces.size(); face_index++)
        {

            parser::Face face = scene.meshes[mesh_index].faces[face_index];

            misc::IntersectingRayData intersectionData = triangleIntersection(scene, face, eye_ray);

            if (intersectionData.intersect && intersectionData.t < t_min && intersectionData.t > epsilon)
            {

                t_min = intersectionData.t;
                hitObjectData = {misc::ObjectType::MESH, mesh_index, face_index, scene.meshes[mesh_index].material_id};
            }
        }
    }

    for (int tri_index = 0; tri_index < scene.triangles.size(); tri_index++)
    {
        parser::Triangle triangle = scene.triangles[tri_index];

        misc::IntersectingRayData intersectionData = triangleIntersection(scene, triangle.indices, eye_ray);

        if (intersectionData.intersect && intersectionData.t < t_min && intersectionData.t > epsilon)
        {
            t_min = intersectionData.t;
            hitObjectData = {misc::ObjectType::TRIANGLE, tri_index, -1, triangle.material_id};
        }
    }

    for (int sphere_index = 0; sphere_index < scene.spheres.size(); sphere_index++)
    {

        parser::Sphere sphere = scene.spheres[sphere_index];

        misc::IntersectingRayData intersectionData = sphereIntersection(scene, sphere, eye_ray);

        if (intersectionData.intersect && intersectionData.t < t_min && intersectionData.t > epsilon)
        {
            t_min = intersectionData.t;
            hitObjectData = {misc::ObjectType::SPHERE, sphere_index, -1, sphere.material_id};
        }
    }

    if (hitObjectData.objectType != misc::ObjectType::NONE)
    {
        parser::Material material = scene.materials[hitObjectData.materialId - 1];

        float light_red = 0, light_green = 0, light_blue = 0;

        for (int light_index = 0; light_index < scene.point_lights.size(); light_index++)
        {
            parser::PointLight point_light = scene.point_lights[light_index];
            parser::Vec3f intersection_point = eye_ray.e + eye_ray.d * t_min;
            parser::Vec3f pixel_to_light = point_light.position - intersection_point;

            float light_distance_squared = (pixel_to_light.x * pixel_to_light.x + pixel_to_light.y * pixel_to_light.y + pixel_to_light.z * pixel_to_light.z);

            parser::Vec3f normal;

            switch (hitObjectData.objectType)
            {
            case misc::ObjectType::MESH:
                normal = meshNormals[hitObjectData.objectIndex][hitObjectData.faceIndex];
                break;
            case misc::ObjectType::TRIANGLE:
                normal = triangleNormals[hitObjectData.objectIndex];
                break;
            case misc::ObjectType::SPHERE:
                normal = (eye_ray.e + eye_ray.d * t_min - scene.vertex_data[scene.spheres[hitObjectData.objectIndex].center_vertex_id - 1]) /
                         scene.spheres[hitObjectData.objectIndex].radius;

                break;
            default:
                break;
            }

            parser::Vec3f normalized_d = misc::normalizeVector(eye_ray.d);

            parser::ParametricLine shadow_ray = {intersection_point, pixel_to_light};

            bool blocked = false;
            for (int mesh_index = 0; mesh_index < scene.meshes.size(); mesh_index++)
            {
                for (int face_index = 0; face_index < scene.meshes[mesh_index].faces.size(); face_index++)
                {
                    if (!blocked)
                    {
                        parser::Face face = scene.meshes[mesh_index].faces[face_index];

                        misc::IntersectingRayData intersectionData = triangleIntersection(scene, face, shadow_ray);

                        if (intersectionData.intersect && intersectionData.t > epsilon && intersectionData.t < t_min && intersectionData.t < 1 &&
                            (hitObjectData.objectType != misc::ObjectType::MESH || hitObjectData.objectIndex != mesh_index || hitObjectData.faceIndex != face_index))
                        {
                            blocked = true;
                            break;
                        }
                    }
                }
            }

            for (int tri_index = 0; tri_index < scene.triangles.size(); tri_index++)
            {
                if (!blocked)
                {
                    parser::Triangle triangle = scene.triangles[tri_index];

                    misc::IntersectingRayData intersectionData = triangleIntersection(scene, triangle.indices, shadow_ray);

                    if (intersectionData.intersect && intersectionData.t > epsilon && intersectionData.t < t_min && intersectionData.t < 1 &&
                        (hitObjectData.objectType != misc::ObjectType::TRIANGLE || hitObjectData.objectIndex != tri_index))
                    {
                        blocked = true;
                        break;
                    }
                }
            }

            for (int sphere_index = 0; sphere_index < scene.spheres.size(); sphere_index++)
            {
                if (!blocked)
                {
                    parser::Sphere sphere = scene.spheres[sphere_index];

                    misc::IntersectingRayData intersectionData = sphereIntersection(scene, sphere, shadow_ray);

                    if (intersectionData.intersect && intersectionData.t > epsilon && intersectionData.t < t_min && intersectionData.t < 1 &&
                         (hitObjectData.objectType != misc::ObjectType::SPHERE || hitObjectData.objectIndex != sphere_index))
                    {
                        blocked = true;
                        break;
                    }
                }
            }
            if (!blocked)
            {
                float cos_theta = misc::normalizeVector(pixel_to_light) * normal;

                cos_theta = cos_theta > 0 ? cos_theta : 0;

                light_red += (material.diffuse.x * cos_theta * point_light.intensity.x) / light_distance_squared;
                light_green += (material.diffuse.y * cos_theta * point_light.intensity.y) / light_distance_squared;
                light_blue += (material.diffuse.z * cos_theta * point_light.intensity.z) / light_distance_squared;

                parser::Vec3f h = misc::normalizeVector(misc::normalizeVector(pixel_to_light) + normalized_d * (-1));

                float cos_alpha = normal * h > 0 ? normal * h : 0;

                light_red += material.specular.x * std::pow(cos_alpha, material.phong_exponent) * point_light.intensity.x / light_distance_squared;
                light_green += material.specular.y * std::pow(cos_alpha, material.phong_exponent) * point_light.intensity.y / light_distance_squared;
                light_blue += material.specular.z * std::pow(cos_alpha, material.phong_exponent) * point_light.intensity.z / light_distance_squared;

                parser::Vec3f reflected_ray = normalized_d + normal * (normal * (normalized_d * (-1))) * 2;
                parser::ParametricLine new_eye_ray = {intersection_point, reflected_ray};

                parser::Vec3i reflected_color = {0, 0, 0};

                if (material.is_mirror)
                {
                    reflected_color = computeColors(scene, image, new_eye_ray, camera, meshNormals, triangleNormals, depth + 1, i, j);
                }

                light_red += reflected_color.x * material.mirror.x;
                light_green += reflected_color.y * material.mirror.y;
                light_blue += reflected_color.z * material.mirror.z;

            }
        }

        light_red += scene.ambient_light.x * material.ambient.x;
        light_green += scene.ambient_light.y * material.ambient.y;
        light_blue += scene.ambient_light.z * material.ambient.z;

        return {(int)light_red, (int)light_green, (int)light_blue};
    }

    else
        return {scene.background_color.x, scene.background_color.y, scene.background_color.z};
}

void findNormals(parser::Scene &scene, std::vector<std::vector<parser::Vec3f>> &meshNormals,
                 std::vector<parser::Vec3f> &triangleNormals)

{
    for (int i = 0; i < scene.meshes.size(); i++)
    {
        std::vector<parser::Vec3f> newVectors;

        for (int j = 0; j < scene.meshes[i].faces.size(); j++)
        {
            parser::Face face = scene.meshes[i].faces[j];

            misc::Face vectorFace = {scene.vertex_data[face.v0_id - 1], scene.vertex_data[face.v1_id - 1], scene.vertex_data[face.v2_id - 1]};

            newVectors.push_back(misc::triangleNorm(vectorFace));
        }

        meshNormals.push_back(newVectors);
    }

    for (int i = 0; i < scene.triangles.size(); i++)
    {
        parser::Face face = {scene.triangles[i].indices.v0_id, scene.triangles[i].indices.v1_id, scene.triangles[i].indices.v2_id};

        misc::Face vectorFace = {scene.vertex_data[face.v0_id - 1], scene.vertex_data[face.v1_id - 1], scene.vertex_data[face.v2_id - 1]};

        triangleNormals.push_back(misc::triangleNorm(vectorFace));
    }
}

misc::IntersectingRayData triangleIntersection(parser::Scene &scene, parser::Face face, parser::ParametricLine eye_ray)
{

    parser::Vec3f a = scene.vertex_data[face.v0_id - 1];
    parser::Vec3f b = scene.vertex_data[face.v1_id - 1];
    parser::Vec3f c = scene.vertex_data[face.v2_id - 1];

    misc::IntersectingRayData intersectionData = {false, __FLT_MAX__};

    float detA = (a.x - b.x) * (eye_ray.d.z * (a.y - c.y) - eye_ray.d.y * (a.z - c.z)) -
                 (a.x - c.x) * (eye_ray.d.z * (a.y - b.y) - eye_ray.d.y * (a.z - b.z)) +
                 (eye_ray.d.x) * ((a.y - b.y) * (a.z - c.z) - (a.y - c.y) * (a.z - b.z));

    if (detA == 0.0)
        return intersectionData;

    float detBeta = (a.x - eye_ray.e.x) * ((a.y - c.y) * eye_ray.d.z - eye_ray.d.y * (a.z - c.z)) +
                    (a.y - eye_ray.e.y) * (eye_ray.d.x * (a.z - c.z) - eye_ray.d.z * (a.x - c.x)) +
                    (a.z - eye_ray.e.z) * (eye_ray.d.y * (a.x - c.x) - eye_ray.d.x * (a.y - c.y));

    float detGamma = (a.x - b.x) * ((a.y - eye_ray.e.y) * eye_ray.d.z - eye_ray.d.y * (a.z - eye_ray.e.z)) +
                     (a.y - b.y) * (eye_ray.d.x * (a.z - eye_ray.e.z) - eye_ray.d.z * (a.x - eye_ray.e.x)) +
                     (a.z - b.z) * (eye_ray.d.y * (a.x - eye_ray.e.x) - eye_ray.d.x * (a.y - eye_ray.e.y));

    float detTime = (a.x - b.x) * ((a.y - c.y) * (a.z - eye_ray.e.z) - (a.y - eye_ray.e.y) * (a.z - c.z)) +
                    (a.y - b.y) * ((a.x - eye_ray.e.x) * (a.z - c.z) - (a.z - eye_ray.e.z) * (a.x - c.x)) +
                    (a.z - b.z) * ((a.y - eye_ray.e.y) * (a.x - c.x) - (a.x - eye_ray.e.x) * (a.y - c.y));

    float beta = detBeta / detA;
    float gamma = detGamma / detA;
    float t = detTime / detA;

    intersectionData.t = t;

    if (beta + gamma <= 1 && beta >= 0 && gamma >= 0)
        intersectionData.intersect = true;

    return intersectionData;
}

misc::IntersectingRayData sphereIntersection(parser::Scene &scene, parser::Sphere sphere, parser::ParametricLine eye_ray)
{

    parser::Vec3f center = scene.vertex_data[sphere.center_vertex_id - 1];

    float discriminant = (eye_ray.d * (eye_ray.e - center)) * (eye_ray.d * (eye_ray.e - center)) -
                         (eye_ray.d * eye_ray.d) * ((eye_ray.e - center) * (eye_ray.e - center) - sphere.radius * sphere.radius);

    // SPHERE'DA PATLARSAN BURAYA BAK!!!!!!!!!!!!!!!!!!!!!!!
    float t = ((eye_ray.d * (-1)) * (eye_ray.e - center) - std::sqrt(discriminant)) / (eye_ray.d * eye_ray.d);

    misc::IntersectingRayData intersectionData = {false, t};
    if (discriminant >= 0)
        intersectionData.intersect = true;

    return intersectionData;
}
