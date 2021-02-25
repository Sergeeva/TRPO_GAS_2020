//-*- coding: utf-8 -*-
//******************************************************************************
// Обозначение:		readWavFuncs.c 				       
// Назначение:			
// Разработчик:     	Сергеева Е.И.
// Дата последней корректировки: 
//******************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "def_types.h"
#include "readWavFuncs.h"

//читает структуру заголовка файла f_in в структуру header
//и выводит информацию из заголовка на экран.
void readHeader(t_wavhdr* header, FILE* f_in){

  fread(header, sizeof(t_wavhdr), 1, f_in);
  
  //пример печати параметров wav-файла

  //printf("***** Parametrs of WAV-file [%s] *****\n", fin_name);
	
  printf("chunkID =  %s\n", header->chunkID);
  printf("chunkSize =  %i\n", header->chunkSize);
  printf("format =  %s\n", header->format);
  printf("subchunk1ID =  %s\n", header->subchunk1ID);
  printf("subchunk1Size =  %i\n", header->subchunk1Size);
  printf("audioFormat =  %i\n", header->audioFormat);
  printf("numChannels = %i\n", header->numChannels);
  printf("sampleRate =  %i\n", header->sampleRate);
  printf("blockAlign = %i\n", header->blockAlign);
  printf("bitsPerSample = %i\n", header->bitsPerSample);
	
  printf("subchunk2ID =  %s\n", header->subchunk2ID);
  printf("subchunk2Size =  %i\n", header->subchunk2Size);
	
  printf("*********************************************************\n");

  return;
}


//читает из файла f_in порцию данных размером N в массив data_portion.
void readData(double* data_portion, int N, FILE* f_in ){

  short int i, tmp;
  for (i = 0; i < N; i++){
    fread(&tmp, sizeof(short int), 1, f_in);
    data_portion[i]=(double)tmp;
  } 

  return;
}

// Return the number of portions and the tail size
void calcPortions(int* portions_info, t_wavhdr* header, int N){

  int bytesPerSample = bitsToBytes(header->bitsPerSample);
  int sampleNumber = header->subchunk2Size/(bytesPerSample); // моё предположение - subchunk2Size в два раза больше фактического
  portions_info[0] = sampleNumber/N; //number of portions without tail
  portions_info[1] = sampleNumber%N; //size of tail
  
  return;
}

int bitsToBytes(int bits){

  return bits/8;
}

void fftsq(double* data_portion, double* sq, int sqsize){
  fftw_complex out[sqsize];
  fftw_plan plan;
  // N = (sqsize-1)*2
  plan = fftw_plan_dft_r2c_1d((sqsize-1)*2, data_portion, out, FFTW_MEASURE);// FFTW_ESTIMATE or FFTW_MEASURE
  
  fftw_execute(plan);

  int j;
  for(j = 0; j < sqsize; j++){
    sq[j] = sqrt(out[j][0]*out[j][0]+out[j][1]*out[j][1]);
  }

  fftw_destroy_plan(plan);
  
}

void accumulation(double* acc, double* sq, int sqsize){
  double c = 4;
  double koeff  = 1 / c;
  //acc = (1 - koeff)*acc + koeff*sq;

  int i; 
  for (i = 0; i < sqsize; ++i){
    acc[i] = acc[i]*(1 - koeff) + sq[i]*koeff;
  }
}
