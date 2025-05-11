#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    // constructor que genera el shader al vuelo
    // ----------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        // 1. Obtener el código fuente de los shaders desde los archivos
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // configurar ifstream para que lance excepciones:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // abrir archivos
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // leer contenido del buffer a los streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // cerrar archivos
            vShaderFile.close();
            fShaderFile.close();
            // convertir stream a string
            vertexCode   = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::ARCHIVO_NO_LEIDO_CORRECTAMENTE: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        // 2. Compilar shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // programa de shaders
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // eliminar los shaders, ya están vinculados al programa y no son necesarios
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    // activar el shader
    // ----------------------------------------------------------------
    void use() 
    { 
        float timeValue = glfwGetTime();
        
        // Generar valores de color que varían con el tiempo
        float redValue   = (sin(timeValue * 1.5f) * 0.5f) + 0.5f;  // Rango 0-1
        float greenValue = (sin(timeValue * 2.0f) * 0.5f) + 0.5f;  // Frecuencia diferente
        float blueValue  = (sin(timeValue * 1.0f) * 0.5f) + 0.5f; // Frecuencia base
        
        // Obtener la ubicación del uniform y asignar el color
        int vertexColorLocation = glGetUniformLocation(ID, "uColor");
        glUseProgram(ID);
        glUniform4f(vertexColorLocation, redValue, greenValue, blueValue, 1.0f);
    }
    // funciones de utilidad para uniforms
    // ----------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ----------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ----------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }

private:
    // función de utilidad para verificar errores de compilación/enlazado
    // ----------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER::ERROR_DE_COMPILACION de tipo: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER::ERROR_DE_ENLACE de tipo: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif