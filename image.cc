#include <stdio.h>
#include "bmp.h"
int main(){
  Image *img = createImage(1234, 567);
  for(int x=0;x<img->w;x++)for(int y=0;y<img->h;y++){
    double xx=(double)x/img->w;
    double yy=(double)y/img->h;
    double a=(1-xx*xx-yy*yy);
    if(a<0)a=0;
    Color color={
      0xff*xx,
      0xff*yy,
      0xff*a
    };
    img->data[x][y] = color;
  }
  FILE *fp = fopen("out.bmp", "w");
  saveImage(img, fp);
  fclose(fp);
}
