void selectionsort(int n,double*arr,int*indices){
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

void mergesort(int n,double*array,int*indices,int*tmp){
  int bitcount=0;
  for(int step=1;step<n;step*=2){bitcount++;}
  if(bitcount%2==0){int*_tmp=tmp;tmp=indices;indices=_tmp;}
  if(n%2)indices[n-1]=n-1;
  for(int i=0;i<n-1;i+=2){
    if(array[i]>array[i+1]){
      indices[i]=i+1;indices[i+1]=i;
    }else{
      indices[i]=i;indices[i+1]=i+1;
    }
  }
  for(int step=2;step<n;step*=2){
    int*_tmp=tmp;tmp=indices;indices=_tmp;
    int slices=(n+2*step-1)/(2*step);
    for(int i=0;i<slices;i++){
      int a=2*i*step, b=a+step, c=b+step;
      if(b>n)b=n;if(c>n)c=n;
      int j1=a, j2=b;
      for(int jj=a;jj<c;jj++){
        if(j1<b&&(j2==c||array[tmp[j1]]<=array[tmp[j2]])){
          indices[jj]=tmp[j1++];
        }else{
          indices[jj]=tmp[j2++];
        }
      }
    }
  }
}
void sort(int n,double*array,int*indices,int*tmp){
  if(n<16){
    return selectionsort(n,array,indices);
  }else{
    return mergesort(n,array, indices, tmp);
  }
}
