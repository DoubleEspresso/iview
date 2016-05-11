#include <stdio.h>

#include "image.h"
#include "image_jpeg.h"
#include "image_tiff.h"

int main(int argc, char * argv[])
{
  /*
  Image_TIFF * itiff = new Image_TIFF();
  itiff->load_tiff("/home/mjg/Desktop/minisblack-1c-8b.tiff");
  itiff->save_tiff("/home/mjg/Desktop/minisblack-testsave.tiff");
  
  Image_TIFF * itiff2 = new Image_TIFF();
  itiff2->load_tiff("/home/mjg/Desktop/rgb-3c-16b.tiff");
  itiff2->save_tiff("/home/mjg/Desktop/rgb-3c-16b-testsave.tiff");
  */
  Image * im = new Image();
  if (im->load("/home/mjg/Desktop/test-blur.jpg"))///home/mjg/photos/carson-phone/IMG_1215.JPG"))
    {
      printf("..load success!\n");
      Image_JPEG * jptr = im->get_handle();
      
      printf(" .. w(%d), h(%d), comps(%d), size(%d)\n",
	     jptr->width(),
	     jptr->height(),
	     jptr->comps(),
	     jptr->size());
 
      //im->convolve_fft();
      im->binning(5);
      //im->sharpen(3);
      //im->sobel();
      //im->median(3);
      //im->median(3);
      //im->median(3);
      //im->sharpen(3);
      //jptr->flipv();
      //jptr->rotate90();
      
      if (im->save("/home/mjg/Desktop/test-sharpen.jpg",100 ))//"/home/mjg/photos/carson-phone/save-sharpen.jpg", 100))
	{
	  printf("..test save succes!\n");
	}
      //if (jptr->save_jpeg("/home/mjg/photos/carson-phone/save-flipv.jpg", 20))
      //{
      //printf("..test save succes!\n");
      //}
    }
  else
    {
      printf("..failed to load test image\n");
    }
  delete im; im = 0;
  return 0;
}
