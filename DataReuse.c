#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Frame size (QCIF format)
#define M 200
#define N 200

int current[M+2][N+2];
int output[M+2][N+2];
int i, j;

#pragma arm section zidata="ram"
int buffer1[N+2], buffer2[N+2], buffer3[N+2];
#pragma arm section

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

// More fusion, only because M=N
void erosionpadding(){
	for(i = 0; i < M+1; i++){
		current[i][0]=1;
		current[i][N+2]=1;
		current[0][i]=1;
		current[M+2][i]=1;
	}
}

void dilationpadding(){
	for(i = 0; i < M+1; i++){
		current[i][0]=0;
		current[i][N+2]=0;
		current[0][i]=0;
		current[M+2][i]=0;
	}
}

// Optimized erosion using row buffers to reduce memory reads
void erosion() {
    // Load first three rows into buffers
    memcpy(buffer1, current[1], sizeof(buffer1));
    memcpy(buffer2, current[2], sizeof(buffer2));
    memcpy(buffer3, current[3], sizeof(buffer3));

    for (i = 1; i < M+1; i = i+2) {
        for (j = 1; j < N+1; j++) {
            // First row of pair
            if (buffer1[j] == 255 &&
                buffer2[j + 1] == 255 &&
                buffer1[j + 1] == 255 &&
                buffer2[j] == 255) {
                output[i][j] = 255;
            } else {
                output[i][j] = 0;
            }

            // Second row of pair
            if (buffer2[j] == 255 &&
                buffer3[j + 1] == 255 &&
                buffer2[j + 1] == 255 &&
                buffer3[j] == 255) {
                output[i + 1][j] = 255;
            } else {
                output[i + 1][j] = 0;
            }
        }

        // Slide buffers down
        if (i + 2 <= M + 1) {
            memcpy(buffer1, buffer2, sizeof(buffer1));
            memcpy(buffer2, buffer3, sizeof(buffer2));
            memcpy(buffer3, current[i + 3], sizeof(buffer3));
        }
    }
}

// Optimized dilation using row buffers to reduce memory reads
void dilation() {
    // Load first three rows into buffers
    memcpy(buffer1, current[1], sizeof(buffer1));
    memcpy(buffer2, current[2], sizeof(buffer2));
    memcpy(buffer3, current[3], sizeof(buffer3));

    for (i = 1; i < M+1; i = i+2) {
        for (j = 1; j < N + 1; j++) {
            if (buffer1[j] == 255 ||
                buffer2[j + 1] == 255 ||
                buffer1[j + 1] == 255 ||
                buffer2[j] == 255) {
                output[i][j] = 255;
            } else {
                output[i][j] = 0;
            }

            if (buffer2[j] == 255 ||
                buffer3[j + 1] == 255 ||
                buffer2[j + 1] == 255 ||
                buffer3[j] == 255) {
                output[i + 1][j] = 255;
            } else {
                output[i + 1][j] = 0;
            }
        }

        // Slide buffers
        if (i + 2 <= M + 1) {
            memcpy(buffer1, buffer2, sizeof(buffer1));
            memcpy(buffer2, buffer3, sizeof(buffer2));
            memcpy(buffer3, current[i + 3], sizeof(buffer3));
        }
    }
}

int main() {
    read();
    erosionpadding();
    erosion();
    dilationpadding();
    dilation();
    write();
    return 0;
}
