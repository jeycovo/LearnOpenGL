#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream> // Sirve para std::cout y demás cosas de mostrar texto en pantalla.
#include <SHADER/SHADER_H.h>

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

//-- Camera --
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw	= -90.0f;
float pitch	= 0.0f;
float lastX	= 800.0f / 2.0;
float lastY	= 600.0 / 2.0;
float fov	= 45.0f;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f; 

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{

	// .:: GLFW Config ::.
	// .: glfw: Iniciamos GLFW y realizamos una configuración inicial :.
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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Registramos la función
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
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// - VAOs & VBO - 
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	
	// .:: cubeVAO
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	// 1. Vinculamos el Vertex Array Object
	glBindVertexArray(cubeVAO);
	// 2. Copiamos nuestro array de vértices en un búfer para OpenGL lo use
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

	// Cualquier llamada al Búfer que hagamos se utilizará para configurar el bufer actualmente enlazado, un Element Buffer Object (EBO)
	
	// ...........:: Textura ::...........
	// .:: Mapa difuso (diffuse map) ::.
	unsigned int diffuseMap = loadTexture("C:/Users/Unreal DEP/Documents/LibraryNexus/resources/container2.png");
	// .:: Mapa especular ::.
	unsigned int specularMap = loadTexture("C:/Users/Unreal DEP/Documents/LibraryNexus/resources/container2_specular.png");
	
	// .:: Textura 2 ::.
	unsigned int texture2 = loadTexture("C:/Users/Unreal DEP/Documents/LibraryNexus/resources/awesomeface.png");
	//....................................

	
	// .: Le decimos a OpenGL que por cada sampler a que texture unit pertenece :.
	cubeShader.use();												// Hay que activar el shader antes de configurar uniforms
	cubeShader.setInt("material.diffuse",  0);						
	cubeShader.setInt("material.specular", 1);
	cubeShader.setInt("texture2", 2);							    // set it with shader class

	// .: Model Matrix :.
	glm::mat4 model, view, projection;

	// .:: Activamos el Depth testing ::.
	glEnable(GL_DEPTH_TEST);

	// .::: Loop de renderizado :::.
	while (!glfwWindowShouldClose(window))
	{
		// .:. per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// .:. input
		processInput(window);

		// .:. comandos de renderizado aquí. .:.
		// .:: ColorBuffer  ::.
		// Limpiamos el búfer de color (colorbuffer)
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// .:. Textura .:.
		// - Enlazamos las texturas
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		

		// .:. Shaders .:.
		// .:: Cube Shader ::.
		cubeShader.use();

		// .: Light :.
		// . Change color by time .
		/*
		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);
		*/
		glm::vec3 diffuseColor = glm::vec3(0.5f);
		glm::vec3 ambientColor = glm::vec3(0.2f);

		cubeShader.setFloat3("light.position", lightPos.x, lightPos.y, lightPos.z);
		cubeShader.setFloat3("light.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
		cubeShader.setFloat3("light.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
		cubeShader.setFloat3("light.specular", 1.0f, 1.0f, 1.0f);
		
		

		// .:: Material Color ::.
		cubeShader.setFloat3("material.specular", 0.5f, 0.5f, 0.5f);
		cubeShader.setFloat("material.shininess", 32.0f);

		// .:: Model - View - Projection ::.
		// .: Model :.
		model = glm::mat4(1.0f);
		cubeShader.setMat4("model", model);
		// .: View :. 
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		cubeShader.setMat4("view", view);
		//.: Projection :.
		projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		cubeShader.setMat4("projection", projection);
		//.: Camera position vector :.
		cubeShader.setFloat3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
		// .: Renderizamos el cubo
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// .: Light Shader :.
		lightShader.use();
		// .: Light :.
		// .: Model :.
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightShader.setMat4("model", model);
		// .: View :.
		lightShader.setMat4("view", view);
		// .: Projection :.
		lightShader.setMat4("projection", projection);
		// .: Render :.
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// .:: glfw: intercambia búfers y sondeo de eventos IO (teclas pulsadas/liberadas, raton moviendose, etc.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Desasignamos todos los recursos una vez han superado su proposito
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	
	//glDeleteBuffers(1, &EBO);

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
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

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

// Función callback: Cada vez que el tamaño de la ventana varia (por el SO o un reescalado del usuario) se ejecuta este callback
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

	// .: 1.5 sensivity
	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	// .:: 2. Add the offseet values to the camera's yaw and pitch values
	yaw   += xoffset;
	pitch += yoffset;

	// .:: 3. Add some constraints to the minimum/maximum pitch values
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;


	// .:: 4. Calculate the direction vector.
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}


// glfw: Siempre que la rueda del raton se haga scroll, este callback es llamado
//-----------------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	fov -= (float)yoffset;
	if (fov < 1.0f) fov = 1.0f;
	if (fov > 45.0f) fov = 45.0f;
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

		// Establecemos las opciones de texture wrapping/filtering (en el objeto textura vínculado)
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
