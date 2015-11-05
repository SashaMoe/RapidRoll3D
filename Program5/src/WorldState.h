#ifndef __WORLDSTATE_H
#define __WORLDSTATE_H
#include "Model.h"
#include "Plane.h"

#define NUM_TRACKED_FRAMES 10
#include "glm/gtc/type_ptr.hpp"

class WorldState
{
private:
	float frameTimes[NUM_TRACKED_FRAMES];
	float currentTime;
	bool running;
    
	Model model;
    Model model2;
    int shadingMode;
    
    Plane *plane = new Plane[4];
    
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
    glm::mat4 cameraMatrix;
    
    

    
    
    glm::mat4 planeTrans;
    glm::mat4 heroTrans;
    
	bool lightRotating;
	bool modelRotating;

    
    
    
    
    
    
public:
	WorldState()
	{
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = 0.0f;
        
        shadingMode = 0;
		running = true;
		model = Model();
        model.init("resources/cube.obj");
		model2.init("resources/sphere.obj");
		glm::vec3 center = model.getCentroid();
		glm::vec3 max = model.getMaxBound();
		glm::vec3 min = model.getMinBound();
		glm::vec3 dim = model.getDimension();
        printf("model loaded: bounds");
		printf("[%.2f %.2f %.2f]..", min[0], min[1], min[2]);
		printf("[%.2f %.2f %.2f] ", max[0], max[1], max[2]);
		printf("= dim [%.2f %.2f %.2f]\n", dim[0], dim[1], dim[2]);
		float camDistance = std::max(dim[0], dim[1]);
		cameraPos = glm::vec3(0,max[1],camDistance*2);
        cameraLook = glm::vec3(0,0,0);
        cameraUp = glm::vec3(0,1,0);
        
		lightPos = glm::vec4((max-center)*1.3f, 1);
        lightIntensity = glm::vec3(1,1,1);
        lightRotate = glm::mat4(1);
        lightIncrement = glm::rotate(glm::mat4(1), -0.05f, glm::vec3(0,1,0));
        
        modelRotate = glm::mat4(1);
        modelIncrement = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
        modelTranslate = glm::translate(glm::mat4(1), -model.getCentroid());
        
        
        
        lightRotating = false;
		modelRotating = false;
        
        
        
        
        
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
		
        printf("123");
        printf("lightPos : %f-%f-%f\n",lightPos.a,lightPos.b,lightPos.g);

        
		//spin light
		if(lightRotating)
			lightRotate = lightIncrement * lightRotate;
        
        //spin model
		if(modelRotating)
			modelRotate = modelIncrement * modelRotate;
		
        for(size_t i = 0 ;i<8;i++){
            plane[i].timeStep();
        }
        
		this->currentTime = t;
	}
	
	Model & getModel()
	{ return model; }
	
    Model & getModel2()
    { return model2; }
    
	glm::mat4 getModelTranslate() const
	{ return modelTranslate; }
    
    
    Plane* getPlanes(){
        return this->plane;
    }
    
    glm::mat4 getModelRotate() const
    { return modelRotate; }
    
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
};

#endif
