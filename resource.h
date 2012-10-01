#ifndef __GUARD_resource_h__
#define __GUARD_resource_h__

#include <string.h>

int get_resource_fd(const char* text);
int get_resource_size(int fd);
void read_resource_binary(int fd, void* data);

#endif