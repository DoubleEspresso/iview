#include "image_cl.h"



int main(int argc, char * argv[])
{
  const std::string hw("Hello WorldCL\n");
  char * outH = new char [hw.length()+1];
  ImageCL i;
  i.create_cl_context();
  Buffer out_buffer;
  int opts = CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR;
  i.create_cl_kernel_buffer(out_buffer, opts, hw.length()+1);

  // device handles
  vector<Device> devices;
  devices = i.get_devices_list();

  // load kernel code

  Kernel k = i.load_cl_kernel_file("image.cl", "hello", out_buffer);
  CommandQueue q = i.run_kernel(k, devices[0], hw.length()+1); // blocks until finished
  q.enqueueReadBuffer(out_buffer, CL_TRUE, 0, hw.length()+1, (void*)&hw);
  std::cout << hw;
  return 0;
}
