class Plane{

private:
    glm::mat4 translation;
    glm::vec3 location;
    glm::vec3 transVec;
    glm::vec3 transStep;

    
public:
    Plane(){
        location = glm::vec3(0,-5,0);
        transVec = glm::vec3(0,0,0);
        transStep = glm::vec3(0,0.1,0);
        translation = glm::translate(glm::mat4(1), transVec + location);
    };
    
    
    void timeStep(){
        transVec += transStep;
        if(transVec.y > 20){
            transVec = glm::vec3(0,0,0);
        }
        translation = glm::translate(glm::mat4(1), transVec + location);
    }
    
    glm::vec3 getTrans(){
        return location + transVec;
    }
    
    glm::mat4 getTranslation(){
        return this->translation;
    }
    
};