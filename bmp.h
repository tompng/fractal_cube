#include <stdio.h>

typedef struct {
  char r;
  char g;
  char b;
} Color;

typedef struct {
  int w;
  int h;
  Color**data;
} Image;

Image* createImage(int w, int h){
  Image*img = new Image();
  img->w = w;
  img->h = h;
  img->data = new Color*[w];
  for(int x=0;x<w;x++){
    img->data[x] = new Color[h];
  }
  return img;
}

void sprintInt(int x, int size, char*s){
  for(int i=0;i<size;i++){
    s[i]=x&0xff;
    x>>=8;
  }
}

void sprintImageHeader(int w, int h, char*header){
  for(int i=0;i<54;i++)header[i]=0;
  header[0]='B';
  header[1]='M';
  sprintInt(54+w*h*4,4,header+2);
  sprintInt(54,4,header+10);
  sprintInt(40,4,header+14);
  sprintInt(w,4,header+18);
  sprintInt(h,4,header+22);
  sprintInt(1,2,header+26);
  sprintInt(32,2,header+28);
  sprintInt(w*h*4,4,header+34);
}

void saveImage(Image* img, FILE*fp){
  char header[54];
  sprintImageHeader(img->w, img->h, header);
  fwrite(header, 1, 54, fp);
  char*line=new char[4*img->w];
  for(int y=0;y<img->h;y++){
    for(int x=0;x<img->w;x++){
      Color c = img->data[x][y];
      line[4*x+0] = c.b;
      line[4*x+1] = c.g;
      line[4*x+2] = c.r;
      line[4*x+3] = 0xff;
    }
    fwrite(line, 1, 4*img->w, fp);
  }
}
