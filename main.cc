#include <stdio.h>
#include <math.h>
#include "renderer.h"

double fracScale;
#define SUBCOUNT 6
Vec3 fracSubs[SUBCOUNT];
void setFractal(double fscale){
  fracScale=fscale;
  double l=1-fracScale;
  l/=1.2;
  fracSubs[0]=(Vec3){+l,0,0};
  fracSubs[1]=(Vec3){-l,0,0};
  fracSubs[2]=(Vec3){0,+l,0};
  fracSubs[3]=(Vec3){0,-l,0};
  fracSubs[4]=(Vec3){0,0,+l};
  fracSubs[5]=(Vec3){0,0,-l};
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


void fractal(Graphics3D g,int level,double depth){
  if(level>depth){g.cube(1);return;}
  double pixel = g.pixelSize();
  if(pixel<0.5){g.cube(1);return;}
  if(!g.test(1.732))return;
  // g.cube((1-fracScale)/1.7);
  // double dist[SUBCOUNT];
  // int sorted[SUBCOUNT];
  // for(int i=0;i<SUBCOUNT;i++){
  //   dist[i]=g.cameraDistance(fracSubs[i]);
  // }
  // sort(SUBCOUNT,dist,sorted);
  // for(int i=0;i<SUBCOUNT;i++){
  //   Graphics3D g2=g;
  //   g2.translate(fracSubs[sorted[i]]);
  //   g2.scale(fracScale);
  //   int ii=sorted[i];
  //   double rot=fracScale*16;
  //   g2.rotate(sin(ii*12345+6)*rot,sin(ii*78901+2)*rot,sin(ii*23456+7)*rot);
  //   fractal(g2,level+1,depth);
  // }
  Graphics3D gs[8];
  double dist[8];
  int sorted[SUBCOUNT];
  double l=0.5;
  for(int i=0;i<8;i++){
    Graphics3D g2=g;
    double dx=(((i>>0)&1)*2-1)*l;
    double dy=(((i>>1)&1)*2-1)*l;
    double dz=(((i>>2)&1)*2-1)*l;
    g2.translate((Vec3){dx,dy,dz});
    g2.scale(0.5);
    double a=depth-level;
    if(a>1)a=1;
    else a=3*a*a-2*a*a*a;
    if(i==0||i==3||i==5||i==6){
      double b=depth-sin(2*M_PI*depth)/2/M_PI-level;
      if(b<0)b=0;
      g2.rotate(sin(1234*i+1)*b,sin(2345*i+2)*b,sin(3456*i+3)*b);
    }else{
      g2.scale(1-0.2*a);
    }
    if(i%2==0)g2.rotate(1,0,0,M_PI/2);
    if(i%3!=1)g2.rotate(0,1,0,M_PI/2);
    if(i%4!=2)g2.rotate(0,0,1,M_PI/2);
    if(i%2!=1)g2.rotate(0,1,0,M_PI);
    if(i%3!=2)g2.rotate(0,0,1,M_PI);
    if(i%4!=0)g2.rotate(1,0,0,M_PI);
    gs[i]=g2;
    dist[i]=g2.cameraDistance();
  }
  sort(8,dist,sorted);
  for(int i=0;i<8;i++){
    fractal(gs[sorted[i]],level+1,depth);
  }
}

void renderer2img(Renderer*renderer,Image*img){
  double min=0,max=0;
  for(int x=0;x<img->w;x++)for(int y=0;y<img->h;y++){
    double d=renderer->depth->data[x][y];
    if(!min||(d&&d<min))min=d;
    if(!max||(d&&d>max))max=d;
  }
  for(int x=0;x<img->w;x++)for(int y=0;y<img->h;y++){
    double d=renderer->depth->data[x][y];
    double c=d?2.4-d:0;
    if(c<0)c=0;
    Vec3 n = renderer->normal->data[x][y];
    double dot=0.5*n.x+0.4*n.y+0.1*n.z;
    c*=0.8+0.2*dot;
    img->data[x][y] = (Color){c/2,c,c*2};
  }
}
int main(){
  Image *img = new Image(1024, 1024);
  Renderer renderer(1024);
  for(int i=0;i<=120;i++){
    renderer.clear();
    double t=i/120.0;
    double t2=3*t*t-2*t*t*t;
    renderer.camera.set(0.3+t2, M_PI/2+cos(t2), 2);
    Graphics3D g(&renderer);
    g.scale(0.5);
    fractal(g,0,8*t);
    renderer2img(&renderer,img);
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
