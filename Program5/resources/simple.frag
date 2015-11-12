#version 330

uniform sampler2D texSampler;
uniform mat4 P;
uniform mat4 C;
uniform mat4 mT;
uniform mat4 trans;
uniform mat4 mR;
uniform mat4 L;
uniform vec4 lightPos;
uniform vec4 lightPos1;
uniform vec4 lightPos2;
uniform vec4 camPos;
uniform vec4 camLook;
uniform int shadingMode;
uniform int discoMode;
uniform int type;
uniform float timeElapsed;

smooth in vec4 smoothColor;
in vec4 posFrag;
in vec3 normalFrag;
in vec2 texMapping;

out vec4 fragColor;


vec4 computeIa(){
    return vec4(vec3(0.1)*vec3(1),1);
}

float computeDiffuse(vec4 normal, vec4 lp, vec4 pos_transformed ){
    vec4 light_vec = normalize(L*lp-pos_transformed);
    return clamp(dot(light_vec, normal),0,1);
}


//vec4 phong(vec4 lp,vec4 color){
//    vec4 normal = normalize(mR*mT*trans*vec4(normalFrag,0));
//    vec4 pos_transformed = normalize(mR*mT*trans*posFrag);
//    vec4 light = L * lp - pos_transformed;
//    vec4 light_vec = normalize(L*lp-pos_transformed);
//    
//    float distance = length(light);
//    
//    
//    vec4 r = normalize(reflect(-light_vec, normal));
//    vec4 v = normalize(camPos-pos_transformed);
//    float diffuse = clamp(dot(light_vec, normal),0,1);
//    float specular = clamp(dot(r, v),0,1);
//    vec4 Ia = vec4(vec3(0.15)*vec3(1),1);
//    vec4 Id = color*diffuse;
//    vec4 Is = vec4(1)*pow(specular,10);
//    return  Ia + Id;
//
//}


vec4 computeIs(vec4 lp,vec4 normal, vec4 pos_transformed){
    vec4 light_vec = normalize(L*lp-pos_transformed);
    vec4 r = normalize(reflect(-light_vec, normal));
    vec4 v = normalize((camPos-camLook)-pos_transformed);
    float specular = clamp(dot(r, v),0,1);
    vec4 Is = vec4(1)*pow(specular,10);
    return Is;

}


void main()
{
    vec4 color;
    if(type==1){
        color = texture(texSampler, texMapping);
    }else if(type==3){
        color = texture(texSampler, texMapping)*smoothColor;
    }else{
        color = smoothColor;
    }
    
    if (shadingMode==1) {
        vec4 normal = normalize(mR*mT*trans*vec4(normalFrag,0));
        vec4 pos_transformed = normalize(mR*mT*trans*posFrag);
        
        vec4 ia = computeIa();
        float d = computeDiffuse(normal,lightPos,pos_transformed);
        float d1 = computeDiffuse(normal,lightPos1,pos_transformed);
        float d2 = computeDiffuse(normal,lightPos2,pos_transformed);
        vec4 id = color * max(d2,max(d,d1));
        
        vec4 is = computeIs(lightPos,normal,pos_transformed);
        vec4 is1 = computeIs(lightPos1,normal,pos_transformed);
        vec4 is2 = computeIs(lightPos2,normal,pos_transformed);
        
        
        if(discoMode==1){
            fragColor = ia+id+is+is1+is2;

        }
        else{
            fragColor = ia+color*d+is;

        }
        
        
        
        
        
//        vec4 normal = normalize(mR*mT*trans*vec4(normalFrag,0));
//        vec4 pos_transformed = normalize(mR*mT*trans*posFrag);
//        vec4 light = L * lightPos - pos_transformed;
//        vec4 light_vec = normalize(L*lightPos-pos_transformed);
//        
//        float distance = length(light);
//        
//        
//        vec4 r = normalize(reflect(-light_vec, normal));
//        vec4 v = normalize(camPos-pos_transformed);
//        float diffuse = clamp(dot(light_vec, normal),0,1);
//        float specular = clamp(dot(r, v),0,1);
//        vec4 Ia = vec4(vec3(0.1)*vec3(1),1);
//        vec4 Id = color*diffuse;
//        vec4 Is = vec4(100/distance)*pow(specular,10);
//        fragColor = Ia+Id+Is;
    }else{
        fragColor = color;
    }
}
