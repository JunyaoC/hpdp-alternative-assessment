#include <mpi.h>
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
// int line_qty = 500;
// char filename[] = "exam-data.txt";

void walk_line(int line_num, int tlArr[], int trArr[]){
    /// another posibility is that, the top line is the joined with another rectangle

    int left_edge_x = -1;
    int right_edge_x = -1;

    FILE *f = fopen(filename, "r");
    // FILE *f = fopen("exam-data.txt", "r");
    fseek(f, ((line_num)*(width_size+2)), SEEK_SET);
    char part[width_size];
    fread(part, 1,width_size, f);
    fclose(f);

    int leftBufferIndex = 0;
    int rightBufferIndex = 0;

    for(int i=0;i<width_size;i++){
        if ((part[i]) == '1'){
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

        if ((part[i]) == '0'){
            if(left_edge_x > -1){
                trArr[rightBufferIndex] = (i - 1);
                left_edge_x =  -1;
                rightBufferIndex++;
            }
        }

    }
}

int check_is_one(int x,int y){

    // int width_size = 7;
    // int width_size = 10000;
    int is_one = 0;

    FILE *f = fopen("test.txt", "r");
    // FILE *f = fopen("exam-data.txt", "r");
    fseek(f, ((y-1)*(width_size+2)), SEEK_SET);  // set file pointer to 2nd position (0-indexed)
    char part[width_size];    // array for 9 characters plus terminating 0
    fread(part, 1,width_size, f);   // read 9 members of size 1 (characters) from f into part
    fclose(f);


    if ((part[x]) == '1'){
        is_one = 1;
    }
    
}


int readline(int line_num){

    // int width_size = 10000;

    // FILE *f = fopen("exam-data.txt", "r");
    FILE *f = fopen(filename, "r");
    int pointer = (line_num) * (width_size+2);
    // printf("Line %d - PTR= %d\n", line_num,pointer );
    fseek(f, pointer, SEEK_SET);  // set file pointer to 2nd position (0-indexed)
    char part[width_size+2];    // array for 9 characters plus terminating 0
    fread(part, width_size+2,1, f);   // read 9 members of size 1 (characters) from f into part
    fclose(f);

    // printf("%s\n", part);
    

    int left_col = -1;
    int right_col = -1;
    int sum = 0;

    for(int i=0;i<width_size;i++){
        if (((int)part[i] - 48) == 1 ){
            // if(left_col < 0){
            //     left_col = i;
            // }
            // if(right_col < 0){
            //     right_col = i;
            // }
            sum+=1;
        }
    }

    

    // if(sum > 0){
    //     printf("zzz line %d - Sum - %d\n", line_num, sum);
    //     printf("%s\n", part);
    // }


    // if(left_col > 0){
    //     printf("Left %d-Need 2 Map %d\n", left_col, mapRectangle);
    // }
    // printf("Line %d --> %d\n", line_num,sum);
    return sum;
}

void readline_char(int line_num, char line_str[]){

    // int width_size = 10000;

    // FILE *f = fopen("exam-data.txt", "r");
    FILE *f = fopen(filename, "r");
    fseek(f, (line_num*(width_size+2)), SEEK_SET);  // set file pointer to 2nd position (0-indexed)
    char part[width_size];    // array for 9 characters plus terminating 0
    fread(part, 1,width_size, f);   // read 9 members of size 1 (characters) from f into part
    fclose(f);

    

    int left_col = -1;
    int right_col = -1;
    int sum = 0;

    for(int i=0;i<width_size;i++){
        line_str[i] = part[i];
    }

    // line_str = part;

    // line_str[width_size] = '\0';
        // printf("%s\n", line_str);

    // if(sum > 0){
    //     printf("zzz line %d - Sum - %d\n", line_num, sum);
    //     printf("%s\n", part);
    // }


    // if(left_col > 0){
    //     printf("Left %d-Need 2 Map %d\n", left_col, mapRectangle);
    // }
}

int main(){

    MPI_Init(NULL,NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // int width_size = 10000  + 2;
    // int height_size = 10000  + 2;



    if(world_rank == 0){

        int s_id = 0;

        int allSum[line_qty];
        char rowStrings[line_qty][width_size+1];

        // #pragma omp parallel for
        for (int i = 0; i < line_qty; i++)
        {   
            allSum[i] = readline(i);
        }

        // #pragma omp parallel for
        for (int i = 0; i < line_qty; i++)
        {
            char lineBuff[width_size+1];
            // lineBuff[0]='\0';
            readline_char(i,lineBuff);

            for(int j=0;j<width_size;j++){
                rowStrings[i][j] = lineBuff[j];
                // int value = (int)lineBuff[j] - 48;
                // allSum[i]+= (value);
            }

            rowStrings[i][width_size+1] = '\0';

        }

        printf("Scan Complete\n");

        int rowNotZero = -1;

        for(int i=0;i<line_qty;i++){
            // printf("%d\n", allSum[i]);
            // printf("Line %d - SUM = %d\n", i, allSum[i]); //////dump line sum
            // if(allSum[i] > 0 && rowNotZero == -1){
            //     rowNotZero = i;
            // }

            if(allSum[i] > 0 && rowNotZero == -1){
                rowNotZero = i;
            }

            if(rowNotZero != -1 ){



                // printf("RS1 %s  ",rowStrings[i]);
                // printf("RNZ %s  \n\n",rowStrings[rowNotZero]);

                // printf("Result %d\n", strcmp(rowStrings[i], rowStrings[rowNotZero]));

                if(strcmp(rowStrings[i], rowStrings[rowNotZero]) != 0){    //// indicate end of chunk

                    // printf("RS1 %s  ",rowStrings[i]);
                    // printf("RNZ %s  \n\n",rowStrings[rowNotZero]);

                    // int left_x,right_x;
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

                    walk_line(rowNotZero,tlBuffer,trBuffer); /// return the coords of Left-right pairs

                    int totalTestArea = 0;

                    for (int j = 0; j < width_size; ++j){
                        if(tlBuffer[j] > 0){
                            // printf("TL: (%d,%d)  ",tlBuffer[j],rowNotZero);
                            // printf("TR: (%d,%d) \n",trBuffer[j],rowNotZero);

                            int testWidth = (trBuffer[j] - tlBuffer[j])+1;
                            // printf("RNZ %d \n",rowNotZero);
                            int testHeight = ((i-1) - rowNotZero) + 1;

                            int testArea = testWidth * testHeight;
                            rectW[j] = testWidth;
                            rectH[j] = testHeight;

                            // printf("W: %d H:%d T. Area: %d\n",testWidth,testHeight,testArea);
                            totalTestArea+=testArea;
                        }
                    }

                    int actualSum = 0;

                    for(int j=rowNotZero;j<i;j++){
                        actualSum+=allSum[j];
                    }

                    if(actualSum == totalTestArea){      //// we're done with these similar row sum chunk
                        for (int j = 0; j < width_size; ++j){
                            if(tlBuffer[j] > 0){
                                printf("Rectangle %d coordinates [  Top Left  ]: (%d,%d)\n",rectangle_count,tlBuffer[j],rowNotZero);
                                printf("Rectangle %d coordinates [Bottom Right]: (%d,%d)\n",rectangle_count,trBuffer[j],(i-1));
                                printf("Dimension %d x %d\n\n",rectW[j],rectH[j]);
                                rectangle_count++;
                            }
                        }
                    }
                    


                    if(allSum[i] > 0){
                        rowNotZero = i;
                    }else{
                        rowNotZero = -1;
                    }
                }
            }

            
        }

        if(rowNotZero != -1){
            // printf("RNZ: %d\n",rowNotZero);

            int tlBuffer[width_size];
                    int trBuffer[width_size];
                    int rectW[width_size];
                    int rectH[width_size];

                    for (int j = 0; j < width_size; ++j)
                    {
                        tlBuffer[j] = -1;
                        trBuffer[j] = -1;
                        rectW[j] = -1;
                        rectH[j] = -1;
                    }

                    walk_line(rowNotZero,tlBuffer,trBuffer); /// return the coords of Left-right pairs

                    int totalTestArea = 0;

                    for (int j = 0; j < width_size; ++j){
                        if(tlBuffer[j] > 0){
                            // printf("TL: (%d,%d)  ",tlBuffer[j],rowNotZero);
                            // printf("TR: (%d,%d) ",trBuffer[j],rowNotZero);

                            int testWidth = (trBuffer[j] - tlBuffer[j])+1;
                            int testHeight = ((line_qty-1) -  (rowNotZero)) +1 ;

                            int testArea = testWidth * testHeight;
                            rectW[j] = testWidth;
                            rectH[j] = testHeight;

                            // printf("W: %d H:%d T. Area: %d\n",testWidth,testHeight,testArea);
                            totalTestArea+=testArea;
                        }
                    }

                    int actualSum = 0;

                    for(int j=rowNotZero;j<line_qty;j++){
                        actualSum+=allSum[j];
                    }

                    if(actualSum == totalTestArea){      //// we're done with these similar row sum chunk
                        for (int j = 0; j < width_size; ++j){
                            if(tlBuffer[j] > 0){
                                printf("Rectangle %d coordinates [  Top Left  ]: (%d,%d)\n",rectangle_count,tlBuffer[j],rowNotZero);
                                printf("Rectangle %d coordinates [Bottom Right]: (%d,%d)\n",rectangle_count,trBuffer[j],(rowNotZero));
                                printf("Dimension %d x %d\n\n",rectW[j],rectH[j]);
                                rectangle_count++;
                            }
                        }
                    }


        }


    }

    return 0;
}