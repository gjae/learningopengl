/**
 * @file main.cpp
 * @brief Aplicación OpenGL para renderizado 3D con múltiples escenas
 * @details Este programa utiliza GLFW para gestión de ventanas y OpenGL 3.3 Core Profile
 *          para renderizar figuras geométricas con capacidad de cambiar entre 3 escenas.
 */

// Inclusión de bibliotecas necesarias para OpenGL y GLFW
#include "glad/glad.h"  // Cargador de funciones OpenGL (debe incluirse antes que GLFW)
#include <GLFW/glfw3.h> // Biblioteca para manejo de ventanas y entrada de dispositivos
#include <iostream>     // Para salida de consola (debugging y mensajes de error)

using namespace std;    // Usar el espacio de nombres estándar para simplificar código

/**
 * @defgroup tipos Tipos personalizados
 * @brief Definiciones de tipos para mejorar legibilidad y mantenimiento
 * @{
 */

/**
 * @typedef WindowSize
 * @brief Tipo para dimensiones de ventana (ancho/alto en píxeles)
 */
typedef unsigned int WindowSize;

/**
 * @typedef SceneRenderer
 * @brief Tipo para índice de escena (rango 0-2)
 */
typedef unsigned short int SceneRenderer;

/**
 * @typedef WindowBackground
 * @brief Tipo para componentes de color RGBA (valores normalizados 0.0-1.0)
 */
typedef float WindowBackground;

/**
 * @typedef VertexArray
 * @brief Tipo para array de 9 floats (3 vértices con coordenadas x,y,z cada uno)
 */
typedef float VertexArray[9];
/** @} */ // Final del grupo tipos

/**
 * @struct Figure
 * @brief Estructura que representa una figura geométrica renderizable
 * @details Contiene los datos de vértices y los objetos OpenGL necesarios para el renderizado
 */
typedef struct {
    VertexArray figureVertex;   ///< Array con las coordenadas de los vértices (3 vértices x 3 coordenadas)
    unsigned int VBO;           ///< Vertex Buffer Object (almacena datos en memoria de GPU)
    unsigned int vertexShader;  ///< ID del vertex shader compilado
    unsigned int fragmentShader;///< ID del fragment shader compilado
} Figure;

// Constantes de configuración
const WindowSize HEIGH = 600;  ///< Altura inicial de la ventana en píxeles
const WindowSize WIDTH = 800;  ///< Ancho inicial de la ventana en píxeles

/**
 * @var vertexShaderSource
 * @brief Código fuente GLSL del vertex shader (versión 330 core)
 * @details Transforma vértices sin modificación (pasa coordenadas directamente)
 */
const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

/**
 * @var fragmentShaderSource
 * @brief Código fuente GLSL del fragment shader (versión 330 core)
 * @details Genera color naranja fijo (RGB: 1.0, 0.5, 0.2) para todos los fragmentos
 */
const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

SceneRenderer WindowSceneDisplay = 0;  ///< Índice de la escena actualmente activa (0-2)

// Prototipos de funciones
/**
 * @brief Crea y configura la ventana GLFW
 * @return Puntero a la ventana creada o NULL en caso de error
 */
GLFWwindow* getWindowObject();

/**
 * @brief Inicializa GLFW con configuraciones básicas
 */
void initializeGlfw();

/**
 * @brief Callback para redimensionamiento de ventana
 * @param window Ventana que generó el evento
 * @param width Nuevo ancho en píxeles
 * @param height Nuevo alto en píxeles
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/**
 * @brief Callback para eventos de teclado
 * @param window Ventana que recibió el evento
 * @param key Código de tecla presionada
 * @param scanCode Código de escaneo físico
 * @param action Acción (presionar, soltar, repetir)
 * @param mods Teclas modificadoras (Shift, Ctrl, etc)
 */
void keyCallbackListener(GLFWwindow *window, int key, int scanCode, int action, int mods);

/**
 * @brief Crea y configura las figuras geométricas
 * @return Puntero a arreglo de figuras o NULL en fallo de memoria
 * @note El llamante debe liberar la memoria con free()
 */
Figure* getFiguresShapes();

/**
 * @brief Configura y compila un shader OpenGL
 * @param shader ID del shader a configurar
 * @param source Primer carácter del código fuente del shader
 * @warning Versión simplificada - maneja solo primer carácter del source
 */
void configureShader(unsigned int shader, char source);

/**
 * @var SCENE_BACKGROUND
 * @brief Colores de fondo para las 3 escenas disponibles (formato RGBA)
 * 
 * Cada escena tiene definido un color distinto:
 * - Escena 0: Verde azulado oscuro (0.2, 0.3, 0.3)
 * - Escena 1: Naranja (1.0, 0.643, 0.0)
 * - Escena 2: Verde azulado claro (0.0, 1.0, 0.655)
 */
const WindowBackground SCENE_BACKGROUND[3][4] = {
    { .2f, .3f, .3f, 1.0f},    // Escena 0
    { 1.0f, .643f, .0f, 1.0f},  // Escena 1
    { .0f, 1.0f, .655f, 1.0f}   // Escena 2
};

/**
 * @brief Punto de entrada principal del programa
 * @return 0 en éxito, -1 en error
 * 
 * Gestiona el ciclo de vida completo de la aplicación:
 * 1. Inicialización
 * 2. Configuración
 * 3. Loop principal
 * 4. Limpieza
 */
int main(){
    // Inicialización de GLFW y creación de ventana
    initializeGlfw();
    GLFWwindow* window = getWindowObject();
    Figure* figure = getFiguresShapes();

    // Debug: Mostrar ID del VBO
    cout << "VBO ID: " << figure[0].VBO << endl;

    if (window == NULL) {
        cout << "Error creating window object";
        return -1;
    }

    // Configurar callback de teclado
    glfwSetKeyCallback(window, keyCallbackListener);

    // Configuración de shaders
    configureShader(figure[WindowSceneDisplay].vertexShader, *vertexShaderSource);
    configureShader(figure[WindowSceneDisplay].fragmentShader, *fragmentShaderSource);

    // Creación y enlace del programa shader
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, figure[WindowSceneDisplay].vertexShader);
    glAttachShader(shaderProgram, figure[WindowSceneDisplay].fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Loop principal de renderizado
    while (!glfwWindowShouldClose(window)) {
        // Establecer color de fondo según escena actual
        glClearColor(
            SCENE_BACKGROUND[WindowSceneDisplay][0], 
            SCENE_BACKGROUND[WindowSceneDisplay][1], 
            SCENE_BACKGROUND[WindowSceneDisplay][2], 
            1.0f
        );
        glClear(GL_COLOR_BUFFER_BIT);

        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza final de recursos
    glDeleteShader(figure[WindowSceneDisplay].vertexShader);
    glDeleteShader(figure[WindowSceneDisplay].fragmentShader);
    free(figure);
    glfwTerminate();
    return 0;
}

/**
 * @brief Inicializa GLFW con configuración básica
 * @details Configura versión OpenGL 3.3 Core Profile
 *          y compatibilidad con macOS si es necesario
 */
void initializeGlfw() {
    // Inicializar biblioteca GLFW
    glfwInit();
    
    // Configurar versión y perfil de OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Configuración especial para compatibilidad con macOS
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
}

/**
 * @brief Crea y configura la ventana GLFW
 * @return Puntero a la ventana o NULL en error
 * @details Además configura:
 * - Contexto OpenGL
 * - Callback de redimensionamiento
 * - Carga funciones OpenGL con GLAD
 */
GLFWwindow* getWindowObject() {
    // Crear ventana con dimensiones y título especificados
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGH, "OPENGL - TALLER 1", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return NULL;
    }

    // Establecer contexto OpenGL para esta ventana
    glfwMakeContextCurrent(window);
    
    // Configurar callback para redimensionamiento
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Cargar punteros a funciones OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }

    return window;
}

/**
 * @brief Callback para redimensionamiento de ventana
 * @param window Ventana que generó el evento
 * @param width Nuevo ancho en píxeles
 * @param height Nuevo alto en píxeles
 * @details Ajusta el viewport para mantener proporciones correctas
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}   

/**
 * @brief Callback para eventos de teclado
 * @param window Ventana que recibió el evento
 * @param key Código de tecla
 * @param scanCode Código de escaneo físico
 * @param action Acción (presionar, soltar, mantener)
 * @param mods Teclas modificadoras
 * @details Maneja:
 * - Flecha izquierda: Escena anterior
 * - Flecha derecha: Siguiente escena
 * - ESC: Cerrar aplicación
 */
void keyCallbackListener(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_LEFT:  // Tecla izquierda: escena anterior
                if (WindowSceneDisplay > 0) {
                    WindowSceneDisplay--;
                } 
                break;
            case GLFW_KEY_RIGHT: // Tecla derecha: siguiente escena
                if (WindowSceneDisplay < 2) {
                    WindowSceneDisplay++;
                } 
                break;
            case GLFW_KEY_ESCAPE: // Tecla ESC: cerrar ventana
                glfwSetWindowShouldClose(window, true);
                break;
        }
        cout << "Escena actual: " << WindowSceneDisplay << endl;
    }
}

/**
 * @brief Crea y configura las figuras geométricas
 * @return Puntero a arreglo de figuras o NULL en error
 * @note Actualmente solo inicializa un triángulo pero reserva espacio para 3 figuras
 */
Figure* getFiguresShapes() {
    // Reservar memoria para 3 figuras (aunque solo usamos 1 inicialmente)
    Figure* figures = (Figure*)malloc(sizeof(Figure)*3);

    if (figures == NULL) {
        cout << "Error de asignación de memoria" << endl;
        return NULL; 
    } 

    // Configurar primera figura (triángulo)
    figures[0] = (Figure){
        .figureVertex = {
            -0.5f, -0.5f , 0.0f,  // Vértice inferior izquierdo
            0.5f, -0.5f, 0.0f,     // Vértice inferior derecho
            0.0f, 0.5f, 0.0f       // Vértice superior central
        },
        .vertexShader = glCreateShader(GL_VERTEX_SHADER),
        .fragmentShader = glCreateShader(GL_FRAGMENT_SHADER)
    };

    // Generar y configurar VBO (Vertex Buffer Object)
    glGenBuffers(1, &figures[0].VBO);
    glBindBuffer(GL_ARRAY_BUFFER, figures[0].VBO);

    return figures;
}

/**
 * @brief Configura y compila un shader OpenGL
 * @param shader ID del shader a configurar
 * @param source Primer carácter del código fuente
 * @warning Implementación simplificada - solo usa primer carácter del source
 * @note En una implementación real debería manejar strings completos y verificar errores
 */
void configureShader(unsigned int shader, char source) {
    char* shaderSource = &source;
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
}