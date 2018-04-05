#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <program.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


struct Vertex {

    glm::vec3 position; // Position
    glm::vec3 normal; // Normal
    glm::vec2 texCoords; // TexCoords
    glm::vec3 tangent;     // Tangent
    glm::vec3 bitangent; // Bitangent
};

struct Texture {
    GLuint id;
    string type;
    aiString path;
};

class Mesh {
public:

	bool textured;
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    GLfloat shininess;

    GLuint VAO;

    /*  Functions  */
    // Constructor
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, vector<glm::vec3>& colors, GLfloat shininess);

    // Render the mesh
    void draw(Program program);

private:
    /*  Render data  */
    GLuint VBO, EBO;

    /*  Functions    */
    // Initializes all the buffer objects/arrays
    void setupMesh();
};



