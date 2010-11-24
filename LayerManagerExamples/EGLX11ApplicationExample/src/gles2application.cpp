/***************************************************************************
*
* Copyright 2010 BMW Car IT GmbH
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*		http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
****************************************************************************/
#include "egl_helper.h"
#include "gles2application.h"
#include "IlmMatrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct t_shaderObject
{
    GLuint      uiFragShader;
    GLuint      uiVertShader;
    GLuint      uiProgramObject;
    GLint       matrixLocation;
    GLint       colorLocation;
} gles2Shader;

typedef struct vertexBufferObject
{
    GLuint      vbo;
} gles2VertexBuffer;

static gles2Shader shader;
static gles2VertexBuffer vertexBuffer;

/* Fragment and vertex shaders code */
const char* sourceFragShader = "\
        uniform mediump vec4    u_color;\
        void main (void)\
        {\
                gl_FragColor = u_color;\
        }";
const char* sourceVertShader = "\
        attribute highp vec4    a_vertex;\
        uniform mediump mat4    u_matrix;\
        void main(void)\
        {\
                gl_Position = u_matrix*a_vertex;\
        }";
GLfloat triangleVertexData[] = {   -0.4f ,-0.4f ,0.0f,
                                   -0.2f ,-0.4f ,0.0f,
                                   -0.3f ,-0.2f ,0.0f,

                                   -0.2f ,-0.4f ,0.0f,
                                   -0.0f ,-0.4f ,0.0f,
                                   -0.1f ,-0.2f ,0.0f,

                                   -0.0f ,-0.4f ,0.0f,
                                    0.2f ,-0.4f ,0.0f,
                                    0.1f ,-0.2f ,0.0f,

                                    0.2f ,-0.4f ,0.0f,
                                    0.4f ,-0.4f ,0.0f,
                                    0.3f ,-0.2f ,0.0f,

                                    -0.3f ,-0.2f ,0.0f,
                                    -0.1f ,-0.2f ,0.0f,
                                    -0.2f ,-0.0f ,0.0f,

                                    0.1f ,-0.2f ,0.0f,
                                    0.3f ,-0.2f ,0.0f,
                                    0.2f ,-0.0f ,0.0f,

                                    -0.2f ,-0.0f ,0.0f,
                                    -0.0f ,-0.0f ,0.0f,
                                    -0.1f ,0.2f ,0.0f,

                                    0.0f ,-0.0f ,0.0f,
                                    0.2f ,-0.0f ,0.0f,
                                    0.1f ,0.2f ,0.0f,

                                    -0.1f ,0.2f ,0.0f,
                                    0.1f ,0.2f ,0.0f,
                                    0.0f ,0.4f ,0.0f



                               };


t_ilm_bool initGlApplication()
{
  t_ilm_bool result = ILM_TRUE;
  if ( !initShader() ) return ILM_FALSE;
  if ( !initVertexBuffer() ) return ILM_FALSE;

  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  return result;
}

t_ilm_bool initShader()
{
  t_ilm_bool result = ILM_TRUE;

  /* Create the fragment shader object */
  shader.uiFragShader = glCreateShader(GL_FRAGMENT_SHADER);

  /* Load Fragment Source */
  glShaderSource(shader.uiFragShader, 1, (const char**)&sourceFragShader, NULL);

  /* Compile the source code of fragment shader */
  glCompileShader(shader.uiFragShader);

  glGetShaderiv(shader.uiFragShader, GL_COMPILE_STATUS, (GLint*)&result);

  if (!result)
  {
          t_ilm_int infoLength, numberChars;
          glGetShaderiv(shader.uiFragShader, GL_INFO_LOG_LENGTH, &infoLength);

          /* Allocate Log Space */
          char* info = (char*)malloc(sizeof(char)*infoLength);
          glGetShaderInfoLog(shader.uiFragShader, infoLength, &numberChars, info);

          /* Print the error */
          printf("Failed to compile fragment shader: %s\n", info);
          free(info);
          return ILM_FALSE;
  }

  /* Create the fragment shader object */
    shader.uiVertShader = glCreateShader(GL_VERTEX_SHADER);

    /* Load Fragment Source */
    glShaderSource(shader.uiVertShader, 1, (const char**)&sourceVertShader, NULL);

    /* Compile the source code of fragment shader */
    glCompileShader(shader.uiVertShader);

    glGetShaderiv(shader.uiVertShader, GL_COMPILE_STATUS, (GLint*)&result);

    if (!result)
    {
            t_ilm_int infoLength, numberChars;
            glGetShaderiv(shader.uiVertShader, GL_INFO_LOG_LENGTH, &infoLength);

            /* Allocate Log Space */
            char* info = (char*)malloc(sizeof(char)*infoLength);
            glGetShaderInfoLog(shader.uiVertShader, infoLength, &numberChars, info);

            /* Print the error */
            printf("Failed to compile vertex shader: %s\n", info);
            free(info);
            return ILM_FALSE;
    }

    shader.uiProgramObject = glCreateProgram();

    glAttachShader(shader.uiProgramObject, shader.uiFragShader);
    glAttachShader(shader.uiProgramObject, shader.uiVertShader);

    glBindAttribLocation(shader.uiProgramObject, 0, "a_vertex");

    glLinkProgram(shader.uiProgramObject);

    glGetProgramiv(shader.uiProgramObject, GL_LINK_STATUS, (GLint*)&result);

    if (!result)
    {
            t_ilm_int infoLength, numberChars;
            glGetShaderiv(shader.uiProgramObject, GL_INFO_LOG_LENGTH, &infoLength);

            /* Allocate Log Space */
            char* info = (char*)malloc(sizeof(char)*infoLength);
            glGetShaderInfoLog(shader.uiProgramObject, infoLength, &numberChars, info);

            /* Print the error */
            printf("Failed to link program: %s\n", info);
            free(info);
            return ILM_FALSE;
    }
    glUseProgram(shader.uiProgramObject);
    shader.matrixLocation = glGetUniformLocation(shader.uiProgramObject, "u_matrix");
    shader.colorLocation = glGetUniformLocation(shader.uiProgramObject, "u_color");
    return result;
}

t_ilm_bool destroyShader()
{
  t_ilm_bool result = ILM_TRUE;
  glDeleteProgram(shader.uiProgramObject);
  glDeleteShader(shader.uiFragShader);
  glDeleteShader(shader.uiVertShader);
  return result;
}


t_ilm_bool initVertexBuffer()
{
  t_ilm_bool result = ILM_TRUE;

  glGenBuffers(1, &vertexBuffer.vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.vbo);

  unsigned int uiSize = 27 * (sizeof(GLfloat) * 3);
  glBufferData(GL_ARRAY_BUFFER, uiSize, &triangleVertexData[0], GL_STATIC_DRAW);

  return result;
}

void attachVertexBuffer()
{
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

}

void detachVertexBuffer()
{
  glBindBuffer(GL_ARRAY_BUFFER,NULL);
}

void destroyVertexBuffer()
{
  glDeleteBuffers(1,&vertexBuffer.vbo);
}


void draw(t_ilm_uint animTime)
{
  int i,j = 0;
  static t_ilm_uint startTime = 0;
  static t_ilm_uint currentTime = 0;
  static float angle = 0;
  float currentAngle = 0;
  t_ilm_bool result = ILM_TRUE;
  IlmMatrix matrix;
  currentTime = GetTickCount();
  if ( (currentTime - startTime) > animTime )
  {
    glClear(GL_COLOR_BUFFER_BIT);
    for ( i = 10; i > 0 ; i-- )
    {
      currentAngle = angle - ((float)i) * 10.0f;
      IlmMatrixIdentity(matrix);
      IlmMatrixRotateZ(matrix,currentAngle);
      float color[4] = {0.0,1.0,1.0, 0.5 +  (0.3 / (float) i) };
      float lineColor[4] = {0.0,0.0,0.0, 0.5 + (0.4 / (float) i) };
      glUseProgram(shader.uiProgramObject);
      attachVertexBuffer();

      glUniformMatrix4fv( shader.matrixLocation, 1, GL_FALSE, &matrix.f[0]);
      glUniform4fv(shader.colorLocation,1,&color[0]);
      glDrawArrays(GL_TRIANGLES, 0, 27);

      glUniformMatrix4fv( shader.matrixLocation, 1, GL_FALSE, &matrix.f[0]);
      glUniform4fv(shader.colorLocation,1,&lineColor[0]);
      for (j=0;j<9;j++)
        {
          glDrawArrays(GL_LINE_LOOP, j*3, 3);
        }
    }
    angle += 10.0f;
    detachVertexBuffer();
    swapBuffers();
    startTime = currentTime;
  }
}


void destroyGlApplication()
{
  destroyShader();
  destroyVertexBuffer();
}

