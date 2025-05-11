#version 330 core
out vec4 FragColor;

uniform vec4 uColor = vec4(0.0f, 0.0f, 1.0f, 1.0f); // Valor por defecto azul

void main()
{
   FragColor = uColor;
}