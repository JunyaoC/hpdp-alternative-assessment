// #include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int width_size = 7;
int rectangle_count = 1;
int line_qty = 6;
char filename[] = "test.txt";

// int width_size = 10000;
// int rectangle_count = 1;
// int line_qty = 200;
// char filename[] = "exam-data.txt";


void walk_line(char rowString[], int tlArr[], int trArr[]){
    /// another posibility is that, the top line is the joined with another rectangle

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
                trArr[rightBufferIndex] = i; left_edge_x
                =  -1; rightBufferIndex++; 
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

void sum_line(char rowString[], int count, int allSum[]){

    int left_col = -1;
    int right_col = -1;
    int sum = 0;

    for(int i=0;i<width_size;i++){
        if (((int)rowString[i] - 48) == 1 ){
            // if(left_col < 0){
            //     left_col = i;
            // }
            // if(right_col < 0){
            //     right_col = i;
            // }
            sum+=1;
        }
    }

    // return sum;
    allSum[count] = sum;
}


int main(){

	// MPI_Init(NULL,NULL);

	// int world_rank;
	// MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	// int world_size;
	// MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // int width_size = 10000  + 2;
    // int height_size = 10000  + 2;

	// if(world_rank == 0){

		char allStrings[line_qty][width_size+2];
		int allSum[line_qty];



		FILE* filePointer;
		int bufferLength = width_size+3;
		char buffer[bufferLength];

		filePointer = fopen(filename, "r");

		
		int count = 0;
		#pragma omp single
		{
		while(count<line_qty) {

			fgets(allStrings[count], bufferLength, filePointer);
			// printf("[%s]", buffer);

			// for(int i=0;i<width_size;i++){
			// 	printf("<%d>", buffer[i]);
			// }

			// char x[] = "wtf";
			// #pragma omp critical
			allStrings[count][width_size] = '\0';

			// #pragma omp task firstprivate(count) shared(allStrings) shared(allSum)
			sum_line(allStrings[count],count,allSum);


			#pragma omp critical
			count++;
			
			// allSum[count] = sum_line(allStrings[count]);

		}

		#pragma omp taskwait

		}

		printf("Read Complete\n");

		fclose(filePointer);

		for(int i=0;i<count;i++){
			printf("Line %d - SUM >> %d\n", i, allSum[i]);
		}

		///// start to traverse array

		int nonZeroRowIndex = -1;

		for(int i=0;i<line_qty+1;i++){
			if(allSum[i] > 0){



				if(nonZeroRowIndex < 0){	//// fresh encounter of new chunk
					nonZeroRowIndex = i;
				}

				if(nonZeroRowIndex > 0){	//// not fresh encouter (First line of chunk runs too, as rectangle can be 1 line tall)
					if(strcmp(allStrings[i], allStrings[nonZeroRowIndex]) != 0){		//// compare both string, if not the same, means termination of previous chunk
						int tlBuffer[width_size];
						int trBuffer[width_size];
						int rectW[width_size];
						int rectH[width_size];

						for (int j = 0; j < width_size; ++j){
							tlBuffer[j] = -1;
							trBuffer[j] = -1;
							rectW[j] = -1;
							rectH[j] = -1;
						}

						walk_line(allStrings[nonZeroRowIndex],tlBuffer,trBuffer); /// return the coords of Left-right pairs

						int totalTestArea = 0;

	                    for (int j = 0; j < width_size; ++j){
	                        if(tlBuffer[j] > 0){
	                            // printf("TL: (%d,%d)  ",tlBuffer[j],nonZeroRowIndex);
	                            // printf("TR: (%d,%d) \n",trBuffer[j],nonZeroRowIndex);

	                            int testWidth = (trBuffer[j] - tlBuffer[j])+1;
	                            // printf("RNZ %d \n",nonZeroRowIndex);
	                            int testHeight = ((i-1) - nonZeroRowIndex) + 1;

	                            int testArea = testWidth * testHeight;
	                            rectW[j] = testWidth;
	                            rectH[j] = testHeight;

	                            // printf("W: %d H:%d T. Area: %d\n",testWidth,testHeight,testArea);
	                            totalTestArea+=testArea;
	                        }
	                    }

	                    int actualSum = 0;

	                    for(int j=nonZeroRowIndex;j<i;j++){
	                        actualSum+=allSum[j];
	                    }

	                    if(actualSum == totalTestArea){      //// we're done with these similar row sum chunk
	                        for (int j = 0; j < width_size; ++j){
	                            if(tlBuffer[j] > 0){
	                                printf("Rectangle %d coordinates [  Top Left  ]: (%d,%d)\n",rectangle_count,tlBuffer[j],nonZeroRowIndex);
	                                printf("Rectangle %d coordinates [Bottom Right]: (%d,%d)\n",rectangle_count,trBuffer[j],(i-1));
	                                printf("Dimension %d x %d\n\n",rectW[j],rectH[j]);
	                                rectangle_count++;
	                            }
	                        }
	                    }

	                    nonZeroRowIndex = i;

					}



					
				}
			}
		}


	// }

	// MPI_Finalize();


	return 0;
}