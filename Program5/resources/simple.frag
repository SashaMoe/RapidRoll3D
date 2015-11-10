#version 330

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

out vec4 fragColor;


//vec4 swirl()
//{
//    float range = 10000.0;
//    
//    vec3 toFrag = posFrag.xyz;
//    
//
//    
//    
//    float dis = length(toFrag);
//    
//    if(abs(dis-range)<2){
//        return vec4(0,0,0,0);
//    }
//    
//    
//    float scale = ( pow(1-dis/range,cos(elapsedTime)*5))*5;
//    
//    mat2 rotZ = mat2(cos(scale), sin(scale), -sin(scale), cos(scale));
//    vec2 swlCoord = rotZ * toFrag.xy;
//    return texture(texId, (cursorAbsolutePos+swlCoord)/res);
//}






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
        vec4 Id = vec4(normalFrag*0.5+0.5, 1)*diffuse;
        vec4 Is = vec4(1)*pow(specular,10);
        fragColor = Ia+Id+Is;
    }else{
        fragColor = smoothColor;
    }
    
    
}
