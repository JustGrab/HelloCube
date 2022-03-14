#shader vertex
#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 fullTransformMatrix;
out vec3 theColor;
void main()
{
   vec4 v = vec4(position, 1.0);
   gl_Position = fullTransformMatrix * v;
  
   theColor = color;
};

#shader fragment
#version 330 core

out vec4 color;
in vec3 theColor;

void main()
{
	color = vec4(theColor, 1.0);
};