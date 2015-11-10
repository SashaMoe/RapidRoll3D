#ifndef __WORLDSTATE_H
#define __WORLDSTATE_H
#include "Model.h"
#include "Figure.h"
#include "Plane.h"
#include "PointedPlane.h"
#include "ShatterPlane.h"
#include "ModelLoader.h"


#define NUM_TRACKED_FRAMES 10
#include "glm/gtc/type_ptr.hpp"

class WorldState
{
    
    
private:
    float speed = 0.2f;
    float acceleration = 9.8f;
    float mouseSensitive = 0.01f;
    float moveUpV = 0.1f;
    
	float frameTimes[NUM_TRACKED_FRAMES];
	float currentTime;
	bool running;
    
    int bluePlaneCount = 4;
    int pointedPlaneCount = 4;
    int shatterPlaneCount = 4;
    
	Model model;
    //Model model2;
    
//    Model *bluePlanes = new Model[bluePlaneCount];
//    Model *pointedPlanes = new Model[pointedPlaneCount];
//    Model *shatterPlanes = new Model[shatterPlaneCount];
//    
//    glm::mat4 *bluePlaneTranslate = new glm::mat4[bluePlaneCount];
//    glm::mat4 *pointedPlaneTranslate = new glm::mat4[pointedPlaneCount];
//    glm::mat4 *shatterPlaneTranslate = new glm::mat4[shatterPlaneCount];
    
    //Model *onPlane;
    //glm::mat4 onPlaneTranslate;
    

    ModelLoader figureModel;
    ModelLoader bluePlaneModel;
    ModelLoader pointedPlaneModel;
    ModelLoader shatterPlaneModel;
    
    
    
    
    
    
    
    int shadingMode;
    
    Figure figure = Figure();
    Plane *plane = new Plane[bluePlaneCount];
    PointedPlane *pointedPlane = new PointedPlane[pointedPlaneCount];
    ShatterPlane *shatterPlane = new ShatterPlane[shatterPlaneCount];
    
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
    
    //glm::mat4 figureRotate;
    //glm::mat4 figureTranslate;
    
    //glm::mat4 planeTranslate;
    
    glm::mat4 cameraMatrix;
    
    
    //glm::mat4 planeTrans;
    //glm::mat4 heroTrans;
    

    int mousePosX;
    int mousePosY;

    bool lightRotating;
    bool modelRotating;
    bool figureDrop;
    
    float dropV = 0;
    
    
public:
    
    int currentRes[2];
	WorldState()
	{
		for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
			frameTimes[i] = 0.0f;
        
        shadingMode = 0;
		running = true;
		model = Model();
        model.init("resources/sphere.obj");
        
//        for (int i=0; i<bluePlaneCount; i++) {
//            Model m = Model();
//            m.init("resources/bluePlane.obj");
//            bluePlanes[i] = m;
//            bluePlaneTranslate[i] = glm::mat4(1);
//        }
//        
//        for (int i=0; i<pointedPlaneCount; i++) {
//            Model m = Model();
//            m.init("resources/PointedPlane.obj");
//            pointedPlanes[i] = m;
//            pointedPlaneTranslate[i] = glm::mat4(1);
//        }
//        
//        for (int i=0; i<shatterPlaneCount; i++) {
//            Model m = Model();
//            m.init("resources/ShatterPlane.obj");
//            shatterPlanes[i] = m;
//            shatterPlaneTranslate[i] = glm::mat4(1);
//        }
        
        //onPlane = NULL;
        //onPlaneTranslate = glm::mat4(1);
        
		//model2.init("resources/BluePlane.obj");
        figureModel.init("resources/sphere.obj");
        bluePlaneModel.init("resources/bluePlane.obj");
        pointedPlaneModel.init("resources/PointedPlane.obj");
        shatterPlaneModel.init("resources/ShatterPlane.obj");
        
        figure.init(figureModel.getBound(), figureModel.getHighestPoint().y, figureModel.getLowestPoint().y);
        
        for (int i=0; i<bluePlaneCount; i++) {
            plane[i].init(bluePlaneModel.getBound(), bluePlaneModel.getHighestPoint().y, bluePlaneModel.getLowestPoint().y);
            plane[i].translate(glm::translate(glm::mat4(1), glm::vec3(-20+i*15, -5, 0)));
        }
        
        for (int i=0; i<pointedPlaneCount; i++) {
            pointedPlane[i].init(pointedPlaneModel.getBound(), pointedPlaneModel.getHighestPoint().y, pointedPlaneModel.getLowestPoint().y);
            pointedPlane[i].translate(glm::translate(glm::mat4(1), glm::vec3(-20+i*15, -10, 0)));
        }
        
        for (int i=0; i<shatterPlaneCount; i++) {
            shatterPlane[i].init(shatterPlaneModel.getBound(), shatterPlaneModel.getHighestPoint().y, shatterPlaneModel.getLowestPoint().y);
            shatterPlane[i].translate(glm::translate(glm::mat4(1), glm::vec3(-20+i*15, -15, 0)));
        }
        
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
        
        //modelRotate = glm::mat4(1);
        //modelIncrement = glm::rotate(glm::mat4(1), 0.02f, glm::vec3(0,1,0));
        modelTranslate = glm::translate(glm::mat4(1), -model.getCentroid());
        

        //figureRotate = glm::mat4(1);
        //figureTranslate = glm::mat4(1);
        
        //planeTranslate = glm::translate(glm::mat4(1), glm::vec3(0,-5,0));
        
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
        glm::mat4 upTrans = glm::translate(glm::mat4(1), glm::vec3(0,moveUpV,0));
        
		//spin light
		if(lightRotating)
			lightRotate = lightIncrement * lightRotate;
        
        //spin model
		if(modelRotating)
			modelRotate = modelIncrement * modelRotate;
        
        // figure drop
        if (figureDrop) {
            float t = std::max(elapsed, 0.0f);
            if (checkFigureReachPlane(t)) {
                // move with plane
                moveFigure(upTrans);
            }else{
                dropFigure(t);
            }
        }
        
        // plane time step
        for(size_t i = 0 ;i<bluePlaneCount;i++){
            plane[i].translate(upTrans);
        }
        for(size_t i = 0 ;i<pointedPlaneCount;i++){
            pointedPlane[i].translate(upTrans);
        }
        for(size_t i = 0 ;i<shatterPlaneCount;i++){
            shatterPlane[i].translate(upTrans);
        }
        
		this->currentTime = t;
	}
    
    void dropFigure(float t){
        // drop
        float newDropV = dropV + t*9.8f;
        float dis = (newDropV*newDropV-dropV*dropV)/(2*9.8f);
        glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(0, -dis, 0));
        moveFigure(trans);
        dropV = newDropV;
    }
    
    bool checkFigureReachPlane(float t){
        float newDropV = dropV + t*acceleration;
        float dis = (newDropV*newDropV-dropV*dropV)/(2*acceleration);
        glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(0, -dis, 0));
        
        glm::vec4 figureBound = figure.getBound();
        float figureYMin = figure.getLowY()+trans[3][1];
        float figureYMax = figure.getHighY()+trans[3][1];
        
        for (int i=0; i<bluePlaneCount; i++) {
            Plane p = plane[i];
            glm::vec4 planeBound = p.getBound();
            float planeYMin = p.getLowY();
            float planeYMax = p.getHighY();
            
            if (!(figureBound.x>planeBound.y || figureBound.y<planeBound.x || figureBound.z>planeBound.w || figureBound.w<planeBound.z) && figureYMax>planeYMin) {
                
                if (figureYMin<planeYMax) {
                    stickToPlane(figure.getLowY(), planeYMax);
                    return true;
                }
            }
        }
        
        for (int i=0; i<pointedPlaneCount; i++) {
            PointedPlane p = pointedPlane[i];
            glm::vec4 planeBound = p.getBound();
            float planeYMin = p.getLowY();
            float planeYMax = p.getHighY();
            
            if (!(figureBound.x>planeBound.y || figureBound.y<planeBound.x || figureBound.z>planeBound.w || figureBound.w<planeBound.z) && figureYMax>planeYMin) {
                
                if (figureYMin<planeYMax) {
                    stickToPlane(figure.getLowY(), planeYMax);
                    return true;
                }
            }
        }
        
        for (int i=0; i<shatterPlaneCount; i++) {
            ShatterPlane p = shatterPlane[i];
            glm::vec4 planeBound = p.getBound();
            float planeYMin = p.getLowY();
            float planeYMax = p.getHighY();
            
            if (!(figureBound.x>planeBound.y || figureBound.y<planeBound.x || figureBound.z>planeBound.w || figureBound.w<planeBound.z) && figureYMax>planeYMin) {
                
                if (figureYMin<planeYMax) {
                    stickToPlane(figure.getLowY(), planeYMax);
                    return true;
                }
            }
        }
        
        return false;
    }
    
    void stickToPlane(float figureY, float planeY){
        if ((figureY-planeY)>0.01) {
            glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(0,figureY-planeY,0));
            moveFigure(trans);
            dropV = 0;
            printf("stop\n");
        }
        printf("stick\n");
    }
	
	Model & getModel()
	{ return model; }
    
//    Model* & getBluePlanes()
//    { return bluePlanes; }
//    
//    Model* & getPointedPlanes()
//    { return pointedPlanes; }
//    
//    Model* & getShatterPlanes()
//    { return shatterPlanes; }
    
	glm::mat4 getModelTranslate() const
	{ return modelTranslate; }
//    
//    glm::mat4* getBluePlaneTranslate() const
//    { return bluePlaneTranslate; }
//    
//    glm::mat4* getPointedPlaneTranslate() const
//    { return pointedPlaneTranslate; }
//    
//    glm::mat4* getShatterPlaneTranslate() const
//    { return shatterPlaneTranslate; }
    
    
    /*
     Get methods for Models
     */
    
    ModelLoader & getBluePlaneModel()
    { return bluePlaneModel; }
    
    ModelLoader & getPointedPlaneModel()
    { return pointedPlaneModel; }
    
    ModelLoader & getShatterPlaneModel()
    { return shatterPlaneModel; }
    
    ModelLoader & getFigureModel(){
        return figureModel;
    }
    
    /*
     Get methods for plane arrays
     */
    Figure getFigure(){
        return this->figure;
    }
    
    Plane* getPlanes(){
        return this->plane;
    }

    PointedPlane* getPointedPlanes(){
        return this->pointedPlane;
    }
    
    ShatterPlane* getShatterPlanes(){
        return this->shatterPlane;
    }
    
    
    
    
    
    
    
    
//    glm::mat4 getFigureTranslate() const
//    { return figureTranslate; }

    
//    glm::mat4 getPlaneTranslate() const
//    { return planeTranslate; }
    
    glm::mat4 getModelRotate() const
    { return modelRotate; }
    
//    glm::mat4 getFigureRotate() const
//    { return figureRotate; }
    
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
        figure.translate(trans);
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
        
        figure.translate(transBack*rotH*transToLook);
        
        mousePosX = 256;
        mousePosY = 256;
    }
};

#endif
