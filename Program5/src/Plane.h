class Plane{

private:
    glm::mat4 translation;
    glm::vec3 location;
    glm::vec3 transStep;

    
public:
    Plane(){
        location = glm::vec3(0,0,0);
        transStep = glm::vec3(0,0.1,0);
        translation = glm::translate(glm::mat4(1), location);
    }
    
    
    void timeStep(){
        location += transStep;
        if(location.y > 20){
            location = glm::vec3(0,-30,0);
        }
        translation = glm::translate(glm::mat4(1), location);
    }
    
    glm::vec3 getTrans(){
        return location;
    }
    
    glm::mat4 getTranslation(){
        return this->translation;
    }
    
};