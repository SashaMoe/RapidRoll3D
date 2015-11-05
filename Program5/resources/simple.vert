#version 330

uniform mat4 P;
uniform mat4 C;
uniform mat4 mT;
uniform mat4 mR;
uniform mat4 M;
uniform mat4 N;
uniform mat4 L;
uniform vec4 lightPos;
uniform vec4 camPos;
uniform int shadingMode;
uniform mat4 trans;

in vec3 pos;
in vec3 colorIn;


smooth out vec4 smoothColor;
out vec4 actualPos;


vec4 justColor()
{
    return  vec4(colorIn, 1);
}

vec4 computeIa(vec4 Ia, vec4 Ka){
   
    return Ia * Ka;
}


vec4 computeId(vec4 N, vec4 L, vec4 Kd){
    return Kd * dot(N,L);
}


vec4 computeIs(vec4 color, vec4 normal, vec4 direction){

    
    vec4 r = reflect(direction,normal);
    
    r  = normalize(r);
    
    vec4 v =  P * camPos -  M * vec4(pos,1);
    
    v = normalize(v);
    
    vec4 Is = vec4(1) *  pow( max( dot(r,v) , 0)   ,10) ;
    
    return Is;

}


vec4 phong()
{
    vec4 color = vec4(colorIn, 1);
    return color;
}

vec4 gouraud(vec4 normal,vec4 direction)
{
    vec4 color = vec4(colorIn,1);
    
    vec4 id = computeId(normal,direction,color);
    vec4 ia = computeIa(vec4(1),vec4(0.1,0.1,0.1,0.1));
    vec4 is = computeIs(color,normal,direction);
   
    vec4 I = ia+is+id;
    
    return I;
    
}

void main()
{
    
    vec4 pos = vec4(pos, 1);
   
    gl_Position = P*M*trans*pos;

  
    
    vec4 posActual = mR * mT * trans *  pos;
    
    vec4 lpActual = L * lightPos;
    
    vec4 lightVector = lpActual - posActual;
    
    actualPos = posActual;
    
    //TODO add gouraud and phong shading support
    
    vec4 normal = vec4(colorIn,1)*2 - 1;
    
    normal = mR * normal;
    
    vec4 normalActual = normal;
    //vec4 normal = vec4(colorIn,1);
    
    vec4 direction = normalize(lightVector);

    normalActual = normalize(normalActual);

    
    
    if(shadingMode == 0)
        smoothColor = justColor();
    else if (shadingMode == 1)
        smoothColor = gouraud(normalActual,direction);
    else
        smoothColor = phong();
}
