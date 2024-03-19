#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>		//holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <SHADER/SHADER_H.h>

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex 
{
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	//tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
	
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture 
{
	unsigned int id;
	string type;		// Diffuse or Specular.
	string path;		// We store the path of the texture to compare with other textures, so that if we have already loaded it we don't have to do it again.
};

class Mesh 
{
	public:
		// .: Datos de la Malla (Mesh Data) :.
		vector<Vertex>		 vertices;
		vector<unsigned int> indices;
		vector<Texture>		 textures;
		unsigned int		 VAO;

		// .: Constructor :.
		Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
		{
			this->vertices = vertices;
			this->indices  = indices;
			this->textures = textures;

			// Ahora que tenemos los datos necesarios, colocamos los buferes de vertices y sus punteros a atributos.
			setupMesh();
		}

		/*
			+ Renderizamos la malla
			+ Al pasarle el shader a la malla podemos establecer varios uniforms antes de dibujar (commo linkear samplers
			  a texture units).
			+ Para solucionar el problema de no saber cuantás texturas tiene la malla, y querer linkear
			  las Texture Unit y los Shader Samplers estableceremos una cierta convención de nomenclatura:
				* Cada textura difusa se llamará texture_diffuseN.
				* Cada textura especular se llamará texture_specularN.
			  Donde N es cualquier número comprendido entre 1 y el número máximo de texture samplers permitidos.
		*/ 
		void Draw(Shader& shader)
		{
			// bind appropiate textures
			unsigned int diffuseNr	= 1;
			unsigned int specularNr = 1;
			unsigned int normalNr	= 1;
			unsigned int heightNr	= 1;

			for (unsigned int i = 0; i < textures.size(); i++) {
				glActiveTexture(GL_TEXTURE0 + i);	// active proper texture unit before binding
				// retrieve texture number (the N in diffuse_textureN)
				string number;
				string name = textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);
				else if (name == "texture_normal")
					number = std::to_string(normalNr++);
				else if (name == "texture_height")
					number = std::to_string(heightNr++);	// transfer unsigned int to string

				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
			}

			// draw mesh
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// always good practice to set everything back to defaults once configured.
			glActiveTexture(GL_TEXTURE0);
		}

	private:
		// render data
		unsigned int VBO, EBO;

		// initializes all the buffer objects/arrays
		/*
			+ Inicializamos los Búferes.
			+ Especificamos la distribución del Vertex Shader mediante punteros de atributos de vértices.
		*/
		void setupMesh() {
			// create buffers/arrays
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			// load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// A great thing about structs is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// set the vertex attribute pointers
			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
			// vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
			// vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
			// IDs
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_INT  , GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
			// Weights
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

			glBindVertexArray(0);
		}
};
#endif MESH_H