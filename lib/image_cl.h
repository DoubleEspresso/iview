#ifndef LIB_IMAGE_CL
#define LIB_IMAGE_CL

#include <utility>
//#define __NO_STD_VECTOR


#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <vector>
#include <CL/cl.hpp>

//#include "image.h"

using namespace std;
using namespace cl;

inline void check_err(cl_int err, const char * name)
{
  if (err != CL_SUCCESS)
    {
printf("..[Opencl error] abort from CL api call (%s), error: (%d)\n", name, err);
    }
  exit(EXIT_FAILURE);
}

// first draft of opencl image ops
class ImageCL
{
  Context * clcontext;  
  
public:
ImageCL() : clcontext(0) {}
~ImageCL() { if (clcontext) { delete clcontext; clcontext = 0; } }

// openCL specific initialization routines
// TODO: move these to a generic opencl header/wrapper class (?)

// context creation
void create_cl_context();
void create_cl_kernel_buffer(Buffer& cl_buffer, int opts, int _size);
vector<Device> get_devices_list();
Kernel load_cl_kernel_file(const char * fname, const char * entry_point, Buffer &out_buffer);
CommandQueue run_kernel(Kernel& k, Device& d, int size);

};


#endif
