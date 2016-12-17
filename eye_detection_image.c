#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cxcore.h>
static CvMemStorage* storage = 0;

static CvHaarClassifierCascade *cascade = 0;

const char* cascade_name = "/usr/share/opencv/haarcascades/haarcascade_eye.xml";

void detect_and_draw(IplImage *);


int main(int argc, char *argv[])
{
	IplImage * frame;

	if(argc < 2) {
		fpritf(stderr, "Error: Usage %s <image file>", argv[0]);
		return -1;
	}

	cascade = cvLoad(cascade_name, 0, 0, 0);
	int c;

	if(!cascade) {
		fprintf(stderr, "ERROR:Could not load classifier cascade\n");
		return -1;
	}

	frame =  cvLoadImage(argv[1], 1);
	storage = cvCreateMemStorage(0);
	detect_and_draw(frame);

	cvWaitKey(0);
	
	return 0;
}

void detect_and_draw( IplImage * img)
{
	int scale = 1;

	IplImage *temp = cvCreateImage(cvSize(img->width/scale, img->height/scale),8,3);
	CvPoint pt1, pt2;

	int i;

	cvClearMemStorage(storage);

	if(cascade) {
		CvSeq *faces = cvHaarDetectObjects(img, cascade, storage, 1.1, 3,
		 CV_HAAR_DO_CANNY_PRUNING, cvSize(5, 5), cvSize(50, 50));

		for( i = 0; i < (faces ? faces->total: 0); i++){

			
			CvRect* r = (CvRect*)cvGetSeqElem(faces, i);
			pt1.x = r->x*scale;
			pt2.x = (r->x + r->width)*scale;
			pt1.y = r->y*scale;
			pt2.y = (r->y + r->height)*scale;

			cvRectangle(img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0);
		}
	}
	
	cvShowImage("result", img);
	cvReleaseImage(&temp);
}


