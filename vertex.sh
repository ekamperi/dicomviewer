varying vec2 myTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    //gl_Position = ftransform();
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * gl_Vertex;
//    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(gl_Vertex, 1);
    myTexCoord = gl_MultiTexCoord0.xy;
}
