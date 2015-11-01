#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include < opencv/highgui.h >
#include <opencv2/legacy/compat.hpp>


void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr );
void generar_magnitud(IplImage* imgGray, IplImage* img_Re2,IplImage* img_Im2,IplImage* img_Mascara);



void main(){

	//Declaracion de variables a usarse
	IplImage *imgIn;			// imagen de entrada	
	IplImage *imgGray;			// imagen de entrada en en escala de grises	
	int i,j;


	//CREANDO HISTOGRAMA de 1 dimension                                
    CvHistogram* histograma;//histograma 

	IplImage* histograma_salida=cvCreateImage(cvSize(256,256),IPL_DEPTH_8U, 1); //imagen que se visualizara el histograma
	histograma_salida->origin=IPL_ORIGIN_BL;

	IplImage *imgDesplazamiento;
	IplImage* histogramaSalida_Alarg=cvCreateImage(cvSize(256,256),IPL_DEPTH_8U, 1); //imagen que se visualizara el histograma
	histogramaSalida_Alarg->origin=IPL_ORIGIN_BL;

    int nBrillo= 256;                            
    float max_freq = 0,min_freq = 0,max_freq_hist = 0,min_freq_hist = 0;
    float bin_val;//para valores obtenidos del histograma calculado
    int intensidad;//valores obtenidos de histograma calculado, redondeados y normalizados
	
	

	imgIn=cvLoadImage("helicoptero.jpg", 1);		//carga la imagen de entrada		
	imgGray=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);
	cvCvtColor(imgIn,imgGray,CV_BGR2GRAY);  //convierte la img de entrada a img escala de grises
		
	//visualiza la img original en escala de grises
	cvNamedWindow("Imagen",1);
	cvShowImage("Imagen",imgGray);
	cvWaitKey(0);
	
	//*******************************************************************
	//GENERAR HISTOGRAMA
	
	imgDesplazamiento=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);

    //Calcula el Histograma de una imagen 8U
    histograma=cvCreateHist(1,&nBrillo,CV_HIST_ARRAY,NULL,1);
    cvCalcHist(&imgGray,histograma,0,NULL);
		
    //Se obtiene el valor máximo y minimo del histograma
    cvGetMinMaxHistValue(histograma,&min_freq,&max_freq , 0, 0 );
    printf("nBrillo: %d \t min: %f\t max:%f",nBrillo,min_freq,max_freq);     
    //histograma con color de fondo negro   
    cvSetZero(histograma_salida);

	//Marcar las frecuencias tomadas del histograma a la imagen del histograma de salida
	int  flag=0;
	for(int i=0;i<256;i++){  

        bin_val= cvQueryHistValue_1D(histograma,i);
		intensidad= cvRound(bin_val*255/max_freq);
        for(int freq=0;freq<intensidad;freq++)
            ((uchar*)histograma_salida->imageData)[freq*histograma_salida->widthStep+i]=255;
		if (intensidad!=0 && flag==0) {min_freq_hist=i; flag=1;}
		if (intensidad!=0 && flag==1) max_freq_hist=i;
    }

	printf("\nnBrillo: %d \t min: %.4f\t max:%.4f",nBrillo,min_freq_hist,max_freq_hist) ;
	 
	
	//Se define el desplazamiento
	int offsetmax = 255-max_freq_hist;
	int offsetmin = -min_freq_hist;

	for(int i=0;i<imgGray->height;i++){
        for(int j=0;j<imgGray->width;j++){
			((uchar*)(imgDesplazamiento->imageData))[i*imgDesplazamiento->widthStep+j]= (((uchar*)(imgGray->imageData))[i*imgGray->widthStep+j]+offsetmax);
			//((uchar*)(imgDesplazamiento->imageData))[i*imgDesplazamiento->widthStep+j]= (((uchar*)(imgGray->imageData))[i*imgGray->widthStep+j]+offsetmin);
	
		}
	}

	
	cvShowImage( "Histograma", histograma_salida ); cvWaitKey(0);
	cvShowImage( "Imagen_Compresion", imgDesplazamiento ); cvWaitKey(0);

	//Calcula el Histograma de una imagen 8U
   // histograma=cvCreateHist(1,&nBrillo,CV_HIST_ARRAY,NULL,1);
    cvCalcHist(&imgDesplazamiento,histograma,0,NULL);
 
	
    //Se obtiene el valor máximo y minimo del histograma
    cvGetMinMaxHistValue(histograma,&min_freq,&max_freq , 0, 0 );
    printf("nBrillo: %d \t min: %f\t max:%f",nBrillo,min_freq,max_freq);     
    //histograma con color de fondo negro   
    cvSetZero(histograma_salida);

	//Marcar las frecuencias tomadas del histograma a la imagen del histograma de salida
	 for(int i=0;i<256;i++){  

        bin_val= cvQueryHistValue_1D(histograma,i);
		intensidad= cvRound(bin_val*255/max_freq);
        for(int freq=0;freq<intensidad;freq++)
            ((uchar*)histograma_salida->imageData)[freq*histograma_salida->widthStep+i]=255;
    }



    cvShowImage( "Histograma2", histograma_salida ); cvWaitKey(0);

	
}


