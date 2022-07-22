#include "vertexattribute.h"

static void gl_platform_vertex_attribute_init(struct vertex_attribute_t* vertex_attribute, 
                                              char* attribute_name,
                                              unsigned int index, 
                                              int size, 
                                              enum vertex_attribute_data_type_t data_type, 
                                              unsigned int should_normalize, 
                                              unsigned int stride, 
                                              void* ptr_offset_to_attrib)
{
   vertex_attribute->attribute_name       = attribute_name;
   vertex_attribute->index                = index;
   vertex_attribute->size                 = size;
   vertex_attribute->data_type            = data_type;
   vertex_attribute->should_normalize     = should_normalize;
   vertex_attribute->stride               = stride;
   vertex_attribute->ptr_offset_to_attrib = ptr_offset_to_attrib;
}

void platform_vertex_attribute_init(struct vertex_attribute_t* vertex_attribute,
                                    char* attribute_name,
                                    unsigned int index, 
                                    int size, 
                                    enum vertex_attribute_data_type_t data_type, 
                                    unsigned int should_normalize, 
                                    unsigned int stride, 
                                    void* ptr_offset_to_attrib)
{
   gl_platform_vertex_attribute_init(vertex_attribute, attribute_name, index, size, data_type, should_normalize, stride, ptr_offset_to_attrib);
}