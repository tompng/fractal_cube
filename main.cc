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


void fractal(Graphics3D g){
  double pixel = g.pixelSize();
  if(pixel<1){g.sphere(1);return;}
  if(!g.test(1))return;
  g.cube((1-fracScale)/1.7);
  double dist[SUBCOUNT];
  int sorted[SUBCOUNT];
  for(int i=0;i<SUBCOUNT;i++){
    dist[i]=g.cameraDistance(fracSubs[i]);
  }
  sort(SUBCOUNT,dist,sorted);
  for(int i=0;i<SUBCOUNT;i++){
    Graphics3D g2=g;
    g2.translate(fracSubs[sorted[i]]);
    g2.scale(fracScale);
    int ii=sorted[i];
    double rot=fracScale*16;
    g2.rotate(sin(ii*12345+6)*rot,sin(ii*78901+2)*rot,sin(ii*23456+7)*rot);
    fractal(g2);
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
    Color color={
      0xff*(c/2>1?1:c/2),
      0xff*(c>1?1:c),
      0xff*(c*2>1?1:c*2)
    };
    img->data[x][y] = color;
  }
}
int main(){
  Image *img = new Image(1024, 1024);
  Renderer renderer(1024);
  for(int i=0;i<=120;i++){
    renderer.clear();
    double t=i/120.0;
    t*=2-t;
    double t2=i/100.0;t2=t2>1?1:t2;t2*=2-t2;
    renderer.camera.set(0.3+10*t, M_PI/2+cos(4*t), 2);
    Graphics3D g(&renderer);
    setFractal(t2*0.5);
    fractal(g);
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
