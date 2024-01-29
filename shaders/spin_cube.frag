#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D textureSide;

void main() {
    FragColor = texture(textureSide, TexCoord);
}