#include <stdio.h>

#include "image.h"
#include "image_jpeg.h"

int main(int argc, char * argv[])
{
  Image im;
  void * iptr;
  if (im.load("/home/mjg/test-photos/twin-cities/2015-05-31-002801.jpg", iptr))
    {
      printf("..load success!\n");
      Image_JPEG * jptr = (Image_JPEG*) iptr;
      
      printf(" .. w(%d), h(%d), comps(%d), size(%d)\n",
	     jptr->width(),
	     jptr->height(),
	     jptr->comps(),
	     jptr->size());
      
      if (jptr->save_jpeg("/home/mjg/test-photos/test-save.jpg", 20))
	{
	  printf("..test save succes!\n");
	}
    }
  else
    {
      printf("..failed to load test image\n");
    }
  return 0;
}
