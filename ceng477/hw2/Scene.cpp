#define _USE_MATH_DEFINES
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include <set>
#include <tuple>
#include "tinyxml2.h"
#include "Triangle.h"
#include "Helpers.h"
#include "Scene.h"

using namespace tinyxml2;
using namespace std;

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *xmlElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *rootNode = xmlDoc.FirstChild();

	// read background color
	xmlElement = rootNode->FirstChildElement("BackgroundColor");
	str = xmlElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	xmlElement = rootNode->FirstChildElement("Culling");
	if (xmlElement != NULL)
	{
		str = xmlElement->GetText();

		if (strcmp(str, "enabled") == 0)
		{
			this->cullingEnabled = true;
		}
		else
		{
			this->cullingEnabled = false;
		}
	}

	// read cameras
	xmlElement = rootNode->FirstChildElement("Cameras");
	XMLElement *camElement = xmlElement->FirstChildElement("Camera");
	XMLElement *camFieldElement;
	while (camElement != NULL)
	{
		Camera *camera = new Camera();

		camElement->QueryIntAttribute("id", &camera->cameraId);

		// read projection type
		str = camElement->Attribute("type");

		if (strcmp(str, "orthographic") == 0)
		{
			camera->projectionType = ORTOGRAPHIC_PROJECTION;
		}
		else
		{
			camera->projectionType = PERSPECTIVE_PROJECTION;
		}

		camFieldElement = camElement->FirstChildElement("Position");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

		camFieldElement = camElement->FirstChildElement("Gaze");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

		camFieldElement = camElement->FirstChildElement("Up");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

		camera->gaze = normalizeVec3(camera->gaze);
		camera->u = crossProductVec3(camera->gaze, camera->v);
		camera->u = normalizeVec3(camera->u);

		camera->w = inverseVec3(camera->gaze);
		camera->v = crossProductVec3(camera->u, camera->gaze);
		camera->v = normalizeVec3(camera->v);

		camFieldElement = camElement->FirstChildElement("ImagePlane");
		str = camFieldElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &camera->left, &camera->right, &camera->bottom, &camera->top,
			   &camera->near, &camera->far, &camera->horRes, &camera->verRes);

		camFieldElement = camElement->FirstChildElement("OutputName");
		str = camFieldElement->GetText();
		camera->outputFilename = string(str);

		this->cameras.push_back(camera);

		camElement = camElement->NextSiblingElement("Camera");
	}

	// read vertices
	xmlElement = rootNode->FirstChildElement("Vertices");
	XMLElement *vertexElement = xmlElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (vertexElement != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color color;

		str = vertexElement->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = vertexElement->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color.r, &color.g, &color.b);

		vertex->color = color;

		this->vertices.push_back(vertex);

		vertexElement = vertexElement->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	xmlElement = rootNode->FirstChildElement("Translations");
	XMLElement *translationElement = xmlElement->FirstChildElement("Translation");
	while (translationElement != NULL)
	{
		Translation *translation = new Translation();

		translationElement->QueryIntAttribute("id", &translation->translationId);

		str = translationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		this->translations.push_back(translation);

		translationElement = translationElement->NextSiblingElement("Translation");
	}

	// read scalings
	xmlElement = rootNode->FirstChildElement("Scalings");
	XMLElement *scalingElement = xmlElement->FirstChildElement("Scaling");
	while (scalingElement != NULL)
	{
		Scaling *scaling = new Scaling();

		scalingElement->QueryIntAttribute("id", &scaling->scalingId);
		str = scalingElement->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		this->scalings.push_back(scaling);

		scalingElement = scalingElement->NextSiblingElement("Scaling");
	}

	// read rotations
	xmlElement = rootNode->FirstChildElement("Rotations");
	XMLElement *rotationElement = xmlElement->FirstChildElement("Rotation");
	while (rotationElement != NULL)
	{
		Rotation *rotation = new Rotation();

		rotationElement->QueryIntAttribute("id", &rotation->rotationId);
		str = rotationElement->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		this->rotations.push_back(rotation);

		rotationElement = rotationElement->NextSiblingElement("Rotation");
	}

	// read meshes
	xmlElement = rootNode->FirstChildElement("Meshes");

	XMLElement *meshElement = xmlElement->FirstChildElement("Mesh");
	while (meshElement != NULL)
	{
		Mesh *mesh = new Mesh();

		meshElement->QueryIntAttribute("id", &mesh->meshId);

		// read projection type
		str = meshElement->Attribute("type");

		if (strcmp(str, "wireframe") == 0)
		{
			mesh->type = WIREFRAME_MESH;
		}
		else
		{
			mesh->type = SOLID_MESH;
		}

		// read mesh transformations
		XMLElement *meshTransformationsElement = meshElement->FirstChildElement("Transformations");
		XMLElement *meshTransformationElement = meshTransformationsElement->FirstChildElement("Transformation");

		while (meshTransformationElement != NULL)
		{
			char transformationType;
			int transformationId;

			str = meshTransformationElement->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			mesh->transformationTypes.push_back(transformationType);
			mesh->transformationIds.push_back(transformationId);

			meshTransformationElement = meshTransformationElement->NextSiblingElement("Transformation");
		}

		mesh->numberOfTransformations = mesh->transformationIds.size();

		// read mesh faces
		char *row;
		char *cloneStr;
		int v1, v2, v3;
		XMLElement *meshFacesElement = meshElement->FirstChildElement("Faces");
		str = meshFacesElement->GetText();
		cloneStr = strdup(str);

		row = strtok(cloneStr, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

			if (result != EOF)
			{
				mesh->triangles.push_back(Triangle(v1, v2, v3));
			}
			row = strtok(NULL, "\n");
		}
		mesh->numberOfTriangles = mesh->triangles.size();
		this->meshes.push_back(mesh);

		meshElement = meshElement->NextSiblingElement("Mesh");
	}
}

void Scene::assignColorToPixel(int i, int j, Color c)
{
	this->image[i][j].r = c.r;
	this->image[i][j].g = c.g;
	this->image[i][j].b = c.b;
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;
			vector<double> rowOfDepths;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
				rowOfDepths.push_back(1.01);
			}

			this->image.push_back(rowOfColors);
			this->depth.push_back(rowOfDepths);
		}
	}
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				assignColorToPixel(i, j, this->backgroundColor);
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
				this->depth[i][j] = 1.01;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFilename.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFilename << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
*/
void Scene::convertPPMToPNG(string ppmFileName)
{
	string command;

	// TODO: Change implementation if necessary.
	command = "./magick convert " + ppmFileName + " " + ppmFileName + ".png";
	system(command.c_str());
}

/*
	Transformations, clipping, culling, rasterization are done here.
*/
void Scene::forwardRenderingPipeline(Camera *camera, int camera_i)
{
	// TODO: Implement this function
	// start with mesh transformations

	Matrix4 cameraTransformMatrix = applyCameraTransformations(camera);

	Matrix4 projectionTransformMatrix = applyProjectionTransformations(camera);

	Matrix4 viewportTransformMatrix = applyViewportTransformations(camera);

	for (int i = 0; i < meshes.size(); i++)
	{
		Mesh *mesh = meshes[i];
		vector<Vec3 *> transformedVertices;
		vector<Vec4> transformedVerticesWithoutVp;
		vector<Edge *> edges;

		Matrix4 compositeMatrix = applyMeshTransformations(mesh, translations, scalings, rotations);

		compositeMatrix = multiplyMatrixWithMatrix(cameraTransformMatrix, compositeMatrix);
		compositeMatrix = multiplyMatrixWithMatrix(projectionTransformMatrix, compositeMatrix);
		// apply transformations for mesh
		for (int v_index = 0; v_index < vertices.size(); v_index++)
		{
			Vec3 *c_vertex = vertices[v_index];

			Vec4 transformedVertex = multiplyMatrixWithVec4(compositeMatrix, Vec4(c_vertex->x, c_vertex->y, c_vertex->z, 1, c_vertex->color));

			applyPerspectiveDivide(transformedVertex);
			transformedVerticesWithoutVp.push_back(transformedVertex);
		}

		if (this->cullingEnabled)
		{
			applyCulling(camera, mesh->triangles, transformedVerticesWithoutVp);
		}

		switch (mesh->type)
		{
		case 0:
			applyClipping(edges, mesh->triangles, transformedVerticesWithoutVp);

			for (int edge_index = 0; edge_index < edges.size(); edge_index++)
			{
				Edge *edge = edges[edge_index];

				if (!edge->is_visible)
					continue;

				edge->v0 = multiplyMatrixWithVec4(viewportTransformMatrix, edge->v0);
				edge->v1 = multiplyMatrixWithVec4(viewportTransformMatrix, edge->v1);
			}
			drawWireframe(edges, mesh->triangles, transformedVertices, camera_i);
			break;

		case 1:
			for (int v_index = 0; v_index < vertices.size(); v_index++)
			{
				Vec4 transformedVertex = transformedVerticesWithoutVp[v_index];

				transformedVertex = multiplyMatrixWithVec4(viewportTransformMatrix, transformedVertex);

				Vec3 *newVertex = new Vec3(transformedVertex.x, transformedVertex.y, transformedVertex.z, transformedVertex.color);

				transformedVertices.push_back(newVertex);
			}
			drawSolid(mesh->triangles, transformedVertices, camera_i);
		default:
			break;
		}
	}
}

Matrix4 Scene::applyMeshTransformations(Mesh *mesh, vector<Translation *> translations,
										vector<Scaling *> scalings, vector<Rotation *> rotations)
{
	// loop for all the meshes
	// NO CONDITION FOR WIREFRAME AND SOLID
	int numberOfTransformations = mesh->numberOfTransformations;
	Matrix4 compositeMatrix = getIdentityMatrix();
	Vec4 vec(1, 1, 1, 1);
	for (int t = 0; t < numberOfTransformations; t++)
	{
		switch (mesh->transformationTypes[t])
		{
		case 't':
			compositeMatrix = multiplyMatrixWithMatrix(applyTranslation(mesh->transformationIds[t], translations), compositeMatrix);
			break;

		case 'r':
			compositeMatrix = multiplyMatrixWithMatrix(applyRotation(mesh->transformationIds[t], rotations), compositeMatrix);

			break;

		case 's':
			compositeMatrix = multiplyMatrixWithMatrix(applyScaling(mesh->transformationIds[t], scalings), compositeMatrix);
			break;

		default:
			break;
		}
	}
	return compositeMatrix;
}

Matrix4 Scene::applyCameraTransformations(Camera *camera)
{
	Translation *translation = new Translation(-1, -camera->position.x, -camera->position.y, -camera->position.z);

	Matrix4 compositeMatrix = applyTranslation(translation);

	Vec3 u = camera->u;
	Vec3 v = camera->v;
	Vec3 w = camera->w;

	double r[4][4] = {
		{u.x, u.y, u.z, 0},
		{v.x, v.y, v.z, 0},
		{w.x, w.y, w.z, 0},
		{0, 0, 0, 1}};

	Matrix4 rotationMatrix(r);
	compositeMatrix = multiplyMatrixWithMatrix(rotationMatrix, compositeMatrix);

	return compositeMatrix;
}

Matrix4 Scene::applyProjectionTransformations(Camera *camera)
{
	Matrix4 compositeMatrix = getIdentityMatrix();

	switch (camera->projectionType)
	{
	// ortographic
	case 0:
		compositeMatrix = applyOrthographicProjection(camera);
		break;

	// perspective
	case 1:
		compositeMatrix = applyPerspectiveProjection(camera);
		break;

	default:
		break;
	}

	return compositeMatrix;
}

Matrix4 Scene::applyOrthographicProjection(Camera *camera)
{
	double l = camera->left, r = camera->right, b = camera->bottom, t = camera->top;
	double f = camera->far, n = camera->near;

	double values[4][4] = {
		{2 / (r - l), 0, 0, -(r + l) / (r - l)},
		{0, 2 / (t - b), 0, -(t + b) / (t - b)},
		{0, 0, -2 / (f - n), -(f + n) / (f - n)},
		{0, 0, 0, 1}};

	Matrix4 orthoMatrix(values);

	return orthoMatrix;
}

Matrix4 Scene::applyPerspectiveProjection(Camera *camera)
{
	double f = camera->far, n = camera->near;

	double values[4][4] = {
		{n, 0, 0, 0},
		{0, n, 0, 0},
		{0, 0, f + n, f * n},
		{0, 0, -1, 0}};
	Matrix4 p2o(values);

	Matrix4 orthoMatrix = applyOrthographicProjection(camera);

	Matrix4 perspectiveMatrix = multiplyMatrixWithMatrix(orthoMatrix, p2o);

	return perspectiveMatrix;
}

Matrix4 Scene::applyViewportTransformations(Camera *camera)
{
	double nx = camera->horRes;
	double ny = camera->verRes;

	double values[4][4] = {
		{nx / 2, 0, 0, (nx - 1) / 2},
		{0, ny / 2, 0, (ny - 1) / 2},
		{0, 0, 0.5, 0.5},
		{0, 0, 0, 0}};

	Matrix4 viewportMatrix(values);
	return viewportMatrix;
}

void Scene::applyCulling(Camera *camera, vector<Triangle> &faces, vector<Vec4> vertices)
{
	Vec3 cameraPos = camera->position;

	for (int face_index = 0; face_index < faces.size(); face_index++)
	{
		Triangle face = faces[face_index];

		Vec3 v0 = vertices[face.vertexIds[0] - 1];
		Vec3 v1 = vertices[face.vertexIds[1] - 1];
		Vec3 v2 = vertices[face.vertexIds[2] - 1];

		Vec3 v = normalizeVec3(subtractVec3(v0, cameraPos));

		Vec3 n = normalizeVec3(crossProductVec3(subtractVec3(v1, v0), subtractVec3(v2, v0)));

		if (dotProductVec3(v, n) < 0)
		{
			faces[face_index].isCulled = true;
		}
	}
}

void Scene::applyClipping(vector<Edge *> &edges, vector<Triangle> faces, vector<Vec4> &transformedVertices)
{
	int numberOfFaces = faces.size();
	for (int f_index = 0; f_index < numberOfFaces; f_index++)
	{
		Triangle face = faces[f_index];

		if (this->cullingEnabled && face.isCulled)
			continue;

		edges.push_back(clip(transformedVertices[face.vertexIds[0] - 1], transformedVertices[face.vertexIds[1] - 1], f_index));
		edges.push_back(clip(transformedVertices[face.vertexIds[1] - 1], transformedVertices[face.vertexIds[2] - 1], f_index));
		edges.push_back(clip(transformedVertices[face.vertexIds[2] - 1], transformedVertices[face.vertexIds[0] - 1], f_index));
	}
}

bool Scene::visible(double d, double num, double &tE, double &tL)
{
	if (d > 0)
	{
		double t = num / d;
		if (t > tL)
			return false;
		else if (t > tE)
			tE = t;
	}
	else if (d < 0)
	{
		double t = num / d;
		if (t < tE)
			return false;
		else if (t < tL)
			tL = t;
	}
	else if (num > 0)
		return false;
	return true;
}

Edge *Scene::clip(Vec4 v_start, Vec4 v_end, int f_index)
{
	double tE = 0, tL = 1;
	bool is_visible = false;
	double dx = v_end.x - v_start.x;
	double dy = v_end.y - v_start.y;
	double dz = v_end.z - v_start.z;
	Color dcolor = v_end.color - v_start.color;

	if (visible(dx, -1 - v_start.x, tE, tL))
	{
		if (visible(-dx, v_start.x - 1, tE, tL))
		{
			if (visible(dy, -1 - v_start.y, tE, tL))
			{
				if (visible(-dy, v_start.y - 1, tE, tL))
				{
					if (visible(dz, -1 - v_start.z, tE, tL))
					{
						if (visible(-dz, v_start.z - 1, tE, tL))
						{
							// cout << "entering" << v_start << v_end << endl;
							// cout << "tL: " << tL << " tE: " << tE << endl;
							is_visible = true;
							if (tL < 1)
							{
								v_end.x = v_start.x + tL * dx;
								v_end.y = v_start.y + tL * dy;
								v_end.z = v_start.z + tL * dz;
								v_end.color = v_start.color + (dcolor * tL);
							}
							if (tE > 0)
							{
								v_start.x = v_start.x + tE * dx;
								v_start.y = v_start.y + tE * dy;
								v_start.z = v_start.z + tE * dz;
								v_start.color = v_start.color + (dcolor * tE);
							}
							// cout << "exiting" << v_start << v_end << endl;
						}
					}
				}
			}
		}
	}
	return new Edge(v_start, v_end, is_visible, f_index);
}

void Scene::applyPerspectiveDivide(Vec4 &transformedVertex)
{
	transformedVertex.x /= transformedVertex.t;
	transformedVertex.y /= transformedVertex.t;
	transformedVertex.z /= transformedVertex.t;
	transformedVertex.t = 1;
}

void Scene::drawWireframe(vector<Edge *> edges, vector<Triangle> faces, vector<Vec3 *> transformedVertices, int camera_i)
{
	// TODO DEPTH BUFFER
	for (int edge_index = 0; edge_index < edges.size(); edge_index++)
	{
		Edge *edge = edges[edge_index];

		if (!edge->is_visible)
			continue;

		Color blackColor(0, 0, 0);
		Vec3 v1 = edge->v0;
		Vec3 v2 = edge->v1;

		int x_start = (v1.x);
		int y_start = (v1.y);
		Color color_start = v1.color;

		int x_end = (v2.x);
		int y_end = (v2.y);
		Color color_end = v2.color;

		double z_start = (v1.z);
		double z_end = (v2.z);

		// Check if start point is greater than end point along the x-axis
		if (x_start > x_end)
		{
			std::swap(x_start, x_end);
			std::swap(y_start, y_end);
			std::swap(color_start, color_end);
		}

		double slope = static_cast<double>(y_end - y_start) / (x_end - x_start);

		if (0 <= slope && slope <= 1)
		{
			if (x_start > x_end)
			{
				std::swap(x_start, x_end);
				std::swap(y_start, y_end);
				std::swap(color_start, color_end);
			}
			drawLine(true, true, x_start, x_end, y_start, y_end, z_start, z_end, color_start, color_end);
		}
		else if (0 > slope && slope >= -1)
		{
			if (x_start < x_end)
			{
				std::swap(x_start, x_end);
				std::swap(y_start, y_end);
				std::swap(color_start, color_end);
			}

			drawLine(true, false, x_start, x_end, y_start, y_end, z_start, z_end, color_start, color_end);
		}
		else if (slope > 1)
		{
			if (y_start > y_end)
			{
				std::swap(x_start, x_end);
				std::swap(y_start, y_end);
				std::swap(color_start, color_end);
			}
			drawLine(false, true, y_start, y_end, x_start, x_end, z_start, z_end, color_start, color_end);
		}
		else if (slope < -1)
		{
			if (y_start < y_end)
			{
				std::swap(x_start, x_end);
				std::swap(y_start, y_end);
				std::swap(color_start, color_end);
			}
			drawLine(false, false, y_start, y_end, x_start, x_end, z_start, z_end, color_start, color_end);
		}
	}
}

void Scene::drawLine(bool is_primary_x, bool is_increment, int primary_start, int primary_end, int secondary_start, int secondary_end, double depth_start, double depth_end, Color color_start, Color color_end)
{
	if (primary_start > primary_end)
	{
		drawLine(is_primary_x, is_increment, primary_end, primary_start, secondary_end, secondary_start, depth_end, depth_start, color_end, color_start);
	}

	else
	{
		if ((primary_start == 98 || primary_start == 97) && (secondary_start == 499 || secondary_start == 500) && (primary_end == 901) && (secondary_end == 499 || secondary_end == 500))
		{
			cout << "MAL FALAN Bİ RŞEY" << endl;
		}

		int primary = primary_start;
		int secondary = secondary_start;
		double z = depth_start;

		int primary_delta = primary_end - primary_start;
		int secondary_delta = secondary_end - secondary_start;

		if (is_increment)
			secondary_delta *= -1;

		int d = 2 * secondary_delta + primary_delta;
		int big_delta_d = 2 * (secondary_delta + primary_delta);
		int small_delta_d = 2 * secondary_delta;

		Color color = color_start;
		Color delta_color = (color_end - color_start) / primary_delta;
		double depth_delta = (depth_end - depth_start) / primary_delta;

		while (primary <= primary_end)
		{

			if (is_primary_x)
			{
				if (primary == 167 && secondary == 707)
				{
					cout << "z: " << z << " color: " << color << endl;
				}
				if (z < depth[primary][secondary])
				{
					depth[primary][secondary] = z;
					assignColorToPixel(primary, secondary, color);
				}
			}
			else
			{
				if (z < depth[secondary][primary])
				{
					depth[secondary][primary] = z;
					assignColorToPixel(secondary, primary, color);
				}
			}

			if (d < 0)
			{
				if (is_increment)
					secondary += 1;
				else
					secondary -= 1;

				d += big_delta_d;
			}
			else
			{
				d += small_delta_d;
			}
			primary++;
			color = color + delta_color;
			z += depth_delta;
		}
	}
}

void Scene::drawSolid(std::vector<Triangle> faces, std::vector<Vec3 *> transformedVertices, int camera_i)
{

	for (int face_index = 0; face_index < faces.size(); face_index++)
	{
		double error_margin = 0.001;

		Triangle face = faces[face_index];
		if (this->cullingEnabled && face.isCulled)
			continue;

		Vec3 *v0 = transformedVertices[face.vertexIds[0] - 1];
		Vec3 *v1 = transformedVertices[face.vertexIds[1] - 1];
		Vec3 *v2 = transformedVertices[face.vertexIds[2] - 1];

		Color c0 = v0->color;
		Color c1 = v1->color;
		Color c2 = v2->color;

		double f_01_x_incr = v0->y - v1->y;
		double f_01_y_incr = v1->x - v0->x;

		double f_12_x_incr = v1->y - v2->y;
		double f_12_y_incr = v2->x - v1->x;

		double f_20_x_incr = v2->y - v0->y;
		double f_20_y_incr = v0->x - v2->x;

		double f_01_const = v2->x * f_01_x_incr + v2->y * f_01_y_incr + v0->x * v1->y - v0->y * v1->x;
		double f_12_const = v0->x * f_12_x_incr + v0->y * f_12_y_incr + v1->x * v2->y - v1->y * v2->x;
		double f_20_const = v1->x * f_20_x_incr + v1->y * f_20_y_incr + v2->x * v0->y - v2->y * v0->x;

		int x_min = min(v0->x, min(v1->x, v2->x));
		int y_min = min(v0->y, min(v1->y, v2->y));

		int x_max = max(v0->x, max(v1->x, v2->x));
		int y_max = max(v0->y, max(v1->y, v2->y));

		double alpha = (x_min * f_12_x_incr + y_min * f_12_y_incr + v1->x * v2->y - v1->y * v2->x) / f_12_const;
		double alpha_incr_x = f_12_x_incr / f_12_const;
		double alpha_incr_y = f_12_y_incr / f_12_const;

		double beta = (x_min * f_20_x_incr + y_min * f_20_y_incr + v2->x * v0->y - v2->y * v0->x) / f_20_const;
		double beta_incr_x = f_20_x_incr / f_20_const;
		double beta_incr_y = f_20_y_incr / f_20_const;

		double gamma = (x_min * f_01_x_incr + y_min * f_01_y_incr + v0->x * v1->y - v0->y * v1->x) / f_01_const;
		double gamma_incr_x = f_01_x_incr / f_01_const;
		double gamma_incr_y = f_01_y_incr / f_01_const;

		for (int y = y_min; y <= y_max; y++)
		{
			for (int x = x_min; x <= x_max; x++)
			{
				if (alpha >= -error_margin && beta >= -error_margin && gamma >= -error_margin)
				{
					Color newColor = c0 * alpha + c1 * beta + c2 * gamma;

					double z = alpha * v0->z + beta * v1->z + gamma * v2->z;
					if (z < depth[x][y])
					{
						depth[x][y] = z;
						assignColorToPixel(x, y, newColor);
					}
				}
				alpha += alpha_incr_x;
				beta += beta_incr_x;
				gamma += gamma_incr_x;
			}
			alpha += alpha_incr_y - (x_max - x_min + 1) * alpha_incr_x;
			beta += beta_incr_y - (x_max - x_min + 1) * beta_incr_x;
			gamma += gamma_incr_y - (x_max - x_min + 1) * gamma_incr_x;
		}
	}
}

Matrix4 Scene::applyTranslation(int translationId, vector<Translation *> translations)
{
	Translation *translation = translations[translationId - 1];

	return applyTranslation(translation);
}

Matrix4 Scene::applyTranslation(Translation *translation)
{
	Matrix4 transMatrix = getIdentityMatrix();
	transMatrix.values[0][3] = translation->tx;
	transMatrix.values[1][3] = translation->ty;
	transMatrix.values[2][3] = translation->tz;

	return transMatrix;
}

Matrix4 Scene::applyScaling(int scalingId, std::vector<Scaling *> scalings)
{
	Scaling *scaling = scalings[scalingId - 1];
	Matrix4 transMatrix = getIdentityMatrix();
	transMatrix.values[0][0] = scaling->sx;
	transMatrix.values[1][1] = scaling->sy;
	transMatrix.values[2][2] = scaling->sz;

	return transMatrix;
}

Matrix4 Scene::applyRotation(int rotationId, std::vector<Rotation *> rotations)
{
	Rotation *rotation = rotations[rotationId - 1];

	Matrix4 transMatrix = getIdentityMatrix();

	Vec3 u(rotation->ux, rotation->uy, rotation->uz);
	u = normalizeVec3(u);

	Vec3 v;

	double minU = min(u.x, min(u.y, u.z));

	if (u.x == minU)
	{
		v = Vec3(0, -u.z, u.y);
	}
	else if (u.y == minU)
	{
		v = Vec3(-u.z, 0, u.x);
	}
	else
	{
		v = Vec3(-u.y, u.x, 0);
	}

	v = normalizeVec3(v);

	Vec3 w = crossProductVec3(u, v);

	double m[3][3] = {
		{u.x, u.y, u.z},
		{v.x, v.y, v.z},
		{w.x, w.y, w.z}};

	Matrix4 m4x4(m);
	transMatrix = multiplyMatrixWithMatrix(m4x4, transMatrix);

	transMatrix = multiplyMatrixWithMatrix(applyRotationOnX(rotation->angle), transMatrix);

	double m_inverse[3][3] = {
		{u.x, v.x, w.x},
		{u.y, v.y, w.y},
		{u.z, v.z, w.z}};

	Matrix4 m_inverse4x4(m_inverse);

	transMatrix = multiplyMatrixWithMatrix(m_inverse4x4, transMatrix);

	return transMatrix;
}

Matrix4 Scene::applyRotationOnX(double angle)
{
	angle = (angle * M_PI) / 180.0;
	double cosx = cos(angle);
	double sinx = sin(angle);

	double values[4][4] = {
		{1, 0, 0, 0},
		{0, cosx, (-1.0) * sinx, 0},
		{0, sinx, cosx, 0},
		{0, 0, 0, 1}};

	Matrix4 transMatrix(values);

	return transMatrix;
}