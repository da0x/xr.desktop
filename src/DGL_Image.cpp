#if 0
// *** The JPEG.H wrapper layer header file.

#ifndef JPEG_H
#define JPEG_H

class Jpeg
{
public:

  static void *ReadImage(int &width,   // width of the image loaded.
                         int &height,  // height of the image loaded.
                         int &bpp,     // BYTES (not bits) PER PIXEL.
                         const void *buffer, // memory address containing jpeg compressed data.
                         int sizebytes); // size of jpeg compressed data.


  static void * Compress(const void *buffer, // address of image in memory
                         int width, // width of image in pixels
                         int height, // height of image in pixels.
                         int bpp, // *BYTES* per pixel of image 1 or 3
                         int &len, // returns length of compressed data
                         int quality=75); // image quality as a percentage

};

#endif


// ** JPEG.CPP, the code used to wrap IJL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ijl.h" // intel jpeg library header file.

// read image into this buffer.
void * Jpeg::ReadImage(int &width,
                       int &height,
                       int &nchannels,
                       const void *buffer,
                       int sizebytes)
{
  JPEG_CORE_PROPERTIES jcprops;

  if ( ijlInit(&jcprops) != IJL_OK )
  {
    ijlFree(&jcprops);
    return 0;
  }

  jcprops.JPGBytes = (unsigned char *) buffer;
  jcprops.JPGSizeBytes = sizebytes;
  jcprops.jquality = 100;

  if ( ijlRead(&jcprops,IJL_JBUFF_READPARAMS) != IJL_OK )
  {
    ijlFree(&jcprops);
    return 0;
  }

  width  = jcprops.JPGWidth;
  height = jcprops.JPGHeight;
  IJLIOTYPE mode;

  mode = IJL_JBUFF_READWHOLEIMAGE;
  nchannels = jcprops.JPGChannels;
  unsigned char * pixbuff = new unsigned char[width*height*nchannels];
  if ( !pixbuff )
  {
    ijlFree(&jcprops);
    return 0;
  }

  jcprops.DIBWidth  = width;
  jcprops.DIBHeight = height;
  jcprops.DIBChannels = nchannels;
  jcprops.DIBPadBytes = 0;
  jcprops.DIBBytes = (unsigned char *)pixbuff;

  if ( jcprops.JPGChannels == 3 )
  {
    jcprops.DIBColor = IJL_RGB;
    jcprops.JPGColor = IJL_YCBCR;
    jcprops.JPGSubsampling = IJL_411;
    jcprops.DIBSubsampling = (IJL_DIBSUBSAMPLING) 0;
  }
  else
  {
    jcprops.DIBColor = IJL_G;
    jcprops.JPGColor = IJL_G;
    jcprops.JPGSubsampling = (IJL_JPGSUBSAMPLING) 0;
    jcprops.DIBSubsampling = (IJL_DIBSUBSAMPLING) 0;
  }

  if ( ijlRead(&jcprops, mode) != IJL_OK )
  {
    ijlFree(&jcprops);
    return 0;
  }

  if ( ijlFree(&jcprops) != IJL_OK ) return 0;

  return (void *)pixbuff;
}


void * Jpeg::Compress(const void *source,
                      int width,
                      int height,
                      int bpp,
                      int &len,
                      int quality)
{
  JPEG_CORE_PROPERTIES jcprops;

  if ( ijlInit(&jcprops) != IJL_OK )
  {
    ijlFree(&jcprops);
    return false;
  }

  jcprops.DIBWidth    = width;
  jcprops.DIBHeight   = height;
  jcprops.JPGWidth    = width;
  jcprops.JPGHeight   = height;
  jcprops.DIBBytes    = (unsigned char *) source;
  jcprops.DIBPadBytes = 0;
  jcprops.DIBChannels = bpp;
  jcprops.JPGChannels = bpp;

  if ( bpp == 3 )
  {
    jcprops.DIBColor = IJL_RGB;
    jcprops.JPGColor = IJL_YCBCR;
    jcprops.JPGSubsampling = IJL_411;
    jcprops.DIBSubsampling = (IJL_DIBSUBSAMPLING) 0;
  }
  else
  {
    jcprops.DIBColor = IJL_G;
    jcprops.JPGColor = IJL_G;
    jcprops.JPGSubsampling = (IJL_JPGSUBSAMPLING) 0;
    jcprops.DIBSubsampling = (IJL_DIBSUBSAMPLING) 0;
  }

  int size = width*height*bpp;

  unsigned char * buffer = new unsigned char[size];

  jcprops.JPGSizeBytes = size;
  jcprops.JPGBytes     = buffer;

  jcprops.jquality = quality;


  if ( ijlWrite(&jcprops,IJL_JBUFF_WRITEWHOLEIMAGE) != IJL_OK )
  {
    ijlFree(&jcprops);
    delete buffer;
    return 0;
  }


  if ( ijlFree(&jcprops) != IJL_OK )
  {
    delete buffer;
    return 0;
  }

  len = jcprops.JPGSizeBytes;
  return buffer;
}

#endif 
