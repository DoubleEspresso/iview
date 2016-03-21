#include <stdio.h>

#include "image.h"
#include "image_jpeg.h"

int main(int argc, char * argv[])
{
  Image im;
  void * iptr;
  if (im.load("/home/mjg/photos/carson-phone/IMG_1005.JPG", iptr))
    {
      printf("..load success!\n");
      Image_JPEG * jptr = (Image_JPEG*) iptr;
      
      printf(" .. w(%d), h(%d), comps(%d), size(%d)\n",
	     jptr->width(),
	     jptr->height(),
	     jptr->comps(),
	     jptr->size());
 
      //printf("..rotating");
      jptr->flipv();
      //jptr->rotate90();

      if (jptr->save_jpeg("/home/mjg/photos/carson-phone/save-flipv.jpg", 20))
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
