#include "image.h"
#include "transform.h"
class Camera{
public:
  Transform transform;
  double zoom;
  Camera(double z=2){zoom=z;}
  void set(double rotxy, double rotz, double camz=2, double _zoom=2){
    zoom=_zoom;
    transform.identity();
    transform.translate((Vec3){0,0,zoom});
    transform.rotate(0,1,0,rotxy);
    transform.rotate(1,0,0,M_PI/2+rotz);
  }
  double distance(Vec3 p){
    Vec3 tpos = transform.trans(p);
    return tpos.z;
  }
};

class Renderer{
public:
  Array2D<double>*depth;
  Array2D<double>*color;
  Camera camera;
  int size;
  Renderer(int _size){
    size = _size;
    depth = new Array2D<double>(size, size);
    color = new Array2D<double>(size, size);
  }
  #define SPHERE_EACH(code) if(cz>cr){\
    double __distx=sqrt(cx*cx+cz*cz-cr*cr);\
    double __disty=sqrt(cy*cy+cz*cz-cr*cr);\
    double __xmin=camera.zoom*(cx*__distx-cr*cz)/(__distx*cz+cr*cx);\
    double __xmax=camera.zoom*(cx*__distx+cr*cz)/(__distx*cz-cr*cx);\
    double __ymin=camera.zoom*(cy*__disty-cr*cz)/(__disty*cz+cr*cy);\
    double __ymax=camera.zoom*(cy*__disty+cr*cz)/(__disty*cz-cr*cy);\
    if(__xmin<1&&__ymin<1&&__xmax>-1&&__ymax>-1){\
      int __ixmin=ceil(((__xmin<-1?-1:__xmin)+1)*size/2);\
      int __ixmax=ceil(((__xmax>1?1:__xmax)+1)*size/2);\
      int __iymin=ceil(((__ymin<-1?-1:__ymin)+1)*size/2);\
      int __iymax=ceil(((__ymax>1?1:__ymax)+1)*size/2+1);\
      if(__ixmax>size)__ixmax=size;\
      if(__iymax>size)__iymax=size;\
      for(int ix=__ixmin;ix<__ixmax;ix++)for(int iy=__iymin;iy<__iymax;iy++){\
        double x=(2.0*ix/size-1)/camera.zoom;\
        double y=(2.0*iy/size-1)/camera.zoom;\
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
    }\
  };
  void drawSphere(Vec3 p, double cr){
    p=camera.transform.trans(p);
    double cx=p.x,cy=p.y,cz=p.z;
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
  bool testSphere(Vec3 p, double cr){
    p=camera.transform.trans(p);
    double cx=p.x,cy=p.y,cz=p.z;
    SPHERE_EACH({
      double d=depth->data[ix][iy];
      if(!d||z<d)return true;
    })
    return false;
  }
  void drawTriangle(Vec3 a, Vec3 b, Vec3 c){
    a=camera.transform.trans(a);
    b=camera.transform.trans(b);
    c=camera.transform.trans(c);
    if(a.z<0||b.z<0||c.z<0)return;
    double ax=camera.zoom*a.x/a.z,ay=camera.zoom*a.y/a.z;
    double bx=camera.zoom*b.x/b.z,by=camera.zoom*b.y/b.z;
    double cx=camera.zoom*c.x/c.z,cy=camera.zoom*c.y/c.z;
    double xmin=ax<bx?ax:bx;if(cx<xmin)xmin=cx;
    double xmax=ax<bx?bx:ax;if(xmax<cx)xmax=cx;
    double ymin=ay<by?ay:by;if(cy<ymin)ymin=cy;
    double ymax=ay<by?by:ay;if(ymax<cy)ymax=cy;
    if(xmin>1||ymin>1||xmax<-1||ymax<-1)return;
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
        double z=pdot/(x*nx/camera.zoom+y*ny/camera.zoom+nz);
        x*=z;y*=z;
        double d=depth->data[ix][iy];
        if(!d||z<d){
          depth->data[ix][iy]=z;
        }
      }
    }
  }
};

class Graphics3D{
public:
  Renderer *renderer;
  Transform transform;
  Graphics3D(Renderer *r){renderer = r;}
  void scale(double s){transform.scale(s);}
  void rotate(double nx, double ny, double nz, double rot){transform.rotate(nx,ny,nz,rot);}
  void rotate(double x, double y, double z){transform.rotate(x,y,z);}
  void translate(Vec3 p){transform.translate(p);}
  double pixelSize(Vec3 p){
    return renderer->size*transform.scaleRatio/cameraDistance(p)*renderer->camera.zoom;
  }
  double cameraDistance(Vec3 p){
    Vec3 gpos = transform.trans(p);
    Vec3 cpos = renderer->camera.transform.trans(gpos);
    return cpos.z;
  }
  double pixelSize(){return pixelSize((Vec3){0,0,0});}
  double cameraDistance(){return cameraDistance((Vec3){0,0,0});}
  bool test(Vec3 p, double r){
    return renderer->testSphere(transform.trans(p), transform.scaleRatio*r);
  }
  void sphere(Vec3 p, double r){
    renderer->drawSphere(transform.trans(p), transform.scaleRatio*r);
  }
  bool test(double r){return test((Vec3){0, 0, 0}, r);}
  void sphere(double r){return sphere((Vec3){0, 0, 0}, r);}
  void triangle(Vec3 a, Vec3 b, Vec3 c){
    renderer->drawTriangle(transform.trans(a), transform.trans(b), transform.trans(c));
  }
  void cube(double size){}
  void octahedral(double size){}
};
