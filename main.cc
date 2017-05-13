#include <stdio.h>
#include <math.h>
#include "renderer.h"

double fracScale;
#define SUBCOUNT 6
Point fracSubs[SUBCOUNT];
void setFractal(double fscale){
  fracScale=fscale;
  double l=1-fracScale;
  l/=1.2;
  fracSubs[0]=(Point){+l,0,0};
  fracSubs[1]=(Point){-l,0,0};
  fracSubs[2]=(Point){0,+l,0};
  fracSubs[3]=(Point){0,-l,0};
  fracSubs[4]=(Point){0,0,+l};
  fracSubs[5]=(Point){0,0,-l};
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

int CUBE[12][3][3]={
  {{+1,+1,+1},{+1,-1,+1},{+1,-1,-1}},
  {{+1,+1,+1},{+1,+1,-1},{+1,-1,-1}},
  {{-1,+1,+1},{-1,-1,+1},{-1,-1,-1}},
  {{-1,+1,+1},{-1,+1,-1},{-1,-1,-1}},

  {{+1,+1,+1},{-1,+1,+1},{-1,-1,+1}},
  {{+1,+1,+1},{+1,-1,+1},{-1,-1,+1}},
  {{+1,+1,-1},{-1,+1,-1},{-1,-1,-1}},
  {{+1,+1,-1},{+1,-1,-1},{-1,-1,-1}},

  {{+1,+1,+1},{+1,+1,-1},{-1,+1,-1}},
  {{+1,+1,+1},{-1,+1,+1},{-1,+1,-1}},
  {{+1,-1,+1},{+1,-1,-1},{-1,-1,-1}},
  {{+1,-1,+1},{-1,-1,+1},{-1,-1,-1}},
};
int OCTAHEDRAL[8][3][3]={
  {{-1,0,0},{0,-1,0},{0,0,-1}},
  {{-1,0,0},{0,-1,0},{0,0,+1}},
  {{-1,0,0},{0,+1,0},{0,0,-1}},
  {{-1,0,0},{0,+1,0},{0,0,+1}},
  {{+1,0,0},{0,-1,0},{0,0,-1}},
  {{+1,0,0},{0,-1,0},{0,0,+1}},
  {{+1,0,0},{0,+1,0},{0,0,-1}},
  {{+1,0,0},{0,+1,0},{0,0,+1}}
};

void fractal(Renderer*renderer, double x, double y, double z, double r){
  double distance = renderer->camera.distance((Point){x,y,z});
  if(r/distance<0.001){renderer->drawSphere((Point){x,y,z},r);return;}
  if(!renderer->testSphere((Point){x,y,z},r))return;
  double l=r*(1-fracScale)/1.7;
  for(int i=0;i<sizeof(CUBE)/sizeof(CUBE[0]);i++){
    Point p[3];
    for(int j=0;j<3;j++){
      p[j]=(Point){x+l*CUBE[i][j][0],y+l*CUBE[i][j][1],z+l*CUBE[i][j][2]};
    }
    renderer->drawTriangle(p[0],p[1],p[2]);
  }
  double dist[SUBCOUNT];
  int sorted[SUBCOUNT];
  for(int i=0;i<SUBCOUNT;i++){
    Point p=fracSubs[i];
    dist[i]=renderer->camera.distance((Point){x+r*p.x,y+r*p.y,z+r*p.z});
  }
  sort(SUBCOUNT,dist,sorted);
  for(int i=0;i<SUBCOUNT;i++){
    Point p=fracSubs[sorted[i]];
    fractal(renderer,x+r*p.x,y+r*p.y,z+r*p.z,r*fracScale);
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
  Graphics3D g(&renderer);
  for(int i=0;i<=120;i++){
    renderer.clear();
    double t=i/120.0;
    t*=2-t;
    double t2=i/100.0;t2=t2>1?1:t2;t2*=2-t2;
    renderer.camera.set(0.3+10*t, M_PI/2+cos(4*t), 2);
    setFractal(t2*0.5);
    fractal(&renderer,0,0,0,1);
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
