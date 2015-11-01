#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp" 
#include <stdlib.h> 
#include <stdio.h> 
#include "opencv2/opencv.hpp"
#include < opencv/highgui.h >


// Create memory for calculations
static CvMemStorage* storage0 = 0;
static CvMemStorage* storage1 = 0;
static CvMemStorage* storage2 = 0;

// Create a new Haar classifier
static CvHaarClassifierCascade* cascade = 0;

// Function prototype for detecting and drawing an object from an image
void detect_and_draw( IplImage* image );

// Create a string that contains the cascade name
const char* cascade_name ="haarcascade_frontalface_alt.xml";

//FUNCION EFECTO
void presentacion();
void fusionImagenes(IplImage* frame, IplImage* img_effect,CvRect* r, double escalamiento, double despX,double despY);

/*    "haarcascade_profileface.xml";*/

int cabeza_opt=0;//Varaible que indica si se activa efecto rostro o no
int ojo_opt=0;//Variable que indica si se activa efecto ojos o no
int boca_opt=0;//Varaible que indica si se activa efecto boca o no
int nariz_opt=0;//Varaible que indica si se activa efecto oreja o no
int oreja_opt=0;//Varaible que indica si se activa efecto oreja o no
int menton_opt=0;//Varaible que indica si se activa efecto menton o no

 
/** @funcion main */ 
int main( int argc, char** argv ){
	presentacion();
	CvCapture* capture = 0;  
    // Imagen
    IplImage *frame, *frame_copy = 0;	
    
	// Carga el HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );

	 // Se verifica si el archivo Cascade se ha cargado o no
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return -1;
    }

    // Se reserva memoria para el almacenamiento de las caracteristicas a buscar con el archivo cascade
    storage0 = cvCreateMemStorage(0);
    storage1 = cvCreateMemStorage(0);
    storage2 = cvCreateMemStorage(0);

    cvNamedWindow( "result", 1 );

    // Find if the capture is loaded successfully or not.
    // If loaded succesfully, then:
    int i=0;
	//capture=cvCreateFileCapture("6.avi");
    ///*
	capture = cvCreateCameraCapture(0);
    if ( !capture ) {
        capture = cvCaptureFromCAM(0);
        if ( !capture ) {
            fprintf( stderr, "Cannot open initialize webcam!\n" );
            return 1;
        }
    }
	//*/
    if( capture )
    {
        for(;;i++)
        {
            if( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );

            // Si el frame no existe salimos del lazo
            if( !frame )
                break;

            // Le colocamos los mismos parametros de tamaño al frame_copy
            if( !frame_copy )
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, frame->nChannels );

            // Verifica el origen de la imagen para establecer la copia
            if( frame->origin == IPL_ORIGIN_TL ){
                cvCopy( frame, frame_copy, 0 );
				//cvCopy( frame, canny, 0 );
			}
            // Caso contrario se la invierte para poder manejarla
            else
                cvFlip( frame, frame_copy, 0 );

            // Se llama a la funcion que detecta rostros y otros patrones faciales		
			
			
			detect_and_draw( frame_copy );

            
            char c =  cvWaitKey( 10 );
           switch(c){
               case 27:
                   exit(0);
                   break;
               case '1':
                   cabeza_opt=!cabeza_opt;				   
                    break;
               case '2':
                   ojo_opt=!ojo_opt;
                   break;            
               case '3':
                   nariz_opt=!nariz_opt;                   
                   break;
               case '4':
				   boca_opt=!boca_opt;
                   break;
			   case '5':			  
					oreja_opt=!oreja_opt;
					break;
			   case '6':			  
					menton_opt=!menton_opt;
					break;
			
        }
        }
		
		
        // Se liberan los recursos de memoria
        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture );
    }
	

    
    cvDestroyWindow("result");
    return 0;
}


void presentacion(){
	char opcion;
	do{
		system("cls");
		printf("\n\n\n");
		printf("\t\t*********************************************\n");
		printf("\t\t*********************************************\n");
		printf("\t\t***   PROCESAMIENTO DIGITAL DE IMAGENES   ***\n");
		printf("\t\t***           PROYECTO PARCIAL            ***\n");
		printf("\t\t***             FACE EFFECTS              ***\n");
		printf("\t\t***                 POR:                  ***\n");
		printf("\t\t***     ANDREA TORRES Y SIANNA PUENTE     ***\n");
		printf("\t\t*********************************************\n");
		printf("\t\t*********************************************\n");
		printf("\n\n\n");
		printf("\t\t M E N U - P R I N C I P A L ");
		printf("\n\t\t------------------------------\n");
		printf("\t\t\t Iniciar? (y/n):");
		scanf("%c", &opcion);
		printf("\n\t\t------------------------------");
		printf("\n\t\t 1: Efecto Cabeza - Sombrero 1\n");
		printf("\n\t\t 2: Efecto Ojos - Gafas \n");
		printf("\n\t\t 3: Efecto Nariz   \n");
		printf("\n\t\t 4: Efecto Boca - Bigote   \n");
		printf("\n\t\t 5: Efecto Orejas - Aretes  \n");
		printf("\n\t\t 6: Efecto Menton - Barba  \n");
		printf("\n\t\t 0: Salir ... \n");
		
	} while (opcion != 'y' && opcion != 'n' && opcion != 'Y' && opcion && 'N');
	switch (opcion){
	case 'y':
		break;
	case 'Y':
		break;
	case 'n':
		exit(0);
		break;
	case 'N':
		exit(0);
		break;
	}
}




// Funcion para detectar y dibujar ciertas caracteristicas dentro de una imagen
void detect_and_draw( IplImage* img )
{
    int scale = 1;


    // Se crean 2 puntos que nos indicaran el origen del rostro
    CvPoint pt1, pt2,pt1_roi, pt2_roi, pt_boca;
	double boca_x=0,boca_y=0;
	double cabeza_x=0,cabeza_y=0;
	double oreja1_x=0,oreja1_y=0,oreja2_x=0,oreja2_y=0;
	double hearAnimal1_x=0,hearAnimal1_y=0,hearAnimal2_x=0,hearAnimal2_y=0;

    int i;

	
	//IplImage* img_efecto=cvLoadImage("sombrero.jpg",1);
	IplImage* img_efecto=cvLoadImage("barba_candado.jpg",1);
	//IplImage* img_head=cvLoadImage("Efectos/efectos cabeza/som.png");
	IplImage* img_head=cvLoadImage("Efectos/efectos cabeza/head0.jpg");
	//IplImage* img_head=cvLoadImage("Efectos/efectos cabeza/sombrero.jpg");
	//IplImage* img_head=cvLoadImage("Efectos/efectos cabeza/cabello3.jpg");

	IplImage* img_hear=cvLoadImage("Efectos/efectos orejas/arete.jpg");
	IplImage* img_orejas=cvLoadImage("Efectos/efectos orejas/orejas.jpg");

    // Limpiamos la memoria que ya fue usada
    cvClearMemStorage( storage0 );
    cvClearMemStorage( storage1 );
    cvClearMemStorage( storage2 );

    // Se verifica que el archivo Cascade ya este cargado
    if( cascade )
    {

        // Pueden haber mas de una cara en una imagen, entonces se procede 
        //a crear una secuencia de imagenes
        CvSeq* faces = cvHaarDetectObjects( img, cascade, storage0, 1.1, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(40, 40) );
        

        // Lazo donde se manejan las caras 
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
           // Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
			
            // Encuentra las dimensiones de las caras y las escala si es necesario
            pt1.x = r->x*scale;
            pt2.x = (r->x+r->width)*scale;
            pt1.y = r->y*scale;
            pt2.y = (r->y+r->height)*scale;

			//printf("\nFRAME (%d,%d)- > ", img->width,img->height);
			//printf("\n\nRECT (%d,%d)- > pt.x= %d, pt.y=%d", r->width,r->height,r->x,r->y);

			
			if(cabeza_opt==1){

				//POSICION DE LA CABEZA - SOMBREROS
				cabeza_x=r->x;
				cabeza_y=r->width*0.7;

				//cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
				fusionImagenes(img,img_head,r,0.000911,-cabeza_x*0.1,-cabeza_y);	//Para el gorro vikingo
				//fusionImagenes(img,img_head,r,0.000631,-cabeza_x*0.3,-cabeza_y*0.85);	//Para el sombrero
				//fusionImagenes(img,img_head,r,0.00111,-cabeza_x*0.21,-cabeza_y*0.7);	//Para el cabello3
				
			}

            if(ojo_opt==1){
				             
                
            }
            
            if(boca_opt==1){	
                //POSICION DE LA BOCA
				boca_y=r->height*(0.777777);
				boca_x=r->width*(0.5);
				//printf("\npt_boca.x: %f, pt_boca.y: %f",boca_x,boca_y);

				//fusionImagenes(img,img_efecto,r,0.00664,boca_x*0.8,boca_y*1.06);
            }
                        
            if(nariz_opt==1){
                
            }
			if(oreja_opt==1){
				oreja1_x = r->width*0.001;
				oreja1_y = r->height*0.65;
				oreja2_x = r->width*0.87;
				oreja2_y = r->height*0.65;

				fusionImagenes(img,img_hear,r,0.000091,-oreja1_x,oreja1_y*1.1);
				//fusionImagenes(img,img_hear,r,0.000091,oreja2_x,oreja2_y*1.1);

				hearAnimal1_x=r->width*0.19375;
				hearAnimal1_y=r->height*0.3;
				
				//fusionImagenes(img,img_orejas,r,0.00041,hearAnimal1_x*0.25,-hearAnimal1_y);

            }
			if(menton_opt==1){
                
            }

        }
    }

    cvShowImage( "result", img );
	
}



//**********************************************************************************************************************
//************** FUNCIONES PARA LOS EFECTOS INTERACTIVOS ***************************************************************
//**********************************************************************************************************************


IplImage* crearMascaraBinarizada(IplImage* img_effect){
	IplImage *img_gray = cvCreateImage(cvSize(img_effect->width,img_effect->height),img_effect->depth,1);
	IplImage *img_binaria = cvCreateImage(cvSize(img_effect->width,img_effect->height),img_effect->depth,img_effect->nChannels);
	
	//cvShowImage("binarizacion",img_effect);

	//Convertir img_effect a gris
	cvCvtColor(img_effect,img_gray,CV_BGR2GRAY);

	//Binarizacion
	cvThreshold(img_gray,img_gray,230,255,CV_THRESH_BINARY);
	//cvShowImage("binarizacion",img_gray);

	//EL negativo
	cvNot(img_gray,img_gray);
	//cvShowImage("negativo",img_gray);

	//Dos veces cerradura (dos dilataciones +dos erosiones)
	//se realiza dos dilataciones
	cvDilate(img_gray,img_gray,NULL,2);
	//se realiza dos erosiones
	cvErode(img_gray,img_gray,NULL,2);

	//cvShowImage("cerradura",img_gray);
	
	//Una vez apertura(erosion+dilatacion)
	cvErode(img_gray,img_gray,NULL,1);
	cvDilate(img_gray,img_gray,NULL,1);

	//cvShowImage("apertura",img_gray);

	cvErode(img_gray,img_gray,NULL,2);
	//cvShowImage("erosion",img_gray);
	
	//Convertir la imagen binarizada en 3 canales
	cvCvtColor(img_gray,img_binaria,CV_GRAY2BGR);
	return img_binaria;

}

//Funcion recibe el frame y la imagen del efecto que le deseo añadir para su fusion.
// Tambien se recibe el escalamiento para que se encuentre redimensionada el efecto sobre el frame-video, y ademàs los desplazamientos de la imagen efecto 
void fusionImagenes(IplImage* frame, IplImage* img_effect,CvRect* r, double escalamiento, double despX,double despY){
	IplImage *cloneFrame,*imgObj,*imgObj2,*mascara,*mascara2,*resize_imgEffect,*resize_mascara;
	
	//Creo mi mascara
	IplImage* mascara_imgEffect = crearMascaraBinarizada(img_effect);
	//cvShowImage("MI MASCARA",mascara_imgEffect);
	
	//Se clona la Imagen que captura la camara
	cloneFrame = cvCloneImage(frame);
	
	//Incializo imagenes base de mi imagen efectos
	imgObj = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,frame->nChannels);//imagen para pegar el img_effect
	imgObj2 = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,frame->nChannels);//img_effect pero trasladada
	
	//Pinto mi imagen base de color blanco
	cvSet(imgObj, cvScalar(255,255,255,0),NULL);
	cvSet(imgObj2, cvScalar(255,255,255,0),NULL);

	//Incializo imagenes base de mi macara
	mascara = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,frame->nChannels);//imagen para pegar la Mascara
	mascara2 = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,frame->nChannels);//mascara pero  trasladada
	
	//Pinto mi imagen base de color negro
	cvSet(mascara, cvScalar(0,0,0,0),NULL);
	cvSet(mascara2, cvScalar(0,0,0,0),NULL);

	//Se define el nuevo tamaño
	int nuevoWidth = (int)(escalamiento*img_effect->width*r->width);//tamaño horizontal nuevo para el img_effect, tiene que ser proporcional al de r, por eso lo multiplico
	int nuevoHeight = (int)(escalamiento*img_effect->height*r->height);//tamaño vertical nuevo para el img_effect..
	//printf("\neffect(%d,%d)-> %d,%d",img_effect->width,img_effect->height,r->width,r->height);

	if(nuevoWidth > frame->width) nuevoWidth = frame->width;	// valida que el nuevo tamaño horizontal del objeto no sobrepase el del frame
	if(nuevoHeight > frame->height) nuevoHeight = frame->height;  // valida que el nuevo tamaño vertical del objeto no sobrepase el del frame

	//Se redimensiona la imagen effecto con los nuevos valores para el width-heigth
	resize_imgEffect = cvCreateImage(cvSize(nuevoWidth,nuevoHeight),img_effect->depth,img_effect->nChannels);//Imagen con el nuevo tamaño
	cvResize(img_effect,resize_imgEffect,1);

	resize_mascara = cvCreateImage(cvSize(nuevoWidth,nuevoHeight),mascara_imgEffect->depth,mascara_imgEffect->nChannels);//Mascara con el nuevo tamaño
	cvResize(mascara_imgEffect,resize_mascara,1);//crea la mascara con el nuevo tamaño

	cvShowImage("imgObj-antes",resize_imgEffect);cvShowImage("mascara-antes",resize_mascara);
	//ROI de la imagen effecto
	cvSetImageROI(imgObj, cvRect(0,0,resize_imgEffect->width,resize_imgEffect->height));//crea un roi en el origen con el tamaño de la imagen efecto
	cvCopy(resize_imgEffect,imgObj);//copia el objeto
	cvResetImageROI(imgObj);//quita el roi
	cvShowImage("copy",imgObj);

	//ROI de la mascara
	cvSetImageROI(mascara, cvRect(0,0,resize_mascara->width,resize_mascara->height));
	cvCopy(resize_mascara,mascara);
	cvResetImageROI(mascara);
	cvShowImage("MASK-ROI",mascara);
	
	//Se define los deplazamientos
	int dx = r->x+despX;//La posicion x del rectangulo que detecta la cara+desplazamiento basado en el ancho del rectangulo
	int dy = r->y+despY;

	//printf("\ndespx= %d , despy= %d ----- %d,%d",dx,dy,despX,despY);
	
	//cvShowImage("imgObj-antes",imgObj);cvShowImage("mascara-antes",mascara);
	//Matriz de transformacion, solo tomamos en cuenta dx,dy para la traslacion
	CvMat *M = cvCreateMat( 2, 3, CV_32FC1);
	cvmSet(M,0,0,1); // asignamos valor 1 al elemento (0,0)
	cvmSet(M,0,1,0); // asignamos valor 0 al elemento (0,1)
	cvmSet(M,1,0,0); // asignamos valor 0 al elemento (1,0)
	cvmSet(M,1,1,1); // asignamos valor 1 al elemento (1,1)
	cvmSet(M,0,2,dx); // el cuarto número indica los píxeles que se recorren en el eje x
	cvmSet(M,1,2,dy);
	
	cvWarpAffine (imgObj, imgObj2, M, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(255));// aplicamos la transformación para obtener la imagen trasladada
	cvWarpAffine (mascara,mascara2, M, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS,cvScalarAll(0));//lo mismo pero para la mascara
	//cvShowImage("imgObj-despues",imgObj2);cvShowImage("mascara-despues",mascara2);

	cvShowImage("imgObj-despues",imgObj2);cvShowImage("mascara-despues",mascara2);
	cvAnd(imgObj2,mascara2,imgObj2,0);//Se recorta el objeto usando la mascara
	cvShowImage("AND imgObj2-mascara2",imgObj2);
	cvNot(mascara2,mascara2);			//Se crea la mascara inversa
	cvShowImage("NOT mascara2",mascara2);
	cvShowImage("CLoNE FRAME",cloneFrame);
	cvAnd(cloneFrame,mascara2,cloneFrame,0);	//Se usa la macara inversa para quitar la parte en donde ira el objeto
	cvShowImage("AND cloneFrame-mascara2",cloneFrame);
	cvOr(cloneFrame,imgObj2,frame,0);		// Se unen las dos imagenes con OR


	//Libero memoria de cada una de las imagenes		
	/*
	cvReleaseImage(&cloneFrame);
	cvReleaseImage(&imgObj);
	cvReleaseImage(&imgObj2);
	cvReleaseImage(&mascara_imgEffect);
	cvReleaseImage(&mascara);
	cvReleaseImage(&mascara2);
	cvReleaseImage(&resize_imgEffect);
	cvReleaseImage(&resize_mascara);
	*/
}
