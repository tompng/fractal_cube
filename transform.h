#include <math.h>

typedef struct{double x,y,z;}Point;

class Matrix3{
public:
  double m[3][3];
  Matrix3(){
    for(int i=0;i<3;i++)for(int j=0;j<3;j++)m[i][j]=i==j;
  }
  Matrix3(double nx, double ny, double nz, double rot){
    double c=cos(rot),s=sin(rot);
    m[0][0]=nx*nx*(1-c)+c;    m[0][1]=nx*ny*(1-c)-nz*s; m[0][2]=nz*nx*(1-c)+ny*s;
    m[1][0]=nx*ny*(1-c)+nz*s; m[1][1]=ny*ny*(1-c)+c;    m[1][2]=ny*nz*(1-c)-nx*s;
    m[2][0]=nz*nx*(1-c)-ny*s; m[2][1]=ny*nz*(1-c)+nx*s; m[2][2]=nz*nz*(1-c)+c;
  }
  void scale(double s){
    for(int i=0;i<3;i++)for(int j=0;j<3;j++)m[i][j]*=s;
  }
  void rotate(double nx, double ny, double nz, double rot){
    Matrix3 matrix(nx,ny,nz,rot);
    Matrix3 result = matrix.mult(*this);
    for(int i=0;i<3;i++)for(int j=0;j<3;j++)m[i][j]=result.m[i][j];
  }
  void rotate(double x, double y, double z){
    double r=sqrt(x*x+y*y+z*z);
    if(r)rotate(x/r, y/r, z/r, r);
  }
  Matrix3 mult(Matrix3 matrix){
    Matrix3 out;
    for(int i=0;i<3;i++)for(int j=0;j<3;j++){
      double v=0;
      for(int k=0;k<3;k++)v+=m[i][k]*matrix.m[k][j];
      out.m[i][j]=v;
    }
    return out;
  }
  Point trans(Point p){
    return (Point){
      m[0][0]*p.x+m[0][1]*p.y+m[0][2]*p.z,
      m[1][0]*p.x+m[1][1]*p.y+m[1][2]*p.z,
      m[2][0]*p.x+m[2][1]*p.y+m[2][2]*p.z
    };
  }
};

class Transform{
public:
  Point position;
  Matrix3 matrix;
  Transform(){}
  Transform(Point p,Matrix3 m){
    position = p;
    matrix = m;
  }
  void scale(double s){
    position.x*=s;position.y*=s;position.z*=s;
    matrix.scale(s);
  }
  void translate(Point p){
    position.x+=p.x;position.y+=p.y;position.z+=p.z;
  }
  void rotate(double nx, double ny, double nz, double rot){
    Matrix3 rotate(nx, ny, nz, rot);
    position = rotate.trans(position);
    matrix = rotate.mult(matrix);
  }
  Transform mult(Transform t){
    return Transform(trans(t.position), matrix.mult(t.matrix));
  }
  Point trans(Point p){
    p=matrix.trans(p);
    return (Point){position.x+p.x, position.y+p.y, position.z+p.z};
  }
};

class Camera{
  Transform transform;
  double zoom;
  Camera(double z=2){zoom=z;}
  void set(double rotxy, double rotz, double camz=2, double _zoom=2){
    zoom=_zoom;
    transform.rotate(1,0,0,M_PI/2+rotz);
    transform.rotate(0,1,0,rotxy);
    transform.translate((Point){0,0,zoom});
  }
  double distance(Point p){
    Point tpos = transform.trans(p);
    return tpos.z;
  }
};
