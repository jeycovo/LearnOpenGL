#ifndef MODEL_H
#define MODEL_H

// ---- Utilidades, glad (OpenGL), glm (math), gtc(math/matrixTransformation), stb (loading and configuring images)
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <STB/stb_image.h>

// ---- To import a model and translate it to our own structure ----
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ---- Clases propias Mesh & Shader ----
#include "Mesh.h"
#include <SHADER/SHADER_H.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char* path, const string& directory);

class Model
{
	public:
		Model(const char* path)
		{
			loadModel(path);
		}

		void Draw(Shader& shader) 
		{
			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].Draw(shader);
		};

	private:
		// model data
		vector<Mesh> meshes;
		string directory;
		vector<Texture> textures_loaded;

		void loadModel(string path) 
		{
			Assimp::Importer import;
			const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
				return;
			}
			directory = path.substr(0, path.find_last_of('/'));

processNode(scene->mRootNode, scene);
		};

		/*
			+ Estructura recursiva que se encargara de procesar recursivamente todos
			los nodos de la escena.
		*/
		void processNode(aiNode* node, const aiScene* scene)
		{
			// procesamos todas las mallas (meshes) de los nodos (si hay)
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			// Hacemos lo mismo para cada uno de sus hijos.
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		};

		/*
			+ Traducimos un objeto aiMesh (malla assimp) en un objeto de malla propio.
			+ Para ello, accedemos a cada una de las propiedades relevantes de la malla y almacenamos cada
			una de ellas en nuestro propio objeto.
			+
		*/
		Mesh processMesh(aiMesh* mesh, const aiScene* scene)
		{
			vector<Vertex> vertices;
			vector<unsigned int> indices;
			vector<Texture> textures;

			// .: Process vertex positions, normals and texture coordinates
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex;

				// Vertex Positions
				glm::vec3 vector;

				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.Position = vector;

				// Normals 
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;

				// Texture Coordinates
				if (mesh->mTextureCoords[0]) // ¿Acaso el mesh contiene coordenadas de textura?
				{
					glm::vec2 vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.TexCoords = vec;
				}
				else
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);

				vertices.push_back(vertex); //vector::push_back -> Adds a new element at the end of the vector, after its current last element.

			}
			// .: Process indices
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			// .: Process material
			if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

				vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

				vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			}

			return Mesh(vertices, indices, textures);
		};

		/*
			+ Helper that retrieve, load, and initialize the textures from the material.
			+ Do:: Iterates over all the texture locations of the given texture type, retrieves the texture's file location and then loads and
			generates the texture and stores the information in a Vertex struct.
			+ Returns:: vector of Texture structs that we store at the end of the model's vector.
		*/
		vector<Texture>	loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
		{
			vector<Texture> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				bool skip = false;
				// We check that there isn't any repeated textures (same name)
				for (unsigned int j = 0; j < textures_loaded.size(); j++)
				{
					// strcmp == 0 --> The contents of both strings are equal.
					if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
					{
						textures.push_back(textures_loaded[j]);
						skip = true;
						break;
					}
				}
				if (!skip)
				{
					Texture texture;
					texture.id = TextureFromFile(str.C_Str(), directory);
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);
					textures_loaded.push_back(texture); //Add to loaded textures.
				}
			}
			return textures;
		};
};

unsigned int TextureFromFile(const char *path, const string &directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	
	return textureID;
}
#endif MODEL_H
