#include <iostream>
#include <sys/time.h>
#include <fstream>

using namespace std;

// constants
const int disp_width = 30000;
const int disp_height = 30000;
const int real_min = -2;
const int real_max = 2;
const int imag_min = -2;
const int imag_max = 2;

// complex structure holds real/imaginary part of num
struct complex {
	float real;
	float imag;
};

// function prototypes
int cal_pixel( complex );
void display( int * );

// main crap
int main() 
{	
	// init variables
	timeval start, end;
	double runTime;	
	ofstream fout;
	fout.open( "image.PPM" );

	fout << "P2\n";
	fout << disp_width << " " << disp_height << "\n";
	fout << "256\n";

	int color = 0;
	int array[disp_width][disp_height];	

	// initalize complex num
	complex c;
	c.real = 0.0;
	c.imag = 0.0;
	
	// scale complex num	
	float scale_real = (float) ( real_max - real_min ) / disp_width;
	float	scale_imag = (float) ( imag_max - imag_min ) / disp_height;

	// start timer
	gettimeofday( &start, NULL );

	for( int x = 0; x < disp_width; x++ )
		{
			for( int y = 0; y < disp_height; y++ )
				{
					c.real = real_min + ( (float) x * scale_real );
					c.imag = imag_min + ( (float) y * scale_imag );
					color = cal_pixel( c );
					array[x][y] = color;
				}
		}
	
	// stop timer
	gettimeofday( &end, NULL );

	// calc TIme
	runTime = ( end.tv_sec - start.tv_sec ) * 1000.0;
	runTime += ( end.tv_usec - start.tv_usec ) / 1000.0;
   cout << disp_width << " x " << disp_height << " took..." << endl;
	cout << runTime/1000.0 << " sec" << endl;

	for( int x = 0; x < disp_width; x++ ) 
		{
			for( int y = 0; y < disp_height; y++ )
				{
//					fout << array[y][x] << " ";
				}
//			fout << endl;
		}	
	return 0;
}

// functions
int cal_pixel( complex c )
{
	int count, max_iter;
	complex z;
	float temp, lengthsq;
	max_iter = 256;
	z.real = 0.0;
	z.imag = 0.0;
	count = 0;

	// don't stop until length == 4 ( radius = 2 ) or max iterations
	do {
		temp = z.real * z.real - z.imag * z.imag + c.real;
		z.imag = 2 * z.real * z.imag + c.imag;
		z.real = temp;
		lengthsq = z.real * z.real + z.imag * z.imag;
		count++;		
	} while ( ( lengthsq < 4.0 ) && ( count < max_iter ) );
	
	return count;
}

