#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp" 
#include <stdlib.h> 
#include <stdio.h> 
#include "opencv2/opencv.hpp" 
#include < opencv/highgui.h > 
#include <opencv2/legacy/compat.hpp> 
#include "opencv2/objdetect/objdetect.hpp"


using namespace std;    
using namespace cv;

void main(){
	//Declaracion de variables a usarse
	IplImage *imgIn;			// imagen de entrada	
	IplImage *imgGray;			// imagen de entrada en en escala de grises	
	IplImage *imgIn2;			// imagen sin nada

	imgIn=cvLoadImage("img1.jpg", 1);		//carga la imagen de entrada		
	imgGray=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);
	cvCvtColor(imgIn,imgGray,CV_BGR2GRAY);			//convierte la img de entrada a img escala de grises
	
	imgIn2= cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);		//Se inicializa una imagen en blanco/sin nada

	//visualiza la img original en escala de grises
	cvNamedWindow("Imagen",1);
	//cvShowImage("Imagen",imgGray);	cvWaitKey(0);
	cvWaitKey(0);

	HOGDescriptor hog;
	Mat Hogfeat;
	
	vector<float> features;
	vector<Point>locs;

	hog.compute(imgGray,features,Size(32,32),Size(0,0),locs);
	cout<<features.size()<<endl;;
	
	Hogfeat.create(features.size(),1,CV_32FC1);


	for(int i=0;i<features.size();i++)
	{
	  Hogfeat.at<float>(i,0)=features.at(i);
	}

	hog.blockSize=Size(16,16);
	hog.cellSize=Size(4,4);
	hog.blockStride=Size(8,8);



}