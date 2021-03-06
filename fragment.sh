uniform sampler2D mysampler;
varying vec2 myTexCoord;

uniform float tmin;
uniform float tmax;

vec4 transfer(vec4 x, float tmin, float tmax)
{
    return smoothstep(tmin, tmax, x);
}

void main()
{
    vec4 color = texture2D(mysampler, myTexCoord);
    vec4 val = transfer(color, tmin, tmax);
    gl_FragColor = val;
}
