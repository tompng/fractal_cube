#include <stdio.h>

template <class T> class Array2D{
public:
  int w, h;
  T**data;
  Array2D(int _w, int _h){
    w = _w;
    h = _h;
    data = new T*[w];
    for(int x=0;x<w;x++){
      data[x] = new T[h];
    }
  }
  ~Array2D(){
    for(int x=0;x<w;x++){
      delete[] data[x];
    }
    delete[] data;
  }
};

typedef struct {
  char r;
  char g;
  char b;
} Color;

class Image : public Array2D<Color>{
public:
  Image(int w, int h) : Array2D(w, h){}
  void save(FILE*fp){
    char header[54];
    sprintImageHeader(header);
    fwrite(header, 1, 54, fp);
    char*line=new char[4*w];
    for(int y=0;y<h;y++){
      for(int x=0;x<w;x++){
        Color c = data[x][y];
        line[4*x+0] = c.b;
        line[4*x+1] = c.g;
        line[4*x+2] = c.r;
        line[4*x+3] = 0xff;
      }
      fwrite(line, 1, 4*w, fp);
    }
    delete[] line;
  }
private:
  void sprintInt(int x, int size, char*s){
    for(int i=0;i<size;i++){
      s[i]=x&0xff;
      x>>=8;
    }
  }
  void sprintImageHeader(char*header){
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
};
