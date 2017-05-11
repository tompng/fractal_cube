#include <stdio.h>
#include <math.h>
#include "image.h"
typedef struct{double x,y,z;}Point;
class Canvas{
public:
  Array2D<double>*depth;
  Array2D<double>*color;
  double zoom;
  // Camera *camera;
  int size;
  Canvas(int _size){
    zoom = 1.5;
    size = _size;
    depth = new Array2D<double>(size, size);
    color = new Array2D<double>(size, size);
  }
  #define SPHERE_EACH(code) if(cz>cr){\
    double __distx=sqrt(cx*cx+cz*cz-cr*cr);\
    double __disty=sqrt(cy*cy+cz*cz-cr*cr);\
    double __xmin=zoom*(cx*__distx-cr*cz)/(__distx*cz+cr*cx);\
    double __xmax=zoom*(cx*__distx+cr*cz)/(__distx*cz-cr*cx);\
    double __ymin=zoom*(cy*__disty-cr*cz)/(__disty*cz+cr*cy);\
    double __ymax=zoom*(cy*__disty+cr*cz)/(__disty*cz-cr*cy);\
    int __ixmin=ceil(((__xmin<-1?-1:__xmin)+1)*size/2);\
    int __ixmax=ceil(((__xmax>1?1:__xmax)+1)*size/2);\
    int __iymin=ceil(((__ymin<-1?-1:__ymin)+1)*size/2);\
    int __iymax=ceil(((__ymax>1?1:__ymax)+1)*size/2+1);\
    if(__ixmax>size)__ixmax=size;\
    if(__iymax>size)__iymax=size;\
    for(int ix=__ixmin;ix<__ixmax;ix++)for(int iy=__iymin;iy<__iymax;iy++){\
      double x=(2.0*ix/size-1)/zoom;\
      double y=(2.0*iy/size-1)/zoom;\
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
  void drawSphere(double cx, double cy, double cz, double cr){
    SPHERE_EACH({
      double d=depth->data[ix][iy];
      if(!d||z<d){
        depth->data[ix][iy]=z;
      }
    })
  }
  bool testSphere(double cx, double cy, double cz, double cr){
    SPHERE_EACH({
      double d=depth->data[ix][iy];
      if(!d||z<d)return true;
    })
    return false;
  }
  void drawPolygon(Point a, Point b, Point c){
    if(a.z<0||b.z<0||c.z<0)return;
    double ax=zoom*a.x/a.z,ay=zoom*a.y/a.z;
    double bx=zoom*b.x/b.z,by=zoom*b.y/b.z;
    double cx=zoom*c.x/c.z,cy=zoom*c.y/c.z;
    double xmin=ax<bx?ax:bx;if(cx<xmin)xmin=cx;
    double xmax=ax<bx?bx:ax;if(xmax<cx)xmax=cx;
    double ymin=ay<by?ay:by;if(cy<ymin)ymin=cy;
    double ymax=ay<by?by:ay;if(ymax<cy)ymax=cy;
    int ixmin=ceil(((xmin<-1?-1:xmin)+1)*size/2);
    int ixmax=ceil(((xmax>1?1:xmax)+1)*size/2);
    int iymin=ceil(((ymin<-1?-1:ymin)+1)*size/2);
    int iymax=ceil(((ymax>1?1:ymax)+1)*size/2+1);
    if(ixmax>size)ixmax=size;
    if(iymax>size)iymax=size;
    double abx=b.x-a.x,aby=b.y-a.y,abz=b.z-a.z;
    double acx=c.x-a.x,acy=c.y-a.y,acz=c.z-a.z;
    double nx=aby*acz-abz*acy;
    double ny=abz*acx-abx*acz;
    double nz=abx*acy-aby*acx;
    double pdot=a.x*nx+a.y*ny+a.z*nz;
    for(int ix=ixmin;ix<ixmax;ix++)for(int iy=iymin;iy<iymax;iy++){
      double x=2.0*ix/size-1;
      double y=2.0*iy/size-1;
      double ab=(ax-x)*(by-y)-(ay-y)*(bx-x);
      double bc=(bx-x)*(cy-y)-(by-y)*(cx-x);
      double ca=(cx-x)*(ay-y)-(cy-y)*(ax-x);
      if((ab>0&&bc>0&&ca>0)||(ab<0&&bc<0&&ca<0)){
        double z=pdot/(x*nx/zoom+y*ny/zoom+nz);
        x*=z;y*=z;
        double d=depth->data[ix][iy];
        if(!d||z<d){
          depth->data[ix][iy]=z;
        }
      }
    }
  }
};



double xycos,xysin,zcos,zsin,fracScale,fracBottomR,fracBottomZ;
Point fracSubs[4];
void initCam(double xytheta,double ztheta,double fscale){
  fracScale=fscale;
  xycos=cos(xytheta);xysin=sin(xytheta);
  zcos=cos(ztheta);zsin=sin(ztheta);
  fracBottomR=(1-fracScale)*sqrt(2)*2/3;
  fracBottomZ=-(1-fracScale)/3;
  fracSubs[0]=(Point){0.0,0.0,1-fracScale};
  fracSubs[1]=(Point){fracBottomR,0,fracBottomZ};
  fracSubs[2]=(Point){-fracBottomR/2,fracBottomR*sqrt(3)/2,fracBottomZ};
  fracSubs[3]=(Point){-fracBottomR/2,-fracBottomR*sqrt(3)/2,fracBottomZ};
}

void fractal(Canvas*canvas, double x, double y, double z, double r){
  double tx,ty,tz;
  tx=x*xycos-y*xysin;
  ty=x*xysin+y*xycos;
  tz=z*zcos-y*zsin;
  ty=z*zsin+y*zcos;
  tz+=2;
  if(r/tz<0.001){canvas->drawSphere(tx,ty,tz,r);return;}
  if(!canvas->testSphere(tx,ty,tz,r))return;
  canvas->drawSphere(tx,ty,tz,r*(1-fracScale));
  for(int i=0;i<4;i++){
    Point p=fracSubs[i];
    fractal(canvas,x+r*p.x,y+r*p.y,z+r*p.z,r*fracScale);
  }
}

void canvas2img(Canvas*canvas,Image*img){
  double min=0;
  for(int x=0;x<img->w;x++)for(int y=0;y<img->h;y++){
    double d=canvas->depth->data[x][y];
    if(!min||(d&&d<min))min=d;
  }
  for(int x=0;x<img->w;x++)for(int y=0;y<img->h;y++){
    double d=canvas->depth->data[x][y];
    double c=d?1/(1+4*(d-min)):0;
    Color color={
      0xff*c,
      0xff*(c*0.8+0.2*(0.5+0.5*sin(17*canvas->depth->data[x][y]))),
      0xff*(c*0.8+0.2*(0.5+0.5*sin(32*canvas->depth->data[x][y])))
    };
    img->data[x][y] = color;
  }
}
int main(){
  initCam(0.3,0.2,0.4);
  Image *img = new Image(1024, 1024);
  Canvas canvas(1024);
  fractal(&canvas,0,0,0,1);
  canvas2img(&canvas,img);
  FILE *fp = fopen("out.bmp", "w");
  img->save(fp);
  fclose(fp);
}
