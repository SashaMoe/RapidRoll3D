#ifndef __WORLDSTATE_H
#define __WORLDSTATE_H
#include "Model.h"

#define NUM_TRACKED_FRAMES 10

class WorldState
{
private:
    float speed = 0.1f;
    float acceleration = 9.8f;
    float mouseSensitive = 0.01f;
	float frameTimes[NUM_TRACKED_FRAMES];
	float currentTime;
	bool running;
	Model model;
    int shadingMode;
    Model model2;
    glm::vec3 cameraPos;
    glm::vec3 cameraLook;
    glm::vec3 cameraUp;
    
    glm::vec4 lightPos;
    glm::vec3 lightIntensity;
    
    glm::mat4 lightRotate;
    glm::mat4 lightIncrement;
    glm::mat4 modelRotate;
    glm::mat4 modelIncrement;
    glm::mat4 modelTranslate;
    
    glm::mat4 figureRotate;
    glm::mat4 figureTranslate;
    
    glm::mat4 planeTranslate;
    
    glm::mat4 cameraMatrix;
    
    int mousePosX;
    int mousePosY;
	
    bool lightRotating;
    bool modelRotating;
    bool figureDrop;
    float dropV = 0;

public:
	WorldState()
	{
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = 0.0f;
        
        shadingMode = 0;
		running = true;
		model = Model();
        model.init("resources/sphere.obj");
		model2.init("resources/BluePlane.obj");
		glm::vec3 center = model.getCentroid();
		glm::vec3 max = model.getMaxBound();
		glm::vec3 min = model.getMinBound();
		glm::vec3 dim = model.getDimension();
        printf("model loaded: bounds");
		printf("[%.2f %.2f %.2f]..", min[0], min[1], min[2]);
		printf("[%.2f %.2f %.2f] ", max[0], max[1], max[2]);
		printf("= dim [%.2f %.2f %.2f]\n", dim[0], dim[1], dim[2]);
		float camDistance = std::max(dim[0], dim[1]);
		cameraPos = glm::vec3(0,max[1],camDistance);
        cameraLook = glm::vec3(0,0,0);
        cameraUp = glm::vec3(0,1,0);
        
		lightPos = glm::vec4((max-center)*1.3f, 1);
        lightIntensity = glm::vec3(1,1,1);
        
        lightRotate = glm::mat4(1);
        lightIncrement = glm::rotate(glm::mat4(1), -0.05f, glm::vec3(0,1,0));
        
        modelRotate = glm::mat4(1);
        modelIncrement = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
        modelTranslate = glm::translate(glm::mat4(1), -model.getCentroid());
        
        figureRotate = glm::mat4(1);
        figureTranslate = glm::mat4(1);
        
        planeTranslate = glm::translate(glm::mat4(1), glm::vec3(0,-5,0));
        
        mousePosX = 256;
        mousePosY = 256;
		
		lightRotating = false;
		modelRotating = false;
        figureDrop = false;
	}
	
	void updateFrameTime(float timeAsSeconds)
	{
		for(size_t i=1; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = frameTimes[i-1];
		frameTimes[0] = timeAsSeconds;
	}
	
	void printFPS() const
	{
		float sum = 0.0f;
		float avg = 0.0f;
		float fps = 0.0f;
		
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			sum += frameTimes[i];
		
		avg = sum / NUM_TRACKED_FRAMES;
		fps = 1.0f / avg;
		printf("fps %f\n", fps);
	}
	
	Model const & getModel() const
	{ return model; }
	

 
    
	void setRunning(bool r)
	{ running = r; }

	bool isRunning() const
	{ return running; }

	float getCurrentTime() const
	{ return this->currentTime; }

	void timeStep(float t)
	{
		float elapsed = t - this->currentTime;
		this->updateFrameTime(elapsed);
        
		//spin light
		if(lightRotating)
			lightRotate = lightIncrement * lightRotate;
        
        //spin model
		if(modelRotating)
			modelRotate = modelIncrement * modelRotate;
        
        // figure drop
        if (figureDrop) {
            dropFigure(std::max(elapsed, 0.0f));
        }
		
		this->currentTime = t;
	}
    
    void dropFigure(float t){
        if (checkFigureReachPlane(t)) {
            // stay on plane
            stickToPlane();
        }else{
            // drop
            float newDropV = dropV + t*9.8f;
            float dis = (newDropV*newDropV-dropV*dropV)/(2*9.8f);
            glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(0, -dis, 0));
            moveFigure(trans);
            dropV = newDropV;
            printf("drop\n");
        }
    }
    
    bool checkFigureReachPlane(float t){
        float newDropV = dropV + t*acceleration;
        float dis = (newDropV*newDropV-dropV*dropV)/(2*acceleration);
        glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(0, -dis, 0));
        
        glm::vec4 figureBound = model.getBound();
        glm::vec4 planeBound = model2.getBound();
        float figureXMin = figureBound.x + figureTranslate[3][0];
        float figureXMax = figureBound.y + figureTranslate[3][0];
        float figureZMin = figureBound.z + figureTranslate[3][2];
        float figureZMax = figureBound.w + figureTranslate[3][2];
        float planeXMin = planeBound.x + planeTranslate[3][0];
        float planeXMax = planeBound.y + planeTranslate[3][0];
        float planeZMin = planeBound.z + planeTranslate[3][2];
        float planeZMax = planeBound.w + planeTranslate[3][2];
        
        if (figureXMin>planeXMax || figureXMax<planeXMin || figureZMin>planeZMax || figureZMax<planeZMin) {
            return false;
        }
        
        printf("%f %f %f %f, %f %f %f %f\n", figureXMin, figureXMax, figureZMin, figureZMax, planeXMin, planeXMax, planeZMin, planeZMax);
        glm::vec3 figurePoint = model.getLowestPoint();
        glm::vec3 planePoint = model2.getHighestPoint();
        float figureY = figurePoint.y + trans[3][1] + figureTranslate[3][1];
        float planeY = planePoint.y + planeTranslate[3][1];
        //glm::vec3 translatedFigure = glm::vec3(trans * figureTranslate * glm::vec4(figurePoint, 1));
        //glm::vec3 translatedPlane = glm::vec3(planeTranslate * glm::vec4(planePoint, 1));
        
        if (figureY<planeY) {
            return true;
        }
        return false;
    }
    
    void stickToPlane(){
        glm::vec3 figurePoint = model.getLowestPoint();
        glm::vec3 planePoint = model2.getHighestPoint();
        glm::vec3 translatedFigure = glm::vec3(figureTranslate * glm::vec4(figurePoint, 1));
        glm::vec3 translatedPlane = glm::vec3(planeTranslate * glm::vec4(planePoint, 1));
        if (translatedFigure.y-translatedPlane.y>0.01) {
            glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(0,translatedFigure.y-translatedPlane.y,0));
            moveFigure(trans);
            dropV = 0;
            printf("stop\n");
        }
        printf("%f\n", translatedFigure.y-translatedPlane.y);
        printf("stick\n");
    }
	
	Model & getModel()
	{ return model; }
	
    Model & getModel2()
    { return model2; }
    
	glm::mat4 getModelTranslate() const
	{ return modelTranslate; }
    
    glm::mat4 getFigureTranslate() const
    { return figureTranslate; }
    
    glm::mat4 getPlaneTranslate() const
    { return planeTranslate; }
    
    glm::mat4 getModelRotate() const
    { return modelRotate; }
    
    glm::mat4 getFigureRotate() const
    { return figureRotate; }
    
    glm::mat4 getLightRotate() const
    { return lightRotate; }
    
    glm::vec4 getLightPos() const
    { return this->lightPos; }
    
    glm::vec3 getLightIntensity() const
    { return this->lightIntensity; }
    
    glm::mat4 getCameraMatrix() const
    { return glm::lookAt(cameraPos, cameraLook, cameraUp);; }
    
    void setShadingMode(int m)
    { this->shadingMode = m; }
    
    int getShadingMode() const
    { return this->shadingMode; }
    
    glm::vec4 getCameraPos() const
    { return glm::vec4(this->cameraPos, 1); }
	
	void toggleModelRotate()
	{ modelRotating = !modelRotating; }
	
	void toggleLightRotate()
	{ lightRotating = !lightRotating; }
    
    void toggleFigureDrop()
    { figureDrop = !figureDrop; }
    
    void moveFigure(glm::mat4 trans)
    {
        figureTranslate = trans * figureTranslate;
        cameraPos = glm::vec3(trans * glm::vec4(cameraPos, 1));
        cameraLook = glm::vec3(trans * glm::vec4(cameraLook, 1));
    }
    
    void moveUp()
    {
        GLfloat x = (cameraLook-cameraPos).x;
        GLfloat z = (cameraLook-cameraPos).z;
        glm::vec3 forwardVec = normalize(glm::vec3(x, 0, z));
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), forwardVec*speed);
        moveFigure(trans);
    }
    
    void moveDown()
    {
        GLfloat x = (cameraLook-cameraPos).x;
        GLfloat z = (cameraLook-cameraPos).z;
        glm::vec3 forwardVec = normalize(glm::vec3(x, 0, z));
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), -forwardVec*speed);
        moveFigure(trans);
    }
    
    void moveLeft()
    {
        GLfloat x = (cameraLook-cameraPos).x;
        GLfloat z = (cameraLook-cameraPos).z;
        glm::vec3 rightVec = normalize(cross(glm::vec3(x, 0, z), glm::vec3(0, 1, 0)));
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), -rightVec*speed);
        moveFigure(trans);
    }
    
    void moveRight()
    {
        GLfloat x = (cameraLook-cameraPos).x;
        GLfloat z = (cameraLook-cameraPos).z;
        glm::vec3 rightVec = normalize(cross(glm::vec3(x, 0, z), glm::vec3(0, 1, 0)));
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), rightVec*speed);
        moveFigure(trans);
    }
    
    void rotateCamera(int x, int y){
        GLfloat camX = (cameraLook-cameraPos).x;
        GLfloat camZ = (cameraLook-cameraPos).z;
        glm::vec3 rightVec = normalize(cross(glm::vec3(camX, 0, camZ), glm::vec3(0, 1, 0)));
        glm::mat4 rotH = glm::rotate(glm::mat4(1), (x-mousePosX)*mouseSensitive, glm::vec3(0,1,0));
        
        glm::mat4 rotV = glm::rotate(glm::mat4(1), (y-mousePosY)*mouseSensitive, rightVec);
        
        glm::mat4 transToLook = glm::translate(glm::mat4(1.0f), -cameraLook);
        glm::mat4 transBack = glm::translate(glm::mat4(1.0f), cameraLook);
        
        cameraPos = glm::vec3(transBack*rotH*rotV*transToLook*glm::vec4(cameraPos, 1));
        
        float angle = abs(dot(normalize(cameraLook-cameraPos), cameraUp));
        if (angle>0.99) {
            glm::mat4 rotBack = glm::rotate(glm::mat4(1), -(y-mousePosY)*mouseSensitive, rightVec);
            cameraPos = glm::vec3(transBack*rotH*rotBack*transToLook*glm::vec4(cameraPos, 1));
        }
        
        figureRotate = rotH*figureRotate;
        
        mousePosX = 256;
        mousePosY = 256;
    }
};

#endif
