uniform sampler2D mysampler;
varying vec2 myTexCoord;

float transfer(float x, float tmin, float tmax)
{
    return smoothstep(tmin, tmax, x);
}

void main()
{
    vec4 color = texture2D(mysampler, myTexCoord);

    float val = transfer(color, tmin, tmax);

    gl_FragColor = color;
    gl_FragColor.r = 1.0;
}
