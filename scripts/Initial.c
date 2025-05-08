#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Frame size (QCIF format)
#define M 200
#define N 200

// 2D matrices for the current frame and output with +2 padding on each side
int current[M+2][N+2];
int output[M+2][N+2];
int i,j;

// Reads a binary image file into the 'current' matrix with offset (+1,+1) for padding
void read(){
  FILE *frame_c;
  if((frame_c=fopen("cherry_200x200_binary.yuv","rb"))==NULL)
  {
    printf("Current frame doesn't exist\n");
    exit(-1);
  }

  // Load image data into central part of 'current' matrix
  for(i = 0; i < M; i++)
  {
    for(j = 0; j < N; j++)
    {
      current[i+1][j+1] = fgetc(frame_c);
    }
  }
  fclose(frame_c);
}

// Writes the 'output' matrix to a binary file, ignoring the padded borders
void write(){
	FILE *frame_c;
  	frame_c=fopen("out_cherry_200x200_binary.yuv","wb");
  	
  	for(i = 0; i < M; i++){
    	for(j = 0; j < N; j++){
      		fputc(output[i+1][j+1],frame_c);	
    	}
 	}
	fclose(frame_c);
}

// Applies 1-padding around the image for erosion
void erosionpadding(){
	//Left column
	for(i = 0; i < M+1; i++){
		current[i][0]=1;
	}
	//Top row
	for(i = 0; i < N+1; i++){
		current[0][i]=1;
	}
	//Right column
	for(i = 0; i < M+1; i++){
		current[i][N+2]=1;
	}//Bottom row
	for(i = 0; i < N+1; i++){
		current[M+2][i]=1;
	}
}

// Applies 0-padding around the image for dilation
void dilationpadding(){
	//Left column
	for(i = 0; i < M+1; i++){
		current[i][0]=0;
	}
	//Top row
	for(i = 0; i < N+1; i++){
		current[0][i]=0;
	}
	//Right column
	for(i = 0; i < M+1; i++){
		current[i][N+2]=0;
	}//Bottom row
	for(i = 0; i < N+1; i++){
		current[M+2][i]=0;
	}
}

// Erosion algorithm using a 2x2 structuring element of 1s
void erosion(){
	for(i = 1; i < M+1; i++){//For each row
		for(j = 1; j < N+1; j++){//For each column
			// All 4 pixels in the 2x2 window must match (255) to set output to 255
			if(current[i][j]==255 && 
			   current[i+1][j+1]==255 &&
			   current[i][j+1]==255 &&
			   current[i+1][j]==255){
			   output[i][j]=255;
			}
			else{
				output[i][j]=0;	
			}
		}
	}		
}

// Dilation algorithm using a 2x2 structuring element of 1s
void dilation(){
	for(i = 1; i < M+1; i++){//For each row
		for(j = 1; j < N+1; j++){//For each column
			// At least one pixel in the 2x2 window must match (255) to set output to 255
			if(current[i][j]==255 ||
			   current[i+1][j+1]==255 ||
			   current[i][j+1]==255 ||
			   current[i+1][j]==255){	
			   output[i][j]=255;
			}
			else{
				output[i][j]=0;
			}
		}
	}
}

// Main function: reads input, performs erosion and dilation, writes output
int main(){
	read();
	erosionpadding();
	erosion();
	dilationpadding();
	dilation();
	write();	
	return 0;
}


