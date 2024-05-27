#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
//#include <OpenGL/gl3.h>   // The GL Header File
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#define BUFFER_OFFSET(i) ((char*)NULL + (i))


using namespace std;

void resetGame();
GLuint gProgram[6];
int gWidth=1200, gHeight=800;

GLint modelingMatrixLoc[5];
GLint viewingMatrixLoc[5];
GLint projectionMatrixLoc[5];
GLint eyePosLoc[5];
GLint modelingMatrixWithoutTranslationLoc;

//matrices that are share within the scene
glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::vec3 eyePos(0, 0, 0);

glm::mat4 modelingMatrixBunny;

glm::mat4 modelingMatrixGround;

glm::mat4 modelingMatrixRedCheckpointFirst;
glm::mat4 modelingMatrixRedCheckpointSecond;
glm::mat4 modelingMatrixYellowCheckpoint;

glm::mat4 modelingMatrixText = glm::mat4(1.0f);
glm::vec3 textColor = glm::vec3(1.0, 1.0, 0.0);

glm::vec3 bunnyPosition(0,0,0);

vector< GLuint> vao(6);

string scoreText = "";
int score = 0;
int deltaScore = 1;

float gameSpeed = 0.0;
int yellowCheckpointPos = 1;
int bunnyJumpCounter = 0;
float bunnyHappyCounter = 0;
int shuffleCounter;
float jumpSpeed = 0.04;
//int jumpDirection = 1;

glm::mat4 bunnyInitRot = glm::rotate<float>(glm::mat4(1.0), (90. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));
glm::mat4 bunnyInitTrans = glm::translate<float>(glm::mat4(1.0), glm::vec3(-0.5, 0.0, -4.0));
glm::mat4 bunnyInitScale = glm::scale(glm::mat4(1.0), glm::vec3(0.6, 0.6, 0.6));

glm::mat4 bunnyKillRot = glm::rotate<float>(glm::mat4(1.0), (-90. / 180.) * M_PI, glm::vec3(0.0, 0.0, 1.0));

glm::mat4 checkpointInitScale = glm::scale(glm::mat4(1.0), glm::vec3(0.6, 1.2, 0.6));

glm::mat4 checkpointInitMiddle = glm::translate<float>(glm::mat4(1.0), glm::vec3(0.0, 1.8, 60.0)) * checkpointInitScale;
glm::mat4 checkpointInitRight = glm::translate<float>(glm::mat4(1.0), glm::vec3(-3.5, 1.8, 60.0)) * checkpointInitScale;
glm::mat4 checkpointInitLeft = glm::translate<float>(glm::mat4(1.0), glm::vec3(3.5, 1.8, 60.0)) * checkpointInitScale;

glm::mat4 translationCheckpointMiddle = checkpointInitMiddle;
glm::mat4 translationCheckpointRight = checkpointInitRight;
glm::mat4 translationCheckpointLeft = checkpointInitLeft;

bool canCollide = true;
bool disableYellow = false;
bool disableFirstRed = false;
bool disableSecondRed = false;
bool isPlayerMovementEnabled = true;
bool isBunnyHappy = false;
bool isNewGame = true;

glm::mat4 deltaBunnyJump = glm::translate<float>(glm::mat4(1.0), glm::vec3(0.0, jumpSpeed, 0.0));
glm::mat4 translationBunny = glm::mat4(1.0);
glm::mat4 deltaBunnyLeft = glm::translate<float>(glm::mat4(1.0), glm::vec3(1.0/7.0, 0.0, 0.0));
glm::mat4 deltaBunnyRight = glm::translate<float>(glm::mat4(1.0), glm::vec3(-1.0/7.0, 0.0, 0.0)); 

glm::mat4 rotationTest = glm::mat4(1.0);
glm::mat4 deltaHappyBunny = glm::rotate<float>(glm::mat4(1.0), (9. / 180.) * M_PI, glm::vec3(0.0, 1.0, 0.0));

glm::mat4 deltaGroundMovement = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f, -gameSpeed));
glm::mat4 translationGround = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, -0.7, 0.0));
glm::mat4 modelingMatrixWithoutTranslation;

float checkpointSpeedMultiplier = 1.05f;

glm::mat4 deltaCheckpoint = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f,-checkpointSpeedMultiplier * gameSpeed));



int movingDirection = 0;
int activeProgramIndex = 0;

struct Vertex
{
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Texture
{
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
	GLfloat u, v;
};

struct Normal
{
	Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Face
{
	Face(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};


//0->Bunny 1->Ground
vector<vector<Vertex>> gVertices(5);
vector < vector<Texture>> gTextures(5);
vector < vector<Normal>> gNormals(5);
vector < vector<Face>> gFaces(5);

vector<GLuint> gVertexAttribBuffer(5), gIndexBuffer(5);
vector<GLint> gInVertexLoc(5), gInNormalLoc(5);
vector<int> gVertexDataSizeInBytes(5), gNormalDataSizeInBytes(5);

GLuint gTextVBO;



/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;

bool ParseObj(const string& fileName, int obj_index)
{
	fstream myfile;

	// Open the input 
	myfile.open(fileName.c_str(), std::ios::in);

	if (myfile.is_open())
	{
		string curLine;

		while (getline(myfile, curLine))
		{
			stringstream str(curLine);
			GLfloat c1, c2, c3;
			GLuint index[9];
			string tmp;

			if (curLine.length() >= 2)
			{
				if (curLine[0] == 'v')
				{
					if (curLine[1] == 't') // texture
					{
						str >> tmp; // consume "vt"
						str >> c1 >> c2;
						gTextures[obj_index].push_back(Texture(c1, c2));
					}
					else if (curLine[1] == 'n') // normal
					{
						str >> tmp; // consume "vn"
						str >> c1 >> c2 >> c3;
						gNormals[obj_index].push_back(Normal(c1, c2, c3));
					}
					else // vertex
					{
						str >> tmp; // consume "v"
						str >> c1 >> c2 >> c3;
						gVertices[obj_index].push_back(Vertex(c1, c2, c3));
					}
				}
				else if (curLine[0] == 'f') // face
				{
					str >> tmp; // consume "f"
					char c;
					int vIndex[3], nIndex[3], tIndex[3];
					str >> vIndex[0]; str >> c >> c; // consume "//"
					str >> nIndex[0];
					str >> vIndex[1]; str >> c >> c; // consume "//"
					str >> nIndex[1];
					str >> vIndex[2]; str >> c >> c; // consume "//"
					str >> nIndex[2];

					assert(vIndex[0] == nIndex[0] &&
						vIndex[1] == nIndex[1] &&
						vIndex[2] == nIndex[2]); // a limitation for now

					// make indices start from 0
					for (int c = 0; c < 3; ++c)
					{
						vIndex[c] -= 1;
						nIndex[c] -= 1;
						tIndex[c] -= 1;
					}

					gFaces[obj_index].push_back(Face(vIndex, tIndex, nIndex));
				}
				else
				{
					cout << "Ignoring unidentified line in obj file: " << curLine << endl;
				}
			}

			//data += curLine;
			if (!myfile.eof())
			{
				//data += "\n";
			}
		}

		myfile.close();
	}
	else
	{
		return false;
	}

	

	assert(gVertices[obj_index].size() == gNormals[obj_index].size());

	return true;
}

bool ReadDataFromFile(
	const string& fileName, ///< [in]  Name of the shader file
	string& data)     ///< [out] The contents of the file
{
	fstream myfile;

	// Open the input 
	myfile.open(fileName.c_str(), std::ios::in);

	if (myfile.is_open())
	{
		string curLine;

		while (getline(myfile, curLine))
		{
			data += curLine;
			if (!myfile.eof())
			{
				data += "\n";
			}
		}

		myfile.close();
	}
	else
	{
		return false;
	}

	return true;
}

GLuint createVS(const char* shaderName)
{
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &shader, &length);
	glCompileShader(vs);

	char output[1024] = { 0 };
	glGetShaderInfoLog(vs, 1024, &length, output);
	printf("VS compile log: %s\n", output);

	return vs;
}

GLuint createFS(const char* shaderName)
{
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &shader, &length);
	glCompileShader(fs);

	char output[1024] = { 0 };
	glGetShaderInfoLog(fs, 1024, &length, output);
	printf("FS compile log: %s\n", output);

	return fs;
}

void initShaders()
{
	// Create the programs
	for(int i = 0; i<6 ; i++)
		gProgram[i] = glCreateProgram();
	

	// Create the shaders 

	GLuint bunnyVert = createVS("bunnyVert.glsl");
	GLuint bunnyFrag = createFS("bunnyFrag.glsl");

	GLuint groundVert = createVS("groundVert.glsl");
	GLuint groundFrag = createFS("groundFrag.glsl");

	GLuint redCheckpointFirstVert = createVS("redCheckpointFirstVert.glsl");
	GLuint redCheckpointFirstFrag = createFS("redCheckpointFirstFrag.glsl");

	GLuint redCheckpointSecondVert = createVS("redCheckpointSecondVert.glsl");
	GLuint redCheckpointSecondFrag = createFS("redCheckpointSecondFrag.glsl");

	GLuint yellowCheckpointVert = createVS("yellowCheckpointVert.glsl");
	GLuint yellowCheckpointFrag = createFS("yellowCheckpointFrag.glsl");

	GLuint textVert = createVS("textVert.glsl");
	GLuint textFrag = createFS("textFrag.glsl");


	
	// Attach the shaders to the programs

	glAttachShader(gProgram[0], bunnyVert);
	glAttachShader(gProgram[0], bunnyFrag);

	glAttachShader(gProgram[1], groundVert);
	glAttachShader(gProgram[1], groundFrag);

	glAttachShader(gProgram[2], redCheckpointFirstVert);
	glAttachShader(gProgram[2], redCheckpointFirstFrag);

	glAttachShader(gProgram[3], redCheckpointSecondVert);
	glAttachShader(gProgram[3], redCheckpointSecondFrag);

	glAttachShader(gProgram[4], yellowCheckpointVert);
	glAttachShader(gProgram[4], yellowCheckpointFrag);

	glAttachShader(gProgram[5], textVert);
	glAttachShader(gProgram[5], textFrag);

	glBindAttribLocation(gProgram[5], 2, "vertex");
	
	// Link the programs
	
	GLint status;
	
	for (int i = 0; i < 6; i++)
	{
		glLinkProgram(gProgram[i]);
		glGetProgramiv(gProgram[i], GL_LINK_STATUS, &status);

		if (status != GL_TRUE)
		{
			cout << "Program link failed" << endl;
			exit(-1);
		}
	}

	

	// Get the locations of the uniform variables from both programs

	for (int i = 0; i < 5; ++i)
	{
		modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
		viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
		projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
		eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
	}
	modelingMatrixWithoutTranslationLoc = glGetUniformLocation(gProgram[1], "modelingMatrixWithoutTranslation");
}

void initVBO(int obj_index)
{
	//GLuint vao;
	glGenVertexArrays(1, &vao[obj_index]);
	assert(vao[obj_index] > 0);
	glBindVertexArray(vao[obj_index]);
	cout << "vao = " << vao[obj_index] << endl;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &gVertexAttribBuffer[obj_index]);
	glGenBuffers(1, &gIndexBuffer[obj_index]);

	assert(gVertexAttribBuffer[obj_index] > 0 && gIndexBuffer[obj_index] > 0);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer[obj_index]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer[obj_index]);

	gVertexDataSizeInBytes[obj_index] = gVertices[obj_index].size() * 3 * sizeof(GLfloat);
	gNormalDataSizeInBytes[obj_index] = gNormals[obj_index].size() * 3 * sizeof(GLfloat);

	int indexDataSizeInBytes = gFaces[obj_index].size() * 3 * sizeof(GLuint);

	GLfloat* vertexData = new GLfloat[gVertices[obj_index].size() * 3];
	GLfloat* normalData = new GLfloat[gNormals[obj_index].size() * 3];
	GLuint* indexData = new GLuint[gFaces[obj_index].size() * 3];

	float minX = 1e6, maxX = -1e6;
	float minY = 1e6, maxY = -1e6;
	float minZ = 1e6, maxZ = -1e6;

	for (int i = 0; i < gVertices[obj_index].size(); ++i)
	{
		vertexData[3 * i] = gVertices[obj_index][i].x;
		vertexData[3 * i + 1] = gVertices[obj_index][i].y;
		vertexData[3 * i + 2] = gVertices[obj_index][i].z;

		minX = std::min(minX, gVertices[obj_index][i].x);
		maxX = std::max(maxX, gVertices[obj_index][i].x);
		minY = std::min(minY, gVertices[obj_index][i].y);
		maxY = std::max(maxY, gVertices[obj_index][i].y);
		minZ = std::min(minZ, gVertices[obj_index][i].z);
		maxZ = std::max(maxZ, gVertices[obj_index][i].z);
	}

	for (int i = 0; i < gNormals[obj_index].size(); ++i)
	{
		normalData[3 * i] = gNormals[obj_index][i].x;
		normalData[3 * i + 1] = gNormals[obj_index][i].y;
		normalData[3 * i + 2] = gNormals[obj_index][i].z;
	}

	for (int i = 0; i < gFaces[obj_index].size(); ++i)
	{
		indexData[3 * i] = gFaces[obj_index][i].vIndex[0];
		indexData[3 * i + 1] = gFaces[obj_index][i].vIndex[1];
		indexData[3 * i + 2] = gFaces[obj_index][i].vIndex[2];
	}


	glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes[obj_index] + gNormalDataSizeInBytes[obj_index], 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes[obj_index], vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes[obj_index], gNormalDataSizeInBytes[obj_index], normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	// done copying to GPU memory; can free now from CPU memory
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes[obj_index]));
	glBindVertexArray(0);
}

glm::vec3 calculateBunnyPosition()
{
	glm::vec3 sum(0.0f);
	for (const auto& vertex : gVertices[0])
	{
		sum += glm::vec3(vertex.x, vertex.y, vertex.z);
	}
	glm::vec3 center = sum / static_cast<float>(gVertices[0].size());
	return center;
}

void initFonts(int windowWidth, int windowHeight)
{
	// Set OpenGL options
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
	glUseProgram(gProgram[5]);
	glUniformMatrix4fv(glGetUniformLocation(gProgram[5], "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSerif-Italic.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	//
	// Configure VBO for texture quads
	//
	

	glGenBuffers(1, &gTextVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glGenVertexArrays(1, &vao[5]);
	assert(vao[5] > 0);
	glBindVertexArray(vao[5]);
	glEnableVertexAttribArray(2);
	
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	assert(glGetError() == GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init()
{
	ParseObj("bunny.obj", 0);
	ParseObj("quad.obj", 1);
	ParseObj("cube.obj", 2);
	ParseObj("cube.obj", 3);
	ParseObj("cube.obj", 4);

	bunnyPosition = calculateBunnyPosition();

	glEnable(GL_DEPTH_TEST);
	initShaders();

	initFonts(gWidth, gHeight);
	
	for (int i = 0; i < 5; i++)
		initVBO(i);
	
	
}

void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	glUseProgram(gProgram[5]);
	glUniform3f(glGetUniformLocation(gProgram[5], "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)

		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	
}
void drawModel(int obj_index)
{
	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer[obj_index]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer[obj_index]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes[obj_index]));

	glDrawElements(GL_TRIANGLES, gFaces[obj_index].size() * 3, GL_UNSIGNED_INT, 0);
}

void shuffleCheckpoints()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 2);
	int randomNum = dis(gen);

	yellowCheckpointPos = randomNum;
}

void display()
{
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	// Calculate the camera's position relative to the bunny
	glm::vec3 yOffset = glm::vec3(0.0f, 2.0f, 0.0f);  // Y offset from the bunny's position
	glm::vec3 zOffset = glm::vec3(0.0f, 0.0f, -5.0f); // Z offset from the bunny's position

	// Assuming 'bunnyPosition' holds the bunny's position (x, y, z)
	glm::vec3 cameraPosition = glm::vec3(bunnyPosition.x, bunnyPosition.y, bunnyPosition.z) + yOffset + zOffset;

	// Calculate the point the camera should look at (bunny's direction)
	glm::vec3 lookAtPosition = glm::vec3(bunnyPosition.x, bunnyPosition.y, bunnyPosition.z - 1.0f);

	// Update the viewing matrix to position the camera relative to the bunny
	viewingMatrix = glm::lookAt(cameraPosition, lookAtPosition, glm::vec3(0, 1, 0));

	if (isNewGame) {
		if (shuffleCounter >= 60) {
			isNewGame = false;
			isPlayerMovementEnabled = true;
			gameSpeed = 0.3f;
			deltaGroundMovement = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f, -gameSpeed));
			deltaCheckpoint = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f, -checkpointSpeedMultiplier * gameSpeed));
			shuffleCounter = 0;
		
		}
		else if (shuffleCounter % 3 == 0)
			shuffleCheckpoints();
		shuffleCounter++;
	}

	if (isPlayerMovementEnabled)
	{
		if (modelingMatrixBunny[3][1] <= 0.0)
		{
			deltaBunnyJump = glm::translate<float>(glm::mat4(1.0), glm::vec3(0.0, jumpSpeed, 0.0));
		}

			
		else if (modelingMatrixBunny[3][1] >= 0.5)
		{
			deltaBunnyJump = glm::translate<float>(glm::mat4(1.0), glm::vec3(0.0, -1*jumpSpeed, 0.0));
		}
			

			translationBunny = translationBunny * deltaBunnyJump;

		if (movingDirection == -1 && translationBunny[3][0] <= 4.75)
		{
			translationBunny = translationBunny * deltaBunnyLeft;
		}
		else if (movingDirection == 1 && translationBunny[3][0] >= -4.75)
		{
			translationBunny = translationBunny * deltaBunnyRight;
		}

	}


	modelingMatrixBunny =  translationBunny * bunnyInitScale * bunnyInitTrans * bunnyInitRot;

	if (isBunnyHappy && isPlayerMovementEnabled)
	{
		bunnyHappyCounter+= gameSpeed * 10;

		modelingMatrixBunny = glm::rotate(modelingMatrixBunny, glm::radians(bunnyHappyCounter), glm::vec3(0.0f, 1.0f, 0.0f));

		if (bunnyHappyCounter >= 360.0f)
		{
			bunnyHappyCounter = 0.0f;
			isBunnyHappy = false;
		}
	}

	if (!isPlayerMovementEnabled && !isNewGame)
	{
		modelingMatrixBunny = glm::rotate(modelingMatrixBunny, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		textColor = glm::vec3(1.0, 0.0, 0.0);

	}

	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0), glm::vec3(7.6, 1.0f, 1000000.0f));
	glm::mat4 rotationMatrix = glm::rotate<float>(glm::mat4(1.0), (90. / 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
	
	modelingMatrixGround = scalingMatrix * rotationMatrix;
	modelingMatrixWithoutTranslation = modelingMatrixGround;

	translationGround = translationGround * deltaGroundMovement;

	modelingMatrixGround = translationGround * modelingMatrixGround;

	translationCheckpointLeft = translationCheckpointLeft * deltaCheckpoint;
	translationCheckpointRight = translationCheckpointRight * deltaCheckpoint;
	translationCheckpointMiddle = translationCheckpointMiddle * deltaCheckpoint;

	if (translationCheckpointLeft[3][2] <= -1.0f)
	{
		if (canCollide)
		{
			canCollide= false;
			//left collision
			if (translationBunny[3][0] >= 2.5)
			{
				switch (yellowCheckpointPos)
				{
				case 0:
					disableYellow = true;
					break;

				case 1:
					disableFirstRed = true;
					break;

				case 2:
					disableFirstRed = true;
					break;
				default:
					break;
				}

			}

			//middle collision
			if (translationBunny[3][0] <= 1.1 && translationBunny[3][0] >= -1.1)
			{
				switch (yellowCheckpointPos)
				{
				case 0:
					disableFirstRed = true;
					break;

				case 1:
					disableYellow = true;
					break;

				case 2:
					disableSecondRed = true;
					break;
				default:
					break;
				}
			}

			//right collision
			if (translationBunny[3][0] <= -2.5)
			{
				switch (yellowCheckpointPos)
				{
				case 0:
					disableSecondRed = true;
					break;

				case 1:
					disableSecondRed = true;
					break;

				case 2:
					disableYellow = true;
					break;
				default:
					break;
				}

			}
			
			if (disableYellow)
			{
				isBunnyHappy = true;
				score += 1000;
			}
			if (disableFirstRed || disableSecondRed)
			{
				gameSpeed = 0.0f;
				deltaGroundMovement = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f, -gameSpeed));
				deltaCheckpoint = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f, -checkpointSpeedMultiplier * gameSpeed));
				isPlayerMovementEnabled = false;
			}
		}
	}

	if (translationCheckpointLeft[3][2] <= -3) {
		shuffleCheckpoints();

		translationCheckpointLeft = checkpointInitLeft;
		translationCheckpointRight = checkpointInitRight;
		translationCheckpointMiddle = checkpointInitMiddle;

		gameSpeed += 0.05;
		jumpSpeed += 0.005;
		deltaGroundMovement = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f, -gameSpeed));
		deltaCheckpoint = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f, -checkpointSpeedMultiplier * gameSpeed));

		canCollide = true;

		disableYellow = false;
		disableFirstRed = false;
		disableSecondRed = false;
	}
	
	switch (yellowCheckpointPos)
	{
	case 0:
		modelingMatrixYellowCheckpoint = translationCheckpointLeft;
		modelingMatrixRedCheckpointFirst = translationCheckpointMiddle;
		modelingMatrixRedCheckpointSecond = translationCheckpointRight;
		break;
	case 1:
		modelingMatrixYellowCheckpoint = translationCheckpointMiddle;
		modelingMatrixRedCheckpointFirst = translationCheckpointLeft;
		modelingMatrixRedCheckpointSecond = translationCheckpointRight;
		break;
	case 2:
		modelingMatrixYellowCheckpoint = translationCheckpointRight;
		modelingMatrixRedCheckpointFirst = translationCheckpointLeft;
		modelingMatrixRedCheckpointSecond = translationCheckpointMiddle;
		break;
	default:
		break;
	}
	
	
	for (int i = 0; i < 5; i++)
	{
		if ((i == 2 && disableFirstRed) ||
			(i == 3 && disableSecondRed) ||
			(i == 4 && disableYellow))
		{
			continue;
		}
	
		glUseProgram(gProgram[i]);
		glUniformMatrix4fv(projectionMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(viewingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
		switch (i)
		{
		case 0:
			glUniformMatrix4fv(modelingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(modelingMatrixBunny));
			break;
		case 1:
			glUniformMatrix4fv(modelingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(modelingMatrixGround));
			glUniformMatrix4fv(modelingMatrixWithoutTranslationLoc, 1, GL_FALSE, glm::value_ptr(modelingMatrixWithoutTranslation));
			break;
		case 2:
			glUniformMatrix4fv(modelingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(modelingMatrixRedCheckpointFirst));
			break;
		case 3:
			glUniformMatrix4fv(modelingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(modelingMatrixRedCheckpointSecond));
			break;
		case 4:
			glUniformMatrix4fv(modelingMatrixLoc[i], 1, GL_FALSE, glm::value_ptr(modelingMatrixYellowCheckpoint));
			break;
		default:
			break;
		}
		
		glUniform3fv(eyePosLoc[i], 1, glm::value_ptr(eyePos));
		glBindVertexArray(vao[i]);

		drawModel(i);
	}

	
	if(bunnyJumpCounter % 6 == 0) score += gameSpeed * 25;
	scoreText = "Score: " + to_string(score);
	glBindVertexArray(vao[5]);
	renderText(scoreText, 0.0, gHeight-40.0, 1.0f, textColor);

}

void reshape(GLFWwindow* window, int w, int h)
{
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;

	gWidth = w;
	gHeight = h;

	glViewport(0, 0, w, h);
	initFonts(gWidth, gHeight);

	// Use perspective projection with 90 degrees, aspect ratio = width/height, 0.1 near and 200 far
	float fovyRad = (float)(90.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, w / (float)h, 0.1f, 200.0f);



	//camera higher in the y axis
	//TODO follow the bunny in the z axis
	viewingMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0) + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		movingDirection = -1;
	}
	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		if(movingDirection == -1)
			movingDirection = 0;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		movingDirection = 1;
	}
	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		if (movingDirection == 1)
			movingDirection = 0;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		resetGame();
	}
	//TODO Implement movement with A and D
	//TODO Implement resetting with R
}

void mainLoop(GLFWwindow* window)
{
	
	while (!glfwWindowShouldClose(window))
	{
		if (bunnyJumpCounter >= 60) bunnyJumpCounter = 0;
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
		bunnyJumpCounter++ ;
	}
}

void resetGame()
{
	bunnyJumpCounter = 0;
	bunnyHappyCounter = 0;
	translationBunny = glm::mat4(1.0);

	modelingMatrixBunny = bunnyInitScale * bunnyInitTrans * bunnyInitRot;
	gameSpeed = 0.0;
	jumpSpeed = 0.04f;
	score = 0;
	textColor = glm::vec3(1.0, 1.0, 0.0);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 2);
	int randomNum = dis(gen);

	yellowCheckpointPos = randomNum;

	translationCheckpointLeft = checkpointInitLeft;
	translationCheckpointRight = checkpointInitRight;
	translationCheckpointMiddle = checkpointInitMiddle;

	deltaBunnyJump = deltaBunnyJump = glm::translate<float>(glm::mat4(1.0), glm::vec3(0.0, jumpSpeed, 0.0));
	deltaGroundMovement = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f, -gameSpeed));
	deltaCheckpoint = glm::translate<float>(glm::mat4(1.0), glm::vec3(0, 0.0f, -checkpointSpeedMultiplier * gameSpeed));

	canCollide = true;

	disableYellow = false;
	disableFirstRed = false;
	disableSecondRed = false;

	isNewGame = true;
	isBunnyHappy = false;
}


int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
	GLFWwindow* window;
	if (!glfwInit())
	{
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	

	//int width = 1000, height = 800;
	window = glfwCreateWindow(gWidth, gHeight, "HW3", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	char rendererInfo[512] = { 0 };
	strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER)); // Use strcpy_s on Windows, strcpy on Linux
	strcat(rendererInfo, " - "); // Use strcpy_s on Windows, strcpy on Linux
	strcat(rendererInfo, (const char*)glGetString(GL_VERSION)); // Use strcpy_s on Windows, strcpy on Linux
	glfwSetWindowTitle(window, rendererInfo);

	init();

	glfwSetKeyCallback(window, keyboard);
	glfwSetWindowSizeCallback(window, reshape);
	
	
	
	reshape(window, gWidth, gHeight); // need to call this once ourselves
	mainLoop(window); // this does not return unless the window is closed

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
