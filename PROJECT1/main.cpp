// Inclusión de bibliotecas necesarias para OpenGL y GLFW
#include "glad/glad.h"  // Cargador de funciones OpenGL
#include <GLFW/glfw3.h> // Biblioteca para manejo de ventanas y entrada

#include <iostream>     // Para salida de consola

using namespace std;

// Definición de tipos para mejorar la legibilidad
typedef unsigned int WindowSize;         // Tipo para dimensiones de ventana
typedef unsigned short int SceneRenderer;// Tipo para índice de escena
typedef float WindowBackground;         // Tipo para componentes de color
typedef float VertexArray[];            // Tipo para definir un array de vertices de tipo float

// Constantes de configuración
const WindowSize HEIGH = 600;  // Altura de la ventana
const WindowSize WIDTH = 800;  // Ancho de la ventana

SceneRenderer WindowSceneDisplay = 0;  // Índice de la escena actual (0-2)

// Declaraciones de funciones
GLFWwindow* getWindowObject();  // Crea y configura la ventana GLFW
void initializeGlfw();          // Inicializa GLFW con configuraciones básicas
void framebuffer_size_callback(GLFWwindow* window, int width, int height); // Callback para redimensionamiento
void keyCallbackListener(GLFWwindow *window, int key, int scanCode, int action, int mods); // Callback para teclas

// Configuraciones de color de fondo para diferentes escenas
const WindowBackground SCENE_BACKGROUND[3][4] = {
    { .2f, .3f, .3f, 1.0f},    // Escena 0: Verde azulado oscuro
    { 1.0f, .643f, .0f, 1.0f},  // Escena 1: Naranja
    { .0f, 1.0f, .655f, 1.0f}   // Escena 2: Verde azulado claro
};

int main(){
    // Inicialización de GLFW y creación de ventana
    initializeGlfw();
    GLFWwindow* window = getWindowObject();
    
    if (window == NULL) {
        cout << "Error creating window object";
        return -1;  // Salir si hay error
    }

    // Configurar callback para eventos de teclado
    glfwSetKeyCallback(window, keyCallbackListener);
    
    // Loop principal de renderizado
    while (!glfwWindowShouldClose(window)) {
        // Establecer color de fondo según la escena actual
        glClearColor(
            SCENE_BACKGROUND[WindowSceneDisplay][0], 
            SCENE_BACKGROUND[WindowSceneDisplay][1], 
            SCENE_BACKGROUND[WindowSceneDisplay][2], 
            1.0f
        );
        glClear(GL_COLOR_BUFFER_BIT);  // Limpiar el buffer de color

        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpieza final
    glfwTerminate();
    return 0;
}

/**
 * Inicializa la biblioteca GLFW con configuraciones básicas
 */
void initializeGlfw() {
    // Inicializar GLFW
    glfwInit();
    
    // Configurar versión de OpenGL (3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Configuración especial para macOS
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
}

/**
 * Crea y configura la ventana GLFW
 * @return Puntero a la ventana creada o NULL en caso de error
 */
GLFWwindow* getWindowObject() {
    // Crear ventana con dimensiones predefinidas
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGH, "OPENGL - TALLER 1", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window " << endl;
        glfwTerminate();
        return NULL;
    }

    // Establecer contexto OpenGL y callback para redimensionamiento
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Cargar funciones OpenGL con GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return NULL;
    }

    return window;
}

/**
 * Callback para cuando la ventana cambia de tamaño
 * @param window Ventana que se redimensionó
 * @param width  Nuevo ancho
 * @param height Nuevo alto
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Ajustar viewport para que coincida con las nuevas dimensiones
    glViewport(0, 0, width, height);
}   

/**
 * Callback para eventos de teclado (maneja flechas y ESC)
 * @param window   Ventana que recibió el evento
 * @param key      Tecla presionada
 * @param scanCode Código de escaneo del teclado
 * @param action   Acción (presionar, soltar, mantener)
 * @param mods     Teclas modificadoras (Shift, Ctrl, etc)
 */
void keyCallbackListener(GLFWwindow* window, int key, int scanCode, int action, int mods) {
    // Solo procesar cuando se presiona una tecla
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
        cout << "Procesando " << WindowSceneDisplay << endl;
    }
}