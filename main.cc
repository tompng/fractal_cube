#include <stdio.h>
#include <math.h>
#include "image.h"

class Canvas{
public:
  Array2D<double>*depth;
  Array2D<double>*color;
  // Camera *camera;
  int size;
  Canvas(int _size){
    size = _size;
    depth = new Array2D<double>(size, size);
    color = new Array2D<double>(size, size);
  }
  #define SPHERE_EACH(code) if(cz>cr){\
    double __distx=sqrt(cx*cx+cz*cz-cr*cr);\
    double __disty=sqrt(cy*cy+cz*cz-cr*cr);\
    double __xmin=(cx*__distx-cr*cz)/(__distx*cz+cr*cx);\
    double __xmax=(cx*__distx+cr*cz)/(__distx*cz-cr*cx);\
    double __ymin=(cy*__disty-cr*cz)/(__disty*cz+cr*cy);\
    double __ymax=(cy*__disty+cr*cz)/(__disty*cz-cr*cy);\
    int __ixmin=ceil(((__xmin<-1?-1:__xmin)+1)*size/2);\
    int __ixmax=ceil(((__xmax>1?1:__xmax)+1)*size/2);\
    int __iymin=ceil(((__ymin<-1?-1:__ymin)+1)*size/2);\
    int __iymax=ceil(((__ymax>1?1:__ymax)+1)*size/2+1);\
    if(__ixmax>size)__ixmax=size;\
    if(__iymax>size)__iymax=size;\
    for(int ix=__ixmin;ix<__ixmax;ix++)for(int iy=__iymin;iy<__iymax;iy++){\
      double x=2.0*ix/size-1;\
      double y=2.0*iy/size-1;\
      double vv=x*x+y*y+1;\
      double cc=cx*cx+cy*cy+cz*cz;\
      double vc=x*cx+y*cy+cz;\
      double det=vc*vc-vv*(cc-cr*cr);\
      if(det<0)continue;\
      double z=(vc-sqrt(det))/vv;\
      x*=z;\
      y*=z;\
      code\
    }\
  };
  void drawCircle(double cx, double cy, double cz, double cr){
    SPHERE_EACH({
      double d=depth->data[ix][iy];
      if(!d||z<d){
        depth->data[ix][iy]=z;
      }
    })
  }
};



int main(){
  Image *img = new Image(1024, 1024);
  Canvas canvas(1024);
  canvas.drawCircle(0.1,0.3,0.5,0.1);
  canvas.drawCircle(0.4,-0.2,0.4,0.2);
  canvas.drawCircle(-0.1,0.1,0.3,0.1);
  canvas.drawCircle(0.1,0,0.4,0.2);
  canvas.drawCircle(-0.3,-0.4,0.5,0.3);
  for(int x=0;x<img->w;x++)for(int y=0;y<img->h;y++){
    Color color={
      0xff*canvas.depth->data[x][y],
      0xff*0.2,
      0xff*0.3
    };
    img->data[x][y] = color;
  }
  FILE *fp = fopen("out.bmp", "w");
  img->save(fp);
  fclose(fp);
}
