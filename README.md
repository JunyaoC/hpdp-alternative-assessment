# High Performance Data Processing
### Background
High Performance Data Processing, or HPDP, is a subject taught in Universiti Teknologi Malaysia. To complete the course, students are required to complete an alternative assessment which tests the student's understanding and the ability to develop Parallel Programs using MPI and OpenMP.

Specifically, I am enrolled in the 2020/2021 batch, section 2 of the subject, which taught by Ts. Dr. Nur Haliza binti Abdul Wahab.

## Assessment Objective

> In your program, you MUST implement at least MPI, OpenMP, MPIPoint to
> Point Communication, MPI Collective Communication, Decomposition.

Let me decipher the message:

1. From the given file which contains a 10,000 x 10,000 binary matrix, we are required to locate the **Top Left** and **Bottom Right** coordinates of the 1's Rectangles.
2. Implement MPI
3. Implement OpenMP
4. Use MPI Collective **AND** Point to Point Communication
5. Applies Decomposition (Strategy/Functional)

Yep! Those are the minimum requirements! 🥳 **At Least** !

## Precaution ⚠
For those who are unaware, please, **avoid** opening the `exam-data.txt` without care. Use an appropriate reader, for example Sublime Text or VS Code. Reading it using notepad had made it Not Responding in my machine. **Careful!**

## Challenges
Because the file is too big, while developing this solution I had met Segmentation Error a couple of times (actually all the times). This is due to not enough memory to allocate new variables (as far as I understand). Take this example:

    while(1){
    	int lineBuffer[10000];
    	//do something here//
    }
this will cause the program to initiate a **new variable every time it runs** the while loop. So, this made 2D array **not** an option (such that you can traverse not only by row, but you can traverse in column).

C programming does not play well with Files. After a lot of struggles, I finally found a way to use fseek properly.

## Solutions
~~So eventually after crying silently for some time, drop a bunch of hair, toss myself around the bed in the middle of the night, I finally developed a parallel solution after the morning coffee.~~

The latest solution is the refactor version after 3 times writing the code. In summary, I wrote 4 versions of code😂. All of them can be found in the folder `development-archive` if you are interested, but those are not parallel solutions!

Now, let's take you to the promised land (Listen > [🎸](https://www.youtube.com/watch?v=l482T0yNkeo)):
1. Rectangle Detection Algorithm
2. Parallel Model
3. Decomposition Strategy
4. MPI
5. OpenMP decorations
6. Benchmarking

### Rectangle Detection
Because we're reading line by line, we detect the rectangle on the fly.
![enter image description here](https://i.imgur.com/qCsx89z.png)

So the rectangle will be splitted into smaller chunks. The initial 2 Rectangles, will be splitted into 4 as shown in the image above.

![enter image description here](https://i.imgur.com/rmm4DRI.png)

Not a fan of Flow Chart?

Okay lah, I explain in text.

1. Read the string from the file.
2. If the program did not encounter non-empty string before, we check the current line whether it got 1 or not.
3. If yes, copy the line into the memory.
4. Record the current Y (row number) into the memory.
5. Detect the edges using the Walk Line Function. (traverse the line, if we read from 0 into 1, it means we found the left edge. If we read from 1 to 0, meaning we reached the right edge).
6. Read next line. (Assume we already have a non-empty line in memory)
7. Compare the next line with the previously encoutered non-empty line.
8. If same, meaning there's still the same chunk. Continue reading in this case.
9. Once we found a different line, meaning the previous chunk ended.
10. The bottom right corner's X is equal to the right edge's X.
11. The bottom right corner's Y is equal to the previous line row number.
12. Handle the current line as the current line may consists of 1.
13. Continue until the end.
14. In the end, maybe the last line is still the same as the second last line. We **need** to handle it, and consider the chunk ended in the last line.
15. Thank you for reading this guide.

### Parallel Model
![enter image description here](https://i.imgur.com/EGX57qt.png)

> I'm sorry Lincoln, but they really coin it as Master and Slave model

We're using master and slave model. Goes hand in hand with domain decomposition. The master tells the slave to start read from which lines.
|Slave  |Read From Line |
|--|--|
| 1 |1-5000  |
|2|5001-10000|

Of  course, if you have more slaves, just divide them equally. In case of unequal division, just distribute the remainder to the first few slaves.

### Decomposition Strategy
![enter image description here](https://i.imgur.com/cdPjbsr.png)

Well, things cut short. I do domain decomposition. Everyone reads a smaller chunk. Work gets done faster.

### MPI
![enter image description here](https://i.imgur.com/XeahLji.png)

I use MPI to....generate parallel threads? And use MPI_Scatter to broadcast data from Master to Slaves. Then Slaves transmit back to Master using MPI_Send, Master receive via MPI_Recv. MPI_Barrier is like the road block during MCO, to block everyone so that everyone starts from the same line.

### OpenMP Decoration
Yup. Just Decorations.
![enter image description here](https://i.imgur.com/QF9G3Fc.png)

After running a benchmarking I found that With or Without it does not affect performance. So pure decoration. But *pretty* pretty.

### Benchmarking
I mentioned benchmarking also right? It's a pretty simple thing. [I ran](https://www.youtube.com/watch?v=iIpfWORQWhU)  a simple benchmarking procedure as folow:
1. Run mpicc with different -np each time, from 2-19.
2. Each trials run 5 times, and get the average execution time.
3. Plot the graph or whatever.

The script is written in Python. A stupid simple test, not gonna maintain it, **do not** create pull request.

## Video Presentation

If you want a video for someone explain this, check this out:
https://www.youtube.com/watch?v=izlSJLKNyKY&t=400s

## Appendix
exam-data.txt - https://1drv.ms/t/s!Ak4Quo6aIOr_yc8x-qD-wivDGxEu7g
question paper - https://1drv.ms/b/s!Ak4Quo6aIOr_yc8yW-l_vJE2A-IKmA
