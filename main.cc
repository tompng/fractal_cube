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
  void clear(){
    for(int x=0;x<size;x++)for(int y=0;y<size;y++)depth->data[x][y]=color->data[x][y]=0;
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
#define SUBCOUNT 8
Point fracSubs[SUBCOUNT];
void initCam(double xytheta,double ztheta,double fscale){
  fracScale=fscale;
  xycos=cos(xytheta);xysin=sin(xytheta);
  zcos=cos(ztheta);zsin=sin(ztheta);
  double l=1-fracScale;
  l/=sqrt(3);
  for(int i=0;i<8;i++){
    fracSubs[i]=(Point){l*(i%2*2-1),l*(i/2%2*2-1),l*(i/4%2*2-1)};
  }
}

void trans(double x,double y,double z,double*tx,double*ty,double*tz){
  double _ty;
  *tx=x*xycos-y*xysin;
  _ty=x*xysin+y*xycos;
  *ty=z*zsin+_ty*zcos;
  *tz=z*zcos-_ty*zsin+2;
}
void sort(int n,double*arr,int*indices){
  for(int i=0;i<n;i++)indices[i]=i;
  for(int i=0;i<n-1;i++){
    int ii=indices[i];
    double ai=arr[ii];
    for(int j=i+1;j<n;j++){
      int jj=indices[j];
      double aj=arr[jj];
      if(aj<ai){
        indices[i]=jj;
        indices[j]=ii;
        ii=jj;
        ai=aj;
      }
    }
  }
}

void fractal(Canvas*canvas, double x, double y, double z, double r){
  double tx,ty,tz;
  trans(x,y,z,&tx,&ty,&tz);
  if(r/tz<0.001){canvas->drawSphere(tx,ty,tz,r);return;}
  if(!canvas->testSphere(tx,ty,tz,r))return;
  canvas->drawSphere(tx,ty,tz,r*(1-fracScale));
  double dist[SUBCOUNT];
  int sorted[SUBCOUNT];
  for(int i=0;i<SUBCOUNT;i++){
    Point p=fracSubs[i];
    double tx,ty,tz;
    trans(x+r*p.x,y+r*p.y,z+r*p.z,&tx,&ty,&tz);
    dist[i]=tz;
  }
  sort(SUBCOUNT,dist,sorted);
  for(int i=0;i<SUBCOUNT;i++){
    Point p=fracSubs[sorted[i]];
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
  Image *img = new Image(1024, 1024);
  Canvas canvas(1024);
  for(int i=0;i<=100;i++){
    canvas.clear();
    double t=i/100.0;
    t*=2-t;
    initCam(0.3+10*t,M_PI/2+cos(4*t),t*0.5);
    fractal(&canvas,0,0,0,1);
    canvas2img(&canvas,img);
    char filename[128];
    sprintf(filename,"out/%d.bmp",i);
    FILE *fp = fopen(filename, "w");
    img->save(fp);
    fclose(fp);
    printf("%d\n",i);
  }
  // g++ -O3 main.cc && ./a.out
  // ffmpeg -i out/%d.bmp -s 512x512 -r 40 -vf format=yuv420p out.mp4
}
