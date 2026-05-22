// ============================================================
// PROYECTO: Escena Minecraft con OpenGL (VERSI�N FINAL CORREGIDA)
// Renderiza una escena 3D con personajes, animaciones,
// iluminaci�n din�mica y texturas UV mapeadas.
// ============================================================

#include <iostream>
#include <cmath>
#include <vector>

#include <GL/glew.h>               
#include <GLFW/glfw3.h>            
#include "stb_image.h"             
#include <glm/glm.hpp>             
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>    
#include "SOIL2/SOIL2.h"           
#include "Shader.h"                
#include "Camera.h"                
#include "Model.h"                 

// ============================================================
// PROTOTIPOS DE FUNCIONES
// ============================================================
void TeclaCallback(GLFWwindow* ventana, int tecla, int scancode, int accion, int modo);
void MouseCallback(GLFWwindow* ventana, double posX, double posY);
void Movimiento();
void Animacion();
void generarEsfera(float radio, int sectores, int pilas,
    std::vector<float>& vertices,
    std::vector<unsigned int>& indices);
void crearBloqueMinecraft(GLuint& VAO, GLuint& VBO, float tx, float ty, float w, float h, float d, float texWidth = 64.0f, float texHeight = 32.0f);

// ============================================================
// CONFIGURACI�N DE VENTANA
// ============================================================
const GLuint ANCHO = 1920, ALTO = 1080;
int anchoPantalla, altoPantalla;

// ============================================================
// C�MARA
// ============================================================
Camera camara(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat ultimoX = ANCHO / 2.0f;
GLfloat ultimoY = ALTO / 2.0f;
bool teclas[1024];
bool primerMouse = true;

// ============================================================
// VARIABLES DE ILUMINACI�N Y POSICIONES
// ============================================================
glm::vec3 posLuz(0.0f, 0.0f, 0.0f);
bool luzActiva;
glm::vec3 colorLuz1 = glm::vec3(0);

glm::vec3 centroCirculo(4.3f, -16.0f, -11.0f);

// Posiciones de las 4 antorchas alrededor del centro (coliseo/plaza)
// NOTA: Se inicializan en main() para garantizar que centroCirculo ya este listo
glm::vec3 posicionesAntorchas[16];

// Luces puntuales asignadas a las antorchas
// NOTA: Se inicializan en main() junto con posicionesAntorchas
glm::vec3 posicionesLuzPuntual[16];

// ============================================================
// GEOMETR�A: V�RTICES DEL CUBO UNITARIO
// ============================================================
float verticesCubo[] = {
    -0.5f,-0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  0.0f,0.0f,
     0.5f,-0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  1.0f,0.0f,
     0.5f, 0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  1.0f,1.0f,
     0.5f, 0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  1.0f,1.0f,
    -0.5f, 0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  0.0f,1.0f,
    -0.5f,-0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  0.0f,0.0f,
    -0.5f,-0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f,0.0f,
     0.5f,-0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f,0.0f,
     0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f,1.0f,
     0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f,1.0f,
    -0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f,1.0f,
    -0.5f,-0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f,0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,  1.0f,0.0f,
    -0.5f, 0.5f,-0.5f, -1.0f, 0.0f, 0.0f,  1.0f,1.0f,
    -0.5f,-0.5f,-0.5f, -1.0f, 0.0f, 0.0f,  0.0f,1.0f,
    -0.5f,-0.5f,-0.5f, -1.0f, 0.0f, 0.0f,  0.0f,1.0f,
    -0.5f,-0.5f, 0.5f, -1.0f, 0.0f, 0.0f,  0.0f,0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,  1.0f,0.0f,
     0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  1.0f,0.0f,
     0.5f, 0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  1.0f,1.0f,
     0.5f,-0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  0.0f,1.0f,
     0.5f,-0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  0.0f,1.0f,
     0.5f,-0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0f,0.0f,
     0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  1.0f,0.0f,
     -0.5f,-0.5f,-0.5f,  0.0f,-1.0f, 0.0f,  0.0f,1.0f,
      0.5f,-0.5f,-0.5f,  0.0f,-1.0f, 0.0f,  1.0f,1.0f,
      0.5f,-0.5f, 0.5f,  0.0f,-1.0f, 0.0f,  1.0f,0.0f,
      0.5f,-0.5f, 0.5f,  0.0f,-1.0f, 0.0f,  1.0f,0.0f,
     -0.5f,-0.5f, 0.5f,  0.0f,-1.0f, 0.0f,  0.0f,0.0f,
     -0.5f,-0.5f,-0.5f,  0.0f,-1.0f, 0.0f,  0.0f,1.0f,
     -0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  0.0f,1.0f,
      0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  1.0f,1.0f,
      0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  1.0f,0.0f,
      0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  1.0f,0.0f,
     -0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  0.0f,0.0f,
     -0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  0.0f,1.0f
};

// ============================================================
// VARIABLES DE ANIMACI�N
// ============================================================
float rotacionsol = 90.0f; // Ahora arranca de d�a
bool animsol = false;
float brazoSaludo = 0.0f;
bool  dirSaludo = false;
bool animCaballo = false;
float patasDelanteras = 0.0f;
float patasTraseras = 0.0f;
float colaCaballo = 0.0f;
bool  pasoCaballo = false;
bool animHojas = false;
float oscHojas = 0.0f;
bool dirHojas = false;
float brazoSaludoIzq = 20.0f;
float brazoPersonajestiendaDer = -60.0f;
float brazoPersonajestiendaIzq = -60.0f;
bool  dirPersonajestienda = false;
bool animPersonajes = false;
float rotacionTiendas[9] = { 0.0f };
float radioTiendas = 24.0f;
float radioCajas = 27.0f;
float radioPersonajes = 24.0f;
float brazoReyDer = 90.0f;
float brazoReyIzq = -60.0f;
bool dirRey = false;
bool animReyes = false;
float brazoReyEscudo = 0.0f;
bool  dirReyEscudo = false;
float faseAtaque = 0.0f;
float anguloTapa = 0.0f;
bool  dirTapa = false;

// Variables para la Catapulta
float anguloCatapulta = -45.0f;
bool  catapultaDisparando = true;
float distPiedra = 0.0f;
float altPiedra = 0.0f;
bool piedraVolando = false;

// Variables Persianas/Toldos
float anguloToldo = 0.0f;

// ============================================================
// POSICIONES DE OBJETOS EN LA ESCENA
// ============================================================
glm::vec3 posCaballo(2.0f, -16.0f, 140.0f);
glm::vec3 posPersonaje7 = posCaballo + glm::vec3(0.0f, 1.1f, 0.0f);

glm::vec3 posicionesArbolesVerdes[2][6] = {
    { glm::vec3(-1.6f,  12.0f, -180.0f), glm::vec3(-1.6f,   8.0f, -160.0f), glm::vec3(-1.6f,   5.0f, -140.0f), glm::vec3(-1.6f,   1.0f, -120.0f), glm::vec3(-1.6f,  -2.5f, -100.0f), glm::vec3(-1.6f,  -6.0f,  -80.0f) },
    { glm::vec3(9.6f, 12.0f, -180.0f), glm::vec3(9.6f,  8.0f, -160.0f), glm::vec3(9.6f,  5.0f, -140.0f), glm::vec3(9.6f, -1.5f, -120.0f), glm::vec3(9.6f, -2.5f, -100.0f), glm::vec3(9.6f, -6.0f,  -80.0f) }
};

glm::vec3 posicionesArbolesRosa[2][6] = {
    { glm::vec3(9.6f,  9.0f, -190.0f), glm::vec3(9.6f,  5.5f, -170.0f), glm::vec3(9.6f,  2.0f, -150.0f), glm::vec3(9.6f, -1.5f, -130.0f), glm::vec3(9.6f, -5.0f, -110.0f), glm::vec3(9.6f, -8.5f,  -90.0f) },
    { glm::vec3(-1.6f,  9.0f, -190.0f), glm::vec3(-1.6f,  5.5f, -170.0f), glm::vec3(-1.6f,  2.0f, -150.0f), glm::vec3(-1.6f, -1.5f, -130.0f), glm::vec3(-1.6f, -5.0f, -110.0f), glm::vec3(-1.6f, -8.5f,  -90.0f) }
};

glm::vec3 posPersonajes[9];
glm::vec3 posicionesM[9];
glm::vec3 posicionesS[9];
glm::vec3 posicionesU[9];
glm::vec3 desplazamientoCaja3[] = {
    glm::vec3(-0.14f, 0.0f,  0.26f), glm::vec3(0.00f, 0.0f,  0.26f), glm::vec3(0.14f, 0.0f,  0.26f),
    glm::vec3(-0.07f, 0.0f,  0.13f), glm::vec3(0.07f, 0.0f,  0.13f), glm::vec3(0.00f, 0.0f,  0.00f),
};

GLfloat tiempoActual = 0.0f;
GLfloat tiempoAnterior = 0.0f;
GLfloat deltaTime = 0.0f;

void generarEsfera(float radio, int sectores, int pilas, std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
    vertices.clear(); indices.clear();
    float pasoPorSector = 2.0f * glm::pi<float>() / sectores;
    float pasoPorPila = glm::pi<float>() / pilas;
    for (int i = 0; i <= pilas; ++i) {
        float anguloPila = glm::pi<float>() / 2.0f - i * pasoPorPila;
        float xy = radio * cosf(anguloPila);
        float z = radio * sinf(anguloPila);
        for (int j = 0; j <= sectores; ++j) {
            float anguloSector = j * pasoPorSector;
            float x = xy * cosf(anguloSector);
            float y = xy * sinf(anguloSector);
            vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);
            vertices.push_back(x / radio); vertices.push_back(y / radio); vertices.push_back(z / radio);
            vertices.push_back((float)j / sectores); vertices.push_back((float)i / pilas);
        }
    }
    for (int i = 0; i < pilas; ++i) {
        int k1 = i * (sectores + 1);
        int k2 = k1 + sectores + 1;
        for (int j = 0; j < sectores; ++j, ++k1, ++k2) {
            if (i != 0) { indices.push_back(k1); indices.push_back(k2); indices.push_back(k1 + 1); }
            if (i != pilas - 1) { indices.push_back(k1 + 1); indices.push_back(k2); indices.push_back(k2 + 1); }
        }
    }
}

int main()
{
    glfwInit();
    GLFWwindow* ventana = glfwCreateWindow(ANCHO, ALTO, "Escena Minecraft", nullptr, nullptr);
    glfwMakeContextCurrent(ventana);
    glfwGetFramebufferSize(ventana, &anchoPantalla, &altoPantalla);
    glfwSetKeyCallback(ventana, TeclaCallback);
    glfwSetCursorPosCallback(ventana, MouseCallback);

    glewExperimental = GL_TRUE;
    glewInit();
    glViewport(0, 0, anchoPantalla, altoPantalla);

    Shader shaderIluminacion("Shader/lighting.vs", "Shader/lighting.frag");
    Shader shaderLampara("Shader/lamp.vs", "Shader/lamp.frag");

    Model Escenario((char*)"Models/escena.obj");
    Model EspadaModel((char*)"Models/espada.obj");
    Model CarritoModel((char*)"Models/carrito.obj");
    // Agregamos el modelo de la antorcha
    Model AntorchaModel((char*)"Models/Torch.obj");

    GLuint VAOCubo, VBOCubo;
    glGenVertexArrays(1, &VAOCubo);
    glGenBuffers(1, &VBOCubo);
    glBindVertexArray(VAOCubo);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCubo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    GLuint VAOCabeza, VBOCabeza, VAOTorso, VBOTorso, VAOBrazo, VBOBrazo, VAOPierna, VBOPierna;
    crearBloqueMinecraft(VAOCabeza, VBOCabeza, 0, 0, 8, 8, 8, 64.0f, 32.0f);
    crearBloqueMinecraft(VAOTorso, VBOTorso, 16, 16, 8, 12, 4, 64.0f, 32.0f);
    crearBloqueMinecraft(VAOBrazo, VBOBrazo, 40, 16, 4, 12, 4, 64.0f, 32.0f);
    crearBloqueMinecraft(VAOPierna, VBOPierna, 0, 16, 4, 12, 4, 64.0f, 32.0f);

    GLuint VAOCabezaCaballo, VBOCabezaCaballo, VAOHocicoCaballo, VBOHocicoCaballo;
    GLuint VAOCuelloCaballo, VBOCuelloCaballo, VAOTorsoCaballo, VBOTorsoCaballo;
    GLuint VAOPataCaballo, VBOPataCaballo, VAOColaCaballo, VBOColaCaballo;
    crearBloqueMinecraft(VAOCabezaCaballo, VBOCabezaCaballo, 0, 13, 6, 5, 6, 128.0f, 128.0f);
    crearBloqueMinecraft(VAOHocicoCaballo, VBOHocicoCaballo, 0, 25, 4, 5, 4, 128.0f, 128.0f);
    crearBloqueMinecraft(VAOCuelloCaballo, VBOCuelloCaballo, 0, 35, 4, 12, 4, 128.0f, 128.0f);
    crearBloqueMinecraft(VAOTorsoCaballo, VBOTorsoCaballo, 0, 58, 14, 10, 22, 128.0f, 128.0f);
    crearBloqueMinecraft(VAOPataCaballo, VBOPataCaballo, 96, 29, 4, 11, 4, 128.0f, 128.0f);
    crearBloqueMinecraft(VAOColaCaballo, VBOColaCaballo, 42, 36, 3, 14, 3, 128.0f, 128.0f);

    int texW, texH, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    GLuint texturaCaballerito;
    glGenTextures(1, &texturaCaballerito);
    glBindTexture(GL_TEXTURE_2D, texturaCaballerito);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unsigned char* data = stbi_load("images/caballerito.png", &texW, &texH, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texW, texH, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    GLuint texturaGriego;
    glGenTextures(1, &texturaGriego);
    glBindTexture(GL_TEXTURE_2D, texturaGriego);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unsigned char* dataGriego = stbi_load("images/griego.png", &texW, &texH, &nrChannels, 0);
    if (dataGriego) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texW, texH, 0, format, GL_UNSIGNED_BYTE, dataGriego);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(dataGriego);

    GLuint texturaRey1;
    glGenTextures(1, &texturaRey1);
    glBindTexture(GL_TEXTURE_2D, texturaRey1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unsigned char* dataRey1 = stbi_load("images/rey1.png", &texW, &texH, &nrChannels, 0);
    if (dataRey1) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texW, texH, 0, format, GL_UNSIGNED_BYTE, dataRey1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(dataRey1);

    GLuint texturaRey2;
    glGenTextures(1, &texturaRey2);
    glBindTexture(GL_TEXTURE_2D, texturaRey2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unsigned char* dataRey2 = stbi_load("images/rey2.png", &texW, &texH, &nrChannels, 0);
    if (dataRey2) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texW, texH, 0, format, GL_UNSIGNED_BYTE, dataRey2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(dataRey2);

    GLuint texturaCaballo;
    glGenTextures(1, &texturaCaballo);
    glBindTexture(GL_TEXTURE_2D, texturaCaballo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unsigned char* dataCaballo = stbi_load("images/caballo.png", &texW, &texH, &nrChannels, 0);
    if (dataCaballo) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texW, texH, 0, format, GL_UNSIGNED_BYTE, dataCaballo);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(dataCaballo);

    GLuint texturaEscudo;
    glGenTextures(1, &texturaEscudo);
    glBindTexture(GL_TEXTURE_2D, texturaEscudo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unsigned char* dataEscudo = stbi_load("images/textura_escudo.png", &texW, &texH, &nrChannels, 0);
    if (dataEscudo) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, texW, texH, 0, format, GL_UNSIGNED_BYTE, dataEscudo);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(dataEscudo);

    std::vector<float> verticesEsfera;
    std::vector<unsigned int> indicesEsfera;
    generarEsfera(0.25f, 36, 18, verticesEsfera, indicesEsfera);

    GLuint VAOEsfera, VBOEsfera, EBOEsfera;
    glGenVertexArrays(1, &VAOEsfera);
    glGenBuffers(1, &VBOEsfera);
    glGenBuffers(1, &EBOEsfera);
    glBindVertexArray(VAOEsfera);
    glBindBuffer(GL_ARRAY_BUFFER, VBOEsfera);
    glBufferData(GL_ARRAY_BUFFER, verticesEsfera.size() * sizeof(float), verticesEsfera.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOEsfera);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesEsfera.size() * sizeof(unsigned int), indicesEsfera.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    glm::mat4 proyeccion = glm::perspective(camara.GetZoom(), (float)anchoPantalla / (float)altoPantalla, 0.1f, 10000.0f);

    // ============================================================
    // INICIALIZAR POSICIONES DE ANTORCHAS Y SUS LUCES PUNTUALES
    // Se hace aqui dentro de main() para garantizar que centroCirculo
    // ya este correctamente inicializado (evita bug de orden estatico)
    // ============================================================
    posicionesAntorchas[0] = centroCirculo + glm::vec3(30.0f, 0.0f, 0.0f);
    posicionesAntorchas[1] = centroCirculo + glm::vec3(-30.0f, 0.0f, 0.0f);
    posicionesAntorchas[2] = centroCirculo + glm::vec3(0.0f, 0.0f, 30.0f);
    posicionesAntorchas[3] = centroCirculo + glm::vec3(0.0f, 0.0f, -30.0f);

    // Antorchas en la calle/colina (indices 4-15, 6 pares a lo largo de Z=-80 a -190)
    // Lado izquierdo X=-3.5, lado derecho X=12.7, Y interpolada segun la pendiente
    float calleZ[6] = { -85.0f, -105.0f, -125.0f, -145.0f, -165.0f, -185.0f };
    float calleY[6] = { -7.5f,   -4.0f,   -0.5f,    3.0f,    6.5f,   10.0f };
    for (int k = 0; k < 6; k++) {
        posicionesAntorchas[4 + k * 2] = glm::vec3(-3.5f, calleY[k], calleZ[k]);
        posicionesAntorchas[4 + k * 2 + 1] = glm::vec3(12.7f, calleY[k], calleZ[k]);
    }

    posicionesLuzPuntual[0] = posicionesAntorchas[0] + glm::vec3(0.0f, 2.5f, 0.0f);
    posicionesLuzPuntual[1] = posicionesAntorchas[1] + glm::vec3(0.0f, 2.5f, 0.0f);
    posicionesLuzPuntual[2] = posicionesAntorchas[2] + glm::vec3(0.0f, 2.5f, 0.0f);
    posicionesLuzPuntual[3] = posicionesAntorchas[3] + glm::vec3(0.0f, 2.5f, 0.0f);
    for (int k = 4; k < 16; k++)
        posicionesLuzPuntual[k] = posicionesAntorchas[k] + glm::vec3(0.0f, 2.5f, 0.0f);

    for (int t = 0; t < 9; t++) {
        float angulo = glm::radians(t * (360.0f / 9.0f));
        float cosA = cos(angulo); float sinA = sin(angulo);
        glm::vec3 centro(centroCirculo.x + radioCajas * cosA, centroCirculo.y, centroCirculo.z + radioCajas * sinA);
        posicionesM[t] = centro + glm::vec3(-sinA * 1.5f, 0.0f, cosA * 1.5f);
        posicionesS[t] = centro;
        posicionesU[t] = centro + glm::vec3(sinA * 1.5f, 0.0f, -cosA * 1.5f);
    }

    for (int t = 0; t < 9; t++) {
        float angulo = glm::radians(t * (360.0f / 9.0f));
        posPersonajes[t] = glm::vec3(centroCirculo.x + radioPersonajes * cos(angulo), centroCirculo.y, centroCirculo.z + radioPersonajes * sin(angulo));
    }

    while (!glfwWindowShouldClose(ventana))
    {
        tiempoActual = glfwGetTime();
        deltaTime = tiempoActual - tiempoAnterior;
        tiempoAnterior = tiempoActual;

        glfwPollEvents();
        Movimiento();
        Animacion();

        glm::vec3 posSolActual(
            0.0f,
            270.0f * sin(glm::radians(rotacionsol)),
            270.0f * cos(glm::radians(rotacionsol))
        );

        float alturaNormalizada = posSolActual.y / 270.0f;

        glm::vec3 colorNocheBase(0.12f, 0.15f, 0.30f);
        glm::vec3 colorDiaBase(1.0f, 1.0f, 0.85f);
        glm::vec3 colorAtardecerBase(1.0f, 0.45f, 0.10f);

        glm::vec3 colorCielo;

        if (alturaNormalizada > 0.6f) {
            colorCielo = colorDiaBase;
        }
        else if (alturaNormalizada > 0.2f) {
            float t = (alturaNormalizada - 0.2f) / 0.4f;
            colorCielo = glm::mix(colorAtardecerBase, colorDiaBase, t);
        }
        else if (alturaNormalizada > -0.1f) {
            float t = (alturaNormalizada + 0.1f) / 0.3f;
            colorCielo = glm::mix(colorNocheBase * 0.5f + colorAtardecerBase * 0.5f, colorAtardecerBase, t);
            if (abs(alturaNormalizada) < 0.15f) {
                float picoT = 1.0f - (abs(alturaNormalizada) / 0.15f);
                colorCielo = glm::mix(colorCielo, colorAtardecerBase, picoT * 0.8f);
            }
        }
        else {
            colorCielo = colorNocheBase;
        }

        glClearColor(colorCielo.r, colorCielo.g, colorCielo.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        shaderIluminacion.Use();

        glm::mat4 vista = camara.GetViewMatrix();
        GLint locModelo = glGetUniformLocation(shaderIluminacion.Program, "model");
        GLint locVista = glGetUniformLocation(shaderIluminacion.Program, "view");
        GLint locProyeccion = glGetUniformLocation(shaderIluminacion.Program, "projection");

        glUniformMatrix4fv(locVista, 1, GL_FALSE, glm::value_ptr(vista));
        glUniformMatrix4fv(locProyeccion, 1, GL_FALSE, glm::value_ptr(proyeccion));
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "viewPos"), camara.GetPosition().x, camara.GetPosition().y, camara.GetPosition().z);

        float factorDia = (alturaNormalizada > 0.0f) ? alturaNormalizada : 0.0f;
        float intAmbiente = 0.45f + factorDia * 0.45f;
        float intDifusa = 0.1f + factorDia * 0.9f;

        glm::vec3 dirLuz = glm::normalize(-posSolActual);
        if (posSolActual.y < -10.0f) {
            dirLuz = glm::vec3(0.0f, 1.0f, 0.3f);
        }

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "dirLight.direction"), dirLuz.x, dirLuz.y, dirLuz.z);

        glm::vec3 colorAmbienteFinal = colorCielo * intAmbiente;
        colorAmbienteFinal.r = std::max(colorAmbienteFinal.r, 0.35f);
        colorAmbienteFinal.g = std::max(colorAmbienteFinal.g, 0.38f);
        colorAmbienteFinal.b = std::max(colorAmbienteFinal.b, 0.50f);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "dirLight.ambient"), colorAmbienteFinal.r, colorAmbienteFinal.g, colorAmbienteFinal.b);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "dirLight.diffuse"), colorCielo.r * intDifusa, colorCielo.g * intDifusa, colorCielo.b * intDifusa);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "dirLight.specular"), intDifusa * 0.5f, intDifusa * 0.5f, intDifusa * 0.5f);

        // ============================================================
        // CONTROL DE LUCES DE ANTORCHAS SEG�N EL CICLO D�A/NOCHE
        // ============================================================
        bool esDeNoche = (alturaNormalizada <= 0.0f);
        float intensidadAntorcha = esDeNoche ? 1.0f : 0.0f;

        for (int i = 0; i < 16; i++) {
            std::string base = "pointLights[" + std::to_string(i) + "].";
            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, (base + "position").c_str()), posicionesLuzPuntual[i].x, posicionesLuzPuntual[i].y, posicionesLuzPuntual[i].z);
            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, (base + "ambient").c_str()), 0.05f * intensidadAntorcha, 0.03f * intensidadAntorcha, 0.0f);
            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, (base + "diffuse").c_str()), 0.8f * intensidadAntorcha, 0.45f * intensidadAntorcha, 0.1f * intensidadAntorcha);
            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, (base + "specular").c_str()), 0.2f * intensidadAntorcha, 0.1f * intensidadAntorcha, 0.0f);
            glUniform1f(glGetUniformLocation(shaderIluminacion.Program, (base + "constant").c_str()), 1.0f);
            glUniform1f(glGetUniformLocation(shaderIluminacion.Program, (base + "linear").c_str()), 0.15f);
            glUniform1f(glGetUniformLocation(shaderIluminacion.Program, (base + "quadratic").c_str()), 0.032f);
        }

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.position"), camara.GetPosition().x, camara.GetPosition().y, camara.GetPosition().z);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.direction"), camara.GetFront().x, camara.GetFront().y, camara.GetFront().z);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.linear"), 0.3f);
        glUniform1f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.quadratic"), 0.7f);
        glUniform1f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
        glUniform1f(glGetUniformLocation(shaderIluminacion.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

        glUniform1f(glGetUniformLocation(shaderIluminacion.Program, "material.shininess"), 16.0f);

        glm::mat4 modelo(1);

        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);
        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "transparency"), 0);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);
        Escenario.Draw(shaderIluminacion);

        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 0);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);

        float characterScale = 0.28f;

        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "transparency"), 0);
        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturaGriego);

        for (int i = 0; i < 9; i++)
        {
            int tipoFruta = i % 3;
            float anguloP = glm::radians(i * (360.0f / 9.0f));

            glm::mat4 basePersonaje = glm::mat4(1.0f);
            basePersonaje = glm::translate(basePersonaje, posPersonajes[i] + glm::vec3(0.0f, 2.67f * characterScale, 0.0f));
            basePersonaje = glm::rotate(basePersonaje, -anguloP + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);

            glm::mat4 modeloTorso = basePersonaje;
            modelo = glm::scale(modeloTorso, glm::vec3(1.8f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOTorso); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 pivoteCabeza = glm::translate(modeloTorso, glm::vec3(0.0f, 1.335f * characterScale, 0.0f));
            modelo = glm::translate(pivoteCabeza, glm::vec3(0.0f, 0.9f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(1.8f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCabeza); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 pivoteBrazoIzq = glm::translate(modeloTorso, glm::vec3(-0.9f * characterScale, 1.335f * characterScale, 0.0f));
            pivoteBrazoIzq = glm::rotate(pivoteBrazoIzq, glm::radians(brazoPersonajestiendaIzq), glm::vec3(1.0f, 0.0f, 0.0f));
            modelo = glm::translate(pivoteBrazoIzq, glm::vec3(-0.45f * characterScale, -1.335f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOBrazo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 pivoteBrazoDer = glm::translate(modeloTorso, glm::vec3(0.9f * characterScale, 1.335f * characterScale, 0.0f));
            pivoteBrazoDer = glm::rotate(pivoteBrazoDer, glm::radians(brazoPersonajestiendaDer), glm::vec3(1.0f, 0.0f, 0.0f));
            modelo = glm::translate(pivoteBrazoDer, glm::vec3(0.45f * characterScale, -1.335f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOBrazo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 0);
            glm::mat4 posMano = glm::translate(pivoteBrazoDer, glm::vec3(0.45f * characterScale, -2.67f * characterScale, 0.0f));

            if (tipoFruta == 0) {
                glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.85f, 0.07f, 0.07f);
                modelo = glm::scale(posMano, glm::vec3(0.6f));
                glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
                glBindVertexArray(VAOEsfera); glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);
            }
            else if (tipoFruta == 1) {
                glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.13f, 0.55f, 0.13f);
                modelo = glm::scale(posMano, glm::vec3(1.0f, 0.7f, 0.7f));
                glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
                glBindVertexArray(VAOEsfera); glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);
            }
            else {
                glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.45f, 0.10f, 0.55f);
                modelo = glm::scale(posMano, glm::vec3(0.6f));
                glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
                glBindVertexArray(VAOEsfera); glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);
            }

            glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);
            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);

            glm::mat4 pivotePiernaDer = glm::translate(modeloTorso, glm::vec3(0.45f * characterScale, -1.335f * characterScale, 0.0f));
            modelo = glm::translate(pivotePiernaDer, glm::vec3(0.0f, -1.335f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOPierna); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 pivPiernaIzq = glm::translate(modeloTorso, glm::vec3(-0.45f * characterScale, -1.335f * characterScale, 0.0f));
            modelo = glm::translate(pivPiernaIzq, glm::vec3(0.0f, -1.335f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOPierna); glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturaCaballo);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);

        glm::mat4 modelTempCaballo;
        modelo = glm::mat4(1);
        modelTempCaballo = modelo = glm::translate(modelo, posCaballo + glm::vec3(0.0f, 0.90f, 0.0f));
        modelTempCaballo = modelo = glm::rotate(modelo, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(glm::scale(modelo, glm::vec3(0.70f, 0.60f, 1.40f))));
        glBindVertexArray(VAOTorsoCaballo); glDrawArrays(GL_TRIANGLES, 0, 36);

        modelo = modelTempCaballo; modelo = glm::translate(modelo, glm::vec3(0.0f, 0.37f, 0.75f)); modelo = glm::rotate(modelo, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.4f, 0.8f, 0.4f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOCuelloCaballo); glDrawArrays(GL_TRIANGLES, 0, 36);

        modelo = modelTempCaballo; modelo = glm::translate(modelo, glm::vec3(0.0f, 0.5f, 1.0f)); modelo = glm::rotate(modelo, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.5f, 0.35f, 0.7f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOCabezaCaballo); glDrawArrays(GL_TRIANGLES, 0, 36);

        modelo = modelTempCaballo; modelo = glm::translate(modelo, glm::vec3(0.0f, 0.295f, 1.35f)); modelo = glm::rotate(modelo, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.30f, 0.35f, 0.35f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOHocicoCaballo); glDrawArrays(GL_TRIANGLES, 0, 36);

        modelo = modelTempCaballo; modelo = glm::translate(modelo, glm::vec3(-0.25f, -0.35f, 0.50f)); modelo = glm::rotate(modelo, glm::radians(patasDelanteras), glm::vec3(1.0f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.25f, 0.70f, 0.25f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOPataCaballo); glDrawArrays(GL_TRIANGLES, 0, 36);

        modelo = modelTempCaballo; modelo = glm::translate(modelo, glm::vec3(0.25f, -0.35f, 0.50f)); modelo = glm::rotate(modelo, glm::radians(-patasDelanteras), glm::vec3(1.0f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.25f, 0.70f, 0.25f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOPataCaballo); glDrawArrays(GL_TRIANGLES, 0, 36);

        modelo = modelTempCaballo; modelo = glm::translate(modelo, glm::vec3(-0.25f, -0.35f, -0.50f)); modelo = glm::rotate(modelo, glm::radians(patasTraseras), glm::vec3(1.0f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.25f, 0.70f, 0.25f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOPataCaballo); glDrawArrays(GL_TRIANGLES, 0, 36);

        modelo = modelTempCaballo; modelo = glm::translate(modelo, glm::vec3(0.25f, -0.35f, -0.50f)); modelo = glm::rotate(modelo, glm::radians(-patasTraseras), glm::vec3(1.0f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.25f, 0.70f, 0.25f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOPataCaballo); glDrawArrays(GL_TRIANGLES, 0, 36);

        modelo = modelTempCaballo; modelo = glm::translate(modelo, glm::vec3(0.0f, 0.04f, -0.75f)); modelo = glm::rotate(modelo, glm::radians(25.0f + colaCaballo), glm::vec3(1.0f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.20f, 0.45f, 0.15f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOColaCaballo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 baseJinete = glm::mat4(1.0f);
        baseJinete = glm::translate(baseJinete, posPersonaje7 + glm::vec3(0.0f, 2.67f * characterScale, 0.0f));
        baseJinete = glm::rotate(baseJinete, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturaCaballerito);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);

        glm::mat4 modeloTorsoJ = baseJinete;
        modelo = glm::scale(modeloTorsoJ, glm::vec3(1.8f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOTorso); glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 pivoteCabezaJ = glm::translate(modeloTorsoJ, glm::vec3(0.0f, 1.335f * characterScale, 0.0f));
        modelo = glm::translate(pivoteCabezaJ, glm::vec3(0.0f, 0.9f * characterScale, 0.0f));
        modelo = glm::scale(modelo, glm::vec3(1.8f * characterScale));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOCabeza); glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 pivoteBrazoIzqJ = glm::translate(modeloTorsoJ, glm::vec3(-0.9f * characterScale, 1.335f * characterScale, 0.0f));
        modelo = glm::translate(pivoteBrazoIzqJ, glm::vec3(-0.45f * characterScale, -1.335f * characterScale, 0.0f));
        modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOBrazo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 pivoteBrazoDerJ = glm::translate(modeloTorsoJ, glm::vec3(0.9f * characterScale, 0.95f * characterScale, 0.0f));
        pivoteBrazoDerJ = glm::rotate(pivoteBrazoDerJ, glm::radians((brazoSaludo - 90.0f)), glm::vec3(0.1f, 0.0f, 0.0f));
        modelo = glm::translate(pivoteBrazoDerJ, glm::vec3(0.45f * characterScale, -1.335f * characterScale, 0.0f));
        modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOBrazo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 pivPiernaDerJ = glm::translate(modeloTorsoJ, glm::vec3(0.45f * characterScale, -1.335f * characterScale, 0.0f));
        pivPiernaDerJ = glm::rotate(pivPiernaDerJ, glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, -0.8));
        modelo = glm::translate(pivPiernaDerJ, glm::vec3(0.0f, -1.335f * characterScale, 0.0f));
        modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOPierna); glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 pivPiernaIzqJ = glm::translate(modeloTorsoJ, glm::vec3(-0.45f * characterScale, -1.335f * characterScale, 0.0f));
        pivPiernaIzqJ = glm::rotate(pivPiernaIzqJ, glm::radians(-70.0f), glm::vec3(1.0f, 0.0f, 0.8f));
        modelo = glm::translate(pivPiernaIzqJ, glm::vec3(0.0f, -1.335f * characterScale, 0.0f));
        modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOPierna); glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);
        glm::mat4 posManoJ = glm::translate(pivoteBrazoDerJ, glm::vec3(0.45f * characterScale, -2.67f * characterScale, 0.0f));
        glm::mat4 baseBandera = glm::rotate(posManoJ, glm::radians(brazoSaludo + 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelo = glm::translate(baseBandera, glm::vec3(0.0f, 0.75f, 0.0f));
        modelo = glm::scale(modelo, glm::vec3(0.06f, 1.5f, 0.06f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 0.0f, 0.0f);
        modelo = glm::translate(baseBandera, glm::vec3(0.35f, 1.25f, 0.0f));
        modelo = glm::scale(modelo, glm::vec3(0.7f, 0.5f, 0.05f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::vec3 posReyes[2] = {
            glm::vec3(4.0f, 17.5f, -205.0f),
            glm::vec3(4.0f, 17.5f, -202.5f)
        };

        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "transparency"), 0);

        for (int r = 0; r < 2; r++)
        {
            glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);
            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);
            glActiveTexture(GL_TEXTURE0);

            if (r == 0) glBindTexture(GL_TEXTURE_2D, texturaRey1);
            else        glBindTexture(GL_TEXTURE_2D, texturaRey2);

            glm::mat4 baseRey = glm::mat4(1.0f);
            baseRey = glm::translate(baseRey, posReyes[r] + glm::vec3(0.0f, 2.67f * characterScale, 0.0f));
            if (r == 1) baseRey = glm::rotate(baseRey, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 modeloTorsoR = baseRey;
            modelo = glm::scale(modeloTorsoR, glm::vec3(1.8f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOTorso); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 pivoteCabezaR = glm::translate(modeloTorsoR, glm::vec3(0.0f, 1.335f * characterScale, 0.0f));
            modelo = glm::translate(pivoteCabezaR, glm::vec3(0.0f, 0.9f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(1.8f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCabeza); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 pivoteBrazoIzqR = glm::translate(modeloTorsoR, glm::vec3(-0.9f * characterScale, 1.335f * characterScale, 0.0f));
            pivoteBrazoIzqR = glm::rotate(pivoteBrazoIzqR, glm::radians(-brazoReyDer), glm::vec3(1.0f, 0.0f, 0.0f));
            modelo = glm::translate(pivoteBrazoIzqR, glm::vec3(-0.45f * characterScale, -1.335f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOBrazo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);
            glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texturaEscudo);

            glm::mat4 posManoIzq = glm::translate(pivoteBrazoIzqR, glm::vec3(-1.0f * characterScale, -2.735f * characterScale, 0.1f * characterScale));
            modelo = glm::rotate(posManoIzq, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(1.2f * characterScale, 1.8f * characterScale, 0.05f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);
            glActiveTexture(GL_TEXTURE0);
            if (r == 0) glBindTexture(GL_TEXTURE_2D, texturaRey1);
            else        glBindTexture(GL_TEXTURE_2D, texturaRey2);

            glm::mat4 pivoteBrazoDerR = glm::translate(modeloTorsoR, glm::vec3(0.9f * characterScale, 1.335f * characterScale, 0.0f));
            pivoteBrazoDerR = glm::rotate(pivoteBrazoDerR, glm::radians(-brazoReyDer), glm::vec3(1.0f, 0.0f, 0.0f));
            pivoteBrazoDerR = glm::rotate(pivoteBrazoDerR, glm::radians(brazoReyEscudo), glm::vec3(0.0f, 0.0f, 1.0f));
            modelo = glm::translate(pivoteBrazoDerR, glm::vec3(0.45f * characterScale, -1.335f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOBrazo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 0);
            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.7f, 0.7f, 0.7f);

            modelo = glm::translate(pivoteBrazoDerR, glm::vec3(-0.2f, -1.0f, 0.0f));
            modelo = glm::rotate(modelo, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelo = glm::rotate(modelo, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(1.5f, 1.5f, 1.5f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            EspadaModel.Draw(shaderIluminacion);

            glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);
            glActiveTexture(GL_TEXTURE0);
            if (r == 0) glBindTexture(GL_TEXTURE_2D, texturaRey1);
            else        glBindTexture(GL_TEXTURE_2D, texturaRey2);

            glm::mat4 pivPiernaDerR = glm::translate(modeloTorsoR, glm::vec3(0.45f * characterScale, -1.335f * characterScale, 0.0f));
            modelo = glm::translate(pivPiernaDerR, glm::vec3(0.0f, -1.335f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOPierna); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 pivPiernaIzqR = glm::translate(modeloTorsoR, glm::vec3(-0.45f * characterScale, -1.335f * characterScale, 0.0f));
            modelo = glm::translate(pivPiernaIzqR, glm::vec3(0.0f, -1.335f * characterScale, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.9f * characterScale, 2.67f * characterScale, 0.9f * characterScale));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOPierna); glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        for (int col = 0; col < 2; col++) {
            for (int fila = 0; fila < 6; fila++) {
                glm::mat4 modelTempVerdeHojas = glm::translate(glm::mat4(1), posicionesArbolesVerdes[col][fila] + glm::vec3(oscHojas, 0.0f, 0.0f));
                glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.40f, 0.22f, 0.08f);
                modelo = glm::translate(glm::mat4(1), posicionesArbolesVerdes[col][fila] + glm::vec3(0.0f, 1.4f, 0.0f));
                modelo = glm::scale(modelo, glm::vec3(1.5f, 10.5f, 1.5f));
                glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
                glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

                glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.13f, 0.55f, 0.13f);
                modelo = modelTempVerdeHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 3.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(6.0f, 1.5f, 6.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                modelo = modelTempVerdeHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 1.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(4.5f, 1.5f, 1.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                modelo = modelTempVerdeHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 1.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(1.5f, 1.5f, 4.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                modelo = modelTempVerdeHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 5.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(4.5f, 1.5f, 1.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                modelo = modelTempVerdeHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 5.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(1.5f, 1.5f, 4.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                modelo = modelTempVerdeHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 7.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(4.5f, 1.5f, 1.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                modelo = modelTempVerdeHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 7.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(1.5f, 1.5f, 4.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        for (int col = 0; col < 2; col++) {
            for (int fila = 0; fila < 6; fila++) {
                glm::vec3 baseRosa = posicionesArbolesRosa[col][fila];
                glm::mat4 modelTempRosaHojas = glm::translate(glm::mat4(1), baseRosa + glm::vec3(oscHojas, 0.0f, 0.0f));
                glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.40f, 0.22f, 0.08f);
                modelo = glm::translate(glm::mat4(1), baseRosa + glm::vec3(0.0f, 2.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(1.5f, 3.0f, 1.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

                glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.95f, 0.41f, 0.70f);
                modelo = modelTempRosaHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 4.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(6.0f, 1.5f, 6.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                modelo = modelTempRosaHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 5.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(4.5f, 1.5f, 4.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                modelo = modelTempRosaHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 6.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(4.5f, 1.5f, 1.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                modelo = modelTempRosaHojas; modelo = glm::translate(modelo, glm::vec3(0.0f, 6.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(1.5f, 1.5f, 4.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        for (int i = 0; i < 9; i++) {
            glm::vec3 posCaja1 = posicionesM[i];
            glm::vec3 c1 = posCaja1 + glm::vec3(-0.025f, 0.55f, -0.025f);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.40f, 0.22f, 0.10f);
            modelo = glm::translate(glm::mat4(1), c1 + glm::vec3(0.0f, -0.5f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(2.3f, 0.1f, 1.2f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c1 + glm::vec3(0.0f, 0.0f, 0.55f)); modelo = glm::scale(modelo, glm::vec3(2.3f, 1.1f, 0.1f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c1 + glm::vec3(0.0f, 0.0f, -0.55f)); modelo = glm::scale(modelo, glm::vec3(2.3f, 1.1f, 0.1f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c1 + glm::vec3(-1.1f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.1f, 1.1f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c1 + glm::vec3(1.1f, 0.0f, 0.0f));  modelo = glm::scale(modelo, glm::vec3(0.1f, 1.1f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.85f, 0.07f, 0.07f);
            for (int r = 0; r < 4; r++)
                for (int c = 0; c < 8; c++) {
                    modelo = glm::mat4(1); modelo = glm::translate(modelo, posCaja1 + glm::vec3(-0.9f + c * 0.25f, 1.0f, -0.4f + r * 0.25f)); modelo = glm::scale(modelo, glm::vec3(0.45f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOEsfera); glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);
                }
        }

        for (int i = 0; i < 9; i++) {
            glm::vec3 posCaja2 = posicionesS[i];
            glm::vec3 c2 = posCaja2 + glm::vec3(-0.04f, 0.55f, 0.0f);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.40f, 0.22f, 0.10f);
            modelo = glm::translate(glm::mat4(1), c2 + glm::vec3(0.0f, -0.5f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 0.1f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c2 + glm::vec3(0.0f, 0.0f, 0.45f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 1.1f, 0.1f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c2 + glm::vec3(0.0f, 0.0f, -0.45f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 1.1f, 0.1f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c2 + glm::vec3(-0.75f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.1f, 1.1f, 0.8f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c2 + glm::vec3(0.75f, 0.0f, 0.0f));  modelo = glm::scale(modelo, glm::vec3(0.1f, 1.1f, 0.8f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.13f, 0.55f, 0.13f);
            for (int r = 0; r < 3; r++)
                for (int c = 0; c < 3; c++) {
                    modelo = glm::mat4(1); modelo = glm::translate(modelo, posCaja2 + glm::vec3(-0.60f + c * 0.56f, 1.0f, -0.30f + r * 0.30f)); modelo = glm::scale(modelo, glm::vec3(1.1f, 0.75f, 0.75f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOEsfera); glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);
                }
        }

        for (int i = 0; i < 9; i++) {
            glm::vec3 posCaja3 = posicionesU[i];
            glm::vec3 c3 = posCaja3 + glm::vec3(0.0f, 0.55f, -0.15f);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.40f, 0.22f, 0.10f);
            modelo = glm::translate(glm::mat4(1), c3 + glm::vec3(0.0f, -0.5f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(2.0f, 0.1f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c3 + glm::vec3(0.0f, 0.0f, 0.45f)); modelo = glm::scale(modelo, glm::vec3(2.0f, 1.1f, 0.1f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c3 + glm::vec3(0.0f, 0.0f, -0.45f)); modelo = glm::scale(modelo, glm::vec3(2.0f, 1.1f, 0.1f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c3 + glm::vec3(-0.95f, 0.0f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.1f, 1.1f, 0.8f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = glm::translate(glm::mat4(1), c3 + glm::vec3(0.95f, 0.0f, 0.0f));  modelo = glm::scale(modelo, glm::vec3(0.1f, 1.1f, 0.8f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.45f, 0.10f, 0.55f);
            for (int r = 0; r < 2; r++)
                for (int c = 0; c < 4; c++)
                    for (int uva = 0; uva < 6; uva++) {
                        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCaja3 + glm::vec3(-0.65f + (-0.1f + c * 0.5f) + desplazamientoCaja3[uva].x, 1.0f, -0.3f + (-0.1f + r * 0.5f) + desplazamientoCaja3[uva].z)); modelo = glm::scale(modelo, glm::vec3(0.5f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOEsfera); glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);
                    }
        }

        for (int t = 0; t < 9; t++) {
            glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);

            float anguloTienda = glm::radians(t * (360.0f / 9.0f));
            glm::vec3 posTienda(centroCirculo.x + radioTiendas * cos(anguloTienda), centroCirculo.y, centroCirculo.z + radioTiendas * sin(anguloTienda));
            glm::mat4 baseTienda = glm::mat4(1);
            baseTienda = glm::translate(baseTienda, posTienda);
            baseTienda = glm::rotate(baseTienda, -(anguloTienda)+glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.88f, 0.84f, 0.76f);
            modelo = baseTienda; modelo = glm::translate(modelo, glm::vec3(-1.8f, 1.875f, 1.2f)); modelo = glm::scale(modelo, glm::vec3(0.33f, 3.75f, 0.33f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = baseTienda; modelo = glm::translate(modelo, glm::vec3(1.8f, 1.875f, 1.2f)); modelo = glm::scale(modelo, glm::vec3(0.33f, 3.75f, 0.33f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = baseTienda; modelo = glm::translate(modelo, glm::vec3(-1.8f, 1.875f, -1.2f)); modelo = glm::scale(modelo, glm::vec3(0.33f, 3.75f, 0.33f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = baseTienda; modelo = glm::translate(modelo, glm::vec3(1.8f, 1.875f, -1.2f)); modelo = glm::scale(modelo, glm::vec3(0.33f, 3.75f, 0.33f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.72f, 0.20f, 0.18f);
            modelo = baseTienda; modelo = glm::translate(modelo, glm::vec3(0.0f, 3.9f, 0.0f));    modelo = glm::scale(modelo, glm::vec3(4.05f, 0.12f, 2.85f));  glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
            modelo = baseTienda; modelo = glm::translate(modelo, glm::vec3(0.0f, 2.85f, -1.35f));  modelo = glm::scale(modelo, glm::vec3(4.05f, 2.25f, 0.12f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 mFront = glm::translate(baseTienda, glm::vec3(0.0f, 3.9375f, 1.35f));
            mFront = glm::rotate(mFront, glm::radians(anguloToldo), glm::vec3(1.0f, 0.0f, 0.0f));
            mFront = glm::translate(mFront, glm::vec3(0.0f, -0.4125f, 0.0f));
            mFront = glm::scale(mFront, glm::vec3(4.05f, 0.825f, 0.12f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(mFront));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 mLeft = glm::translate(baseTienda, glm::vec3(-2.175f, 3.9375f, 0.0f));
            mLeft = glm::rotate(mLeft, glm::radians(-anguloToldo), glm::vec3(0.0f, 0.0f, 1.0f));
            mLeft = glm::translate(mLeft, glm::vec3(0.0f, -0.4125f, 0.0f));
            mLeft = glm::scale(mLeft, glm::vec3(0.12f, 0.825f, 2.85f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(mLeft));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 mRight = glm::translate(baseTienda, glm::vec3(2.175f, 3.9375f, 0.0f));
            mRight = glm::rotate(mRight, glm::radians(anguloToldo), glm::vec3(0.0f, 0.0f, 1.0f));
            mRight = glm::translate(mRight, glm::vec3(0.0f, -0.4125f, 0.0f));
            mRight = glm::scale(mRight, glm::vec3(0.12f, 0.825f, 2.85f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(mRight));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            float offsetYCarrito = 0.45f;
            glm::vec3 posCarrito(
                centroCirculo.x + radioTiendas * cos(anguloTienda) + cos(anguloTienda + glm::radians(90.0f)) * 3.0f,
                centroCirculo.y + offsetYCarrito,
                centroCirculo.z + radioTiendas * sin(anguloTienda) + sin(anguloTienda + glm::radians(90.0f)) * 3.0f
            );
            glm::mat4 baseCarrito = glm::mat4(1);
            baseCarrito = glm::translate(baseCarrito, posCarrito);
            baseCarrito = glm::rotate(baseCarrito, -(anguloTienda)+glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            baseCarrito = glm::scale(baseCarrito, glm::vec3(1.0f, 1.0f, 1.0f));

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);
            glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 0);
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(baseCarrito));
            CarritoModel.Draw(shaderIluminacion);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 0);

        glm::vec3 posCatapultas[2] = {
            glm::vec3(-35.0f, -16.0f, -11.0f),
            glm::vec3(43.0f, -16.0f, -11.0f)
        };

        for (int i = 0; i < 2; i++) {
            glm::mat4 baseCatapulta = glm::mat4(1.0f);
            baseCatapulta = glm::translate(baseCatapulta, posCatapultas[i]);
            if (i == 0) baseCatapulta = glm::rotate(baseCatapulta, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            else        baseCatapulta = glm::rotate(baseCatapulta, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.45f, 0.28f, 0.15f);

            modelo = glm::translate(baseCatapulta, glm::vec3(-1.0f, 0.5f, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.5f, 0.5f, 5.0f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            modelo = glm::translate(baseCatapulta, glm::vec3(1.0f, 0.5f, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.5f, 0.5f, 5.0f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            modelo = glm::translate(baseCatapulta, glm::vec3(-1.0f, 2.0f, -0.5f));
            modelo = glm::scale(modelo, glm::vec3(0.5f, 3.0f, 0.5f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            modelo = glm::translate(baseCatapulta, glm::vec3(1.0f, 2.0f, -0.5f));
            modelo = glm::scale(modelo, glm::vec3(0.5f, 3.0f, 0.5f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.5f, 0.5f, 0.5f);
            modelo = glm::translate(baseCatapulta, glm::vec3(0.0f, 3.0f, -0.5f));
            modelo = glm::scale(modelo, glm::vec3(2.5f, 0.2f, 0.2f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.45f, 0.28f, 0.15f);
            glm::mat4 pivoteBrazo = glm::translate(baseCatapulta, glm::vec3(0.0f, 3.0f, -0.5f));
            pivoteBrazo = glm::rotate(pivoteBrazo, glm::radians(anguloCatapulta), glm::vec3(1.0f, 0.0f, 0.0f));

            modelo = glm::translate(pivoteBrazo, glm::vec3(0.0f, 0.0f, -1.5f));
            modelo = glm::scale(modelo, glm::vec3(0.4f, 0.4f, 5.0f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::vec3 offsetCuchara = glm::vec3(0.0f, 0.0f, -3.5f);

            modelo = glm::translate(pivoteBrazo, offsetCuchara + glm::vec3(0.0f, 0.3f, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(1.0f, 0.2f, 1.0f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            modelo = glm::translate(pivoteBrazo, offsetCuchara + glm::vec3(0.4f, 0.6f, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.2f, 0.4f, 1.0f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            modelo = glm::translate(pivoteBrazo, offsetCuchara + glm::vec3(-0.4f, 0.6f, 0.0f));
            modelo = glm::scale(modelo, glm::vec3(0.2f, 0.4f, 1.0f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            modelo = glm::translate(pivoteBrazo, offsetCuchara + glm::vec3(0.0f, 0.6f, -0.4f));
            modelo = glm::scale(modelo, glm::vec3(0.6f, 0.4f, 0.2f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            modelo = glm::translate(pivoteBrazo, offsetCuchara + glm::vec3(0.0f, 0.6f, 0.4f));
            modelo = glm::scale(modelo, glm::vec3(0.6f, 0.4f, 0.2f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.4f, 0.4f, 0.4f);
            if (!piedraVolando) {
                modelo = glm::translate(pivoteBrazo, offsetCuchara + glm::vec3(0.0f, 0.5f, 0.0f));
                modelo = glm::scale(modelo, glm::vec3(0.6f));
            }
            else {
                modelo = glm::translate(baseCatapulta, glm::vec3(0.0f, 3.0f + altPiedra, distPiedra + 0.0f));
                modelo = glm::scale(modelo, glm::vec3(0.6f));
            }
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOEsfera); glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);

            glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.25f, 0.15f, 0.08f);
            float rX[] = { -1.3f, 1.3f };
            float rZ[] = { -2.0f, 2.0f };
            for (int rx = 0; rx < 2; rx++) {
                for (int rz = 0; rz < 2; rz++) {
                    modelo = glm::translate(baseCatapulta, glm::vec3(rX[rx], 0.5f, rZ[rz]));
                    modelo = glm::scale(modelo, glm::vec3(0.3f, 1.2f, 1.2f));
                    glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
                    glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }

        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glm::vec3 posCofre(1.5f, 17.5f, -207.0f);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.35f, 0.18f, 0.05f);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(0.0f, 0.04f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 0.08f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(0.0f, 0.44f, 0.5f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 0.80f, 0.08f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(0.0f, 0.44f, -0.5f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 0.80f, 0.08f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(-0.8f, 0.44f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.08f, 0.80f, 1.0f));  glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(0.8f, 0.44f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.08f, 0.80f, 1.0f));  glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 bisagra1 = glm::mat4(1);
        bisagra1 = glm::translate(bisagra1, posCofre + glm::vec3(0.0f, 0.88f, -0.46f));
        bisagra1 = glm::rotate(bisagra1, glm::radians(-anguloTapa), glm::vec3(1.0f, 0.0f, 0.0f));

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.45f, 0.23f, 0.07f);
        modelo = glm::translate(bisagra1, glm::vec3(0.0f, 0.04f, 0.5f));
        modelo = glm::scale(modelo, glm::vec3(1.6f, 0.08f, 1.0f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.35f, 0.18f, 0.05f);
        modelo = glm::translate(bisagra1, glm::vec3(0.0f, 0.0f, 1.0f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 0.08f, 0.08f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::translate(bisagra1, glm::vec3(-0.76f, 0.0f, 0.5f)); modelo = glm::scale(modelo, glm::vec3(0.08f, 0.08f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::translate(bisagra1, glm::vec3(0.76f, 0.0f, 0.5f)); modelo = glm::scale(modelo, glm::vec3(0.08f, 0.08f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 0.85f, 0.10f);
        modelo = glm::translate(bisagra1, glm::vec3(0.0f, 0.0f, 1.02f)); modelo = glm::scale(modelo, glm::vec3(0.18f, 0.10f, 0.06f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 0.85f, 0.10f);
        for (int fila = 0; fila < 8; fila++)
            for (int col = 0; col < 15; col++) {
                modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(-0.70f + col * 0.1f, 0.72f, -0.35f + fila * 0.1f)); modelo = glm::scale(modelo, glm::vec3(0.18f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOEsfera); glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);
            }

        posCofre = glm::vec3(6.5f, 17.5f, -207.0f);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.35f, 0.18f, 0.05f);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(0.0f, 0.04f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 0.08f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(0.0f, 0.44f, 0.5f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 0.80f, 0.08f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(0.0f, 0.44f, -0.5f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 0.80f, 0.08f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(-0.8f, 0.44f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.08f, 0.80f, 1.0f));  glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(0.8f, 0.44f, 0.0f)); modelo = glm::scale(modelo, glm::vec3(0.08f, 0.80f, 1.0f));  glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 bisagra2 = glm::mat4(1);
        bisagra2 = glm::translate(bisagra2, posCofre + glm::vec3(0.0f, 0.88f, -0.46f));
        bisagra2 = glm::rotate(bisagra2, glm::radians(-anguloTapa), glm::vec3(1.0f, 0.0f, 0.0f));

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.45f, 0.23f, 0.07f);
        modelo = glm::translate(bisagra2, glm::vec3(0.0f, 0.04f, 0.5f));
        modelo = glm::scale(modelo, glm::vec3(1.6f, 0.08f, 1.0f));
        glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
        glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 0.35f, 0.18f, 0.05f);
        modelo = glm::translate(bisagra2, glm::vec3(0.0f, 0.0f, 1.0f)); modelo = glm::scale(modelo, glm::vec3(1.6f, 0.08f, 0.08f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::translate(bisagra2, glm::vec3(-0.76f, 0.0f, 0.5f)); modelo = glm::scale(modelo, glm::vec3(0.08f, 0.08f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);
        modelo = glm::translate(bisagra2, glm::vec3(0.76f, 0.0f, 0.5f)); modelo = glm::scale(modelo, glm::vec3(0.08f, 0.08f, 1.0f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 0.85f, 0.10f);
        modelo = glm::translate(bisagra2, glm::vec3(0.0f, 0.0f, 1.02f)); modelo = glm::scale(modelo, glm::vec3(0.18f, 0.10f, 0.06f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOCubo); glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 0.85f, 0.10f);
        for (int fila = 0; fila < 8; fila++)
            for (int col = 0; col < 15; col++) {
                modelo = glm::mat4(1); modelo = glm::translate(modelo, posCofre + glm::vec3(-0.70f + col * 0.1f, 0.72f, -0.35f + fila * 0.1f)); modelo = glm::scale(modelo, glm::vec3(0.18f)); glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo)); glBindVertexArray(VAOEsfera); glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);
            }

        // ============================================================
        // DIBUJAR ANTORCHAS
        // ============================================================
        glUniform1i(glGetUniformLocation(shaderIluminacion.Program, "useTexture"), 1);
        glUniform3f(glGetUniformLocation(shaderIluminacion.Program, "objectColor"), 1.0f, 1.0f, 1.0f);
        for (int i = 0; i < 16; i++) {
            modelo = glm::mat4(1.0f);
            modelo = glm::translate(modelo, posicionesAntorchas[i]);
            modelo = glm::scale(modelo, glm::vec3(1.0f));
            glUniformMatrix4fv(locModelo, 1, GL_FALSE, glm::value_ptr(modelo));
            AntorchaModel.Draw(shaderIluminacion);
        }

        // Dibujar el sol (solo cuando esta sobre el horizonte)
        shaderLampara.Use();
        GLint locModeloLamp = glGetUniformLocation(shaderLampara.Program, "model");
        glUniformMatrix4fv(glGetUniformLocation(shaderLampara.Program, "view"), 1, GL_FALSE, glm::value_ptr(vista));
        glUniformMatrix4fv(glGetUniformLocation(shaderLampara.Program, "projection"), 1, GL_FALSE, glm::value_ptr(proyeccion));
        if (posSolActual.y > 0.0f) {
            glm::vec3 colorAstro = glm::mix(colorAtardecerBase, glm::vec3(1.0f, 1.0f, 0.8f), factorDia);
            glUniform3f(glGetUniformLocation(shaderLampara.Program, "lampColor"), colorAstro.r, colorAstro.g, colorAstro.b);
            modelo = glm::mat4(1);
            modelo = glm::translate(modelo, posSolActual);
            modelo = glm::scale(modelo, glm::vec3(20.0f));
            glUniformMatrix4fv(locModeloLamp, 1, GL_FALSE, glm::value_ptr(modelo));
            glBindVertexArray(VAOEsfera);
            glDrawElements(GL_TRIANGLES, (GLsizei)indicesEsfera.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(ventana);
    }

    glDeleteVertexArrays(1, &VAOEsfera); glDeleteBuffers(1, &VBOEsfera); glDeleteBuffers(1, &EBOEsfera);
    glDeleteVertexArrays(1, &VAOCubo); glDeleteBuffers(1, &VBOCubo);
    glDeleteVertexArrays(1, &VAOCabeza); glDeleteBuffers(1, &VBOCabeza);
    glDeleteVertexArrays(1, &VAOTorso); glDeleteBuffers(1, &VBOTorso);
    glDeleteVertexArrays(1, &VAOBrazo); glDeleteBuffers(1, &VBOBrazo);
    glDeleteVertexArrays(1, &VAOPierna); glDeleteBuffers(1, &VBOPierna);
    glDeleteVertexArrays(1, &VAOCabezaCaballo); glDeleteBuffers(1, &VBOCabezaCaballo);
    glDeleteVertexArrays(1, &VAOHocicoCaballo); glDeleteBuffers(1, &VBOHocicoCaballo);
    glDeleteVertexArrays(1, &VAOCuelloCaballo); glDeleteBuffers(1, &VBOCuelloCaballo);
    glDeleteVertexArrays(1, &VAOTorsoCaballo); glDeleteBuffers(1, &VBOTorsoCaballo);
    glDeleteVertexArrays(1, &VAOPataCaballo); glDeleteBuffers(1, &VBOPataCaballo);
    glDeleteVertexArrays(1, &VAOColaCaballo); glDeleteBuffers(1, &VBOColaCaballo);

    glfwTerminate();
    return 0;
}

void Movimiento()
{
    if (teclas[GLFW_KEY_W] || teclas[GLFW_KEY_UP])    camara.ProcessKeyboard(FORWARD, deltaTime);
    if (teclas[GLFW_KEY_S] || teclas[GLFW_KEY_DOWN])  camara.ProcessKeyboard(BACKWARD, deltaTime);
    if (teclas[GLFW_KEY_A] || teclas[GLFW_KEY_LEFT])  camara.ProcessKeyboard(LEFT, deltaTime);
    if (teclas[GLFW_KEY_D] || teclas[GLFW_KEY_RIGHT]) camara.ProcessKeyboard(RIGHT, deltaTime);
}

void TeclaCallback(GLFWwindow* ventana, int tecla, int scancode, int accion, int modo)
{
    if (tecla == GLFW_KEY_ESCAPE && accion == GLFW_PRESS)
        glfwSetWindowShouldClose(ventana, GL_TRUE);

    if (tecla >= 0 && tecla < 1024) {
        if (accion == GLFW_PRESS)   teclas[tecla] = true;
        if (accion == GLFW_RELEASE) teclas[tecla] = false;
    }

    if (teclas[GLFW_KEY_SPACE]) {
        luzActiva = !luzActiva;
    }

    if (tecla == GLFW_KEY_N && accion == GLFW_PRESS)
        animsol = !animsol;

    if (tecla == GLFW_KEY_B && accion == GLFW_PRESS) {
        animCaballo = !animCaballo;
        animHojas = !animHojas;
        animPersonajes = !animPersonajes;
        animReyes = !animReyes;
    }
}

void Animacion()
{
    if (animsol) {
        rotacionsol += 0.25f;
        if (rotacionsol > 360.0f) rotacionsol -= 360.0f;
    }

    if (animCaballo) {
        if (!pasoCaballo) {
            patasDelanteras += 0.5f;
            patasTraseras -= 0.5f;
            colaCaballo += 0.5f;
            if (patasDelanteras > 30.0f) pasoCaballo = true;
        }
        else {
            patasDelanteras -= 0.5f;
            patasTraseras += 0.5f;
            colaCaballo -= 0.5f;
            if (patasDelanteras < -30.0f) pasoCaballo = false;
        }

        if (!dirSaludo) {
            brazoSaludo += 0.5f;
            if (brazoSaludo > 20.0f) dirSaludo = true;
        }
        else {
            brazoSaludo -= 0.5f;
            if (brazoSaludo < -20.0f) dirSaludo = false;
        }

        posCaballo.z -= 0.025f;
        if (posCaballo.z < -250.0f) posCaballo.z = 150.0f;
        posPersonaje7 = posCaballo + glm::vec3(0.0f, 1.1f, 0.0f);
    }

    if (animHojas) {
        if (!dirHojas) {
            oscHojas += 0.008f;
            if (oscHojas > 0.3f)  dirHojas = true;
        }
        else {
            oscHojas -= 0.008f;
            if (oscHojas < -0.3f) dirHojas = false;
        }

        anguloToldo = (sin(tiempoActual * 4.0f) + 1.0f) * 7.5f;
    }
    else {
        if (anguloToldo > 0.1f) anguloToldo -= 0.5f;
        else anguloToldo = 0.0f;
    }

    if (animPersonajes) {
        if (!dirPersonajestienda) {
            brazoPersonajestiendaDer -= 0.2f;
            brazoPersonajestiendaIzq -= 0.2f;
            if (brazoPersonajestiendaDer < -80.0f) dirPersonajestienda = true;
        }
        else {
            brazoPersonajestiendaDer += 0.4f;
            brazoPersonajestiendaIzq += 0.4f;
            if (brazoPersonajestiendaDer > -10.0f) dirPersonajestienda = false;
        }

        if (catapultaDisparando) {
            anguloCatapulta += 8.0f;
            if (anguloCatapulta >= 45.0f && !piedraVolando) {
                piedraVolando = true;
            }
            if (anguloCatapulta >= 110.0f) {
                catapultaDisparando = false;
            }
        }
        else {
            anguloCatapulta -= 0.5f;
            if (anguloCatapulta <= -45.0f) {
                catapultaDisparando = true;
                piedraVolando = false;
                distPiedra = 0.0f;
                altPiedra = 0.0f;
            }
        }

        if (piedraVolando) {
            distPiedra += 0.8f;
            altPiedra = -(0.02f * (distPiedra - 15.0f) * (distPiedra - 15.0f)) + 4.5f;
            if (altPiedra < -2.0f) altPiedra = -2.0f;
        }
    }

    if (animReyes) {
        if (!dirRey && !dirReyEscudo) {
            brazoReyDer += 0.4f;
            if (brazoReyDer >= 180.0f) {
                brazoReyDer = 180.0f;
                dirReyEscudo = true;
            }
        }
        if (!dirRey && dirReyEscudo) {
            brazoReyEscudo += 0.4f;
            if (brazoReyEscudo >= 90.0f) {
                brazoReyEscudo = 90.0f;
                dirRey = true;
            }
        }
        if (dirRey) {
            brazoReyDer -= 0.4f;
            brazoReyEscudo -= 0.4f;
            if (brazoReyDer <= 90.0f) {
                brazoReyDer = 90.0f;
                brazoReyEscudo = 0.0f;
                dirRey = false;
                dirReyEscudo = false;
            }
        }

        if (!dirTapa) {
            anguloTapa += 0.5f;
            if (anguloTapa >= 110.0f) dirTapa = true;
        }
        else {
            anguloTapa -= 0.5f;
            if (anguloTapa <= 0.0f)   dirTapa = false;
        }
    }
}

void MouseCallback(GLFWwindow* ventana, double posX, double posY)
{
    if (primerMouse) {
        ultimoX = posX;
        ultimoY = posY;
        primerMouse = false;
    }

    GLfloat offsetX = posX - ultimoX;
    GLfloat offsetY = ultimoY - posY;
    ultimoX = posX;
    ultimoY = posY;

    camara.ProcessMouseMovement(offsetX, offsetY);
}

void crearBloqueMinecraft(GLuint& VAO, GLuint& VBO, float tx, float ty, float w, float h, float d, float texWidth, float texHeight) {
    auto calcUV = [&](float x, float y) -> glm::vec2 {
        return glm::vec2(x / texWidth, (texHeight - y) / texHeight);
        };

    std::vector<float> v;
    auto addFace = [&](glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 n, glm::vec2 uvBL, glm::vec2 uvBR, glm::vec2 uvTR, glm::vec2 uvTL) {
        v.insert(v.end(), { p1.x, p1.y, p1.z, n.x, n.y, n.z, uvBL.x, uvBL.y });
        v.insert(v.end(), { p2.x, p2.y, p2.z, n.x, n.y, n.z, uvBR.x, uvBR.y });
        v.insert(v.end(), { p3.x, p3.y, p3.z, n.x, n.y, n.z, uvTR.x, uvTR.y });
        v.insert(v.end(), { p3.x, p3.y, p3.z, n.x, n.y, n.z, uvTR.x, uvTR.y });
        v.insert(v.end(), { p4.x, p4.y, p4.z, n.x, n.y, n.z, uvTL.x, uvTL.y });
        v.insert(v.end(), { p1.x, p1.y, p1.z, n.x, n.y, n.z, uvBL.x, uvBL.y });
        };

    glm::vec3 blb(-0.5f, -0.5f, -0.5f), brb(0.5f, -0.5f, -0.5f), trb(0.5f, 0.5f, -0.5f), tlb(-0.5f, 0.5f, -0.5f);
    glm::vec3 blf(-0.5f, -0.5f, 0.5f), brf(0.5f, -0.5f, 0.5f), trf(0.5f, 0.5f, 0.5f), tlf(-0.5f, 0.5f, 0.5f);

    addFace(blf, brf, trf, tlf, glm::vec3(0, 0, 1), calcUV(tx + d, ty + d + h), calcUV(tx + d + w, ty + d + h), calcUV(tx + d + w, ty + d), calcUV(tx + d, ty + d));
    addFace(brb, blb, tlb, trb, glm::vec3(0, 0, -1), calcUV(tx + d + w + d + w, ty + d + h), calcUV(tx + d + w + d, ty + d + h), calcUV(tx + d + w + d, ty + d), calcUV(tx + d + w + d + w, ty + d));
    addFace(blb, blf, tlf, tlb, glm::vec3(-1, 0, 0), calcUV(tx, ty + d + h), calcUV(tx + d, ty + d + h), calcUV(tx + d, ty + d), calcUV(tx, ty + d));
    addFace(brf, brb, trb, trf, glm::vec3(1, 0, 0), calcUV(tx + d + w, ty + d + h), calcUV(tx + d + w + d, ty + d + h), calcUV(tx + d + w + d, ty + d), calcUV(tx + d + w, ty + d));
    addFace(tlf, trf, trb, tlb, glm::vec3(0, 1, 0), calcUV(tx + d, ty + d), calcUV(tx + d + w, ty + d), calcUV(tx + d + w, ty), calcUV(tx + d, ty));
    addFace(blb, brb, brf, blf, glm::vec3(0, -1, 0), calcUV(tx + d + w, ty + d), calcUV(tx + d + w + w, ty + d), calcUV(tx + d + w + w, ty), calcUV(tx + d + w, ty));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}