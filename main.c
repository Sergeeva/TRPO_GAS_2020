//-*- coding: utf-8 -*-
//******************************************************************************
// Обозначение: main.c 				       
// Назначение:			
// Разработчик: Сергеева Е.И.
// Дата последней корректировки: 
//******************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "def_types.h"
#include "readWavFuncs.h"
#include "fftw3.h"

/*
Путь к wav-файлу можно задать из командной строки,
по умолчанию открывается data/ton_signal.wav
 */

int main(int argc, char** argv) {
  
  char* fin_name = "ton_signal_3.wav";
  if(argc > 1){
    fin_name = argv[1];
  }
  
  FILE* f_in = fopen(fin_name, "rb");
  if(f_in == NULL){
    printf("ERROR in OPEN FILE [%s] \n", fin_name);
	  return 1;
  } else {
    printf("OPEN FILE [%s] is OK \n", fin_name);
  }

  char* f_out_name = "python/output.dat";
  FILE* f_out = fopen(f_out_name, "wb");
  if(f_out == NULL){
    printf("ERROR in OPEN FILE [%s] \n", f_out_name);
	  return 1;
  } else {
    printf("OPEN FILE [%s] is OK \n", f_out_name);
  }

  // Reading a header of file  
  t_wavhdr header;
  readHeader(&header, f_in);
struct timeval tv_start, tv_end;
  
  
  int N = 1024;

  // Calculate the number of portions and the tail size
  int portions_info[2]; //[PortionQuantity, TailSize]
  calcPortions(&portions_info[0], &header, N);

  // initialising accumulation 
  int halfN = N / 2 + 1;
  double acc[halfN]; 
  memset(acc, 0.0, sizeof acc);

  int i; double data_portion[N]; double sq[halfN]; 
  for (i = 0; i < portions_info[0]; i++) {
	  readData(&data_portion[0], N, f_in);

	  fftsq(&data_portion[0], &sq[0], halfN);
	  accumulation(&acc[0], &sq[0], halfN);
  }
   gettimeofday(&tv_start, NULL);
      

  gettimeofday(&tv_end, NULL);
  // Reading portions + FFT + accumulation till the end in cycle
  for(i = 0; i < portions_info[0]; ++i){
      readData(&data_portion[0], N, f_in );

 
      accumulation(&acc[0], &sq[0], halfN);
  }

  // Reading tail + FFT + accumulation
  if (portions_info[1] > 0) {
    memset(data_portion, 0.0, sizeof data_portion);
    readData(&data_portion[0], portions_info[1], f_in );

    fftsq(&data_portion[0], &sq[0], halfN);
 
    accumulation(&acc[0], &sq[0], halfN);
  }

  int a;
  for (a = 0; a < halfN; ++a)
    fprintf(f_out, "%f\n", acc[a]);

  fclose(f_in);
  fclose(f_out);
  printf("Time elapsed for programm: %ld mcrsec \n", (long int)((tv_end.tv_sec-tv_start.tv_sec)*1000000 + (tv_end.tv_usec - tv_start.tv_usec)));
    
  return 0;
}
