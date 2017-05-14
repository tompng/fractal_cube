#include <math.h>

typedef struct{double x,y,z;}Vec3;

class Matrix3{
public:
  double m[3][3];
  Matrix3(){identity();}
  Matrix3(double nx, double ny, double nz, double rot){
    double c=cos(rot),s=sin(rot);
    m[0][0]=nx*nx*(1-c)+c;    m[0][1]=nx*ny*(1-c)-nz*s; m[0][2]=nz*nx*(1-c)+ny*s;
    m[1][0]=nx*ny*(1-c)+nz*s; m[1][1]=ny*ny*(1-c)+c;    m[1][2]=ny*nz*(1-c)-nx*s;
    m[2][0]=nz*nx*(1-c)-ny*s; m[2][1]=ny*nz*(1-c)+nx*s; m[2][2]=nz*nz*(1-c)+c;
  }
  void identity(){
    for(int i=0;i<3;i++)for(int j=0;j<3;j++)m[i][j]=i==j;
  }
  void scale(double s){
    for(int i=0;i<3;i++)for(int j=0;j<3;j++)m[i][j]*=s;
  }
  void rotate(double nx, double ny, double nz, double rot){
    Matrix3 matrix(nx,ny,nz,rot);
    Matrix3 result=matrix.mult(*this);
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
  Vec3 trans(Vec3 p){
    return (Vec3){
      m[0][0]*p.x+m[0][1]*p.y+m[0][2]*p.z,
      m[1][0]*p.x+m[1][1]*p.y+m[1][2]*p.z,
      m[2][0]*p.x+m[2][1]*p.y+m[2][2]*p.z
    };
  }
  Vec3 transposeTrans(Vec3 p){
    return (Vec3){
      m[0][0]*p.x+m[1][0]*p.y+m[2][0]*p.z,
      m[0][1]*p.x+m[1][1]*p.y+m[2][1]*p.z,
      m[0][2]*p.x+m[1][2]*p.y+m[2][2]*p.z
    };
  }
};

class Transform{
public:
  Vec3 position;
  double scaleRatio;
  Matrix3 matrix;
  Transform(){scaleRatio = 1;}
  Transform(Vec3 p,double s,Matrix3 m){
    scaleRatio = s;
    position = p;
    matrix = m;
  }
  void identity(){
    position = (Vec3){0,0,0};
    scaleRatio = 1;
    matrix.identity();
  }
  void scale(double s){
    scaleRatio *= s;
  }
  void translate(Vec3 p){
    position = trans(p);
  }
  void rotate(double nx, double ny, double nz, double rot){
    Matrix3 rotate(nx, ny, nz, rot);
    matrix = matrix.mult(rotate);
  }
  void rotate(double x, double y, double z){
    double r=sqrt(x*x+y*y+z*z);
    if(r)rotate(x/r, y/r, z/r, r);
  }
  Transform mult(Transform t){
    return Transform(trans(t.position), scaleRatio*t.scaleRatio, matrix.mult(t.matrix));
  }
  Vec3 trans(Vec3 p){
    p=matrix.trans(p);
    return (Vec3){position.x+scaleRatio*p.x, position.y+scaleRatio*p.y, position.z+scaleRatio*p.z};
  }
  Vec3 inv(Vec3 p){
    p.x=(p.x-position.x)/scaleRatio;
    p.y=(p.y-position.y)/scaleRatio;
    p.z=(p.z-position.z)/scaleRatio;
    return matrix.transposeTrans(p);
  }
  Vec3 invNormal(Vec3 n){
    return matrix.transposeTrans(n);
  }
};
