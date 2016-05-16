#include "image_cl.h"


void ImageCL::create_cl_context()
{
  vector< Platform > cl_platforms;
  Platform::get(&cl_platforms);
  check_err(cl_platforms.size() != 0 ? CL_SUCCESS : -1, "Platform::get");
  printf("..cl platform nb: %d\n", cl_platforms.size());
  std::string platform_vendor;
  cl_platforms[0].getInfo( (cl_platform_info)CL_PLATFORM_VENDOR, &platform_vendor);
  printf("..cl platform vendor: %s\n",platform_vendor.c_str());
  cl_context_properties cprops[3] =
    {
      CL_CONTEXT_PLATFORM, 
      (cl_context_properties)(cl_platforms[0])(),
      0
    };
  
  cl_int err;
  clcontext = new Context((cl_device_type) CL_DEVICE_TYPE_CPU, &cprops[0], NULL, NULL, &err);
  check_err(err, "Context::Context()");
}

void ImageCL::create_cl_kernel_buffer(Buffer& cl_buffer, int opts, int _size)
{
  cl_int err;
  char * buff = new char[_size+1];
  Buffer outb(*clcontext, (cl_mem_flags) opts, _size+1, (void*) buff, &err);
  std::memcpy(&cl_buffer, &outb, sizeof(Buffer));
  check_err(err, "Buffer::Buffer()");
}

vector<Device> ImageCL::get_devices_list()
{
  vector<Device> devices;
  devices = clcontext->getInfo<CL_CONTEXT_DEVICES>();
  check_err(devices.size() > 0 ? CL_SUCCESS : -1, "context.getInfo<CL_CONTEXT_DEVICES>");
  return devices;
}

Kernel ImageCL::load_cl_kernel_file(const char * fname, const char * entry_point, Buffer &out_buffer)
{
  std::ifstream file(fname);

  char err_c[256];
  sprintf(err_c, "std::load(%s)",fname);
  check_err(file.is_open() ? CL_SUCCESS:-1, err_c);

  std::string prog(std::istreambuf_iterator<char>(file),
		   (std::istreambuf_iterator<char>()));
  
  Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()+1));
  Program program(*clcontext, source);

  cl_int err;
  vector<Device> devices = get_devices_list();
  err = program.build(devices, "");
  check_err(err, "Program::build()");
  
  Kernel kernel(program, entry_point, &err);
  check_err(err, "Kernel::Kernel()");
  err = kernel.setArg(0, out_buffer);
  check_err(err, "Kernel::setArg()");
  return kernel;
}

CommandQueue ImageCL::run_kernel(Kernel& k, Device& d, int size)
{
  cl_int err;
  CommandQueue queue(*clcontext, d, 0, &err);
  check_err(err, "CommandQueue::CommandQueue()");
  Event e;
  err = queue.enqueueNDRangeKernel(k, NullRange, NDRange(size), NDRange(1,1), NULL, &e);
  check_err(err, "CommandQueue::enqueueNDRangeKernel()");
  e.wait();  


  return queue;
}

