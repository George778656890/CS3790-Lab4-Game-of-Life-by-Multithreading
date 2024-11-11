//CS3790 Lab Assignment4---Game of Life by multithreading
//Zhihong Liu(5088806)
#include <iostream>
#include<string>
#include<vector>
#include<iterator>
#include<type_traits>
#include<iterator>
#include<algorithm>
#include<utility>
#include<fstream>
#include <vector>
#include <ncurses.h>
#include <cstring>
#include <thread> // Required for sleep_for  
#include <chrono> // Required for milliseconds
#include<thread>


using namespace std;
using VEC = vector<int>;
using MATRIX = vector<VEC>;

int sizeM=0;//Matrix size
int N=0;



int countNeighbours(MATRIX a,int row1,int column1){

	int finalResult=0;
	
	 // Check upper left  
	if( ((row1-1)>=0) && ((row1-1)<=(sizeM-1)) && ((column1-1)>=0) && ((column1-1)<=(sizeM-1))){
		if(a[row1-1][column1-1]==1){
			++finalResult;
		}
	}
	
	// Check directly above 
	if( ((row1-1)>=0) && ((row1-1)<=(sizeM-1)) ){
		if(a[row1-1][column1]==1){
			++finalResult;
		}
	}
	
	 // Check upper right  
	if( ((row1-1)>=0) && ((row1-1)<=(sizeM-1)) && ((column1+1)>=0) && ((column1+1)<=(sizeM-1))){
		if(a[row1-1][column1+1]==1){
			++finalResult;
		}
	}
	
	
	// Check left  
	if(((column1-1)>=0) && ((column1-1)<=(sizeM-1))){
		if(a[row1][column1-1]==1){
			++finalResult;
		}
	}
	
	// Check right  
	if(((column1+1)>=0) && ((column1+1)<=(sizeM-1))){
		if(a[row1][column1+1]==1){
			++finalResult;
		}
	}
	
	// Check lower left  
	if( ((row1+1)>=0) && ((row1+1)<=(sizeM-1)) && ((column1-1)>=0) && ((column1-1)<=(sizeM-1))){
		if(a[row1+1][column1-1]==1){
			++finalResult;
		}
	}
	
	 // Check directly below 
	if( ((row1+1)>=0) && ((row1+1)<=(sizeM-1)) ){
		if(a[row1+1][column1]==1){
			++finalResult;
		}
	}
	
	// Check lower right 
	if( ((row1+1)>=0) && ((row1+1)<=(sizeM-1)) && ((column1+1)>=0) && ((column1+1)<=(sizeM-1))){
		if(a[row1+1][column1+1]==1){
			++finalResult;
		}
	}
	
	return finalResult;
}




void subRegionComputing(int threadNumber,MATRIX &input,MATRIX &output){

    //Calculate the actual indices for the sub-region of matrix
	int indexRange=sizeM/N;
    int startRow=(threadNumber/N)*indexRange;
    int startColumn=(threadNumber%N)*indexRange;

    for(int i=startRow;i<startRow+indexRange;++i){
     	for(int j=startColumn;j<startColumn+indexRange;++j){
               int neighbourNumber=countNeighbours(input,i,j);
               
               //Any dead cell with exactly three live neighbours becomes a live cell
               if( (input[i][j]==0) && (neighbourNumber==3) ){
               		output[i][j]=1;
               }else if( (input[i][j]==1) ){
               
               	switch(neighbourNumber)
               	{
               	case 0:
               	case 1:output[i][j]=0;break;//Any live cell with fewer than two live neighbours dies
               	
               	case 2://Any live cell with two or three live neighbours lives on to the next generation.
               	case 3:output[i][j]=1;break;
               	
               	default:output[i][j]=0;break; //Any live cell with more than three live neighbours dies
               	}
               	
               }
               
     	}
     	
     }


}


void printMatrix(MATRIX a){

     for(int i=0;i<sizeM;++i){
     	for(int j=0;j<sizeM;++j){
     		if(a[i][j]==1){
     			mvprintw(i,j, "X");// Print "X" at the (i, j) position of the matrix
     		}else {  
                       mvprintw(i, j, " "); // Print empty space for dead cells  
            } 
     	}
     }
     refresh(); // Update the display to show the printed "X".
}



int main(int argc,char * argv[]){

//Argument amount check
    if(argc!=4){
    	cerr<<string("Your program ")+argv[0]+" were not given enough arguments .";
    	return 1;
    }

sizeM=stoi(string(argv[1]));

N=stoi(string(argv[2]));
int threadNumber=N*N;

int generationNumberMAX=stoi(string(argv[3]));


MATRIX currentMatrix(sizeM,VEC(sizeM,0));
MATRIX nextMatrix(sizeM,VEC(sizeM,0));


MEVENT event;

initscr ();// Initialize the ncurses mode, setting up the screen
noecho();// Disable echoing of typed characters to the screen.  
cbreak();// Disable line buffering, so input is available immediately.  
keypad(stdscr, TRUE);// Enable special keys (like arrow keys, function keys, etc.) to be captured.
curs_set(0);// Hide the cursor in the ncurses window. 
mouseinterval(3); // Set the interval for mouse events (this sets how quickly you want to receive mouse events). 
mousemask(ALL_MOUSE_EVENTS, NULL);// Enable all mouse events to be captured.

// Define the boundaries for row and column  
const int rowMin = 0;    // Minimum row limit  
const int rowMax = sizeM-1;   // Maximum row limit  
const int colMin = 0;    // Minimum column limit  
const int colMax = sizeM-1;   // Maximum column limit 

 while (1) {  
        int c = getch();  
        switch (c) {  
            case KEY_MOUSE:  
                if (getmouse(&event) == OK) {  
                    // Check if the clicked position is within bounds  
                    if (event.bstate & BUTTON1_PRESSED) {  
                        if (event.y >= rowMin && event.y <= rowMax &&  
                            event.x >= colMin && event.x <= colMax) {  
							
							if(currentMatrix[event.y][event.x]==0){
								 currentMatrix[event.y][event.x]=1; //Set the live cell
			                     mvprintw(event.y, event.x, "X");  
                                 refresh();                                      
							}else if(currentMatrix[event.y][event.x]==1){
								currentMatrix[event.y][event.x]=0;  //Clear the live cell
								mvprintw(event.y, event.x, " "); 
								refresh();
							}
                        }  
                    }  
                }  
                break;  
            case 'q':  
                goto beginGeneration;  
        }  
}

beginGeneration:

    for(int i=0;i<generationNumberMAX;++i){
       
       	   // Generate the next state 	
           vector<thread> threads;

           // Create and start threads
           for(int i=0;i<threadNumber;++i){
                threads.emplace_back(subRegionComputing,i,ref(currentMatrix),ref(nextMatrix));

           }
           
           //Wait for all threads to finish
           for(auto &a:threads){
              a.join();
           }

       	   
       	   // Print the next state and then swap matrices  
           printMatrix(nextMatrix);  
       	   
       	   // Create a delay to get a better display effect  
           std::this_thread::sleep_for(std::chrono::milliseconds(1000));

       	   // Swap matrices for the next generation 
       	   currentMatrix= nextMatrix;
       	   
       }


    endwin(); 
    return 0;
}

