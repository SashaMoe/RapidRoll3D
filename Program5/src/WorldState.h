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
    float speed = 0.5f;
    float acceleration = 9.8f;
    float mouseSensitive = 0.01f;
    float moveUpV = 0.1f;
    int size = 40;
    float deathHeightMax = 1.8*size;
    float deathHeightMin = -size*2;
    
    float frameTimes[NUM_TRACKED_FRAMES];
    float currentTime;
    bool running;
    
    int bluePlaneCount = 8;
    int pointedPlaneCount = 4;
    int shatterPlaneCount = 4;
    
    ModelLoader figureModel;
    ModelLoader bluePlaneModel;
    ModelLoader pointedPlaneModel;
    ModelLoader shatterPlaneModel;
    
    int shadingMode;
    
    Figure figure = Figure();
    Plane *plane = new Plane[bluePlaneCount];
    PointedPlane *pointedPlane = new PointedPlane[pointedPlaneCount];
    ShatterPlane *shatterPlane = new ShatterPlane[shatterPlaneCount];
    PointedPlane roof = PointedPlane();
    
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
    
    glm::vec3 center = glm::vec3(0);
    glm::vec3 max = glm::vec3(size/2, size*2, size/2);
    glm::vec3 min = glm::vec3(-size/2, -size*2, -size/2);
    glm::vec3 dim = max-min;
    
    glm::mat4 cameraMatrix;
    
    int mousePosX;
    int mousePosY;
    
    bool lightRotating;
    bool modelRotating;
    bool figureDrop;
    bool figureDead;
    
    float dropV = 0;
    
    int bluePlaneHeight[8] = {60, 50, 30, 10, -20, -30, -60, -80};
    int pointedPlaneHeight[4] = {40, 0, -40, -70};
    int shatterPlaneHeight[4] = {20, -10, -50, -90};
    
public:
    
    int currentRes[2];
    
    WorldState()
    {
        for(size_t i=0; i<NUM_TRACKED_FRAMES; i++)
            frameTimes[i] = 0.0f;
        
        shadingMode = 0;
        running = true;
        
        figureModel.init("resources/sphere.obj");
        bluePlaneModel.init("resources/BluePlaneUV.obj");
        pointedPlaneModel.init("resources/PointedPlane.obj");
        shatterPlaneModel.init("resources/ShatterPlane.obj");
        
        figure.init(figureModel.getBound(), figureModel.getHighestPoint().y, figureModel.getLowestPoint().y);
        
        for (int i=0; i<bluePlaneCount; i++) {
            plane[i].init(bluePlaneModel.getBound(), bluePlaneModel.getHighestPoint().y, bluePlaneModel.getLowestPoint().y);
            plane[i].translate(glm::translate(glm::mat4(1), glm::vec3(rand()%(size-10)-(size-10)/2, bluePlaneHeight[i], rand()%(size-10)-(size-10)/2)));
        }
        
        for (int i=0; i<pointedPlaneCount; i++) {
            pointedPlane[i].init(pointedPlaneModel.getBound(), pointedPlaneModel.getHighestPoint().y, pointedPlaneModel.getLowestPoint().y);
            pointedPlane[i].translate(glm::translate(glm::mat4(1), glm::vec3(rand()%(size-10)-(size-10)/2, pointedPlaneHeight[i], rand()%(size-10)-(size-10)/2)));
        }
        
        for (int i=0; i<shatterPlaneCount; i++) {
            shatterPlane[i].init(shatterPlaneModel.getBound(), shatterPlaneModel.getHighestPoint().y, shatterPlaneModel.getLowestPoint().y);
            shatterPlane[i].translate(glm::translate(glm::mat4(1), glm::vec3(rand()%(size-10)-(size-10)/2, shatterPlaneHeight[i], rand()%(size-10)-(size-10)/2)));
        }
        
        roof.init(pointedPlaneModel.getBound(), pointedPlaneModel.getHighestPoint().y, pointedPlaneModel.getLowestPoint().y);
        roof.translate(glm::rotate(glm::mat4(1), 3.1415926f, glm::vec3(1,0,0)));
        roof.translate(glm::scale(glm::mat4(1), glm::vec3(size/2,size/4,size/2)));
        roof.translate(glm::translate(glm::mat4(1), glm::vec3(0,2.0f*size,0)));
        
        printf("model loaded: bounds");
        printf("[%.2f %.2f %.2f]..", min[0], min[1], min[2]);
        printf("[%.2f %.2f %.2f] ", max[0], max[1], max[2]);
        printf("= dim [%.2f %.2f %.2f]\n", dim[0], dim[1], dim[2]);
        float camDistance = 10;
        cameraPos = glm::vec3(0,camDistance,camDistance);
        cameraLook = glm::vec3(0,0,0);
        cameraUp = glm::vec3(0,1,0);
        
        
        lightPos = glm::vec4(15,25,0,0);
        lightPos[1] = 15;
        
        lightIntensity = glm::vec3(1,1,1);
        lightRotate = glm::mat4(1);
        lightIncrement = glm::rotate(glm::mat4(1), -0.05f, glm::vec3(0,1,0));
        
        modelTranslate = glm::translate(glm::mat4(1), -center);
        
        mousePosX = 256;
        mousePosY = 256;
        
        lightRotating = false;
        
        modelRotating = false;
        
        figureDrop = true;
        figureDead = false;
        
        resetFigure();
        
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
    
    void setRunning(bool r)
    { running = r; }
    
    bool isRunning() const
    { return running; }
    
    float getWindowSize()
    { return size; }
    
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
        
        // check if dead
        if(figure.getHighY()>deathHeightMax || figure.getHighY()<deathHeightMin){
            figureDead = true;
        }
        
        if(figureDead){
            resetFigure();
            figureDead = false;
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
    
    void resetFigure(){
        int index = 0;
        for (int i=0; i<bluePlaneCount; i++) {
            if (plane[i].getHighY()<plane[index].getHighY() && plane[i].getHighY()>deathHeightMin/2)
            {
                index = i;
            }
        }
        glm::mat4 figureTrans = figure.getTranslation();
        moveFigure(glm::inverse(figureTrans));
        Plane p = plane[index];
        glm::mat4 planeTrans = p.getTranslation();
        moveFigure(planeTrans);
        float planeYMax = p.getHighY();
        float figureYMin = figure.getLowY();
        glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(0,planeYMax-figureYMin,0));
        moveFigure(trans);
        dropV = 0;
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
                    //stickToPlane(figure.getLowY(), planeYMax);
                    figureDead = true;
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
        }
    }
    
    glm::vec3 getDimension() {
        return dim;
    }
    
    glm::mat4 getModelTranslate() const
    { return modelTranslate; }
    
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
    
    PointedPlane getRoof(){
        return this->roof;
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
    
    void toggleFigureDrop()
    { figureDrop = !figureDrop; }
    
    
    glm::mat4 getFigureTranslate(glm::mat4 trans){
        glm::vec4 figureBound = figure.getBound();
        float figureXMin = figureBound.x+trans[3][0];
        float figureXMax = figureBound.y+trans[3][0];
        float figureYMin = figure.getLowY()+trans[3][1];
        float figureYMax = figure.getHighY()+trans[3][1];
        float figureZMin = figureBound.z+trans[3][2];
        float figureZMax = figureBound.w+trans[3][2];
        
        for (int i=0; i<bluePlaneCount; i++) {
            Plane p = plane[i];
            glm::vec4 planeBound = p.getBound();
            float planeYMin = p.getLowY();
            float planeYMax = p.getHighY();
            if (!(planeYMin+0.1>figureYMax || planeYMax < figureYMin+0.1))
            {
                if ((planeBound.x<figureXMin && figureXMin<planeBound.y)
                    && (figureXMax>planeBound.y)
                    && ((planeBound.z<figureZMin && figureZMin<planeBound.w)
                        || (planeBound.z<figureZMax && figureZMax<planeBound.w)))
                {
                    trans[3][0] = planeBound.y-figureBound.x;
                } else if ((planeBound.x<figureXMax && figureXMax<planeBound.y)
                           && (figureXMin<planeBound.x)
                           && ((planeBound.z<figureZMin && figureZMin<planeBound.w)
                               || (planeBound.z<figureZMax && figureZMax<planeBound.w)))
                {
                    trans[3][0] = planeBound.x-figureBound.y;
                } else if ((planeBound.z<figureZMin && figureZMin<planeBound.w)
                           && (figureZMax>planeBound.w)
                           && ((planeBound.x<figureXMin && figureXMin<planeBound.y)
                               || (planeBound.x<figureXMax && figureXMax<planeBound.y)))
                {
                    trans[3][2] = planeBound.w-figureBound.z;
                } else if ((planeBound.z<figureZMax && figureZMax<planeBound.w)
                           && (figureZMin<planeBound.z)
                           && ((planeBound.x<figureXMin && figureXMin<planeBound.y)
                               || (planeBound.x<figureXMax && figureXMax<planeBound.y)))
                {
                    trans[3][2] = planeBound.z-figureBound.w;
                }
            }
        }
        
        for (int i=0; i<pointedPlaneCount; i++) {
            PointedPlane p = pointedPlane[i];
            glm::vec4 planeBound = p.getBound();
            float planeYMin = p.getLowY();
            float planeYMax = p.getHighY();
            if (!(planeYMin+0.1>figureYMax || planeYMax < figureYMin+0.1))
            {
                if ((planeBound.x<figureXMin && figureXMin<planeBound.y)
                    && (figureXMax>planeBound.y)
                    && ((planeBound.z<figureZMin && figureZMin<planeBound.w)
                        || (planeBound.z<figureZMax && figureZMax<planeBound.w)))
                {
                    trans[3][0] = planeBound.y-figureBound.x;
                } else if ((planeBound.x<figureXMax && figureXMax<planeBound.y)
                           && (figureXMin<planeBound.x)
                           && ((planeBound.z<figureZMin && figureZMin<planeBound.w)
                               || (planeBound.z<figureZMax && figureZMax<planeBound.w)))
                {
                    trans[3][0] = planeBound.x-figureBound.y;
                } else if ((planeBound.z<figureZMin && figureZMin<planeBound.w)
                           && (figureZMax>planeBound.w)
                           && ((planeBound.x<figureXMin && figureXMin<planeBound.y)
                               || (planeBound.x<figureXMax && figureXMax<planeBound.y)))
                {
                    trans[3][2] = planeBound.w-figureBound.z;
                } else if ((planeBound.z<figureZMax && figureZMax<planeBound.w)
                           && (figureZMin<planeBound.z)
                           && ((planeBound.x<figureXMin && figureXMin<planeBound.y)
                               || (planeBound.x<figureXMax && figureXMax<planeBound.y)))
                {
                    trans[3][2] = planeBound.z-figureBound.w;
                }
            }
            
        }
        
        for (int i=0; i<shatterPlaneCount; i++) {
            ShatterPlane p = shatterPlane[i];
            glm::vec4 planeBound = p.getBound();
            float planeYMin = p.getLowY();
            float planeYMax = p.getHighY();
            if (!(planeYMin+0.1>figureYMax || planeYMax < figureYMin+0.1))
            {
                if ((planeBound.x<figureXMin && figureXMin<planeBound.y)
                    && (figureXMax>planeBound.y)
                    && ((planeBound.z<figureZMin && figureZMin<planeBound.w)
                        || (planeBound.z<figureZMax && figureZMax<planeBound.w)))
                {
                    trans[3][0] = planeBound.y-figureBound.x;
                } else if ((planeBound.x<figureXMax && figureXMax<planeBound.y)
                           && (figureXMin<planeBound.x)
                           && ((planeBound.z<figureZMin && figureZMin<planeBound.w)
                               || (planeBound.z<figureZMax && figureZMax<planeBound.w)))
                {
                    trans[3][0] = planeBound.x-figureBound.y;
                } else if ((planeBound.z<figureZMin && figureZMin<planeBound.w)
                           && (figureZMax>planeBound.w)
                           && ((planeBound.x<figureXMin && figureXMin<planeBound.y)
                               || (planeBound.x<figureXMax && figureXMax<planeBound.y)))
                {
                    trans[3][2] = planeBound.w-figureBound.z;
                } else if ((planeBound.z<figureZMax && figureZMax<planeBound.w)
                           && (figureZMin<planeBound.z)
                           && ((planeBound.x<figureXMin && figureXMin<planeBound.y)
                               || (planeBound.x<figureXMax && figureXMax<planeBound.y)))
                {
                    trans[3][2] = planeBound.z-figureBound.w;
                }
            }
            
        }
        
        return trans;
    }
    
    
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
        moveFigure(getFigureTranslate(trans));
    }
    
    void moveDown()
    {
        GLfloat x = (cameraLook-cameraPos).x;
        GLfloat z = (cameraLook-cameraPos).z;
        glm::vec3 forwardVec = normalize(glm::vec3(x, 0, z));
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), -forwardVec*speed);
        moveFigure(getFigureTranslate(trans));
    }
    
    void moveLeft()
    {
        GLfloat x = (cameraLook-cameraPos).x;
        GLfloat z = (cameraLook-cameraPos).z;
        glm::vec3 rightVec = normalize(cross(glm::vec3(x, 0, z), glm::vec3(0, 1, 0)));
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), -rightVec*speed);
        moveFigure(getFigureTranslate(trans));
    }
    
    void moveRight()
    {
        GLfloat x = (cameraLook-cameraPos).x;
        GLfloat z = (cameraLook-cameraPos).z;
        glm::vec3 rightVec = normalize(cross(glm::vec3(x, 0, z), glm::vec3(0, 1, 0)));
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), rightVec*speed);
        moveFigure(getFigureTranslate(trans));
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
