#include <iostream>
#include <mpi.h>
#include <fstream>
#include <sys/time.h>
#include <stdlib.h>

using namespace std;

// constants
const int disp_width = 30000;
const int disp_height = 30000;
const int real_min = -2;
const int real_max = 2;
const int imag_min = -2;
const int imag_max = 2;
const int terminate_var = 0;

// tags for sends/receives
#define DATA_TAG 1
#define TERMINATE_TAG 0
#define RESULT_TAG 2

// complex structure holds real/imag part of num
struct complex {
	float real;
	float imag;
};

// function prototype 
void master(); 
void slave();
void display();
int cal_pixel( complex );
int coords[disp_width][disp_height];	

// main function
int main( int argc, char *argv[] ) {
	
	// init variables
	int rank;

	// get rank to distinguish processors 
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	if( rank == 0 ) { // Master
		master();
	}

	else { 				// Slave
		slave();
	}

	// finialize mpi stuff
	MPI_Finalize();
	return 0;
}

void master() {

	// init variables for master 
	int num_processors;
	MPI_Status status;
	int row = 0;

	//image stuff
//	int coords[disp_width][disp_height];	
	int row_colors[disp_width + 1];	

	// timing stuff
	timeval startTime, endTime;
	double finalTime;	

	// output image is placed into this file
	ofstream fout;
	fout.open( "image.PPM" );
	fout << "P2\n";
	fout << disp_width << " " << disp_height << "\n";
	fout << "256\n";

	gettimeofday( &startTime, NULL );

	// figure out how many processors we have
	MPI_Comm_size( MPI_COMM_WORLD, &num_processors );

	// send row number to all available processors (don't include master!)
	for( int i = 1; i < num_processors; i++ ) {
		MPI_Send( &row, 1, MPI_INT, i, DATA_TAG, MPI_COMM_WORLD );	
		row++;
	}
	
	int doneRows = 0;
	// while slaves idle, keep sending work
	while( doneRows < disp_height ) {
			MPI_Recv( &row_colors, disp_width + 1, MPI_INT, MPI_ANY_SOURCE, RESULT_TAG, MPI_COMM_WORLD, &status );
			
				int doneSlave = status.MPI_SOURCE;
				int receivedRow = row_colors[0];
				
				// set the row which has finished computing
				for( int col = 0; col < disp_width; col++ ) {
					coords[receivedRow][col] = row_colors[col+1];
				}
				
				// increment row to send the next one
				doneRows++;
				if( row < disp_height ) {

					// now we can send more data to whichever slave just finished
					MPI_Send( &row, 1, MPI_INT, doneSlave, DATA_TAG, MPI_COMM_WORLD );
					row++;
				}

			}
 
		for( int i = 1; i < num_processors; i++ ) {
			MPI_Send( 0, 0, MPI_INT, i, TERMINATE_TAG, MPI_COMM_WORLD );
			}
		
		// get time stuff here
		gettimeofday( &endTime, NULL );
		finalTime = ( endTime.tv_sec - startTime.tv_sec ) * 1000.0;
		finalTime += ( endTime.tv_usec - startTime.tv_usec ) / 1000.0;
		cout << disp_height << "x" << disp_width << " with " << num_processors << " processors took..." << endl;
		cout << finalTime/1000.0 << " sec" << endl;

		// print mandelbrot to file :)
		for( int x = 0; x < disp_width; x++ ) {
		
			for( int y = 0; y < disp_height; y++ ) {

				fout << coords[x][y] << " ";
			}
			fout << endl;		
		}

		// always close your file after working with it
		fout.close();
}

void slave() {

	// init slave variables
	float scale_real = (float) ( real_max - real_min ) / disp_width;
	float scale_imag = (float) ( imag_max - imag_min ) / disp_height;

	// temporary coordinate and colors array
	complex c;
	int slave_colors[disp_width + 1];
	int row = 0; 
	MPI_Status status;
	int slave_rank;

	// get slave_rank
	MPI_Comm_rank( MPI_COMM_WORLD, &slave_rank );

	// seed a receive 
	MPI_Recv( &row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status );

	while( status.MPI_TAG == DATA_TAG )	{

		// check for terminate tag to exit process
		if( status.MPI_TAG == TERMINATE_TAG ) {
			exit(0);				
		}	
		
		slave_colors[0] = row;

		// do row calculation (only one calculation since it's a row)
		c.imag = imag_min + ( (float) row * scale_imag );
		for( int x = 0; x < disp_width; x++ ) 
			{
				// do column calculation
				c.real = real_min + ( (float) x * scale_real );
				slave_colors[x+1] = cal_pixel( c );		
			}
	
		// now that a row is computed, send the colors and the row back to master
		MPI_Send( slave_colors, disp_width + 1, MPI_INT, 0, RESULT_TAG, MPI_COMM_WORLD );
		MPI_Recv( &row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status );

	}
}

int cal_pixel( complex c) {

	int count, max_iter;
   complex z;
	float temp, lengthsq;
	max_iter = 256;
	z.real = 0.0;
	z.imag = 0.0;
 	count = 0;

	// don't stop until length == 4 (radius == 2 ) or max iterations
	do {
			temp = z.real * z.real - z.imag * z.imag + c.real;
      	z.imag = 2 * z.real * z.imag + c.imag;
		   z.real = temp;
			lengthsq = z.real * z.real + z.imag * z.imag;
			count++;
	} while( ( lengthsq < 4.0 ) && ( count < max_iter ) );

	return count;
}

