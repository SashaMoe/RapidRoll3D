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

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 colorIn;

out vec4 posFrag;
out vec3 normalFrag;
out vec2 texMapping;
smooth out vec4 smoothColor;

vec4 justColor()
{
    return vec4(colorIn, 1);
}

vec4 gouraud()
{
    vec4 normal = normalize(mR*mT*trans*vec4(colorIn,0));
    vec4 pos_transformed = normalize(mR*mT*trans*vec4(pos,1));
    vec4 light_vec = normalize(L*lightPos-pos_transformed);
    
    vec4 r = normalize(reflect(-light_vec, normal));
    vec4 v = normalize(camPos-pos_transformed);
    
    float diffuse = clamp(dot(light_vec, normal),0,1);
    float specular = clamp(dot(r, v),0,1);
    
    vec4 Ia = vec4(vec3(0.1)*vec3(1),1);
    vec4 Id =justColor()*diffuse;
    vec4 Is =vec4(1)*pow(specular,10);
    return Ia+Id+Is;
}

vec4 phong()
{
    return vec4(1, 1, 1, 1);
}

void main()
{
    vec4 pos = vec4(pos, 1);
    gl_Position = P*M*trans*pos;
    
    if(shadingMode == 0)
    {
        posFrag = pos;
        normalFrag = colorIn;
        smoothColor = justColor();
    }
    else if (shadingMode == 1)
    {
        posFrag = pos;
        normalFrag = colorIn;
        smoothColor = gouraud();
    }
    else
    {
        posFrag = pos;
        normalFrag = colorIn;
        smoothColor = phong();
    }
    
    texMapping = texCoord;
}
