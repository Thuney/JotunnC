#include "vertexattribute.h"

extern void platform_vertex_attribute_init(struct vertex_attribute_t* vertex_attribute, 
                                           unsigned int index, 
                                           int size, 
                                           enum vertex_attribute_data_type_t data_type, 
                                           unsigned int should_normalize, 
                                           unsigned int stride, 
                                           void* ptr_offset_to_attrib);

void vertex_attribute_init(struct vertex_attribute_t* vertex_attribute, 
                           unsigned int index, 
                           int size, 
                           enum vertex_attribute_data_type_t data_type, 
                           unsigned int should_normalize, 
                           unsigned int stride, 
                           void* ptr_offset_to_attrib)
{
   platform_vertex_attribute_init(vertex_attribute, index, size, data_type, should_normalize, stride, ptr_offset_to_attrib);
}