#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include < opencv/highgui.h >
#include <time.h>

void RuidoSal(IplImage* img);
void RuidoPimienta(IplImage* img);
void RuidoSalyPimienta(IplImage* img);
void RuidoUniforme(IplImage* img, IplImage * imgF);
void RuidoGaussiano(IplImage* img);

void FiltroOrden(IplImage* img, IplImage* imgFiltrada,int operacion);
void ordenarArr(double arr[9]);
double CalculoPromedio(double arr[9],int t);

double CalculoMedia();
double CalculoVarianza(double media);

void main(){
	//Declaracion de variables a usarse
	IplImage *imgIn;			// imagen de entrada

	imgIn=cvLoadImage("helicoptero.jpg", 1);		//carga la imagen de entrada	
	//visualiza la img original en escala de grises
	cvNamedWindow("Imagen",1);
	cvShowImage("Imagen",imgIn);	cvWaitKey(0);
	
	//Ruido Sal
	IplImage *imgSal= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 3);
	cvCopy(imgIn,imgSal,0);
	RuidoSal(imgSal);
	cvShowImage("ImgSal", imgSal); cvWaitKey(0);

	//Filtro Mínimo
	IplImage* imgFiltMinimo= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 3);
	FiltroOrden(imgSal, imgFiltMinimo,3);
	cvShowImage("ImgFiltroMinimo", imgFiltMinimo); cvWaitKey(0);

	//Ruido Pimienta
	IplImage *imgPimienta= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 3);
	cvCopy(imgIn,imgPimienta,0);
	RuidoPimienta(imgPimienta);
	cvShowImage("ImgPimienta", imgPimienta); cvWaitKey(0);

	//Filtro Máximo
	IplImage* imgFiltMaximo= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 3);
	FiltroOrden(imgPimienta, imgFiltMaximo,2);
	cvShowImage("ImgFiltroMaximo", imgFiltMaximo); cvWaitKey(0);

	//Ruido Sal y Pimienta
	IplImage *imgSalyPimienta= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 3);
	cvCopy(imgIn,imgSalyPimienta,0);
	RuidoSalyPimienta(imgSalyPimienta);
	cvShowImage("ImgSalyPimienta", imgSalyPimienta); cvWaitKey(0);

	//Filtro Mediano
	IplImage* imgFiltMediano= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 3);
	FiltroOrden(imgSalyPimienta, imgFiltMediano,1);
	cvShowImage("ImgFiltroMediano", imgFiltMediano); cvWaitKey(0);

	//FILTRO UNIFORME Y GAUSSIANO 
	//Se inicializa una imagen de un solo canal
	IplImage *imgGray=cvCreateImage(cvGetSize(imgIn),IPL_DEPTH_8U,1);
	cvCvtColor(imgIn,imgGray,CV_BGR2GRAY);			//convierte la img de entrada a img escala de grises

	//Ruido Uniforme
	IplImage *imgUniforme= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 1);
	cvSet(imgUniforme, cvScalar(0,0,0,0),NULL);
	//cvCopy(imgGray,imgUniforme,0);
	RuidoUniforme(imgGray,imgUniforme);
	cvShowImage("ImgRuidoUniforme", imgUniforme); cvWaitKey(0);
	cvAdd(imgUniforme,imgGray,imgUniforme);
	cvShowImage("ImgconRuidoUniforme", imgUniforme); cvWaitKey(0);

	//Filtro PuntoMedio
	IplImage* imgFiltPuntMedio= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 1);
	FiltroOrden(imgUniforme, imgFiltPuntMedio,4);
	cvShowImage("ImgFiltroPuntMedio", imgFiltPuntMedio); cvWaitKey(0);

	//Filtro Gaussiano
	IplImage *imgGaussiano= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 1);
	IplImage *imgFiltroGaussiano= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 1);
	cvSet(imgGaussiano, cvScalar(0,0,0,0),NULL);
	//cvCopy(imgGray,imgGaussiano,0);
	RuidoGaussiano(imgGaussiano);
	cvShowImage("ImgRuidoGaussiano", imgGaussiano); cvWaitKey(0);
	cvAdd(imgGaussiano,imgGray,imgGaussiano);
	cvShowImage("ImgconRuidoGaussiano", imgGaussiano); cvWaitKey(0);

	//Filtro PuntoMedio
	IplImage* imgFiltPuntMedioG= cvCreateImage(cvGetSize(imgIn),imgIn->depth, 1);
	FiltroOrden(imgGaussiano, imgFiltPuntMedioG,4);
	cvShowImage("ImgFiltroPuntMedioGaussiano", imgFiltPuntMedioG); cvWaitKey(0);


}

void RuidoSal(IplImage* img){
	float prob= 0.05;
	int ri=0,rj=0;
	int n=0;
	int npixeles = int(img->width*img->height*prob);
	srand(time(NULL));

	printf("w: %d, h: %d, npx: %d\n", img->width,img->height,npixeles);

	while (n!=npixeles){
		ri= rand()%(img->width);
		rj= rand()%(img->height);
		cvSet2D(img,rj,ri,cvScalarAll(255));
		n++;
	}

	//cvShowImage("ImgSal", imgSal); cvWaitKey(0);
}

void RuidoPimienta(IplImage* img){
	float prob= 0.02;
	int ri=0,rj=0;
	int n=0;
	int npixeles = int(img->width*img->height*prob);
	srand(time(NULL));

	printf("w: %d, h: %d, npx: %d\n", img->width,img->height,npixeles);

	while (n!=npixeles){
		ri= rand()%(img->width);
		rj= rand()%(img->height);
		cvSet2D(img,rj,ri,cvScalarAll(0));
		n++;
	}

	//cvShowImage("ImgPimienta", imgPimienta); cvWaitKey(0);
}

void RuidoSalyPimienta(IplImage* img){
	RuidoSal(img);
	RuidoPimienta(img);
}

void RuidoUniforme(IplImage* img, IplImage * imgF){
	int min=10, max=70;
	float operacion=(float)1/(float)(max-min);
	int brillo= 0;
	float prob= 0.05;
	int i=0,j=0;
	int n=0,n1=0;
	int npixeles = int(img->width*img->height*prob);

	while (n!=npixeles){

		brillo= rand()%max+min;

		while (n1!=int(img->width*img->height*operacion)){
			i= rand()%(img->width);
			j= rand()%(img->height);

			cvSet2D(imgF,j,i,cvScalar((double)brillo,0,0,0));

			n1++;
		}
		//printf("\tnbrillo %d",brillo);
		n1=0;
		n++;
	}		
}

void RuidoGaussiano(IplImage* img){
	double media = 127;//CalculoMedia();
	double varianza= 121;//CalculoVarianza(media);
	double pi=3.141592654,e=2.718281828;

	double op1= (double)1/(double)pow((2*pi*varianza),0.5);
	double op2= (double)1/(double)(2*varianza);

	int n=0,i,j;
	for (int brillo=0;brillo<256;brillo++){
		double op3= op1*pow(e,-pow((brillo-media),2)*op2);
		int prob= int(img->width*img->height*op3);
		printf("\t%d",prob);
		while(n!=prob){
			i= rand()%(img->width);
			j= rand()%(img->height);
			cvSet2D(img,j,i,cvScalar((double)brillo,0,0,0));
			n++;
		}
		n=0;
	}
	
}

double CalculoMedia(){
	int nBrillo=256, suma=0;
	for(int i=0; i<nBrillo;i++){
		suma=suma+i;
	}
	return (suma/nBrillo);
}

double CalculoVarianza(double media){
	int nBrillo=256,suma=0;
	
	for(int i=0; i<nBrillo;i++){
		suma= suma+(i-media)*(i-media);
	}
	return (suma/nBrillo);
}

void FiltroOrden(IplImage* img, IplImage* imgFiltrada,int operacion){
	
	double arrB[9]={0},arrG[9]={0},arrR[9]={0};
	int n=0;

	for(int i=1;i<img->width-1;i++){
		for(int j=1;j<img->height-1;j++){
			for(int posx=i-1; posx<i+2;posx++){
				for(int posy=j-1;posy<j+2;posy++){
					arrB[n]=cvGet2D(img,posy,posx).val[0];
					arrG[n]=cvGet2D(img,posy,posx).val[1];
					arrR[n]=cvGet2D(img,posy,posx).val[2];					
					n++;
				}	
			}
			n=0;
			//Se ordena el arreglo-mascara
			ordenarArr(arrB);
			ordenarArr(arrG);
			ordenarArr(arrR);

			if(operacion==1)		//Filtro Mediano
				cvSet2D(imgFiltrada,j,i,cvScalar(arrB[4],arrG[4],arrR[4],0));
			else if(operacion==2)	//Filtro Maximo
				cvSet2D(imgFiltrada,j,i,cvScalar(arrB[8],arrG[8],arrR[8],0));
			else if(operacion==3)	//Filtro Minimo
				cvSet2D(imgFiltrada,j,i,cvScalar(arrB[0],arrG[0],arrR[0],0));
			else if(operacion==4){	//Filtro Punto Medio
				double puntoMedioB= (arrB[0]+arrB[8])*0.5;
				double puntoMedioG= (arrG[0]+arrG[8])*0.5;
				double puntoMedioR= (arrR[0]+arrR[8])*0.5;
				cvSet2D(imgFiltrada,j,i,cvScalar(puntoMedioB,puntoMedioG,puntoMedioR,0));
			}
			else if (operacion==5){ //Filtro Alpha-Trimed
			
				int t=3;
				double promedioB= CalculoPromedio(arrB,t);
				double promedioG= CalculoPromedio(arrG,t);
				double promedioR= CalculoPromedio(arrR,t);
				cvSet2D(imgFiltrada,j,i,cvScalar(promedioB,promedioG,promedioR,0));
			}

		}		
	}

}

void ordenarArr(double arr[9]){
	
	int menor=0;
	for (int i=0; i<9;i++){
		for(int j=0;j<9;j++){
			if(arr[i]<= arr[j]){
				menor=arr[i];
				arr[i]=arr[j];
				arr[j]=menor;
			}
		}
	}

}

double CalculoPromedio(double arr[9],int t){
	int suma=0, n=9-t;
	for(int i=0; i<n;i++){
		suma=suma+i;
	}
	return (suma/n);
}