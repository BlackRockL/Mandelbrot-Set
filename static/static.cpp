#include <iostream>
#include <mpi.h>
using namespace std;


// constants
const int disp_width = 50;
const int disp_height = 50;
int row_work;
const int real_min = -2;
const int real_max = 2;
const int imag_min = -2;
const int imag_max = 2;
const int terminate_var = 0;

// data tags
#define DATA_TAG 1
#define X_TAG 2
#define Y_TAG 3

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
	int rank = 0;

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
	int x_coord, y_coord;
	int color;

	cout << "P2\n";
	cout << disp_width << " " << disp_height << "\n";
	cout << "256\n";

	// figure out how many processors we have
	MPI_Comm_size( MPI_COMM_WORLD, &num_processors );
	row_work = disp_height/num_processors-1;

	// send row number to each process
	for( int i = 1; i < num_processors; i++ ) {
		MPI_Send( &row, 1, MPI_INT, i, DATA_TAG, MPI_COMM_WORLD );	
		row += row_work;
	}

	for( int i = 32; i < disp_width * disp_height; i++ ) {
	
		MPI_Recv( &x_coord, 1, MPI_INT, MPI_ANY_SOURCE, X_TAG, MPI_COMM_WORLD, &status );	
		MPI_Recv( &y_coord, 1, MPI_INT, MPI_ANY_SOURCE, Y_TAG, MPI_COMM_WORLD, &status );	
		MPI_Recv( &color, 1, MPI_INT, MPI_ANY_SOURCE, DATA_TAG, MPI_COMM_WORLD, &status );	
		coords[y_coord][x_coord] = color;
		
	}

	for( int i = 0; i < disp_height; i++ ) {
		for( int j = 0; j < disp_width; j++ ) {
			cout << coords[i][j] << " ";
		}
		cout << endl;
	}

}

void slave() {

	// init slave variables
	float scale_real = (float) ( real_max - real_min ) / disp_width;
	float scale_imag = (float) ( imag_max - imag_min ) / disp_height;
	int rank;
	complex c;
	int color;

	// temporary coordinate and colors array
	int row = 0;
	MPI_Status status;

	// get rank for debugging
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );

	// receive row ids
	MPI_Recv( &row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
	
	// we got the row earlier so start outer loop at row
	for( int y = row; y < row+row_work; y++ ) {
		// inner loop goes from 0 to width of image
		for( int columns = 0; columns < disp_width; columns++ ) {
			// do math stuff
			c.real = real_min + ( (float) columns * scale_real );
			c.imag = imag_min + ( (float) y * scale_imag );
			color = cal_pixel( c );
			// send coordinates and colors back to master
			MPI_Send( &columns, 1, MPI_INT, 0, X_TAG, MPI_COMM_WORLD );
			MPI_Send( &y, 1, MPI_INT, 0, Y_TAG, MPI_COMM_WORLD );
	      MPI_Send( &color, 1, MPI_INT, 0, DATA_TAG, MPI_COMM_WORLD );
		}
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


