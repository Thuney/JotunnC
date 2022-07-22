#include "shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef DEBUG
   #include <stdio.h>
#endif

static int gl_vertex_shader_init(struct vertex_shader_t* shader, const char* vertex_shader_src)
{
   //Create and compile our vertex shader from our vertex shader source code
	shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader->vertex_shader, 1, &vertex_shader_src, NULL);
	glCompileShader(shader->vertex_shader);

   #ifdef DEBUG
      //Get the status of our shader compilation. If successful, vertStatus = 1
      glGetShaderiv(shader->vertex_shader, GL_COMPILE_STATUS, &shader->compilation_status);
      if (!shader->compilation_status)
      {
         //If an error occurs during compilation, we grab the error log
         glGetShaderInfoLog(shader->vertex_shader, 512, NULL, shader->compilation_log);
         fprintf(stdout, "%s\n", shader->compilation_log);
      }
      return shader->compilation_status;
   #endif

   return 0;
}

static int gl_fragment_shader_init(struct fragment_shader_t* shader, const char* fragment_shader_src)
{
   //Create and compile our vertex shader from our vertex shader source code
	shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader->fragment_shader, 1, &fragment_shader_src, NULL);
	glCompileShader(shader->fragment_shader);

   #ifdef DEBUG
      //Get the status of our shader compilation. If successful, vertStatus = 1
      glGetShaderiv(shader->fragment_shader, GL_COMPILE_STATUS, &shader->compilation_status);
      if (!shader->compilation_status)
      {
         //If an error occurs during compilation, we grab the error log
         glGetShaderInfoLog(shader->fragment_shader, 512, NULL, shader->compilation_log);
         fprintf(stdout, "%s\n", shader->compilation_log);
      }
      return shader->compilation_status;
   #endif

   return 0;
}

static int gl_shader_program_init(struct shader_program_t* shader_program, struct vertex_shader_t* vertex_shader, struct fragment_shader_t* fragment_shader)
{
   //Create a shader program and add our vertex and fragment shaders to it
	shader_program->shader_program = glCreateProgram();
	glAttachShader(shader_program->shader_program, vertex_shader->vertex_shader);
	glAttachShader(shader_program->shader_program, fragment_shader->fragment_shader);

   return 0;
}

static int gl_shader_program_link(struct shader_program_t* shader_program)
{
   //Link the individual shaders together in the shader program and set the program to be used
	glLinkProgram(shader_program->shader_program);

   #ifdef DEBUG
      //Get the status of our shader program linking. If successful, shaderLinkStatus = 1
      glGetProgramiv(shader_program->shader_program, GL_LINK_STATUS, &shader_program->link_status);
      if (!shader_program->link_status)
      {
         //If an error occurs during compilation, we grab the error log
         glGetProgramInfoLog(shader_program->shader_program, 512, NULL, shader_program->link_log);
         fprintf(stdout, "%s\n", shader_program->link_log);
      }
      return shader_program->link_status;
   #endif

   return 0;
}

static void gl_shader_program_use(struct shader_program_t* shader_program)
{
   glUseProgram(shader_program->shader_program);
}

static void gl_shader_program_bind_fragment_data_location(struct shader_program_t* shader_program, unsigned int buffer_index, const char* var_name)
{
   glBindFragDataLocation(shader_program->shader_program, buffer_index, var_name);
}

static int gl_shader_program_get_attribute_location(struct shader_program_t* shader_program, const char* attribute_name)
{
   return glGetAttribLocation(shader_program->shader_program, attribute_name);
}

static void gl_vertex_shader_destroy(struct vertex_shader_t* shader)
{
   glDeleteShader(shader->vertex_shader);
}

static void gl_fragment_shader_destroy(struct fragment_shader_t* shader)
{
   glDeleteShader(shader->fragment_shader);
}

static void gl_shader_program_destroy(struct shader_program_t* shader_program)
{
   glDeleteProgram(shader_program->shader_program);
}

// Extern functions

int platform_vertex_shader_init(struct vertex_shader_t* shader, const char* vertex_shader_src)
{
   return gl_vertex_shader_init(shader, vertex_shader_src);
}

int platform_fragment_shader_init(struct fragment_shader_t* shader, const char* fragment_shader_src)
{
   return gl_fragment_shader_init(shader, fragment_shader_src);
}

int platform_shader_program_init(struct shader_program_t* shader_program, struct vertex_shader_t* vertex_shader, struct fragment_shader_t* fragment_shader)
{
   return gl_shader_program_init(shader_program, vertex_shader, fragment_shader);
}

int platform_shader_program_link(struct shader_program_t* shader_program)
{
   return gl_shader_program_link(shader_program);
}

void platform_shader_program_use(struct shader_program_t* shader_program)
{
   gl_shader_program_use(shader_program);
}

void platform_shader_program_bind_fragment_data_location(struct shader_program_t* shader_program, unsigned int buffer_index, const char* var_name)
{
   gl_shader_program_bind_fragment_data_location(shader_program, buffer_index, var_name);
}

int platform_shader_program_get_attribute_location(struct shader_program_t* shader_program, const char* attribute_name)
{
   gl_shader_program_get_attribute_location(shader_program, attribute_name);
}

void platform_vertex_shader_destroy(struct vertex_shader_t* shader)
{
   gl_vertex_shader_destroy(shader);
}

void platform_fragment_shader_destroy(struct fragment_shader_t* shader)
{
   gl_fragment_shader_destroy(shader);
}

void platform_shader_program_destroy(struct shader_program_t* shader_program)
{
   gl_shader_program_destroy(shader_program);
}