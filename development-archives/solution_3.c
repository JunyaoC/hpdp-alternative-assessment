#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int width_size = 10000;
int rectangle_count = 1;
int line_qty = 10000;
char filename[] = "exam-data.txt";

void walk_line(char rowString[], int tlArr[], int trArr[]){
	int left_edge_x = -1;
	int right_edge_x = -1;

	int leftBufferIndex = 0;
	int rightBufferIndex = 0;

	for(int i=0;i<width_size;i++){
		if ((rowString[i]) == '1'){
			if(left_edge_x == -1){
				left_edge_x = i;
				tlArr[leftBufferIndex] = (i);
				leftBufferIndex++;                
			}

			if(i == (width_size-1)){ 
				trArr[rightBufferIndex] = i;
				left_edge_x =  -1;
				rightBufferIndex++; 
			}
		}

		if ((rowString[i]) == '0'){
			if(left_edge_x > -1){

				trArr[rightBufferIndex] = (i - 1);
				left_edge_x =  -1;
				rightBufferIndex++;
			}
		}

	}
}


int sum_line(char rowString[], int count){

	int left_col = -1;
	int right_col = -1;
	int sum = 0;

	for(int i=0;i<width_size;i++){
		if (((int)rowString[i] - 48) == 1 ){
			sum+=1;
		}
	}

	return sum;
}

int main(){

	clock_t begin = clock();
	int allSum[line_qty];



	FILE* filePointer;
	int bufferLength = width_size+3;
	char buffer[bufferLength];

	filePointer = fopen(filename, "r");

	int count = 0;
	char currentString[width_size+2];
	int currentStringLineNum = -1;

	int tlBuffer[width_size];
	int trBuffer[width_size];
	int rectW[width_size];
	int rectH[width_size];

	int attemptSum = 0;
	int totalTestArea = 0;
	int testWidth,testHeight,testArea;
	
	while(count < line_qty){

		fgets(buffer, bufferLength, filePointer);
		buffer[width_size] = '\0';

		if(currentStringLineNum<0){

			attemptSum = sum_line(buffer,count);

			if(attemptSum > 0){		//// store fresh encounter

				for(int i=0;i<width_size+2;i++){
					currentString[i] = buffer[i];
				}
				
				currentStringLineNum = count;

				for (int j = 0; j < width_size; ++j){
					tlBuffer[j] = -1;
					trBuffer[j] = -1;
					rectW[j] = -1;
					rectH[j] = -1;
				}
				walk_line(currentString,tlBuffer,trBuffer); /// return the coords of Left-right pairs
			}
		}else{

			if(strcmp(buffer, currentString) != 0){	///// chunk ended

				totalTestArea = 0;

				for (int j = 0; j < width_size; ++j){
					if(tlBuffer[j] > 0){

						testWidth = (trBuffer[j] - tlBuffer[j])+1;
						testHeight = ((count-1) - currentStringLineNum) + 1;

						testArea = testWidth * testHeight;
						rectW[j] = testWidth;
						rectH[j] = testHeight;

						totalTestArea+=testArea;
					}
				}

				for (int j = 0; j < width_size; ++j){
				    if(tlBuffer[j] > 0){
				        printf("Rectangle %d coordinates [  Top Left  ]: (%d,%d)\n",rectangle_count,tlBuffer[j],currentStringLineNum);
				        printf("Rectangle %d coordinates [Bottom Right]: (%d,%d)\n",rectangle_count,trBuffer[j],(count-1));
				        printf("Dimension %d x %d\n\n",rectW[j],rectH[j]);
				        rectangle_count++;
				    }
				}

				attemptSum = sum_line(buffer,count);
				if(attemptSum > 0){

					for(int i=0;i<width_size+2;i++){
						currentString[i] = buffer[i];
					}
					
					currentStringLineNum = count;

					for (int j = 0; j < width_size; ++j){
						tlBuffer[j] = -1;
						trBuffer[j] = -1;
						rectW[j] = -1;
						rectH[j] = -1;
					}
					walk_line(currentString,tlBuffer,trBuffer); /// return the coords of Left-right pairs
				}else{
					currentStringLineNum = -1;
				}


			}



		}

		count++;
	};

	if(currentStringLineNum>0){

		totalTestArea = 0;

		for (int j = 0; j < width_size; ++j){
			if(tlBuffer[j] > 0){
			

				testWidth = (trBuffer[j] - tlBuffer[j])+1;
                    // printf("RNZ %d \n",currentStringLineNum);
				testHeight = ((count-1) - currentStringLineNum) + 1;

				testArea = testWidth * testHeight;
				rectW[j] = testWidth;
				rectH[j] = testHeight;

			
				totalTestArea+=testArea;
			}
		}

		for (int j = 0; j < width_size; ++j){
		    if(tlBuffer[j] > 0){
		        printf("Rectangle %d coordinates [  Top Left  ]: (%d,%d)\n",rectangle_count,tlBuffer[j],currentStringLineNum);
		        printf("Rectangle %d coordinates [Bottom Right]: (%d,%d)\n",rectangle_count,trBuffer[j],(count-1));
		        printf("Dimension %d x %d\n\n",rectW[j],rectH[j]);
		        rectangle_count++;
		    }
		}
	}


		fclose(filePointer);

		clock_t end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		printf("🔥 Task Completed in %f\n",time_spent);

		return 0;

	}