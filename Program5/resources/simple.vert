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
uniform vec4 lightPos1;
uniform vec4 lightPos2;
uniform vec4 camPos;
uniform int shadingMode;
uniform int discoMode;
uniform int type;
uniform float timeElapsed;
uniform vec4 camLook;
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 colorIn;

out vec4 posFrag;
out vec3 normalFrag;
out vec2 texMapping;
smooth out vec4 smoothColor;

vec4 justColor()
{
    if(type==2){
        if(colorIn.y>0.9){
            return vec4(0.88,0.87,0.86,1);
        }else{
            return vec4(0.588,0.313,0.176,1);
        }
    }else if(type==3){
        return vec4(1, 1-timeElapsed, 1-timeElapsed, 1);
    }else if(type==4){
        return vec4(0.88,0.87,0.86,1);
    }else{
        return vec4(colorIn, 1);
    }
}

void main()
{
    vec4 pos = vec4(pos, 1);
    gl_Position = P*M*trans*pos;
    
    posFrag = pos;
    normalFrag = colorIn* 2 -1;
    smoothColor = justColor();
    
    texMapping = texCoord;
}
