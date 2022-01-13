#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;
int glWindowWidth = 1920;
int glWindowHeight = 1080;

const unsigned int SHADOW_WIDTH = 4096;
const unsigned int SHADOW_HEIGHT = 4096;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
float angleY = 0.0f;

//directional light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
glm::mat4 lightRotation;
GLfloat lightAngle;

//point light parameters
glm::vec3 lightDir_p;
glm::vec3 lightColor_p;
glm::vec3 lightPos_p;

glm::vec3 lightDir_p2;
glm::vec3 lightColor_p2;
glm::vec3 lightPos_p2;

// shader uniform locations
GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightDirLoc_p;
GLuint lightDirLoc_p2;
GLuint lightColorLoc;
GLuint lightColorLoc_p;
GLuint lightColorLoc_p2;
GLuint lightPosLoc_p;
GLuint lightPosLoc_p2;
GLuint shadowMapFBO;
GLuint depthMapTexture;
GLuint fogDensityLoc;
GLuint fogLoc;
GLint fog = 0.0f;
GLint point_light = 0.0f;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 2.0f, 70.5f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.1f;
bool firstMouse = true;
bool showDepthMap;
float lastXPos = glWindowWidth / 2.0;
float lastYPos = glWindowHeight / 2.0;
GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
gps::Model3D lightCube;
gps::Model3D sfera;
gps::Model3D screenQuad;
gps::Model3D ground;
gps::Model3D cetate;
gps::Model3D door;
gps::Model3D half_door;
gps::Model3D casa1;
gps::Model3D usa1;
gps::Model3D fereastra;
gps::Model3D pat;
gps::Model3D masa;
gps::Model3D scaun;
gps::Model3D pana;
gps::Model3D carte;
gps::Model3D lumanare;
gps::Model3D biserica;
gps::Model3D casa2;
gps::Model3D pat2;
gps::Model3D usa2;
gps::Model3D car;
gps::Model3D roti;
gps::Model3D salata;
gps::Model3D cosuri;
gps::Model3D piatra;
gps::Model3D wc;
gps::Model3D usa_wc;
gps::Model3D fantana;
gps::Model3D fantana2;
gps::Model3D cufar;
gps::Model3D capac_cufar;
gps::Model3D coins;
gps::Model3D foc;
gps::Model3D torch;
gps::Model3D stilou;
gps::Model3D cer;
gps::Model3D flacara;
gps::Model3D pasari;

// shaders
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

//skybox
std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

//amimatii
float angleDoor1 = 0.0f;
float angleDoor2 = 0.0f;
float angleDoor3 = 0.0f;
float angleDoor4 = 0.0f;
float angleChest = 0.0f;
float angleBird = 0.0f;
int night = false;
bool aprinde_lumanare = false;
bool start_animation = false;
enum PRESENTATION { A1, A2, A3, A4, A5 };
PRESENTATION animation;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
    int retina_height = myWindow.getWindowDimensions().height;
    int retina_width = myWindow.getWindowDimensions().width;
    glfwGetFramebufferSize(myWindow.getWindow(), &retina_width,
        &retina_height);
    myCustomShader.useShaderProgram();
    projection = glm::perspective(glm::radians(45.0f),
        (float)retina_width / retina_height,
        0.1f, 20.0f);
    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_V && action == GLFW_PRESS)
        showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO
    if (firstMouse)
    {
        lastXPos = xpos;
        lastYPos = ypos;
        firstMouse = false;
    }
    else
    {
        float yoffset = lastYPos - ypos;
        float xoffset = xpos - lastXPos;
        myCamera.rotate((-1) * yoffset * cameraSpeed, xoffset * cameraSpeed);
        
        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        lastXPos = xpos;
        lastYPos = ypos;
    }
}


void make_night() {
    lightColor.x = 0.0f;
    lightColor.y = 0.0f;
    lightColor.z = 0.0f;
    myCustomShader.useShaderProgram();
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void make_day() {
    lightColor.x = 1.0f;
    lightColor.y = 1.0f;
    lightColor.z = 1.0f;
    myCustomShader.useShaderProgram();
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        if (myCamera.getCameraTarget().y < -0.5f) {
            return;
        }
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

	if (pressedKeys[GLFW_KEY_S]) {
        if (myCamera.getCameraTarget().y < 0.0f) {
            return;
        }
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myCustomShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_Z]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_X]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_C]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[GLFW_KEY_J]) {
        lightAngle -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_L]) {
        lightAngle += 1.0f;
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angleY -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angleY += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_F]) {
        fog = 1;
    }
    if (pressedKeys[GLFW_KEY_G]) {
        fog = 0;
    }
    if (pressedKeys[GLFW_KEY_N]) {
        night = true;
    }
    if (pressedKeys[GLFW_KEY_M]) {
        night = false;
        make_day();
    }
    if (pressedKeys[GLFW_KEY_Y]) {

        angleDoor1 = -1.0f;
    }
    if (pressedKeys[GLFW_KEY_I]) {

        angleDoor1 = 0.0f;
    }
    if (pressedKeys[GLFW_KEY_U] && pressedKeys[GLFW_KEY_1]) {

        angleDoor2 = -1.0f;
    }
    if (pressedKeys[GLFW_KEY_U] && pressedKeys[GLFW_KEY_2]) {

        angleDoor2 = 0.0f;
    }
    if (pressedKeys[GLFW_KEY_U] && pressedKeys[GLFW_KEY_3]) {

        angleDoor3 = -1.5f;
    }
   
    if (pressedKeys[GLFW_KEY_U] && pressedKeys[GLFW_KEY_4]) {

        angleDoor3 = 0.0f;
    }

    if (pressedKeys[GLFW_KEY_U] && pressedKeys[GLFW_KEY_5]) {

        angleDoor4 = -2.0f;
    }
    if (pressedKeys[GLFW_KEY_U] && pressedKeys[GLFW_KEY_6]) {

        angleDoor4 = 0.0f;
    }
    if (pressedKeys[GLFW_KEY_R]) {

        angleChest = 1.5f;
    }
    if (pressedKeys[GLFW_KEY_T]) {

        angleChest = 0.0f;
    }
    if (pressedKeys[GLFW_KEY_O]) {
        point_light = 1;
        aprinde_lumanare = true;
    }
    if (pressedKeys[GLFW_KEY_P]) {
        point_light = 0;
        aprinde_lumanare = false;
    }
    if (pressedKeys[GLFW_KEY_ENTER]) {

        myCamera.changeCameraTarget(glm::vec3(0.0f, 0.0f, 0.0f));
        animation = A1;
        start_animation = true;
    }
    if (pressedKeys[GLFW_KEY_K]) {
        start_animation = false;
    }
}

void initOpenGLWindow() {
    myWindow.Create(glWindowWidth, glWindowHeight, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
    glEnable(GL_BLEND);
}

void initModels() {
    //teapot.LoadModel("models/teapot/teapot20segUT.obj");
    ground.LoadModel("models/ground/teren.obj");
    lightCube.LoadModel("models/cube/cube.obj");
    sfera.LoadModel("models/sfera/sfera.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    faces.push_back("skybox/right.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/bottom.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/front.tga");
    cetate.LoadModel("models/cetate/cetate.obj");
    half_door.LoadModel("models/door/half_door.obj");
    door.LoadModel("models/door/door.obj");
    casa1.LoadModel("models/casa_mica/casa1.obj");
    usa1.LoadModel("models/casa_mica/usa1.obj");
    fereastra.LoadModel("models/casa_mica/fereastra.obj");
    pat.LoadModel("models/obiecte_casa_mica/beds.obj");
    masa.LoadModel("models/obiecte_casa_mica/masa.obj");
    scaun.LoadModel("models/obiecte_casa_mica/scaun.obj");
    stilou.LoadModel("models/obiecte_casa_mica/stilou.obj");
    carte.LoadModel("models/obiecte_casa_mica/carte.obj");
    lumanare.LoadModel("models/obiecte_casa_mica/lumanare.obj");
    biserica.LoadModel("models/biserica/biserica.obj");
    casa2.LoadModel("models/casa_mare/casa_mare.obj");
    pat2.LoadModel("models/casa_mare/pat.obj");
    usa2.LoadModel("models/casa_mare/usa.obj");
    car.LoadModel("models/obiecte_scena/car.obj");
    roti.LoadModel("models/obiecte_scena/roti.obj");
    salata.LoadModel("models/obiecte_scena/salata.obj");
    cosuri.LoadModel("models/obiecte_scena/cos.obj");
    piatra.LoadModel("models/obiecte_scena/piatra.obj");
    wc.LoadModel("models/wc/wc.obj");
    usa_wc.LoadModel("models/wc/usa_wc.obj");
    fantana.LoadModel("models/obiecte_scena/fantana.obj");
    fantana2.LoadModel("models/obiecte_scena/fantana2.obj");
    cufar.LoadModel("models/cufar/cufar.obj");
    capac_cufar.LoadModel("models/cufar/capac_cufar.obj");
    coins.LoadModel("models/cufar/coins.obj");
    foc.LoadModel("models/foc/foc.obj");
    torch.LoadModel("models/torch/torch.obj");
    cer.LoadModel("models/cer/cer.obj");
    flacara.LoadModel("models/flacara/flacara.obj");
    pasari.LoadModel("models/pasari/pasare.obj");
}

void initShaders() {
	myCustomShader.loadShader(
        "shaders/shaderStart.vert",
        "shaders/shaderStart.frag");
    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    depthMapShader.useShaderProgram();
    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 300.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    /// POINT LIGHT
    lightPos_p = glm::vec3(20.0f, 10.0f, -5.5f);
    lightPosLoc_p = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos_p");
    glUniform3fv(lightPosLoc_p, 1, glm::value_ptr(glm::vec3(view * glm::vec4(lightPos_p, 1.0f))));

    //set the light direction
    lightDir_p = glm::vec3(0.0f, 1.0f, 0.0f);
    lightDirLoc_p = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir_p");
    glUniform3fv(lightDirLoc_p, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir_p));

    lightColorLoc_p = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor_p");
    glUniform3fv(lightColorLoc_p, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

    ///POINT LIGHT HOUSE
    lightPos_p2 = glm::vec3(20.0f, 10.0f, -5.5f);
    lightPosLoc_p2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos_p2");
    glUniform3fv(lightPosLoc_p2, 1, glm::value_ptr(glm::vec3(view * glm::vec4(lightPos_p2, 1.0f))));

    //set the light direction
    lightDir_p2 = glm::vec3(1.0f, 1.0f, 0.0f);
    lightDirLoc_p2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir_p2");
    glUniform3fv(lightDirLoc_p2, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir_p2));

    lightColorLoc_p2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor_p2");
    glUniform3fv(lightColorLoc_p2, 1, glm::value_ptr(glm::vec3(1.0f, 0.3f, 0.3f)));


    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));

    myCustomShader.useShaderProgram();
    fogLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fog");
    //glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    //glUniform3fv(fogLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
    glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fog"), fog);
}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    glGenFramebuffers(1, &shadowMapFBO);
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    //attach texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = 0.1f, far_plane = 55.0f;
    glm::mat4 lightProjection = glm::ortho(-55.0f, 55.0f, -55.0f, 55.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass) {

    shader.useShaderProgram();

    model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    //pamant
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    ground.Draw(shader);

    //cetate ziduri
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    cetate.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    torch.Draw(shader);

    //usa care se deschide - cetate
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.22748f, 2.7248f, 49.553f));
    model = glm::rotate(model, angleDoor1, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(2.22748f, -2.7248f, -49.553f));
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    half_door.Draw(shader);

    //usa cetate
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    door.Draw(shader);

    //prima casa
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    casa1.Draw(shader);

    //usa prima casa
    model = glm::translate(model, glm::vec3(19.72f, 1.51521f, 20.5583f));
    model = glm::rotate(model, angleDoor2, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-19.72f, -1.51521f, -20.5583f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    usa1.Draw(shader);

    //fereastra prima casa
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    fereastra.Draw(shader);

    //paturi prima casa
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    pat.Draw(shader);

    //masa
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    masa.Draw(shader);

    //obiecte prima casa
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    lumanare.Draw(shader);
    
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    stilou.Draw(shader);
    
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    scaun.Draw(shader);
    
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    carte.Draw(shader);
  
    ///biserica
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    biserica.Draw(shader);

    ///a doua casa
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    casa2.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    pat2.Draw(shader);

   
    model = glm::translate(model, glm::vec3(-6.94692f, 6.04674f, -21.5936f));
    model = glm::rotate(model, angleDoor3, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(6.94692f, -6.04674f, 21.5936f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    usa2.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    car.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    roti.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    salata.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    cosuri.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    piatra.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    wc.Draw(shader);

    model = glm::translate(model, glm::vec3(34.4235f, 1.25975f, -23.2887f));
    model = glm::rotate(model, angleDoor4, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-34.4235f, -1.25975f, 23.2887f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    usa_wc.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    fantana.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    fantana2.Draw(shader);

    ///cufar
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    cufar.Draw(shader);

    model = glm::translate(model, glm::vec3(19.7938f, 0.823914f, -6.01121f));
    model = glm::rotate(model, angleChest, glm::vec3(0.0f, 0.0f, -1.0f));
    model = glm::translate(model, glm::vec3(-19.7938f, -0.823914f, 6.01121f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    capac_cufar.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.9f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    coins.Draw(shader);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.3f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    foc.Draw(shader);

    angleBird += 0.001f;
    model = glm::rotate(model, angleBird, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    pasari.Draw(shader);

    if (aprinde_lumanare) {
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        flacara.Draw(shader);
    }

}

void renderScene() {

    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1,
        GL_FALSE,
        glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    drawObjects(depthMapShader, true);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // render depth map on screen - toggled with the M key

    if (showDepthMap) {
        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else {

        // final scene rendering pass (with shadows)

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myCustomShader.useShaderProgram();

        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        //bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
            1,
            GL_FALSE,
            glm::value_ptr(computeLightSpaceTrMatrix()));

        lightPos_p = glm::vec3(20.0f, 10.0f, -5.5f);
        glUniform3fv(lightPosLoc_p, 1, glm::value_ptr(glm::vec3(view * glm::vec4(lightPos_p, 1.0f))));

        lightPos_p2 = glm::vec3(24.93f, 0.84f, 14.12f);
        glUniform3fv(lightPosLoc_p2, 1, glm::value_ptr(glm::vec3(view * glm::vec4(lightPos_p2, 1.0f))));

    
        drawObjects(myCustomShader, false);

        //draw a white cube around the light

        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        model = lightRotation;
        model = glm::translate(model, glm::vec3(5.0f, 3.0f, 0.0f));
        model = glm::translate(model, 1.0f * lightDir);
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        lightCube.Draw(lightShader);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, 1.0f * lightPos_p);
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        sfera.Draw(lightShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, 1.0f * lightPos_p2);
        model = glm::scale(model, glm::vec3(1/40.0f, 1/40.0f, 1/40.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        sfera.Draw(lightShader);

        mySkyBox.Draw(skyboxShader, view, projection);
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
      
    }
 
    myCustomShader.useShaderProgram();
    glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "point"), point_light);

    myCustomShader.useShaderProgram();
    glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fog"), fog);

    if (night) {
        make_night();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.1f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cer.Draw(myCustomShader);
    }
    
    if (start_animation)
    {
        switch (animation)
        {

        case A1:
            myCamera.changeCameraPosition(glm::vec3(80.0f, 30.0f, 20.0f));
            //myCamera.changeCameraTarget(glm::vec3(0.0f, 0.0f, 0.0f));
            view = myCamera.getViewMatrix();
            myCustomShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            myCamera.move(gps::MOVE_FORWARD, cameraSpeed * 0.5f);
            //update view matrix
            view = myCamera.getViewMatrix();
            myCustomShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            // compute normal matrix for teapot
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            if (abs(myCamera.getCameraTarget().z) > 40.0f) {
                animation = A2;

            }
            break;
        case A2:
            myCamera.changeCameraPosition(glm::vec3(-65.0f, 21.0f, -1.0f));
            view = myCamera.getViewMatrix();
            myCustomShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeed * 0.5f);
            //update view matrix
            view = myCamera.getViewMatrix();
            myCustomShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            //std::cout << "x=" << myCamera.getCameraTarget().x << "   y=" << myCamera.getCameraTarget().y << "   z=" << myCamera.getCameraTarget().z;
            if (myCamera.getCameraTarget().z > 6.0f) {
                animation = A3;
            }
            break;
        case A3:
            myCamera.move(gps::MOVE_RIGHT, cameraSpeed * 0.5f);
            //update view matrix
            view = myCamera.getViewMatrix();
            myCustomShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            if (myCamera.getCameraTarget().x > 54.0f) {
                animation = A4;
            }
            break;
        case A4:
            myCamera.rotate(5*cameraSpeed, -3*cameraSpeed);
            //update view matrix
            view = myCamera.getViewMatrix();
            myCustomShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            animation = A5;
            break;
        case A5:
            myCamera.move(gps::MOVE_BACKWARD, cameraSpeed * 0.5f);
            //update view matrix
            view = myCamera.getViewMatrix();
            myCustomShader.useShaderProgram();
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            if (myCamera.getCameraTarget().z > 121.0f) {
                start_animation = false;
                break;
            }
            break;
        default:
            break;
        }
    }
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    setWindowCallbacks();
    initFBO();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
