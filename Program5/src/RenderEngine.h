#ifndef __RENDERENGINE
#define __RENDERENGINE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "GLHelper.h"
#include "WorldState.h"

class RenderEngine
{
public:
	RenderEngine()
	{
		initialized = false;
		
		//camera
        this->P = glm::perspective(1.0f, 1.0f, 0.1f, 100.0f);
        this->C = glm::mat4(1);
		this->M = glm::mat4(1);
	}

	~RenderEngine()
	{
		if(initialized)
		{
			// Clean up the buffers
			//glDeleteBuffers(1, &positionBuffer);
			//glDeleteBuffers(1, &colorBuffer);
		}
	}

	void init(WorldState & state)
	{
		initialized = true;

		float ver = initLoader();
		if( ver < 1.0f ) {
			printf("OpenGL is not supported.\n");
			exit(1);
		}
		printf("OpenGL version %.1f is supported.\n", ver);
		
		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
        
		glm::vec3 dim = state.getDimension();
		float maxDim = std::max(dim[0], std::max(dim[1], dim[2]));
        printf("max: %f\n", maxDim);
		this->P = glm::perspective(1.0f, 1.0f, maxDim*0.001f, maxDim*100.0f);
        
        setupShader();
        setupTextures();
        setupBuffersTexture();
        setupBuffersBluePlane(state.getBluePlaneModel());
        setupBuffersFigure(state.getFigureModel());
        setupBuffersPointedPlane(state.getPointedPlaneModel());
        setupBuffersShatterPlane(state.getShatterPlaneModel());
        setupBuffersRoof(state.getPointedPlaneModel());

        
	}
    
    
    void buildRenderBuffers(size_t xSize, size_t ySize)
    {
        if(renderTexture != 0) {
            glDeleteTextures(1, &renderTexture);
            glDeleteRenderbuffers(1, &renderBuffer);
        }
        
        //framebuffer
        glGenFramebuffers(1, &frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        
        //make renderbuffer for depth, attach
        glGenRenderbuffers(1, &renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, xSize, ySize);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
        
        //make texture
        glGenTextures(1, &renderTexture);
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xSize, ySize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //		glBindTexture(GL_TEXTURE_RECTANGLE, renderTexture);
        //		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, xSize, ySize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        //		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        //attach texture to framebuffer
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTexture, 0);
        GLenum colorBuffer = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(1, &colorBuffer);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            fprintf(stderr, "Frame buffer setup failed\n");
            exit(3);
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        checkGLError("frame buffer");
    }


	void display(WorldState & state)
	{
		//clear the old frame
//		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 mT = state.getModelTranslate();
        glm::mat4 mR = state.getModelRotate();
        
		glm::mat4 M = state.getCameraMatrix()*mR*mT;
		glm::mat4 N = glm::inverseTranspose(mR*mT);
        glm::vec4 lightPos = state.getLightPos();
        glm::vec4 camPos = state.getCameraPos();
        glm::mat4 L = state.getLightRotate();
        
        //printf("lightPos : %f-%f-%f\n",lightPos.a,lightPos.b,lightPos.g);
        //hacky light source size change
        GLfloat maxDis = state.getDimension()[2] * 3;
        GLfloat distScale = 1.0f / (glm::length(L*lightPos - camPos) / maxDis);
        glPointSize(glm::mix(1.0f, 10.0f, distScale));
        
        //printf("cam %f %f %f\n", camPos[0], camPos[1], camPos[2]);
        //printf("light %f %f %f\n", lightPos[0], lightPos[1], lightPos[2]);
		
        glUseProgram(lightProg);
        glUniformMatrix4fv(glGetUniformLocation(lightProg, "P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightProg, "C"), 1, GL_FALSE, &C[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightProg, "mR"), 1, GL_FALSE, &mR[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightProg, "mT"), 1, GL_FALSE, &mT[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightProg, "M"), 1, GL_FALSE, &M[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightProg, "N"), 1, GL_FALSE, &N[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightProg, "L"), 1, GL_FALSE, &L[0][0]);
        glUniform4fv(glGetUniformLocation(lightProg, "lightPos"), 1, &lightPos[0]);
        glUniform4fv(glGetUniformLocation(lightProg, "camPos"), 1, &camPos[0]);
        glUniform1i(glGetUniformLocation(lightProg, "shadingMode"), state.getShadingMode());
        glBindVertexArray(lightArray);
        glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
        
        
		//use shader
		glUseProgram(shaderProg);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "C"), 1, GL_FALSE, &C[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "mR"), 1, GL_FALSE, &mR[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "mT"), 1, GL_FALSE, &mT[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "M"), 1, GL_FALSE, &M[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "N"), 1, GL_FALSE, &N[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProg, "L"), 1, GL_FALSE, &L[0][0]);
        glUniform4fv(glGetUniformLocation(shaderProg, "lightPos"), 1, &lightPos[0]);
        glUniform4fv(glGetUniformLocation(shaderProg, "camPos"), 1, &camPos[0]);
        glUniform1i(glGetUniformLocation(shaderProg, "shadingMode"), state.getShadingMode());
        
        for(int i=0; i<1; i++)
        {
            //TODO bind the texture object
            glBindTexture(GL_TEXTURE_2D, bluePlaneTextures[i]);
            
            //TODO activate a texture unit
            GLint texUnitId = 0;
            glActiveTexture(GL_TEXTURE0+texUnitId);
            
            //TODO bind the sampler to a texture unit
            glUniform1i(glGetUniformLocation(shaderProg, "texSampler"), texUnitId);
        }
       
		//draw figure
        glBindVertexArray(vertexArrayFigure);
        trans = state.getFigure().getTranslation();
        glUniformMatrix4fv(glGetUniformLocation(shaderProg,"trans"),1,GL_FALSE,&trans[0][0]);
        glUniform1i(glGetUniformLocation(shaderProg, "type"), 0);
        glDrawElements(GL_TRIANGLES, state.getFigureModel().getElements().size(), GL_UNSIGNED_INT, 0);
        checkGLError("model");
        
        //draw blue plane
         glBindVertexArray(vertexArrayBluePlane);
        for(int i=0;i<8;i++){
            if (state.getPlanes()[i].getLocation().y>-80) {
                trans = state.getPlanes()[i].getTranslation();
                glUniformMatrix4fv(glGetUniformLocation(shaderProg,"trans"),1,GL_FALSE,&trans[0][0]);
                glUniform1i(glGetUniformLocation(shaderProg, "type"), 1);
                glDrawElements(GL_TRIANGLES, state.getBluePlaneModel().getElements().size(), GL_UNSIGNED_INT, 0);
            }
        }
        
        //draw pointed plane
         glBindVertexArray(vertexArrayPointedPlane);
        for(int i=0;i<4;i++){
            if(state.getPointedPlanes()[i].getLocation().y>-80){
                trans = state.getPointedPlanes()[i].getTranslation();
                glUniformMatrix4fv(glGetUniformLocation(shaderProg,"trans"),1,GL_FALSE,&trans[0][0]);
                glUniform1i(glGetUniformLocation(shaderProg, "type"), 2);
                glDrawElements(GL_TRIANGLES, state.getPointedPlaneModel().getElements().size(), GL_UNSIGNED_INT, 0);
            }
        }
       
        //draw shatter plane
        glBindVertexArray(vertexArrayShatterPlane);
        for(int i=0;i<4;i++){
            if (state.getShatterPlanes()[i].getLocation().y>-80) {
                trans = state.getShatterPlanes()[i].getTranslation();
                glUniformMatrix4fv(glGetUniformLocation(shaderProg,"trans"),1,GL_FALSE,&trans[0][0]);
                glUniform1i(glGetUniformLocation(shaderProg, "type"), 3);
                glUniform1f(glGetUniformLocation(shaderProg, "timeElapsed"), state.getShatterPlanes()[i].getTimeElapsed());
                glDrawElements(GL_TRIANGLES, state.getShatterPlaneModel().getElements().size(), GL_UNSIGNED_INT, 0);
            }
        }
        
        //draw roof
        glBindVertexArray(vertexArrayRoof);
        trans = state.getRoof().getTranslation();
        glUniformMatrix4fv(glGetUniformLocation(shaderProg,"trans"),1,GL_FALSE,&trans[0][0]);
        glUniform1i(glGetUniformLocation(shaderProg, "type"), 4);
        glDrawElements(GL_TRIANGLES, state.getPointedPlaneModel().getElements().size(), GL_UNSIGNED_INT, 0);
        
        
//        glBindVertexArray(0);
//        glUseProgram(0);
//        
//        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(textureProg);
       
        glUniformMatrix4fv(glGetUniformLocation(textureProg, "P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(textureProg, "C"), 1, GL_FALSE, &C[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(textureProg, "mR"), 1, GL_FALSE, &mR[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(textureProg, "mT"), 1, GL_FALSE, &mT[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(textureProg, "M"), 1, GL_FALSE, &M[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(textureProg, "N"), 1, GL_FALSE, &N[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(textureProg, "L"), 1, GL_FALSE, &L[0][0]);
        glUniform4fv(glGetUniformLocation(textureProg, "lightPos"), 1, &lightPos[0]);
        glUniform4fv(glGetUniformLocation(textureProg, "camPos"), 1, &camPos[0]);
        glUniform1i(glGetUniformLocation(textureProg, "shadingMode"), state.getShadingMode());
        glUniform2f(glGetUniformLocation(textureProg, "resolution"), state.currentRes[0], state.currentRes[1]);
        glUniform1f(glGetUniformLocation(textureProg, "elapsedTime"), state.getCurrentTime());
        glUniform1i(glGetUniformLocation(textureProg, "enable"), state.getSwirlEnable());
        GLuint texId = 0;
        glActiveTexture(GL_TEXTURE0+texId);
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        glUniform1i( glGetUniformLocation(textureProg, "texId"), texId);

        glBindVertexArray(quadVertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        
        glBindVertexArray(0);
        glUseProgram(0);

	}


private:
	bool initialized;
    GLuint quadVertexArray;
    GLuint frameBuffer;
    GLuint renderTexture;
    GLuint renderBuffer;
    GLuint textureProg;
	GLuint shaderProg;
    GLuint lightProg;
	GLuint vertexArray;
    GLuint lightArray;
    GLuint vertexArrayBluePlane;
    GLuint vertexArrayPointedPlane;
    GLuint vertexArrayShatterPlane;
    GLuint vertexArrayFigure;
    GLuint vertexArrayRoof;
    GLuint bluePlaneTextures[1];
	
    glm::mat4 P;
	glm::mat4 C;
	glm::mat4 M;
    glm::mat4 trans;
	
	float initLoader()
	{
		float ver = 0.0f;
#ifdef GLEW
		glewExperimental = GL_TRUE;
		
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
		
		if (GLEW_VERSION_1_1) { ver = 1.1f; }
		if (GLEW_VERSION_1_2) { ver = 1.2f; }
		if (GLEW_VERSION_1_3) { ver = 1.3f; }
		if (GLEW_VERSION_1_4) { ver = 1.4f; }
		if (GLEW_VERSION_1_5) { ver = 1.5f; }
		
		if (GLEW_VERSION_2_0) { ver = 2.0f; }
		if (GLEW_VERSION_2_1) { ver = 2.1f; }
		
		if (GLEW_VERSION_3_0) { ver = 3.0f; }
		if (GLEW_VERSION_3_1) { ver = 3.1f; }
		if (GLEW_VERSION_3_2) { ver = 3.2f; }
		if (GLEW_VERSION_3_3) { ver = 3.3f; }
		
		if (GLEW_VERSION_4_0) { ver = 4.0f; }
		if (GLEW_VERSION_4_1) { ver = 4.1f; }
		if (GLEW_VERSION_4_2) { ver = 4.2f; }
		if (GLEW_VERSION_4_3) { ver = 4.3f; }
		if (GLEW_VERSION_4_4) { ver = 4.4f; }
		if (GLEW_VERSION_4_5) { ver = 4.5f; }
#endif
		
#ifdef GL3W
		if (gl3wInit()) {
			fprintf(stderr, "failed to initialize OpenGL\n");
		}
		
		if (gl3wIsSupported(1, 1)) { ver = 1.1f; }
		if (gl3wIsSupported(1, 2)) { ver = 1.2f; }
		if (gl3wIsSupported(1, 3)) { ver = 1.3f; }
		if (gl3wIsSupported(1, 4)) { ver = 1.4f; }
		if (gl3wIsSupported(1, 5)) { ver = 1.5f; }
		
		if (gl3wIsSupported(2, 0)) { ver = 2.0f; }
		if (gl3wIsSupported(2, 1)) { ver = 2.1f; }
		
		if (gl3wIsSupported(3, 0)) { ver = 3.0f; }
		if (gl3wIsSupported(3, 1)) { ver = 3.1f; }
		if (gl3wIsSupported(3, 2)) { ver = 3.2f; }
		if (gl3wIsSupported(3, 3)) { ver = 3.3f; }
		
		if (gl3wIsSupported(4, 0)) { ver = 4.0f; }
		if (gl3wIsSupported(4, 1)) { ver = 4.1f; }
		if (gl3wIsSupported(4, 2)) { ver = 4.2f; }
		if (gl3wIsSupported(4, 3)) { ver = 4.3f; }
		if (gl3wIsSupported(4, 4)) { ver = 4.4f; }
		if (gl3wIsSupported(4, 5)) { ver = 4.5f; }
#endif
		
		return ver;
	}

	void setupShader()
	{
		char const * vertPath = "resources/simple.vert";
		char const * fragPath = "resources/simple.frag";
		shaderProg = ShaderManager::shaderFromFile(&vertPath, &fragPath, 1, 1);
        
        char const * lightVPath = "resources/lightPos.vert";
        char const * lightFPath = "resources/lightPos.frag";
        lightProg = ShaderManager::shaderFromFile(&lightVPath, &lightFPath, 1, 1);
        
        
        char const * textVPath = "resources/texture.vert";
        char const * textFPath = "resources/texture.frag";
        textureProg = ShaderManager::shaderFromFile(&textVPath, &textFPath, 1, 1);
        
		checkGLError("shader");
	}
    
    
    void setupBuffersTexture(){
        static const GLfloat quadVertexData[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
        };
        
        glGenVertexArrays(1, &quadVertexArray);
        glBindVertexArray(quadVertexArray);
        GLuint quadVertexBuffer;
        glGenBuffers(1, &quadVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData, GL_STATIC_DRAW);
        glEnableVertexAttribArray(glGetAttribLocation(textureProg, "pos"));
        glVertexAttribPointer(glGetAttribLocation(textureProg, "pos"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    
    
    void setupBuffersFigure(ModelLoader & model){
        glGenVertexArrays(1, &vertexArrayFigure);
        glBindVertexArray(vertexArrayFigure);
        
        GLuint positionBuffer;
        GLuint colorBuffer;
        GLuint elementBuffer;
        GLint colorSlot;
        GLint positionSlot;
        
        //setup position buffer
        glGenBuffers(1, &positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), &model.getPosition()[0], GL_STATIC_DRAW);
        positionSlot = glGetAttribLocation(shaderProg, "pos");
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // Do the same thing for the color data
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getColorBytes(), &model.getColor()[0], GL_STATIC_DRAW);
        colorSlot =    glGetAttribLocation(shaderProg, "colorIn");
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // now the elements
        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getElementBytes(), &model.getElements()[0], GL_STATIC_DRAW);
        
    }

    
    
    
    void setupBuffersShatterPlane(ModelLoader & model){
        glGenVertexArrays(1, &vertexArrayShatterPlane);
        glBindVertexArray(vertexArrayShatterPlane);
        
        GLuint positionBuffer;
        GLuint colorBuffer;
        GLuint elementBuffer;
        GLuint texCoordBuffer;
        GLint colorSlot;
        GLint positionSlot;
        GLint texCoordSlot;
        
        //setup position buffer
        glGenBuffers(1, &positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), &model.getPosition()[0], GL_STATIC_DRAW);
        positionSlot = glGetAttribLocation(shaderProg, "pos");
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glGenBuffers(1, &texCoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getTexCoordBytes(), &model.getTexCoord()[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        texCoordSlot = glGetAttribLocation(shaderProg, "texCoord");
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
        glEnableVertexAttribArray(texCoordSlot);
        glVertexAttribPointer(texCoordSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // Do the same thing for the color data
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getColorBytes(), &model.getColor()[0], GL_STATIC_DRAW);
        colorSlot =    glGetAttribLocation(shaderProg, "colorIn");
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // now the elements
        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getElementBytes(), &model.getElements()[0], GL_STATIC_DRAW);
    }
    
    void setupBuffersPointedPlane(ModelLoader & model){
        glGenVertexArrays(1, &vertexArrayPointedPlane);
        glBindVertexArray(vertexArrayPointedPlane);
        
        GLuint positionBuffer;
        GLuint colorBuffer;
        GLuint elementBuffer;
        GLint colorSlot;
        GLint positionSlot;
        
        //setup position buffer
        glGenBuffers(1, &positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), &model.getPosition()[0], GL_STATIC_DRAW);
        positionSlot = glGetAttribLocation(shaderProg, "pos");
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // Do the same thing for the color data
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getColorBytes(), &model.getColor()[0], GL_STATIC_DRAW);
        colorSlot =    glGetAttribLocation(shaderProg, "colorIn");
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // now the elements
        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getElementBytes(), &model.getElements()[0], GL_STATIC_DRAW);
    }

    
    
    
    void setupBuffersBluePlane(ModelLoader & model){
        glGenVertexArrays(1, &vertexArrayBluePlane);
        glBindVertexArray(vertexArrayBluePlane);
        
        GLuint positionBuffer;
        GLuint texCoordBuffer;
        GLuint colorBuffer;
        GLuint elementBuffer;
        GLint colorSlot;
        GLint positionSlot;
        GLint texCoordSlot;
        
        //setup position buffer
        glGenBuffers(1, &positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), &model.getPosition()[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        positionSlot = glGetAttribLocation(shaderProg, "pos");
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glGenBuffers(1, &texCoordBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getTexCoordBytes(), &model.getTexCoord()[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        texCoordSlot = glGetAttribLocation(shaderProg, "texCoord");
        glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
        glEnableVertexAttribArray(texCoordSlot);
        glVertexAttribPointer(texCoordSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        checkGLError("tex");
        
        // Do the same thing for the color data
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getColorBytes(), &model.getColor()[0], GL_STATIC_DRAW);
        colorSlot =    glGetAttribLocation(shaderProg, "colorIn");
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // now the elements
        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getElementBytes(), &model.getElements()[0], GL_STATIC_DRAW);
    }
    
    
    void setupBuffersRoof(ModelLoader & model){
        glGenVertexArrays(1, &vertexArrayRoof);
        glBindVertexArray(vertexArrayRoof);
        
        GLuint positionBuffer;
        GLuint colorBuffer;
        GLuint elementBuffer;
        GLint colorSlot;
        GLint positionSlot;
        
        //setup position buffer
        glGenBuffers(1, &positionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), &model.getPosition()[0], GL_STATIC_DRAW);
        positionSlot = glGetAttribLocation(shaderProg, "pos");
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // Do the same thing for the color data
        glGenBuffers(1, &colorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glBufferData(GL_ARRAY_BUFFER, model.getColorBytes(), &model.getColor()[0], GL_STATIC_DRAW);
        colorSlot =    glGetAttribLocation(shaderProg, "colorIn");
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // now the elements
        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getElementBytes(), &model.getElements()[0], GL_STATIC_DRAW);
        
        
        //hacky way to draw the light
        glGenVertexArrays(1, &lightArray);
        glBindVertexArray(lightArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBindVertexArray(0);
    }
    
    void setupTextures()
    {
        //generate texture names
        glGenTextures(1, bluePlaneTextures);
        sf::Image image;
        
        char const * imagePaths[1] = {"resources/front.png"};
        
        for(int i=0; i<1; i++)
        {
            if (!image.loadFromFile(imagePaths[i])) {
                fprintf(stderr, "Could not load: %s\n", imagePaths[i]);
                exit(2);
            }
            int texSizeX = image.getSize().x;
            int texSizeY = image.getSize().y;
            unsigned char * texData = (unsigned char*) image.getPixelsPtr();
            
            //TODO bind a texture object
            glBindTexture(GL_TEXTURE_2D, bluePlaneTextures[i]);
            
            //TODO set min/mag sampling parameters
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            
            //TODO set edge wrap parameters
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            //TODO upload texture and generate mipmap
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSizeX, texSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
            bool  mipmapEnabled = true;
            if(mipmapEnabled)
            {
                //mip mapping, upload 0 level, then build maps
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                //no mip mapping, upload 0 level only
            }
            
            //TODO unbind the texture
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        
        checkGLError("texture");
    }

};

#endif