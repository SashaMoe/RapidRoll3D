#version 330

uniform mat4 P;
uniform mat4 C;
uniform mat4 mT;
uniform mat4 trans;
uniform mat4 mR;
uniform mat4 M;
uniform mat4 N;
uniform mat4 L;
uniform vec4 lightPos;
uniform vec4 camPos;
uniform int shadingMode;
uniform vec2 resolution;
uniform sampler2D texId;
smooth in vec4 smoothColor;
uniform float elapsedTime;
uniform int enable;

//out vec4 fragColor;
layout(location = 0) out vec4 fragColor;

int kernelSize = 27;  //larger kernels will blur more, but run slower
int halfSize = kernelSize / 2;

vec2 res = vec2(resolution);
vec2 fragCoord = gl_FragCoord.xy;
vec2 texCoord = fragCoord/res;


vec4 swirl()
{
    float range = 1000000.0;
    
    vec2 toFrag = fragCoord;
    
    
    
    
    float dis = length(toFrag-res/2);

    float angle = 1.4;

    if(dis>range)
        return texture(texId, texCoord);
    
    float scale = ( pow(1- dis/range,cos(elapsedTime)*5))*5;
    
    mat2 rotZ = mat2(cos(scale), sin(scale), -sin(scale), cos(scale));
    mat2 rot2 = mat2(cos(angle), sin(angle), -sin(angle), cos(angle));

    vec2 swlCoord = rot2 * rotZ * toFrag;
    
    
    return texture(texId, swlCoord/res*3);
}




void main()
{
    
    fragColor = smoothColor;
    //fragColor = vec4(texCoord, 1, 1);
    
    if (enable==0) {
        fragColor = texture(texId, texCoord);
    }else{
        fragColor = swirl();
    }
    
}