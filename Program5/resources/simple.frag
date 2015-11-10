#version 330

uniform sampler2D texSampler;
uniform mat4 P;
uniform mat4 C;
uniform mat4 mT;
uniform mat4 trans;
uniform mat4 mR;
uniform mat4 L;
uniform vec4 lightPos;
uniform vec4 camPos;
uniform int shadingMode;

smooth in vec4 smoothColor;
in vec4 posFrag;
in vec3 normalFrag;
in vec2 texMapping;

out vec4 fragColor;

void main()
{
    if (shadingMode!=0&&shadingMode!=1) {
        vec4 normal = normalize(mR*mT*trans*vec4(normalFrag,0));
        vec4 pos_transformed = normalize(mR*mT*trans*posFrag);
        vec4 light_vec = normalize(L*lightPos-pos_transformed);
        
        vec4 r = normalize(reflect(-light_vec, normal));
        vec4 v = normalize(camPos-pos_transformed);
        
        float diffuse = clamp(dot(light_vec, normal),0,1);
        float specular = clamp(dot(r, v),0,1);
        
        vec4 Ia = vec4(vec3(0.1)*vec3(1),1);
        vec4 Id = vec4(normalFrag, 1)*diffuse;
        vec4 Is = vec4(1)*pow(specular,10);
        fragColor = Ia+Id+Is;
    }else{
        fragColor = smoothColor;
    }
    
    fragColor = texture(texSampler, texMapping);
}
