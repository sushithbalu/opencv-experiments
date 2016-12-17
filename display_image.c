
#include <stdio.h>
#include <highgui.h>
#include <cv.h>

int main( int argc, char** argv ) {

	if(argc < 2) {
		printf("Usage: %s <image file>\n", argv[0]);
		return -1;
	}

	IplImage* img = cvLoadImage( argv[1],-1);

	cvNamedWindow( "image_disp", CV_WINDOW_AUTOSIZE );
	cvShowImage( "image_disp", img );

	cvWaitKey(0);

	//free memory
	cvReleaseImage( &img );
	cvDestroyWindow( "image_disp" );
	return 0;
}
