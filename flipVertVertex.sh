varying vec2 myTexCoord;

void main()
{
    gl_Position = ftransform();
    myTexCoord = gl_MultiTexCoord0.xy ;
}
