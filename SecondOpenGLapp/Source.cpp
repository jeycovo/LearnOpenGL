#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream> // Sirve para std::cout y dem�s cosas de mostrar texto en pantalla.
#include <SHADER/SHADER_H.h>
#include <camera/camera.h>

//#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

// OpenGL Mathematics 
 #include <glm/glm.hpp>
 #include <glm/gtc/matrix_transform.hpp>
 #include <glm/gtc/type_ptr.hpp>

// .:: callbacks ::.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// .:: Utility Functions ::.
unsigned int loadTexture(const char* path);
//-- Settings --
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//-- Camera -
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f; 

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{

	// .:: GLFW Config ::.
	// .: glfw: Iniciamos GLFW y realizamos una configuraci�n inicial :.
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Esto es si tienes un Mac
#ifdef __APPLE__
	glfwWindowsHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// .: glfw: Creamos la ventana :.
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Setting Callbacks (window & cursor)
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Registramos la funci�n
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// .: glad:  cargamos todos los punteros de las funciones OpenGL. :.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	// .:: Cursor ::.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// .:: Viewport ::.
	glViewport(0, 0, 800, 600);

	// .:: Shader  :.
	Shader cubeShader("C:/Users/Unreal DEP/Documents/LibraryNexus/shaders/shader.vs", "C:/Users/Unreal DEP/Documents/LibraryNexus/shaders/shader.fs");
	Shader lightShader("C:/Users/Unreal DEP/Documents/LibraryNexus/shaders/shaderLight.vs", "C:/Users/Unreal DEP/Documents/LibraryNexus/shaders/shaderLight.fs");

	float verticesCubo[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	0.0f, 0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,	0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,	0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f, 0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	0.0f, 0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	0.0f, 0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,	0.0f, 0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,	0.0f, 0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	0.0f, 0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	-1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	-1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,	1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,	1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,	1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,	1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,	0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,	0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,	0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,	0.0f,  1.0f,  0.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f ),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f ),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f ),
		glm::vec3(-1.7f,  3.0f, -7.5f ),
		glm::vec3( 1.3f, -2.0f, -2.5f ),
		glm::vec3( 1.5f,  2.0f, -2.5f ),
		glm::vec3( 1.5f,  0.2f, -1.5f ),
		glm::vec3(-1.3f,  1.0f, -1.5f )
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3( 0.7f,  0.2f,  2.0f ),
		glm::vec3( 2.3f, -3.3f, -4.0f ),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3( 0.0f,  0.0f, -3.0f )
	};

	// - VAOs & VBO - 
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	
	// .:: cubeVAO
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	// 1. Vinculamos el Vertex Array Object
	glBindVertexArray(cubeVAO);
	// 2. Copiamos nuestro array de v�rtices en un b�fer para OpenGL lo use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);
	// 3. Set our vertex attributes pointers
	// 3.1 Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 3.2 Texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 3.3 Normal Attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	// .: LightVAO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	// 1.
	glBindVertexArray(lightVAO);
	// 2. Usamos el mismo VBO que el anterior
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 3. Only position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Cualquier llamada al B�fer que hagamos se utilizar� para configurar el bufer actualmente enlazado, un Element Buffer Object (EBO)
	
	// ...........:: Texturas ::...........
	// .:: Mapa difuso (diffuse map) ::.
	unsigned int diffuseMap = loadTexture("C:/Users/Unreal DEP/Documents/LibraryNexus/resources/container2.png");
	// .:: Mapa especular ::.
	unsigned int specularMap = loadTexture("C:/Users/Unreal DEP/Documents/LibraryNexus/resources/container2_specular.png");
	// .:: Textura 2 ::.
	unsigned int texture2 = loadTexture("C:/Users/Unreal DEP/Documents/LibraryNexus/resources/awesomeface.png");
	
	// .: Le decimos a OpenGL que por cada sampler a que texture unit pertenece :.
	cubeShader.use();												// Hay que activar el shader antes de configurar uniforms
	cubeShader.setInt("material.diffuse",  0);						
	cubeShader.setInt("material.specular", 1);
	cubeShader.setInt("texture2", 2);							    // set it with shader class

	// .: Model Matrix :.
	glm::mat4 model, view, projection;

	// .:: Activamos el Depth testing ::.
	glEnable(GL_DEPTH_TEST);

	// .: Light Casters


	// .::: Loop de renderizado :::.
	while (!glfwWindowShouldClose(window))
	{
		// .:. per-frame Time Logic .:.
		// ----------------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// .:. Input .:.
		// -------------
		processInput(window);

		// .:. Render .:.
		// --------------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// .:. Shaders .:.
		// ---------------

		// .::: CUBE SHADER :::.
		cubeShader.use();
		cubeShader.setVecf3("viewPos", camera.Position);

		// .:: Light ::.
		// .: Directional light :.
		cubeShader.setFloat3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		cubeShader.setFloat3("dirLight.ambient"  , 0.05f, 0.05f, 0.1f);
		cubeShader.setFloat3("dirLight.diffuse"  , 0.2f, 0.2f, 0.7f);
		cubeShader.setFloat3("dirLight.specular" ,  0.7f,  0.7f,  0.7f);

		// .: Point Lights :. :: We can calculate each one separetly if we wan to tune each one.
		for (int i = 0; i < 4; i++)
		{
			cubeShader.setVecf3("pointLights[" + std::to_string(i) + "].position",  pointLightPositions[i]);
			cubeShader.setFloat3("pointLights[" + std::to_string(i) + "].ambient", 0.02f, 0.02f, 0.06f);
			cubeShader.setFloat3("pointLights[" + std::to_string(i) + "].diffuse", 0.2f, 0.2f, 0.6f);
			cubeShader.setFloat3("pointLighs[" + std::to_string(i) + "].specular", 0.2f, 0.2f, 0.6f);
			cubeShader.setFloat("pointLights[" + std::to_string(i) + "].constant",   1.0f);
			cubeShader.setFloat("pointLights[" + std::to_string(i) + "].linear",     0.09f);
			cubeShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic",  0.032f);
		}

		// .: Spot Light :.
		cubeShader.setVecf3("spotLight.position",    camera.Position);
		cubeShader.setVecf3("spotLight.direction",   camera.Front);
		cubeShader.setFloat3("spotLight.ambient",    0.0f, 0.0f, 0.0f);
		cubeShader.setFloat3("spotLight.diffuse",    1.0f, 1.0f, 1.0f);
		cubeShader.setFloat3("spotLight.specular",   1.0f, 1.0f, 1.0f);
		cubeShader.setFloat("spotLight.constant",    1.0f);
		cubeShader.setFloat("spotLight.linear",      0.009f);
		cubeShader.setFloat("spotLight.quadratic",   0.0032f);
		cubeShader.setFloat("spotLight.cutOff",      glm::cos(glm::radians(10.0f)));
		cubeShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.5f)));

		//------------------------------------------------
		
		// .:: Material ::.
		// .: Properties :.
		cubeShader.setFloat("material.shininess", 32.0f);

		// .: Textura :.
		// - Enlazamos las texturas
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		// bind smily texture
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//------------------------------------------------
		
		// .:: View/Projection transformations::.
		// .: View :. 
		view = camera.GetViewMatrix();
		cubeShader.setMat4("view", view);

		//.: Projection :.
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		cubeShader.setMat4("projection", projection);
		
		//.: World transformation :.
		cubeShader.setMat4("model", model);

		//------------------------------------------------

		// .: Enlazamos el VAO :.
		glBindVertexArray(cubeVAO);

		// .:Renderizamos los cubos :.
		for (unsigned int i = 0; i < 10; i++) 
		{
			// .: Model :.
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			cubeShader.setMat4("model", model);

			// .: Renderizamos el cubo
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//::::::::::::::::::::::::::::::::::::::::::::

		// .:: Light Shader ::.

		lightShader.use();
		// .:: View/Projection transformations::.
		// .: View :.
		lightShader.setMat4("view", view);
		// .: Projection :.
		lightShader.setMat4("projection", projection);

		// .: Model :.
		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightShader.setMat4("model", model);

			// .: Render :.
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// .:: glfw: intercambia b�fers y sondeo de eventos IO (teclas pulsadas/liberadas, raton moviendose, etc.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Desasignamos todos los recursos una vez han superado su proposito
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: termina, limpiando todos los recursos GLFW previamente asignados.
	glfwTerminate();
	return 0;
}

//Procesa todas las entradas: Consulta GLFW si se han pulsado/soltado teclas relevantes para reaccionar en consecuencia. 
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	
	//-------- Camera Movement --------
	const float cameraSpeed = static_cast<float>(2.5 * deltaTime); //ajustado correctamente

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//-------- Light Movement ---------
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		lightPos.x -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		lightPos.x += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		lightPos.y += cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		lightPos.y -=  cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		lightPos.z -= cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		lightPos.z += cameraSpeed;
}

// Funci�n callback: Cada vez que el tama�o de la ventana varia (por el SO o un reescalado del usuario) se ejecuta este callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// glfw:: Whenever the mouse moves, this callback is called
//-----------------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	//.:: 1. Calculate the mouse's offset since the last frame
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // Reversed since y-coordinatos go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: Siempre que la rueda del raton se haga scroll, este callback es llamado
//-----------------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// Funcion utilitaria para cargar texturas 2D desde un archivo
//---------------------------------------------------------------
unsigned int loadTexture(char const* path)
{
	// .: Creamos la textura :.
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//Cargamos la imagen que queremos acoplar a la textura
	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		// Enlazamos la textura
		glBindTexture(GL_TEXTURE_2D, textureID);
		// Generamos la textura usando los datos de la imagen previamente cargada
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		// Generamos el MipMap
		glGenerateMipmap(GL_TEXTURE_2D);

		// Establecemos las opciones de texture wrapping/filtering (en el objeto textura v�nculado)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Liberamos la memoria asignada a la imagen
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
