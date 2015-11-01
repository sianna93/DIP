#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include < opencv/highgui.h >
#include <opencv2/legacy/compat.hpp>

//LLAMADA DE FUNCIONES
void Filtro_PasoBajo(IplImage* imgGray, IplImage* img_FPB, int freq_corte);
void Compresion_Histograma(IplImage* imgGray, IplImage* img_CompHist);
void Alargamiento_Histograma(IplImage* imgGray, IplImage* img_AlargHist,int nBrillo);
void generar_Histograma(IplImage* imgGray,int *max_freq_hist,int*min_freq_hist,int nBrillo, char *nameHist );

void main(){

	//Declaracion de variables a usarse
	IplImage *imgIn;			// imagen de entrada	
	IplImage *imgGray;			// imagen de entrada en en escala de grises	
	IplImage *img_FPB;
	IplImage *img_CompHist;
	IplImage *Sub_Gray_HistComp;
	IplImage *img_AlargHist;

	int nBrillo= 256; 
	int max_freq_hist = 0,min_freq_hist = 0;

	imgIn=cvLoadImage("img_formaT.jpg", 1);		//carga la imagen de entrada		
	imgGray=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);
	cvCvtColor(imgIn,imgGray,CV_BGR2GRAY);  //convierte la img de entrada a img escala de grises
	

	//visualiza la img original en escala de grises
	cvNamedWindow("Imagen",1);
	cvShowImage("Imagen",imgGray);	cvWaitKey(0);

	generar_Histograma(imgGray,&max_freq_hist,&min_freq_hist,nBrillo, "Hist-ImgGray" );

	//PARTE 1 - FILTRO PASO BAJO
	img_FPB= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_8U, 1); //Creamos una img_Re2 para la parte real con profundidad de 8, 1 canal.
	int freq_corte= 45;

	//Se aplica el filtro
	Filtro_PasoBajo(imgGray,img_FPB,freq_corte);
	//Se muestra la imagen
	cvShowImage("IMG- F.PASO BAJO",img_FPB);cvWaitKey(0);

	//PARTE 2 - COMPRESION DE HISTOGRAMA
	//Se inicializa al imagen aplicada el efecto de realzado 
	img_CompHist=cvCreateImage(cvGetSize(imgGray),IPL_DEPTH_8U,1);
	Compresion_Histograma(img_FPB, img_CompHist);

	//PARTE 3 - Substraccion imagen inicial a escalas de grises y la imagen luego del realzado de compresion de histograma
	Sub_Gray_HistComp=cvCreateImage(cvGetSize(imgGray),IPL_DEPTH_8U,1);
	cvSub(imgGray,img_CompHist,Sub_Gray_HistComp,0);
	
	//Se muestra la imagen
	cvShowImage("Subtraccion",Sub_Gray_HistComp);cvWaitKey(0);

	generar_Histograma(Sub_Gray_HistComp,&max_freq_hist,&min_freq_hist,nBrillo,"Hist_ImgAntesSub" );

	//PARTE 4 - ALARGAMIENTO DE HISTOGRAMA

	//Se inicializa al imagen aplicada el efecto de realzado 
	img_AlargHist=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);
	Alargamiento_Histograma(Sub_Gray_HistComp, img_AlargHist,256);	

	//cvSaveImage("img_substraccion1.jpg",Sub_Gray_HistComp,0);
	//Se libera memoria
	cvReleaseImage(&imgGray); cvReleaseImage(&img_FPB);cvReleaseImage(&img_CompHist);
	cvReleaseImage(&Sub_Gray_HistComp);cvReleaseImage(&img_AlargHist);
}

void Filtro_PasoBajo(IplImage* imgGray, IplImage* img_FPB, int freq_corte){
	//Declaracion de variables a usarse
	IplImage *img_Re,*img_Im;	// resultado img real e imaginario de la tranformada de fourier 
	IplImage *img_Mascara;		// pasar la img parte real a 8 bits	
	IplImage *img_Re2,*img_Im2,*img_Re3,*img_Im3;		// pasar la img parte real a 8 bits	
	IplImage *fft,*fft2;				// almacena la transformada de fourier

	int i,j;

	img_Re= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1); //Crea la img_RE para la parte real con profundidad de 64, 1 canal.
	img_Im= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1);//Crea la img_Im para la parte imaginaria con profundidad de 64, 1 canal.
	fft= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 2); //Crea una Img para la union de las partes real e imaginaria de la Transf. de Fourier, profundidad de 64, 2 canales.

	fft2= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 2); //Crea una Img para la union de las partes real e imaginaria de la Transf. de Fourier, profundidad de 64, 2 canales.
	img_Mascara= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_8U, 1); //Creamos una img_Re2 para la parte real con profundidad de 8, 1 canal.
		
	img_Re2= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1); //Creamos una img_Re2 para la parte real con profundidad de 8, 1 canal.
	img_Im2= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1);//Crea la img_Im para la parte imaginaria con profundidad de 64, 1 canal.

	img_Re3= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1); //Creamos una img_Re2 para la parte real con profundidad de 8, 1 canal.
	img_Im3= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1);//Crea la img_Im para la parte imaginaria con profundidad de 64, 1 canal.
	
	/*Cambia las caracteristicas de la imgGray de 8 bits/pixel a img_Re de 64 bits/pixel de profundidad*/	
	cvScale(imgGray,img_Re,1.0,0.0);

	/*Setea todos los valores a 0 de la imagen imaginaria*/
	cvSetZero(img_Im);
		
	/*Combina las imagenes para juntar la parte real a la parte imaginaria*/	
	cvMerge(img_Re,img_Im, 0, 0,fft);

	/*Aplica Fourier a la imagen que contiene las parte real y la imaginaria*/
	cvDFT(fft,fft,CV_DXT_FORWARD,0);

	/*Separa la parte real y la parte imaginaria de la imagen tranformada de Fourier*/
	cvSplit( fft,img_Re,img_Im, 0, 0 );	
	cvSplit( fft,img_Re2,img_Im2, 0, 0 );		//Se guarda en esas variables para poder hacer los calculos necesarios para la magnitud.
	cvSplit( fft,img_Re3,img_Im3, 0, 0 );		//Se guarda en esas variables para poder hacer los calculos necesarios para la magnitud.

	//generar_magnitud(imgGray, img_Re2,img_Im2,img_Mascara);	

	
	//FILTRO PASO Bajo
	for(i = 0; i < img_Re->height; i++){
		for(j = 0; j < img_Re->width; j++){
			//if(cvGet2D(img_Re,i,j).val[0]>freq_corte){
			if(((uchar*)img_Re->imageData)[i*img_Re->widthStep+j]>freq_corte){  // Se establece una frecuencia de corte , y menores a ellas es eliminado
				//Modificando las frecuencias tanto en real como imaginaria
				//cvSet2D(img_Re3,i,j,cvScalar(0,0,0,0));
				//cvSet2D(img_Im3,i,j,cvScalar(0,0,0,0));
				((uchar*)img_Re3->imageData)[i*img_Re3->widthStep+j]=0;
				((uchar*)img_Im3->imageData)[i*img_Im3->widthStep+j]=0;
			}
		}
	}


	/*Combina las imagenes para juntar la parte real a la parte imaginaria*/	
	cvMerge(img_Re3,img_Im3, 0, 0,fft2);

	/*Aplica La Transformada Inversa de Fourier a la imagen que contiene las parte real y la imaginaria*/
	cvDFT(fft2,fft2,CV_DXT_INV_SCALE,0);

	/*Separa la parte real y la parte imaginaria de la imagen tranformada de Inversa de Fourier*/
	cvSplit( fft2,img_Re3,img_Im3, 0, 0 );
	cvSplit( fft2,img_Re2,img_Im2, 0, 0 ); //Se guarda en esas variables para poder hacer los calculos necesarios para la magnitud.

	//Regresamos la caracteristica original a la imagen dandole 8 como profundidad.
	cvScale(img_Re3,img_FPB,1.0,0.0);

	//Se muestra la imagen filtrada
	//cvShowImage("Imagen filtrada", img_FPB);cvWaitKey(0);

	//generar_magnitud(imgGray, img_Re2,img_Im2,img_Mascara);	
	
	//Se libera memoria
	cvReleaseImage(&img_Re); cvReleaseImage(&img_Im);
	cvReleaseImage(&img_Re2); cvReleaseImage(&img_Im2);
	cvReleaseImage(&img_Re3); cvReleaseImage(&img_Im3);
	cvReleaseImage(&fft); cvReleaseImage(&fft2);
	cvReleaseImage(&img_Mascara);
	
}

void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr )
{
    CvMat * tmp;
    CvMat q1stub, q2stub;
    CvMat q3stub, q4stub;
    CvMat d1stub, d2stub;
    CvMat d3stub, d4stub;
    CvMat * q1, * q2, * q3, * q4;
    CvMat * d1, * d2, * d3, * d4;

    CvSize size = cvGetSize(src_arr);
    CvSize dst_size = cvGetSize(dst_arr);
    int cx, cy;

    if(dst_size.width != size.width || 
       dst_size.height != size.height){
        cvError( CV_StsUnmatchedSizes, "cvShiftDFT", "Source and Destination arrays must have equal sizes", __FILE__, __LINE__ );   
    }

	tmp = cvCreateMat(size.height/2, size.width/2, cvGetElemType(src_arr));
    if(src_arr==dst_arr){
        tmp = cvCreateMat(size.height/2, size.width/2, cvGetElemType(src_arr));
    }
    
    cx = size.width/2;
    cy = size.height/2; // image center

    q1 = cvGetSubRect( src_arr, &q1stub, cvRect(0,0,cx, cy) );
    q2 = cvGetSubRect( src_arr, &q2stub, cvRect(cx,0,cx,cy) );
    q3 = cvGetSubRect( src_arr, &q3stub, cvRect(cx,cy,cx,cy) );
    q4 = cvGetSubRect( src_arr, &q4stub, cvRect(0,cy,cx,cy) );
    d1 = cvGetSubRect( src_arr, &d1stub, cvRect(0,0,cx,cy) );
    d2 = cvGetSubRect( src_arr, &d2stub, cvRect(cx,0,cx,cy) );
    d3 = cvGetSubRect( src_arr, &d3stub, cvRect(cx,cy,cx,cy) );
    d4 = cvGetSubRect( src_arr, &d4stub, cvRect(0,cy,cx,cy) );

    if(src_arr!=dst_arr){
        if( !CV_ARE_TYPES_EQ( q1, d1 )){
            cvError( CV_StsUnmatchedFormats, "cvShiftDFT", "Source and Destination arrays must have the same format", __FILE__, __LINE__ ); 
        }
        cvCopy(q3, d1, 0);
        cvCopy(q4, d2, 0);
        cvCopy(q1, d3, 0);
        cvCopy(q2, d4, 0);
    }
    else{
        cvCopy(q3, tmp, 0);
        cvCopy(q1, q3, 0);
        cvCopy(tmp, q1, 0);
        cvCopy(q4, tmp, 0);
        cvCopy(q2, q4, 0);
        cvCopy(tmp, q2, 0);
    }
}

void generar_magnitud(IplImage* imgGray, IplImage* img_Re2,IplImage* img_Im2,IplImage* img_Mascara){

	double m,M;	

	//Aplica la ecuacion para generar la imagen magnitud --> magnitud=sqrt(img_Re*img_Re + img_Im*img_Im)
	cvPow( img_Re2, img_Re2, 2); //img_Re^2
	cvPow( img_Im2, img_Im2, 2);//img_Im^2
	cvAdd( img_Re2, img_Im2, img_Re2);// abs_ifft = iimg_Re + iimg_Im
	cvPow( img_Re2, img_Re2, 0.5); //sqrt(abs_ifft)

	//Hacemos remapeo logaritmico para mejorar la visualizacion de los coeficientes de Fourier
	cvLog(img_Re2,img_Re2); // logaritmo de la imagen img_Re
	cvPow(img_Re2,img_Re2,2);  //img_Re^2

	//Reorganizamos los cuadrantes de la imagen de Fourier tal que el origen este en el centro de la imagen (matrices dst & src de igual tamano & tipo)
	cvShiftDFT( img_Re2, img_Re2 );
	cvMinMaxLoc( img_Re2, &m, &M, NULL, NULL, NULL );

	//Regresamos la caracteristica original a la imagen dandole 8 como profundidad.
	cvScale(img_Re2,img_Mascara ,323/(M-m),323*(-m)/(M-m));

	//Se muestra la magnitud
	//cvShowImage("Magnitud", img_Mascara);cvWaitKey(0);
}


void generar_Histograma(IplImage* imgGray,int *max_freq_hist,int*min_freq_hist,int nBrillo, char *nameHist ){
 

	//CREANDO HISTOGRAMA de 1 dimension                                
    CvHistogram* histograma;//histograma 
	IplImage* histograma_salida=cvCreateImage(cvSize(256,256),IPL_DEPTH_8U, 1); //imagen que se visualizara el histograma
	histograma_salida->origin=IPL_ORIGIN_BL;

                           
    float max_freq = 0,min_freq = 0;
    float bin_val;//para valores obtenidos del histograma calculado
    int intensidad;//valores obtenidos de histograma calculado, redondeados y normalizados

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
	int  flag=0;
	for(int i=0;i<256;i++){  

        bin_val= cvQueryHistValue_1D(histograma,i);
		intensidad= cvRound(bin_val*255/max_freq);
        for(int freq=0;freq<intensidad;freq++)
            ((uchar*)histograma_salida->imageData)[freq*histograma_salida->widthStep+i]=255;
		if (intensidad!=0 && flag==0) {*min_freq_hist=i; flag=1;}
		if (intensidad!=0 && flag==1) *max_freq_hist=i;
    }
	printf("\nnBrillo: %d \t min: %d\t max:%d",nBrillo,*min_freq_hist,*max_freq_hist) ;

	cvShowImage( nameHist, histograma_salida ); cvWaitKey(0);
}

void Compresion_Histograma(IplImage* imgGray, IplImage* img_CompHist){

	//Declaracion de variables a usarse
	int i,j;	
	int nBrillo= 256; 
	int max_freq_hist = 0,min_freq_hist = 0;
	
	//*******************************************************************
	//GENERAR HISTOGRAMA 

	
	//Se crea el histograma
	generar_Histograma(imgGray,&max_freq_hist,&min_freq_hist,nBrillo,"Hist_ImgAntesComp" );

	//Operaciones para compresion
	float medio = max_freq_hist-min_freq_hist;
	float shrinkMin = 0,shrinkMax=40;//float shrinkMin = (256*0.5)-medio,shrinkMax=(256*0.5)+medio;//float shrinkMin = 30,shrinkMax=120//
	float operacion = (shrinkMax-shrinkMin)/(max_freq_hist-min_freq_hist);

	for(int i=0;i<imgGray->height;i++){
        for(int j=0;j<imgGray->width;j++){
			((uchar*)(img_CompHist->imageData))[i*img_CompHist->widthStep+j]= (operacion*(((uchar*)(imgGray->imageData))[i*imgGray->widthStep+j]-min_freq_hist)+shrinkMin);
		}

	}
	
	//Se muestra la imagen con compresion de histograma.
	cvShowImage( "Imagen_Compresion", img_CompHist ); cvWaitKey(0);

	//Se crea el histograma
	generar_Histograma(img_CompHist,&max_freq_hist,&min_freq_hist,nBrillo,"Hist_ImgDdespComp" );

}



void Alargamiento_Histograma(IplImage* imgGray, IplImage* img_AlargHist,int nivel){

	int i,j;


	//CREANDO HISTOGRAMA de 1 dimension                                
    CvHistogram* histograma;//histograma 

	IplImage* histograma_salida=cvCreateImage(cvSize(256,256),IPL_DEPTH_8U, 1); //imagen que se visualizara el histograma
	histograma_salida->origin=IPL_ORIGIN_BL;

	//IplImage *imgAlargamiento;
	IplImage* histogramaSalida_Alarg=cvCreateImage(cvSize(256,256),IPL_DEPTH_8U, 1); //imagen que se visualizara el histograma
	histogramaSalida_Alarg->origin=IPL_ORIGIN_BL;

    int nBrillo= 256;                            
    float max_freq = 0,min_freq = 0,max_freq_hist = 0,min_freq_hist = 0;
    float bin_val;//para valores obtenidos del histograma calculado
    int intensidad;//valores obtenidos de histograma calculado, redondeados y normalizados


		
	//*******************************************************************
	//GENERAR HISTOGRAMA
	
	//imgAlargamiento=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);

    //Calcula el Histograma de una imagen 8U
    histograma=cvCreateHist(1,&nBrillo,CV_HIST_ARRAY,NULL,1);
    cvCalcHist(&imgGray,histograma,0,NULL);
 
	
    //Se obtiene el valor máximo y minimo del histograma
    cvGetMinMaxHistValue(histograma,&min_freq,&max_freq , 0, 0 );
    printf("nBrillo: %d \t min: %.4f\t max:%.4f",nBrillo,min_freq,max_freq);     
    //histograma con color de fondo negro   
     cvSetZero(histograma_salida);

	//Marcar las frecuencias tomadas del histograma a la imagen del histograma de salida
	 int flag=0;
	 for(int i=0;i<256;i++){  
		 
        bin_val= cvQueryHistValue_1D(histograma,i);
		intensidad= cvRound(bin_val*255/max_freq);
        for(int freq=0;freq<intensidad;freq++)
            ((uchar*)histograma_salida->imageData)[freq*histograma_salida->widthStep+i]=255;
		if (intensidad!=0 && flag==0) {min_freq_hist=i; flag=1;}
		if (intensidad!=0 && flag==1) max_freq_hist=i;
		
    }

	 printf("\nnBrillo: %d \t min: %.4f\t max:%.4f",nBrillo,min_freq_hist,max_freq_hist) ;

	 int max=255, min=0;

	 for(int i=0;i<imgGray->height;i++){
		 for(int j=0;j<imgGray->width;j++){
				 ((uchar*)(img_AlargHist->imageData))[i*img_AlargHist->widthStep+j]= (((((uchar*)(imgGray->imageData))[i*imgGray->widthStep+j]-min_freq_hist)/(max_freq_hist-min_freq_hist))*(max-min))+min;
			}
	}

	
	cvShowImage( "Histograma", histograma_salida ); cvWaitKey(0);
	cvShowImage( "Imagen_Alarg", img_AlargHist ); cvWaitKey(0);

	//Calcula el Histograma de una imagen 8U
   // histograma=cvCreateHist(1,&nBrillo,CV_HIST_ARRAY,NULL,1);
    cvCalcHist(&img_AlargHist,histograma,0,NULL);
 
	
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