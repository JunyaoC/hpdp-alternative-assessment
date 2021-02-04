#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <omp.h>

int lineRows = 10000;
int lineWidth = 10000;
char filename[] = "exam-data.txt";

void walk_line(char rowString[], int tlArr[], int trArr[]){
    int left_edge_x = -1;
    int right_edge_x = -1;

    int leftBufferIndex = 0;
    int rightBufferIndex = 0;

    for(int i=0;i<lineWidth;i++){
        if ((rowString[i]) == '1'){
            if(left_edge_x == -1){
                left_edge_x = i;
                tlArr[leftBufferIndex] = (i);
                leftBufferIndex++;                
            }

            if(i == (lineWidth-1)){ 
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


int got_one(char buffer[]){

    for(int i=0;i<lineWidth;i++){
        if (buffer[i] == '1'){
            return 1;
        }
    }

    return -1;
}

void read_file(int yStartNum,int yReadLineQty, int rank, int masterID){
    FILE *fp;
    int c;
    char lineBuffer[lineWidth+2];
    lineBuffer[lineWidth] = '\0';
    fp = fopen(filename,"r");

    int yNum = 1;
    int lineQty = 0;

    int currentStringY = -1;
    char currentString[lineWidth+2];

    int tlBuffer[lineWidth];
    int trBuffer[lineWidth];
    int rectW[lineWidth];
    int rectH[lineWidth];

    int sendPayload[4];
    

    for(int i=0;i<yReadLineQty;i++){
        int count = 0;
        fseek( fp, (yStartNum + i) * (lineWidth+1), SEEK_SET );
        while(1) {
            c = fgetc(fp);
            if(c < 48 ) {
                break;
            }else{
                lineBuffer[count] = c;
            }
            count++;
        }


        if(currentStringY < 0){

            if(got_one(lineBuffer)){

                for(int i=0;i<lineWidth+2;i++){
                    currentString[i] = lineBuffer[i];
                }
                
                currentStringY = (yStartNum+i);

                for (int j = 0; j < lineWidth; ++j){
                    tlBuffer[j] = -1;
                    trBuffer[j] = -1;
                }

                walk_line(currentString,tlBuffer,trBuffer);
            }
        }else{
            if(strcmp(lineBuffer, currentString) != 0){ 


                for (int j = 0; j < lineWidth; ++j){
                    if(tlBuffer[j] > 0){                      

                        sendPayload[0] = tlBuffer[j];
                        sendPayload[1] = currentStringY;
                        sendPayload[2] = trBuffer[j];
                        sendPayload[3] = (yStartNum+i)-1;

                        MPI_Send(&sendPayload, 4, MPI_INT, masterID, 0, MPI_COMM_WORLD);
                    }
                }
                if(got_one(lineBuffer)){

                    for(int i=0;i<lineWidth+2;i++){
                        currentString[i] = lineBuffer[i];
                    }

                    currentStringY = (yStartNum+i);

                    for (int j = 0; j < lineWidth; ++j){
                        tlBuffer[j] = -1;
                        trBuffer[j] = -1;
                    }
                    walk_line(currentString,tlBuffer,trBuffer);
                }else{
                    currentStringY = -1;
                }


            }
        }

    }

    if(currentStringY>0){

        for (int j = 0; j < lineWidth; ++j){
            if(tlBuffer[j] > 0){

                sendPayload[0] = tlBuffer[j];
                sendPayload[1] = currentStringY;
                sendPayload[2] = trBuffer[j];
                sendPayload[3] = yStartNum+yReadLineQty;
                MPI_Send(&sendPayload, 4, MPI_INT, masterID, 0, MPI_COMM_WORLD);
            }
        }
    }

    fclose(fp);

    sendPayload[0] = 0;
    sendPayload[1] = 0;
    sendPayload[2] = 0;
    sendPayload[3] = 0;
    MPI_Send(&sendPayload, 4, MPI_INT, masterID, 0, MPI_COMM_WORLD);

}


int main(){


    double startTime,endTime;
    startTime=MPI_Wtime();

    MPI_Init(NULL,NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    int numSlave = world_size-1;

    int scatterReadQty[1];
    int scatterStartingPt[1];
    int readQty[numSlave];
    int startingLine[numSlave];


    if (world_rank == 0){

        printf("Master Thread\n");

        // We are using a Master Slave Style. #Sorry NIGGA

        if(lineRows%numSlave>0){
            int diff = lineRows%numSlave;

            for(int i=0;i<numSlave;i++){
                readQty[i] = lineRows/numSlave;
            }

            int slaveID = 0;
            while(diff>0){
                readQty[slaveID] = readQty[slaveID]+1;
                diff--;
                slaveID++;
            }
        }else{
            for(int i=0;i<numSlave;i++){
                readQty[i] = lineRows/numSlave;
            }
        }



        int startLinePointer = 0;

        for (int i = 0; i < (numSlave); ++i){            
            printf("Slave %d Read %d Lines From Line %d\n", i,readQty[i],startLinePointer);
            startingLine[i] = startLinePointer;
            startLinePointer+=readQty[i];
        }
    }
    MPI_Scatter(&readQty, 1, MPI_INT, &scatterReadQty,1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&startingLine, 1, MPI_INT, &scatterStartingPt,1, MPI_INT, 0, MPI_COMM_WORLD);
    

    int identifiedTL_X[(lineWidth/2)];
    int identifiedTL_Y[(lineRows/2)];
    int identifiedBR_X[(lineWidth/2)];
    int identifiedBR_Y[(lineRows/2)];

    for(int i=0;i<(lineWidth/2);i++){
        identifiedTL_X[i] = -1;
        identifiedBR_X[i] = -1;
    }

    for(int i=0;i<(lineRows/2);i++){
        identifiedTL_Y[i] = -1;
        identifiedBR_Y[i] = -1;
    }

    int identifiedCursor = 0;


    MPI_Barrier( MPI_COMM_WORLD );

    if(world_rank == (world_size-1)){

        int bufferX[4];
        int sumTest = 0;

        while(1){
            MPI_Recv(&bufferX, 4, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sumTest=0;
            for(int i=0;i<4;i++){
                sumTest+=bufferX[i];
            }

            if(sumTest==0){
                break;
            }else{
                identifiedTL_X[identifiedCursor] = bufferX[0];
                identifiedTL_Y[identifiedCursor] = bufferX[1];
                identifiedBR_X[identifiedCursor] = bufferX[2];
                identifiedBR_Y[identifiedCursor] = bufferX[3];

                identifiedCursor++;
            }
        }




    }else{
        printf("I am slave %d, I will read %d Lines from Line %d\n", world_rank, scatterReadQty[0], scatterStartingPt[0]);

        int startLine = scatterStartingPt[0];
        int amountToRead = scatterReadQty[0];
        read_file(startLine,amountToRead,world_rank, world_size-1);
    }


    MPI_Barrier( MPI_COMM_WORLD );

    if(world_rank == world_size-1){

        //// run rectangle aggregator

        int combinedTL_X[identifiedCursor];
        int combinedTL_Y[identifiedCursor];
        int combinedBR_X[identifiedCursor];
        int combinedBR_Y[identifiedCursor];

        int combinedCursor = 0;

        for(int i=0;i<identifiedCursor;i++){
            for(int j=0;j<identifiedCursor;j++){
                if(identifiedTL_X[i] == identifiedTL_X[j] && identifiedBR_X[i] == identifiedBR_X[j] && (identifiedBR_Y[i]+1)==identifiedTL_Y[j]){

                    combinedTL_X[combinedCursor] = identifiedTL_X[i];
                    combinedTL_Y[combinedCursor] = identifiedTL_Y[i];
                    combinedBR_X[combinedCursor] = identifiedBR_X[j];
                    combinedBR_Y[combinedCursor] = identifiedBR_Y[j];

                    combinedCursor++;

                    identifiedTL_X[i] = -1;
                    identifiedTL_Y[i] = -1;
                    identifiedBR_X[i] = -1;
                    identifiedBR_Y[i] = -1;

                    identifiedTL_X[j] = -1;
                    identifiedTL_Y[j] = -1;
                    identifiedBR_X[j] = -1;
                    identifiedBR_Y[j] = -1;


                }
            }
        }

        int rectangle_count = 1;


        for(int i=0;i<identifiedCursor;i++){
            if(identifiedTL_X[i] > 0){
                printf("Rectangle %d coordinates [  Top Left  ]: (%d,%d)\n",rectangle_count,identifiedTL_X[i],identifiedTL_Y[i]);
                printf("Rectangle %d coordinates [Bottom Right]: (%d,%d)\n",rectangle_count,identifiedBR_X[i],identifiedBR_Y[i]);

                rectangle_count++;

            }
        }

        for(int i=0;i<combinedCursor;i++){
            printf("Rectangle %d coordinates [  Top Left  ]: (%d,%d)\n",rectangle_count,combinedTL_X[i],combinedTL_Y[i]);
            printf("Rectangle %d coordinates [Bottom Right]: (%d,%d)\n",rectangle_count,combinedBR_X[i],combinedBR_Y[i]);
            rectangle_count++;
        }




    }



    MPI_Finalize();

    endTime=MPI_Wtime();
    double time_spent=endTime-startTime;
    printf("Task Completed in %f seconds\n",time_spent);

    return 0;
}