#pragma once

enum vertex_attribute_data_type_t
{
   V_BYTE = 0x00,
   V_UNSIGNED_BYTE,
   V_SHORT,
   V_UNSIGNED_SHORT,
   V_INT,
   V_UNSIGNED_INT,
   V_HALF_FLOAT,
   V_FLOAT,
   V_DOUBLE,
   V_FIXED
};

struct vertex_attribute_t
{
   char* attribute_name;
   unsigned int index;
   int size;
   enum vertex_attribute_data_type_t data_type;
   unsigned int should_normalize;
   unsigned int stride;
   void* ptr_offset_to_attrib;
};

void vertex_attribute_init(struct vertex_attribute_t* vertex_attribute,
                           char* attribute_name,
                           unsigned int index, 
                           int size, 
                           enum vertex_attribute_data_type_t data_type, 
                           unsigned int should_normalize, 
                           unsigned int stride, 
                           void* ptr_offset_to_attrib);