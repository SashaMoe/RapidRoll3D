#ifndef __MODEL
#define __MODEL

#include <vector>
#include "glm/glm.hpp"
#include "objload/objLoader.h"
using namespace std; //makes using vectors easy

class Model
{
public:
	

    void init(const char *path)
    {
        objLoader loader;
        //loader.load("resources/cube.obj");
        //loader.load("resources/sphere.obj");
        //loader.load("resources/teapot.obj");
        loader.load(path);

        
		for(size_t i=0; i<loader.vertexCount; i++) {
			positions.push_back(loader.vertexList[i]->e[0]);
			positions.push_back(loader.vertexList[i]->e[1]);
			positions.push_back(loader.vertexList[i]->e[2]);
			//printf("v%zu: %f %f %f\n", i, positions[i*3+0], positions[i*3+1], positions[i*3+2]);
		}
		
		for(size_t i=0; i<loader.faceCount; i++) {
			if(loader.faceList[i]->vertex_count != 3) {
				fprintf(stderr, "Only triangle primitives are supported.\n");
				exit(1);
			}
			
			elements.push_back(loader.faceList[i]->vertex_index[0]);
			elements.push_back(loader.faceList[i]->vertex_index[1]);
			elements.push_back(loader.faceList[i]->vertex_index[2]);
			//printf("f%zu: %i %i %i\n", i, elements[i*3+0], elements[i*3+1], elements[i*3+2]);
		}
        
        for(size_t i=0; i<positions.size(); i++) {
            colors.push_back( 1 );
            colors.push_back( 1 );
            colors.push_back( 1 );
        }
        
        //TODO compute the vertex normals by averaging the face normals
        vector<glm::vec3> faceNormals;
        
        for(int i =0;i<elements.size()*3;i++){
            colors[i] = 0;
        }
        
        
        
        for(int i = 0 ; i < elements.size(); i += 3 ){
            glm::vec3 vertices[3];
            for(size_t v=0;v<3;v++){
                for(size_t c=0;c<3;c++){
                    size_t vertexId = elements[i+v];
                    vertices[v][c] = positions[vertexId*3+c];
                }
            }
            glm::vec3 a = vertices[1] - vertices[0];
            glm::vec3 b = vertices[2] - vertices[1];
            glm::vec3 n = glm::cross(a,b);
            
            
            
            for (size_t v=0;v<3;v++){
                colors[elements[i+v]*3 + 0 ] += n[0];
                colors[elements[i+v]*3 + 1 ] += n[1];
                colors[elements[i+v]*3 + 2 ] += n[2];
            }

            
        }
        
        
        for (int i = 0; i < elements.size() * 3 ; i += 3){
            glm::vec3  vert = glm::vec3(1.0f);
            vert[0] = colors[i];
            vert[1] = colors[i+1];
            vert[2] = colors[i+2];
            
            glm::vec3 n = glm::normalize(vert);
            //n = (n+1.0f) * 0.5f;
            
            
            colors[i] = n[0];
            colors[i+1] = n[1];
            colors[i+2] = n[2];
            
        }
        
        
        
//        for (size_t i=0;i<elements.size();i+=3){
//            glm::vec3 vertices[3];
//            for(size_t v=0;v<3;v++){
//                for(size_t c=0;c<3;c++){
//                    size_t vertexId = elements[i+v];
//                    vertices[v][c] = positions[vertexId*3+c];
//                }
//            }
//            glm::vec3 a = vertices[1] - vertices[0];
//            glm::vec3 b = vertices[2] - vertices[1];
//            glm::vec3 n = glm::normalize(glm::cross(a,b));
//            
//            
//            
//            
//            n = (n+1.0f) * 0.5f;
//            for (size_t v=0;v<3;v++){
//                colors[elements[i+v]*3 + 0 ] = n[0];
//                colors[elements[i+v]*3 + 1 ] = n[1];
//                colors[elements[i+v]*3 + 2 ] = n[2];
//            }
//            
//        }
        
        
       
        
        
        
        center = computeCentroid();
        dim = computeDimension();
	}
	
	vector<GLfloat> const getPosition() const
	{ return positions; }
	
	vector<GLfloat> const getColor() const
	{ return colors; }
	
	vector<GLuint> const getElements() const
	{ return elements; }
	
	size_t getVertexCount() const
	{ return positions.size()/3; }
	
	size_t getPositionBytes() const
	{ return positions.size()*sizeof(GLfloat); }
	
	size_t getColorBytes() const
	{ return colors.size()*sizeof(GLfloat); }
	
	size_t getElementBytes() const
	{ return elements.size()*sizeof(GLuint); }
    
    glm::vec3 getMinBound(){
        return glm::vec3(-30,-30,-30);}
    
    glm::vec3 getMaxBound(){
        return glm::vec3(30,30,30);}
    
    glm::vec3 getCentroid()
    { return glm::vec3(0,0,0); }
    
    glm::vec3 getDimension()
    { return dim; }
    
    glm::vec3 getHighestPoint()
    {
        glm::vec3 point;
        point.x = positions[0];
        point.y = positions[1];
        point.z = positions[2];
        for(int i=0; i<positions.size(); i+=3)
        {
            for(int c=0; c<3; c++)
            {
                if(positions[i+1] > point.y){
                    point.x = positions[i+0];
                    point.y = positions[i+1];
                    point.z = positions[i+2];
                }
            }
        }
        return point;
    }
    
    glm::vec3 getLowestPoint()
    {
        glm::vec3 point;
        point.x = positions[0];
        point.y = positions[1];
        point.z = positions[2];
        for(int i=0; i<positions.size(); i+=3)
        {
            for(int c=0; c<3; c++)
            {
                if(positions[i+1] < point.y){
                    point.x = positions[i+0];
                    point.y = positions[i+1];
                    point.z = positions[i+2];
                }
            }
        }
        return point;
    }
    
    glm::vec4 getBound()
    {
        glm::vec4 bound;
        bound.x = positions[0];
        bound.y = positions[0];
        bound.z = positions[2];
        bound.w = positions[2];
        for(int i=0; i<positions.size(); i+=3)
        {
            for(int c=0; c<3; c++)
            {
                if(positions[i] < bound.x){
                    bound.x = positions[i];
                }
                if(positions[i] > bound.y){
                    bound.y = positions[i];
                }
                if(positions[i+2] < bound.z){
                    bound.z = positions[i+2];
                }
                if(positions[i+2] > bound.w){
                    bound.w = positions[i+2];
                }
            }
        }
        return bound;
    }
    
private:
	

	glm::vec3 computeCentroid()
	{
        return glm::vec3(0,0,0);
	}
	
	glm::vec3 computeDimension()
        {
		glm::vec3 max = getMaxBound();
		glm::vec3 min = getMinBound();
		glm::vec3 dim = max - min;
		return dim;
	}
    
	vector<GLfloat> positions;
	vector<GLfloat> colors;
	vector<GLuint> elements;
	size_t objectCount;
    
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 dim;
    glm::vec3 center;
};

#endif