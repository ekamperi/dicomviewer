uniform sampler2D mysampler;
varying vec2 myTexCoord;

uniform float tmin;
uniform float tmax;

float transfer(float x, float tmin, float tmax)
{
    return smoothstep(tmin, tmax, x);
}

void main()
{
    vec4 color = texture2D(mysampler, myTexCoord);
    float val = transfer(color, tmin, tmax);
    gl_FragColor = val;
//    gl_FragColor.r = 1.0;
}
