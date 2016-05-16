#pragma OPENCL EXTENSION cl_khr_byte_addresable_store : enable
__const char hw[] "Hello WorldDL\n";

__kernel void hello(__global char * out)
{
	size_t tid = get_global_id(0);
	out[tid] = hw[id];
}