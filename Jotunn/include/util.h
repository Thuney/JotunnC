#pragma once

#include <stdlib.h>

#define STRINGIFY(x) #x
#define ROOT_PATHIFY(x) STRINGIFY(x)
#define PATH_FROM_JOTUNN_ROOT(x) ROOT_PATHIFY(JOTUNN_ROOT_PATH) x

char* read_raw_file_contents(const char* file_path);