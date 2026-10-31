#include "shader.h"
#include "../../../core/graphics/datatypes/shader_internal.h"
#include "log.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef DEBUG
   #include <stdio.h>
#endif

/*******************************************************************************
 *
 *   STATIC PROTOTYPES
 *
 ******************************************************************************/

// Creates a vertex shader and compiles it from source, logging the
//  compiler's messages in DEBUG builds when it fails
//
//  in:     in_vertex_shader_src
//  out:    out_shader
//  ret:    0 when the shader compiled; 1 when it didn't. Only DEBUG builds
//            check, so other builds always return 0
static int vertex_shader_gl_init(
  struct shader_vertex_t* out_shader,
  const char* in_vertex_shader_src);

// Creates a fragment shader and compiles it from source, logging the
//  compiler's messages in DEBUG builds when it fails
//
//  in:     in_fragment_shader_src
//  out:    out_shader
//  ret:    0 when the shader compiled; 1 when it didn't. Only DEBUG builds
//            check, so other builds always return 0
static int fragment_shader_gl_init(
  struct shader_fragment_t* out_shader,
  const char* in_fragment_shader_src);

// Creates a shader program and attaches a vertex and a fragment shader to it
//
//  in:     in_vertex_shader, in_fragment_shader
//  out:    out_shader_program
//  ret:    0
static int shader_program_gl_init(
  struct shader_program_t* out_shader_program,
  const struct shader_vertex_t* in_vertex_shader,
  const struct shader_fragment_t* in_fragment_shader);

// Links the shaders attached to a program, logging the linker's messages in
//  DEBUG builds when it fails
//
//  in/out: inout_shader_program
//  ret:    0 when the program linked; 1 when it didn't. Only DEBUG builds
//            check, so other builds always return 0
static int shader_program_gl_link(
  struct shader_program_t* inout_shader_program);

// Makes a linked program the one used for drawing
//
//  in:     in_shader_program
static void shader_program_gl_use(
  const struct shader_program_t* in_shader_program);

// Binds a fragment shader output variable to a draw buffer, taking effect
//  when the program is next linked
//
//  in:     in_shader_program, in_buffer_index, in_var_name
static void shader_program_gl_bind_fragment_data_location(
  const struct shader_program_t* in_shader_program,
  unsigned int in_buffer_index,
  const char* in_var_name);

// Finds the location of a vertex attribute in a linked program
//
//  in:     in_shader_program, in_attribute_name
//  ret:    The attribute's location; -1 when the program has no active
//            attribute of that name
static int shader_program_gl_get_attribute_location(
  const struct shader_program_t* in_shader_program,
  const char* in_attribute_name);

// Sets a uniform of the program in use, looking it up by name in
//  in_shader_program. in_uniform_data points to a value of the C type that
//  matches in_uniform_type (an int for SHADER_PROGRAM_UNIFORM_BOOL); an
//  unknown type is logged in DEBUG builds and otherwise ignored
//
//  in:     in_shader_program, in_uniform_name, in_uniform_data,
//            in_uniform_type
static void shader_program_gl_set_uniform(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  const void* in_uniform_data,
  enum shader_program_uniform_t in_uniform_type);

// Deletes a vertex shader
//
//  in/out: inout_shader
static void vertex_shader_gl_destroy(
  struct shader_vertex_t* inout_shader);

// Deletes a fragment shader
//
//  in/out: inout_shader
static void fragment_shader_gl_destroy(
  struct shader_fragment_t* inout_shader);

// Deletes a shader program
//
//  in/out: inout_shader_program
static void shader_program_gl_destroy(
  struct shader_program_t* inout_shader_program);

/*******************************************************************************
 *
 *   Function: vertex_shader_internal_platform_init()
 *
 ******************************************************************************/
int vertex_shader_internal_platform_init(
  struct shader_vertex_t* out_shader,
  const char* in_vertex_shader_src)
{
  return vertex_shader_gl_init(
    out_shader,
    in_vertex_shader_src);
}

/*******************************************************************************
 *
 *   Function: fragment_shader_internal_platform_init()
 *
 ******************************************************************************/
int fragment_shader_internal_platform_init(
  struct shader_fragment_t* out_shader,
  const char* in_fragment_shader_src)
{
  return fragment_shader_gl_init(
    out_shader,
    in_fragment_shader_src);
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_init()
 *
 ******************************************************************************/
int shader_program_internal_platform_init(
  struct shader_program_t* out_shader_program,
  const struct shader_vertex_t* in_vertex_shader,
  const struct shader_fragment_t* in_fragment_shader)
{
  return shader_program_gl_init(
    out_shader_program,
    in_vertex_shader,
    in_fragment_shader);
}


/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_link()
 *
 ******************************************************************************/
int shader_program_internal_platform_link(
  struct shader_program_t* inout_shader_program)
{
  return shader_program_gl_link(inout_shader_program);
}


/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_use()
 *
 ******************************************************************************/
void shader_program_internal_platform_use(
  const struct shader_program_t* in_shader_program)
{
  shader_program_gl_use(in_shader_program);
}


/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_bind_fragment_data_location()
 *
 ******************************************************************************/
void shader_program_internal_platform_bind_fragment_data_location(
  const struct shader_program_t* in_shader_program,
  unsigned int in_buffer_index,
  const char* in_var_name)
{
  shader_program_gl_bind_fragment_data_location(
    in_shader_program,
    in_buffer_index,
    in_var_name);
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_get_attribute_location()
 *
 ******************************************************************************/
int shader_program_internal_platform_get_attribute_location(
  const struct shader_program_t* in_shader_program,
  const char* in_attribute_name)
{
  return shader_program_gl_get_attribute_location(
    in_shader_program,
    in_attribute_name);
}


/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_set_uniform()
 *
 ******************************************************************************/
void shader_program_internal_platform_set_uniform(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  const void* in_uniform_data,
  enum shader_program_uniform_t in_uniform_type)
{
  shader_program_gl_set_uniform(
    in_shader_program,
    in_uniform_name,
    in_uniform_data,
    in_uniform_type);
}


/*******************************************************************************
 *
 *   Function: vertex_shader_internal_platform_destroy()
 *
 ******************************************************************************/
void vertex_shader_internal_platform_destroy(
  struct shader_vertex_t* inout_shader)
{
  vertex_shader_gl_destroy(inout_shader);
}

/*******************************************************************************
 *
 *   Function: fragment_shader_internal_platform_destroy()
 *
 ******************************************************************************/
void fragment_shader_internal_platform_destroy(
  struct shader_fragment_t* inout_shader)
{
  fragment_shader_gl_destroy(inout_shader);
}

/*******************************************************************************
 *
 *   Function: shader_program_internal_platform_destroy()
 *
 ******************************************************************************/
void shader_program_internal_platform_destroy(
  struct shader_program_t* inout_shader_program)
{
  shader_program_gl_destroy(inout_shader_program);
}

/*******************************************************************************
 *
 *   Function: vertex_shader_gl_init()
 *
 ******************************************************************************/
static int vertex_shader_gl_init(
  struct shader_vertex_t* out_shader,
  const char* in_vertex_shader_src)
{
  // Create and compile our vertex shader from our vertex shader source code
  out_shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(
    out_shader->vertex_shader,
    1,
    &(in_vertex_shader_src),
    NULL);
  glCompileShader(out_shader->vertex_shader);

   #ifdef DEBUG
  // Get the status of our shader compilation. If successful,
  //  compilation_status = 1
  glGetShaderiv(
    out_shader->vertex_shader,
    GL_COMPILE_STATUS,
    &(out_shader->compilation_status));
  if (!out_shader->compilation_status) {
    // If an error occurs during compilation, we grab the error log
    glGetShaderInfoLog(
      out_shader->vertex_shader,
      512,
      NULL,
      out_shader->compilation_log);
    log_write(
      LOG_LEVEL_ERROR,
      "%s",
      out_shader->compilation_log);
  }
  return !out_shader->compilation_status;
   #endif

  return 0;
}

/*******************************************************************************
 *
 *   Function: fragment_shader_gl_init()
 *
 ******************************************************************************/
static int fragment_shader_gl_init(
  struct shader_fragment_t* out_shader,
  const char* in_fragment_shader_src)
{
  // Create and compile our fragment shader from our fragment shader source code
  out_shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(
    out_shader->fragment_shader,
    1,
    &(in_fragment_shader_src),
    NULL);
  glCompileShader(out_shader->fragment_shader);

   #ifdef DEBUG
  // Get the status of our shader compilation. If successful,
  //  compilation_status = 1
  glGetShaderiv(
    out_shader->fragment_shader,
    GL_COMPILE_STATUS,
    &(out_shader->compilation_status));
  if (!out_shader->compilation_status) {
    // If an error occurs during compilation, we grab the error log
    glGetShaderInfoLog(
      out_shader->fragment_shader,
      512,
      NULL,
      out_shader->compilation_log);
    log_write(
      LOG_LEVEL_ERROR,
      "%s",
      out_shader->compilation_log);
  }
  return !out_shader->compilation_status;
   #endif

  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_gl_init()
 *
 ******************************************************************************/
static int shader_program_gl_init(
  struct shader_program_t* out_shader_program,
  const struct shader_vertex_t* in_vertex_shader,
  const struct shader_fragment_t* in_fragment_shader)
{
  // Create a shader program and add our vertex and fragment shaders to it
  out_shader_program->shader_program = glCreateProgram();
  glAttachShader(
    out_shader_program->shader_program,
    in_vertex_shader->vertex_shader);
  glAttachShader(
    out_shader_program->shader_program,
    in_fragment_shader->fragment_shader);

  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_gl_link()
 *
 ******************************************************************************/
static int shader_program_gl_link(
  struct shader_program_t* inout_shader_program)
{
  // Link the individual shaders together in the shader program
  glLinkProgram(inout_shader_program->shader_program);

   #ifdef DEBUG
  // Get the status of our shader program linking. If successful,
  //  link_status = 1
  glGetProgramiv(
    inout_shader_program->shader_program,
    GL_LINK_STATUS,
    &(inout_shader_program->link_status));
  if (!inout_shader_program->link_status) {
    // If an error occurs during linking, we grab the error log
    glGetProgramInfoLog(
      inout_shader_program->shader_program,
      512,
      NULL,
      inout_shader_program->link_log);
    log_write(
      LOG_LEVEL_ERROR,
      "%s",
      inout_shader_program->link_log);
  }
  return !inout_shader_program->link_status;
   #endif

  return 0;
}

/*******************************************************************************
 *
 *   Function: shader_program_gl_use()
 *
 ******************************************************************************/
static void shader_program_gl_use(
  const struct shader_program_t* in_shader_program)
{
  glUseProgram(in_shader_program->shader_program);
}

/*******************************************************************************
 *
 *   Function: shader_program_gl_bind_fragment_data_location()
 *
 ******************************************************************************/
static void shader_program_gl_bind_fragment_data_location(
  const struct shader_program_t* in_shader_program,
  unsigned int in_buffer_index,
  const char* in_var_name)
{
  glBindFragDataLocation(
    in_shader_program->shader_program,
    in_buffer_index,
    in_var_name);
}

/*******************************************************************************
 *
 *   Function: shader_program_gl_get_attribute_location()
 *
 ******************************************************************************/
static int shader_program_gl_get_attribute_location(
  const struct shader_program_t* in_shader_program,
  const char* in_attribute_name)
{
  return glGetAttribLocation(
    in_shader_program->shader_program,
    in_attribute_name);
}

/*******************************************************************************
 *
 *   Function: shader_program_gl_set_uniform()
 *
 ******************************************************************************/
static void shader_program_gl_set_uniform(
  const struct shader_program_t* in_shader_program,
  const char* in_uniform_name,
  const void* in_uniform_data,
  enum shader_program_uniform_t in_uniform_type)
{
  GLint uniform_location = glGetUniformLocation(
    in_shader_program->shader_program,
    in_uniform_name);

  switch (in_uniform_type)
  {
    case SHADER_PROGRAM_UNIFORM_BOOL:
    {
      glUniform1i(
        uniform_location,
        *((const int*)in_uniform_data));
    }
    break;
    case SHADER_PROGRAM_UNIFORM_INT:
    {
      int uniform_val = (*((const int*)in_uniform_data));
      glUniform1i(
        uniform_location,
        uniform_val);
    }
    break;
    case SHADER_PROGRAM_UNIFORM_FLOAT:
    {
      glUniform1f(
        uniform_location,
        *((const float*)in_uniform_data));
    }
    break;
    case SHADER_PROGRAM_UNIFORM_FVEC2:
    {
      glUniform2fv(
        uniform_location,
        1,
        (const GLfloat*)in_uniform_data);
    }
    break;
    case SHADER_PROGRAM_UNIFORM_FVEC3:
    {
      glUniform3fv(
        uniform_location,
        1,
        (const GLfloat*)in_uniform_data);
    }
    break;
    case SHADER_PROGRAM_UNIFORM_FVEC4:
    {
      glUniform4fv(
        uniform_location,
        1,
        (const GLfloat*)in_uniform_data);
    }
    break;
    case SHADER_PROGRAM_UNIFORM_FMAT2X2:
    {
      glUniformMatrix2fv(
        uniform_location,
        1,
        GL_FALSE,
        (const GLfloat*)in_uniform_data);
    }
    break;
    case SHADER_PROGRAM_UNIFORM_FMAT3X3:
    {
      glUniformMatrix3fv(
        uniform_location,
        1,
        GL_FALSE,
        (const GLfloat*)in_uniform_data);
    }
    break;
    case SHADER_PROGRAM_UNIFORM_FMAT4X4:
    {
      const union fmatrix_4x4_t* matrix = (const union fmatrix_4x4_t*)in_uniform_data;

      // #ifdef DEBUG
      //    fmatrix_4x4_print(matrix);
      // #endif

      glUniformMatrix4fv(
        uniform_location,
        1,
        GL_FALSE,
        &(matrix->mat[0][0]));
    }
    break;
    default:
    {
         #ifdef DEBUG
      log_write(
        LOG_LEVEL_ERROR,
        "Unknown shader uniform enum type");
         #endif
    }
    break;
  }

  // #ifdef DEBUG
  //  fprintf(stdout, "Uploaded uniform of type %d at location %d\n",
  //  uniform_type, uniform_location);
  // #endif
}

/*******************************************************************************
 *
 *   Function: vertex_shader_gl_destroy()
 *
 ******************************************************************************/
static void vertex_shader_gl_destroy(
  struct shader_vertex_t* inout_shader)
{
  glDeleteShader(inout_shader->vertex_shader);
}

/*******************************************************************************
 *
 *   Function: fragment_shader_gl_destroy()
 *
 ******************************************************************************/
static void fragment_shader_gl_destroy(
  struct shader_fragment_t* inout_shader)
{
  glDeleteShader(inout_shader->fragment_shader);
}

/*******************************************************************************
 *
 *   Function: shader_program_gl_destroy()
 *
 ******************************************************************************/
static void shader_program_gl_destroy(
  struct shader_program_t* inout_shader_program)
{
  glDeleteProgram(inout_shader_program->shader_program);
}
