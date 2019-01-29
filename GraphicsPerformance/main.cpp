//
//  main.cpp
//  GraphicsPerformance
//
//  Created by Bo Miller on 1/17/19.
//  Copyright © 2019 Bo Miller. All rights reserved.
//

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
// Include GLEW
#include <GL/glew.h>
#include <iostream>
// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
using namespace glm;

#include "shader.hpp"
#include <chrono>
#include "texture.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
int main( void )
{
    
    //Write the random Texture we will use as our really large texture
    int w = (int) 10000;
    int h = (int) 10000;
    
    uint8_t *pix;
    pix = (uint8_t *)malloc(w*h*3 * sizeof(uint8_t));
    memset(pix, 0xff, w*h*3);
    for(int i = 0; i<h; i++)
    {
        for(int j = 0;j<w;j++)
        {
            int elem = (i*w*3) + (j*3);
            pix[elem+0] = (uint8_t) ((((float) rand()) / (float) RAND_MAX) * 255.0);
            pix[elem+1] = (uint8_t) ((((float) rand()) / (float) RAND_MAX) * 255.0);
            pix[elem+2] = (uint8_t) ((((float) rand()) / (float) RAND_MAX) * 255.0);
            
        }
    }
    stbi_write_bmp("/tmp/largeTexture.bmp", w, h, 3, pix);

    
    
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Graphics Perfomance", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    
    
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "/Users/bo/Documents/normal_vert.txt", "/Users/bo/Documents/frag.txt" );
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    
    // Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 View       = glm::lookAt(
                                       glm::vec3(32,50,40), // Camera is at (4,3,-3), in World Space
                                       glm::vec3(32,0,-16), // and looks at the origin
                                       glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                       );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model      = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
    
    
    
    int numsamples = 0;
    double samples = 0.0;
    
    int numTriangles = 64;
    //****NOTE: might have to specify absolute path to texture****
    GLuint Texture = loadBMP_custom("/tmp/largeTexture.bmp");
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    do{
        std::vector<glm::vec3> verts;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvCoords;
        for(int i = 0; i<numTriangles;i++)
        {
            for(int j = 0; j<numTriangles; j++)
            {
                glm::vec3 v1 = glm::vec3(float(i)*(64.0f/float(numTriangles)),0.0f,float(-j*(64.0f/float(numTriangles))));
                glm::vec3 v2 = glm::vec3(float(i+1)*(64.0f/float(numTriangles)),0.0f,float(-j)*(64.0f/float(numTriangles)));
                glm::vec3 v3 = glm::vec3(float(i+1)*(64.0f/float(numTriangles)),0.0f,float(-(j+1))*(64.0f/float(numTriangles)));
                glm::vec3 v4 = glm::vec3(float(i)*(64.0f/float(numTriangles)),0.0f,float(-j)*(64.0f/float(numTriangles)));
                glm::vec3 v5 = glm::vec3(float(i)*(64.0f/float(numTriangles)),0.0f,float(-(j+1))*(64.0f/float(numTriangles)));
                glm::vec3 v6 = glm::vec3(float(i+1)*(64.0f/float(numTriangles)),0.0f,float(-(j+1))*(64.0f/float(numTriangles)));
                
                verts.push_back(v1);
                uvCoords.push_back(glm::vec2((float(i)/float(numTriangles)),(float(j)/float(numTriangles))));
                verts.push_back(v2);
                uvCoords.push_back(glm::vec2((float(i+1)/float(numTriangles)),(float(j)/float(numTriangles))));
                verts.push_back(v3);
                uvCoords.push_back(glm::vec2((float(i+1)/float(numTriangles)),(float(j+1)/float(numTriangles))));
                
                verts.push_back(v4);
                uvCoords.push_back(glm::vec2((float(i)/float(numTriangles)),(float(j)/float(numTriangles))));
                verts.push_back(v5);
                uvCoords.push_back(glm::vec2((float(i)/float(numTriangles)),(float(j+1)/float(numTriangles))));
                verts.push_back(v6);
                uvCoords.push_back(glm::vec2((float(i+1)/float(numTriangles)),(float(j+1)/float(numTriangles))));
            }
        }
        
        
        float i_norm = 0.0f;
        float i_normm1 = -0.1;
        float inc = (2.0f*M_PI)/14.0f;
        for(int i = 0; i<numTriangles;i++)
        {
            i_normm1 = i_norm;
            i_norm = i_norm + inc;
            
            float j_norm = -0.1f;
            float j_normp1 = 0.0;
            for(int j = 0; j<numTriangles; j++)
            {
                int loc = i*2*numTriangles*3+j*6;
                verts[loc][1] = .5*sin(i_normm1)+2*sin(j_norm);
                verts[loc+1][1] = .5*sin(i_norm)+2*sin(j_norm);
                verts[loc+2][1]= .5*sin(i_norm)+2*sin(j_normp1);
                verts[loc+3][1] = .5*sin(i_normm1)+2*sin(j_norm);
                verts[loc+4][1] = .5*sin(i_normm1)+2*sin(j_normp1);
                verts[loc+5][1] = .5*sin(i_norm)+2*sin(j_normp1);
                
                glm::vec3 n1 = glm::normalize(glm::cross((verts[loc+1]-verts[loc]),(verts[loc+2]-verts[loc])));
                glm::vec3 n2 = glm::normalize(glm::cross((verts[loc+4]-verts[loc+3]),(verts[loc+5]-verts[loc+3])));
                normals.push_back(n1);
                normals.push_back(n1);
                normals.push_back(n1);
                //normal was facing wrong way
                normals.push_back(-n2);
                normals.push_back(-n2);
                normals.push_back(-n2);
                
                j_norm   = j_normp1;
                j_normp1 = j_normp1 + inc;
                
            }
        }
        
        //used to fill a interleaved vert/uv array
        /*float *triangleData;
         int trianglesSize = numTriangles*numTriangles*2*3*3+numTriangles*numTriangles*2*3*2;
         triangleData = (float *)malloc(trianglesSize * sizeof(float));
         int locInArray = 0;
         for(int i = 0; i<numTriangles;i++)
         {
         for(int j = 0; j<numTriangles; j++)
         {
         int loc = i*2*numTriangles*3+j*6;
         
         triangleData[locInArray] = verts[loc][0];
         triangleData[locInArray+1] = verts[loc][1];
         triangleData[locInArray+2] = verts[loc][2];
         triangleData[locInArray+3] = uvCoords[loc][0];
         triangleData[locInArray+4] = uvCoords[loc][1];
         locInArray += 5;
         
         triangleData[locInArray] = verts[loc+1][0];
         triangleData[locInArray+1] = verts[loc+1][1];
         triangleData[locInArray+2] = verts[loc+1][2];
         triangleData[locInArray+3] = uvCoords[loc+1][0];
         triangleData[locInArray+4] = uvCoords[loc+1][1];
         locInArray += 5;
         
         triangleData[locInArray] = verts[loc+2][0];
         triangleData[locInArray+1] = verts[loc+2][1];
         triangleData[locInArray+2] = verts[loc+2][2];
         triangleData[locInArray+3] = uvCoords[loc+2][0];
         triangleData[locInArray+4] = uvCoords[loc+2][1];
         locInArray += 5;
         
         triangleData[locInArray] = verts[loc+3][0];
         triangleData[locInArray+1] = verts[loc+3][1];
         triangleData[locInArray+2] = verts[loc+3][2];
         triangleData[locInArray+3] = uvCoords[loc+3][0];
         triangleData[locInArray+4] = uvCoords[loc+3][1];
         locInArray += 5;
         
         triangleData[locInArray] = verts[loc+4][0];
         triangleData[locInArray+1] = verts[loc+4][1];
         triangleData[locInArray+2] = verts[loc+4][2];
         triangleData[locInArray+3] = uvCoords[loc+4][0];
         triangleData[locInArray+4] = uvCoords[loc+4][1];
         locInArray += 5;
         
         triangleData[locInArray] = verts[loc+5][0];
         triangleData[locInArray+1] = verts[loc+5][1];
         triangleData[locInArray+2] = verts[loc+5][2];
         triangleData[locInArray+3] = uvCoords[loc+5][0];
         triangleData[locInArray+4] = uvCoords[loc+5][1];
         locInArray += 5;
         }
         }*/
        
        //used to fill a interleaved vert/normal array
        float *triangleData;
        int trianglesSize = numTriangles*numTriangles*2*3*3+numTriangles*numTriangles*2*3*3;
        triangleData = (float *)malloc(trianglesSize * sizeof(float));
        int locInArray = 0;
        for(int i = 0; i<numTriangles;i++)
        {
            for(int j = 0; j<numTriangles; j++)
            {
                int loc = i*2*numTriangles*3+j*6;
                
                triangleData[locInArray] = verts[loc][0];
                triangleData[locInArray+1] = verts[loc][1];
                triangleData[locInArray+2] = verts[loc][2];
                triangleData[locInArray+3] = normals[loc][0];
                triangleData[locInArray+4] = normals[loc][1];
                triangleData[locInArray+5] = normals[loc][2];
                locInArray += 6;
                
                triangleData[locInArray] = verts[loc+1][0];
                triangleData[locInArray+1] = verts[loc+1][1];
                triangleData[locInArray+2] = verts[loc+1][2];
                triangleData[locInArray+3] = normals[loc+1][0];
                triangleData[locInArray+4] = normals[loc+1][1];
                triangleData[locInArray+5] = normals[loc+1][2];
                locInArray += 6;
                
                triangleData[locInArray] = verts[loc+2][0];
                triangleData[locInArray+1] = verts[loc+2][1];
                triangleData[locInArray+2] = verts[loc+2][2];
                triangleData[locInArray+3] = normals[loc+2][0];
                triangleData[locInArray+4] = normals[loc+2][1];
                triangleData[locInArray+5] = normals[loc+2][2];
                locInArray += 6;
                
                triangleData[locInArray] = verts[loc+3][0];
                triangleData[locInArray+1] = verts[loc+3][1];
                triangleData[locInArray+2] = verts[loc+3][2];
                triangleData[locInArray+3] = normals[loc+3][0];
                triangleData[locInArray+4] = normals[loc+3][1];
                triangleData[locInArray+5] = normals[loc+3][2];
                locInArray += 6;
                
                triangleData[locInArray] = verts[loc+4][0];
                triangleData[locInArray+1] = verts[loc+4][1];
                triangleData[locInArray+2] = verts[loc+4][2];
                triangleData[locInArray+3] = normals[loc+4][0];
                triangleData[locInArray+4] = normals[loc+4][1];
                triangleData[locInArray+5] = normals[loc+4][2];
                locInArray += 6;
                
                triangleData[locInArray] = verts[loc+5][0];
                triangleData[locInArray+1] = verts[loc+5][1];
                triangleData[locInArray+2] = verts[loc+5][2];
                triangleData[locInArray+3] = normals[loc+5][0];
                triangleData[locInArray+4] = normals[loc+5][1];
                triangleData[locInArray+5] = normals[loc+5][2];
                locInArray += 6;
            }
        }
        
        
        //puts all of our information into individual arrays
        float *vertslist;
        vertslist = (float *)malloc((numTriangles*numTriangles*2*3*3) * sizeof(float));
        int numverts = 0;
        float *normalslist;
        normalslist = (float *)malloc((numTriangles*numTriangles*2*3*3) * sizeof(float));
        float *uvlist;
        uvlist = (float *)malloc((numTriangles*numTriangles*2*3*2) * sizeof(float));
        
        for(glm::vec3 v:verts)
        {
            vertslist[numverts] = v[0];
            vertslist[numverts+1] = v[1];
            vertslist[numverts+2] = v[2];
            numverts+=3;
        }
        int numnormals = 0;
        for(glm::vec3 n:normals)
        {
            normalslist[numnormals] = n[0];
            normalslist[numnormals+1] = n[1];
            normalslist[numnormals+2] = n[2];
            numnormals+=3;
        }
        int numuv = 0;
        for(glm::vec2 uv:uvCoords)
        {
            uvlist[numuv] = uv[0];
            uvlist[numuv+1] = uv[1];
            numuv+=2;
        }
        
        /*GLuint vertexbuffer;
         glGenBuffers(1, &vertexbuffer);
         glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
         glBufferData(GL_ARRAY_BUFFER, (numTriangles*numTriangles*2*3*3)*sizeof(float), &vertslist[0], GL_STATIC_DRAW);*/
        
        
        /*GLuint normalBuffer;
         glGenBuffers(1, &normalBuffer);
         glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
         glBufferData(GL_ARRAY_BUFFER, (numTriangles*numTriangles*2*3*3)*sizeof(float), &normalslist[0], GL_STATIC_DRAW);*/
        
        /*GLuint uv;
         glGenBuffers(1, &uv);
         glBindBuffer(GL_ARRAY_BUFFER, uv);
         glBufferData(GL_ARRAY_BUFFER, (numTriangles*numTriangles*2*3*2)*sizeof(float), &uvlist[0], GL_STATIC_DRAW);*/
        
        //one large vertex buffer, interleaved
        GLuint triangles;
        glGenBuffers(1, &triangles);
        glBindBuffer(GL_ARRAY_BUFFER, triangles);
        glBufferData(GL_ARRAY_BUFFER, (trianglesSize)*sizeof(float), &triangleData[0], GL_STATIC_DRAW);
    
    
        
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        // Use our shader
        glUseProgram(programID);
        
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);
        
        // 1st attribute buffer : vertices
        /*glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                              0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // 2nd attribute buffer : normals
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(
                              1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );
         
         //seperate vertex buffer for uv
         /* glEnableVertexAttribArray(1);
         glBindBuffer(GL_ARRAY_BUFFER, uv);
         glVertexAttribPointer(
                              1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                              2,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );*/
        
        
        //Both verts and UV/Normals in one buffer
        //****Adjust parameters if needed, size 2 for uv, size 3 for normals
        //****Stride 5 for uv, stride 6 for normals
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, triangles);
        glVertexAttribPointer(
                              0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              sizeof(float)*6,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, triangles);
        glVertexAttribPointer(
                              1,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              sizeof(float)*6,                  // stride
                              (void*)(3 * sizeof(float))            // array buffer offset
                              );
        
        
        // Draw the triangles!
        glFinish();
        double before = glfwGetTime();
        glDrawArrays(GL_TRIANGLES, 0, numTriangles*numTriangles*2*3);
        glFinish();
        double after = glfwGetTime();
        numsamples++;
        samples = samples + after-before;
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        //Cleanup VBO and shader
        //glDeleteBuffers(1, &vertexbuffer);
        //glDeleteBuffers(1, &normalBuffer);
        //glDeleteBuffers(1, &uv);
        glDeleteBuffers(1, &triangles);
        glDeleteProgram(programID);
        
        
        free(triangleData);
        free(vertslist);
        free(normalslist);
        free(uvlist);
        //see if we have collected enough samples yet
        if(numsamples == 1000)
        {
            printf("%d samples %f ms/frame\n", numTriangles*numTriangles*2,(samples/numsamples)*1000);
            samples = 0.0;
            numsamples = 0;
            numTriangles +=64;
        }
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);
    glfwTerminate();
    return 0;
}
