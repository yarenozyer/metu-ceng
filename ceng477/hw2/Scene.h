#ifndef _SCENE_H_
#define _SCENE_H_
#include "Vec3.h"
#include "Vec4.h"
#include "Color.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Camera.h"
#include "Mesh.h"
#include "Matrix4.h"
#include <set>
#include <tuple>
#include "Edge.h"

class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;

	std::vector<std::vector<Color> > image;
	std::vector<std::vector<double> > depth;
	std::vector<Camera *> cameras;
	std::vector<Vec3 *> vertices;
	std::vector<Scaling *> scalings;
	std::vector<Rotation *> rotations;
	std::vector<Translation *> translations;
	std::vector<Mesh *> meshes;
	

	Scene(const char *xmlPath);

	void assignColorToPixel(int i, int j, Color c);
	void initializeImage(Camera *camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera *camera);
	void convertPPMToPNG(std::string ppmFileName);
	void forwardRenderingPipeline(Camera *camera, int camera_i);

	Matrix4 applyMeshTransformations(Mesh * meshes, std::vector<Translation*> translations, 
		std::vector<Scaling *> scalings, std::vector<Rotation *> rotations);
	Matrix4 applyCameraTransformations(Camera* camera);
	Matrix4 applyProjectionTransformations(Camera* camera);
	Matrix4 applyOrthographicProjection(Camera* camera);
	Matrix4 applyPerspectiveProjection(Camera* camera);
	Matrix4 applyViewportTransformations(Camera* camera);
	bool visible(double d, double start, double &tE, double &tL);
	Edge* clip(Vec4 v_start, Vec4 v_end, int f_index);
	void applyClipping(std::vector<Edge *> &edges, std::vector<Triangle> faces, std::vector<Vec4>& transformedVertices);
	void applyCulling(Camera* camera, std::vector<Triangle> &faces, std::vector<Vec4> vertices);
	void applyPerspectiveDivide(Vec4& transformedVertex);
	void drawWireframe(std::vector<Edge *> edges, std::vector<Triangle> faces, std::vector<Vec3 *> transformedVertices, int camera_i);
	void drawLine(bool is_primary_x, bool is_increment, int primary_start, int primary_end, int secondary_start, int secondary_end, double depth_start, double depth_end, Color color_start, Color color_end);
	void drawSolid(std::vector<Triangle> faces, std::vector<Vec3 *> transformedVertices, int camera_i);
	Matrix4 applyTranslation(int translationId,  std::vector<Translation*> translations);
	Matrix4 applyTranslation(Translation* translation);
	Matrix4 applyScaling(int scalingId,  std::vector<Scaling*> scalings);
	Matrix4 applyRotation(int rotationId, std::vector<Rotation*> rotations);
	Matrix4 applyRotationOnX(double angle);
};

#endif
