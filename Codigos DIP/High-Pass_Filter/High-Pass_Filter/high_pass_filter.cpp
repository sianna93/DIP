#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include < opencv/highgui.h >


void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr );

void main(){

	//Declaracion de variables a usarse
	IplImage *imgIn;			// imagen de entrada	
	IplImage *imgGray;			// imagen de entrada en en escala de grises	
	IplImage *img_Re,*img_Im;	// resultado img real e imaginario de la tranformada de fourier 
	IplImage *img_ReInv, *img_Mascara, *img_Mascara1;		// pasar la img parte real a 8 bits	
	IplImage *img_Re2,*img_Im2, *img_Re3,*img_Im3,*img_Re4,*img_Im4;		// pasar la img parte real a 8 bits	
	IplImage *mascaraRuido;
	IplImage *fft,*fft2;				// almacena la transformada de fourier
	
	double m, M;
	float r, freq;
	
	CvPoint centro;
	int i,j,bandaIzqA,bandaIzqB,bandaDerA,bandaDerB;


	CvMat* kernel=0;	

	imgIn=cvLoadImage("img_formaT.jpg", 1);		//carga la imagen de entrada		
	imgGray=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);
	cvCvtColor(imgIn,imgGray,CV_BGR2GRAY);  //convierte la img de entrada a img escala de grises
		
	//visualiza la img original en escala de grises
	cvNamedWindow("Imagen",1);
	cvShowImage("Imagen",imgGray);
	cvWaitKey(0);

	img_Re= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1); //Crea la img_RE para la parte real con profundidad de 64, 1 canal.
	img_Im= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1);//Crea la img_Im para la parte imaginaria con profundidad de 64, 1 canal.
	fft= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 2); //Crea una Img para la union de las partes real e imaginaria de la Transf. de Fourier, profundidad de 64, 2 canales.

	fft2= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 2); //Crea una Img para la union de las partes real e imaginaria de la Transf. de Fourier, profundidad de 64, 2 canales.
	img_ReInv= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_8U, 1); //Creamos una img_Re2 para la parte real con profundidad de 8, 1 canal.
	img_Mascara= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_8U, 1); //Creamos una img_Re2 para la parte real con profundidad de 8, 1 canal.
		
	img_Re2= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1); //Creamos una img_Re2 para la parte real con profundidad de 8, 1 canal.
	img_Im2= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1);//Crea la img_Im para la parte imaginaria con profundidad de 64, 1 canal.
	
	
	img_Re3= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1); //Crea la img_RE para la parte real con profundidad de 64, 1 canal.
	img_Im3= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1);//Crea la img_Im para la parte imaginaria con profundidad de 64, 1 canal.
		
	img_Re4= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1); //Crea la img_RE para la parte real con profundidad de 64, 1 canal.
	img_Im4= cvCreateImage(cvGetSize(imgGray), IPL_DEPTH_64F, 1);//Crea la img_Im para la parte imaginaria con profundidad de 64, 1 canal.
	

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
	cvSplit( fft,img_Re2,img_Im2, 0, 0 );
	cvSplit( fft,img_Re3,img_Im3, 0, 0 );

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
	cvShowImage("Magnitud", img_Mascara);cvWaitKey(0);

	
	//FILTRO PASO ALTO
	for(i = 0; i < img_Re->height; i++){
		for(j = 0; j < img_Re->width; j++){
			if(((uchar*)img_Re->imageData)[i*img_Re->widthStep+j]>65){  // Se establece una frecuencia de corte , y menores a ellas es eliminado
				//Modificando las frecuencias tanto en real como imaginaria
				cvSet2D(img_Re3,i,j,cvScalar(0,0,0,0));
				cvSet2D(img_Im3,i,j,cvScalar(0,0,0,0));
				//((uchar*)img_Re3->imageData)[i*img_Re3->widthStep+j]=0;
				//((uchar*)img_Im3->imageData)[i*img_Im3->widthStep+j]=0;
			}
		}
	}


	/*Combina las imagenes para juntar la parte real a la parte imaginaria*/	
	cvMerge(img_Re3,img_Im3, 0, 0,fft2);

	/*Aplica La Transformada Inversa de Fourier a la imagen que contiene las parte real y la imaginaria*/
	cvDFT(fft2,fft2,CV_DXT_INV_SCALE,0);

	/*Separa la parte real y la parte imaginaria de la imagen tranformada de Inversa de Fourier*/
	cvSplit( fft2,img_Re4,img_Im4, 0, 0 );
	cvSplit( fft2,img_Re2,img_Im2, 0, 0 );

	//Regresamos la caracteristica original a la imagen dandole 8 como profundidad.
	cvScale(img_Re4,img_ReInv,1.0,0.0);

	//Se muestra la imagen filtrada
	cvShowImage("Imagen filtrada", img_ReInv);cvWaitKey(0);


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
	cvShowImage("Magnitud_CON FLITRO", img_Mascara);cvWaitKey(0);

	
	
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