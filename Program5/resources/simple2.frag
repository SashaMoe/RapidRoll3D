#version 330

uniform mat4 P;
uniform mat4 C;
uniform mat4 mT;
uniform mat4 mR;
uniform mat4 L;
uniform vec4 lightPos;
uniform vec4 camPos;
uniform int shadingMode;

smooth in vec4 smoothColor;
in vec4 actualPos;


out vec4 fragColor;


vec4 computeIa(vec4 Ia, vec4 Ka){
    
    return Ia * Ka;
}


vec4 computeId(vec4 N, vec4 L, vec4 Kd){
    return Kd * dot(N,L);
}


vec4 computeIs(vec4 color, vec4 normal, vec4 direction){
    
    //vec4 r = 2 * dot(direction,normal) * normal - direction ;
    
    //r = normalize(r);
    
    vec4 r = reflect(direction,normal);
    
    r  = normalize(r);
    
    vec4 v =  P * camPos -  C * actualPos;
    
    v = normalize(v);
    
    vec4 Is = vec4(1) *  pow( max( dot(r,v) , 0)   ,10) ;
    
    return Is;
    
}

vec4 phong(){
    
    vec4 normal =  smoothColor*2 - 1;
    normal = mR * normal;
    normal = normalize(normal);
    
    vec4 lpActual = L * lightPos;
    vec4 lightVector = lpActual - actualPos;
    vec4 direction = normalize(lightVector);

    vec4 color = smoothColor;
    
    vec4 id = computeId(normal,direction,color);
    vec4 ia = computeIa(vec4(1),vec4(0.1,0.1,0.1,0.1));
    vec4 is = computeIs(color,normal,direction);
    
    vec4 I = ia+is+id;
    
    return I;

}



void main()
{
    if (shadingMode == 2) {
        fragColor = phong();
    }
    else{
        fragColor = smoothColor;
    }
    //TODO add gouraud and phong shading support
}
