#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include < opencv/highgui.h >
#include <opencv2/legacy/compat.hpp>

void generar_HistogramaEcualizacion(IplImage* imgGray);
void generar_Histograma(IplImage* imgGray );

void main()	{
	//Declaracion de variables a usarse
	IplImage *imgIn;			// imagen de entrada	
	IplImage *imgGray;			// imagen de entrada en en escala de grises	

	imgIn=cvLoadImage("img1.jpg", 1);		//carga la imagen de entrada		
	imgGray=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);
	cvCvtColor(imgIn,imgGray,CV_BGR2GRAY);			//convierte la img de entrada a img escala de grises
	

	//visualiza la img original en escala de grises
	cvNamedWindow("Imagen",1);
	cvShowImage("Imagen",imgGray);	cvWaitKey(0);

	generar_HistogramaEcualizacion(imgGray);
}



void generar_HistogramaEcualizacion(IplImage* imgGray ){
 

	//CREANDO HISTOGRAMA de 1 dimension                                
    CvHistogram* histograma;//histograma 
	IplImage* histograma_salida=cvCreateImage(cvSize(256,256),IPL_DEPTH_8U, 1); //imagen que se visualizara el histograma
	histograma_salida->origin=IPL_ORIGIN_BL;

	IplImage* img_nueva=cvCreateImage(cvGetSize(imgGray),IPL_DEPTH_8U, 1); //imagen que se visualizara la ecualizacion
	cvCopy(imgGray,img_nueva,0);

	//cvShowImage("hsjs",img_nueva); cvWaitKey(0);


    float max_freq = 0,min_freq = 0;
    float bin_val;//para valores obtenidos del histograma calculado
    int intensidad;//valores obtenidos de histograma calculado, redondeados y normalizados
	int max_freq_hist=0,min_freq_hist=0;
	int nBrillo=256;
	int i;


	//*******************************************************************
	//GENERAR HISTOGRAMA
	
	//Calcula el Histograma de una imagen 8U
    histograma=cvCreateHist(1,&nBrillo,CV_HIST_ARRAY,NULL,1);  
	cvCalcHist(&imgGray,histograma,0,NULL);
    //Se obtiene el valor máximo y minimo del histograma
    cvGetMinMaxHistValue(histograma,&min_freq,&max_freq , 0, 0 );
    
    //histograma con color de fondo negro   
    cvSetZero(histograma_salida);

	//Marcar las frecuencias tomadas del histograma a la imagen del histograma de salida
	int suma=0;
	for(i=0;i<256;i++){  

        bin_val= cvQueryHistValue_1D(histograma,i);
		//Valor de frecuencia
		intensidad= cvRound(bin_val*255/max_freq);
        for(int freq=0;freq<intensidad;freq++)
            ((uchar*)histograma_salida->imageData)[freq*histograma_salida->widthStep+i]=255;

		suma = suma +intensidad;
    }
	printf("suma %d\n\n",suma);
	
	int new_pixel=0, suma2=0;
	CvScalar s;
	for(i=0;i<256;i++){   //Itero en el rango de los valores de brillo
        bin_val= cvQueryHistValue_1D(histograma,i);
		intensidad= cvRound(bin_val*255/max_freq);

		suma2 = suma2 +intensidad;
		printf("intensidad %d \t", intensidad);
		new_pixel=cvRound(suma2*255/suma);			//Se normaliza
		printf("nuevo pixel %d \n", new_pixel);
			
		for(int ancho=0; ancho< imgGray->width;ancho++){
			for (int alto=0; alto<imgGray->height;alto++){
				if((int)(cvGet2D(imgGray,alto,ancho).val[0])==i){
					s.val[0]=new_pixel;
					cvSet2D(img_nueva,alto,ancho,s);
				}
					
			}	
		}			
    }
	
	cvShowImage( "histograma salida", histograma_salida ); cvWaitKey(0);
	cvShowImage("Nueva", img_nueva);cvWaitKey(0);
	generar_Histograma(img_nueva );
}

void generar_Histograma(IplImage* imgGray ){
 

	//CREANDO HISTOGRAMA de 1 dimension                                
    CvHistogram* histograma;//histograma 
	IplImage* histograma_salida=cvCreateImage(cvSize(256,256),IPL_DEPTH_8U, 1); //imagen que se visualizara el histograma
	histograma_salida->origin=IPL_ORIGIN_BL;

                           
    float max_freq = 0,min_freq = 0;
    float bin_val;//para valores obtenidos del histograma calculado
    int intensidad;//valores obtenidos de histograma calculado, redondeados y normalizados
	int max_freq_hist=0,min_freq_hist=0,nBrillo=256;
	//*******************************************************************
	//GENERAR HISTOGRAMA
	
	//Calcula el Histograma de una imagen 8U
    histograma=cvCreateHist(1,&nBrillo,CV_HIST_ARRAY,NULL,1);  
	cvCalcHist(&imgGray,histograma,0,NULL);
    //Se obtiene el valor máximo y minimo del histograma
    cvGetMinMaxHistValue(histograma,&min_freq,&max_freq , 0, 0 );
    //printf("nBrillo: %d \t min: %f\t max:%f",nBrillo,min_freq,max_freq);     
    //histograma con color de fondo negro   
    cvSetZero(histograma_salida);

	//Marcar las frecuencias tomadas del histograma a la imagen del histograma de salida
	for(int i=0;i<256;i++){  

        bin_val= cvQueryHistValue_1D(histograma,i);
		intensidad= cvRound(bin_val*255/max_freq);
        for(int freq=0;freq<intensidad;freq++)
            ((uchar*)histograma_salida->imageData)[freq*histograma_salida->widthStep+i]=255;
    }
	

	cvShowImage( "his", histograma_salida ); cvWaitKey(0);
}
