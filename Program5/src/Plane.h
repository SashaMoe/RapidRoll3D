class Plane{

private:
    glm::mat4 translation;
    glm::vec3 location;
    glm::vec4 bound;
    float lowY;
    float highY;
    
public:
    Plane(){
        location = glm::vec3(0,0,0);
        translation = glm::translate(glm::mat4(1), location);
        bound = glm::vec4(0);
    }
    
    void init(glm::vec4 b, float h, float l){
        bound = b;
        highY = h;
        lowY = l;
    }
    
    glm::vec3 getLocation(){
        return location;
    }
    
    glm::mat4 getTranslation(){
        return this->translation;
    }
    
    void translate(glm::mat4 trans){
        this->translation = trans*this->translation;
        this->location = glm::vec3(trans*glm::vec4(location, 1));
    }
    
    glm::vec4 getBound(){
        glm::vec4 newBound;
        newBound.x = bound.x + location.x;
        newBound.y = bound.y + location.x;
        newBound.z = bound.z + location.z;
        newBound.w = bound.w + location.z;
        return newBound;
    }
    
    float getHighY(){
        return highY + location.y;
    }
    
    float getLowY(){
        return lowY + location.y;
    }
    
};