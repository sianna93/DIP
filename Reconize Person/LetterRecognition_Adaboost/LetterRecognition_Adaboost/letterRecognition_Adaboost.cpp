#include "opencv2/core/core_c.h"
#include "opencv2/ml/ml.hpp"
#include <cstdio>
#include <vector> 
#include <iostream>
#include <iomanip>

int read_num_class_data( const char* filename, int var_count, CvMat** data, CvMat** responses )
{
    const int M = 1024;
    FILE* f = fopen( filename, "rt" );
    CvMemStorage* storage;
    CvSeq* seq;
    char buf[M+2];
    float* el_ptr;
    CvSeqReader reader;
    int i, j;

    if( !f )
        return 0;

    el_ptr = new float[var_count+1];
    storage = cvCreateMemStorage();
    seq = cvCreateSeq( 0, sizeof(*seq), (var_count+1)*sizeof(float), storage );

    for(;;)
    {
        char* ptr;
        if( !fgets( buf, M, f ) || !strchr( buf, ',' ) )
            break;
        el_ptr[0] = buf[0];
        ptr = buf+2;
        for( i = 1; i <= var_count; i++ )
        {
            int n = 0;
            sscanf( ptr, "%f%n", el_ptr + i, &n );
            ptr += n + 1;
        }
        if( i <= var_count )
            break;
        cvSeqPush( seq, el_ptr );
    }
    fclose(f);

    *data = cvCreateMat( seq->total, var_count, CV_32F );
    *responses = cvCreateMat( seq->total, 1, CV_32F );

    cvStartReadSeq( seq, &reader );

    for( i = 0; i < seq->total; i++ )
    {
        const float* sdata = (float*)reader.ptr + 1;
        float* ddata = data[0]->data.fl + var_count*i;
        float* dr = responses[0]->data.fl + i;

        for( j = 0; j < var_count; j++ )
            ddata[j] = sdata[j];
        *dr = sdata[-1];
        CV_NEXT_SEQ_ELEM( seq->elem_size, reader );
    }

    cvReleaseMemStorage( &storage );
    delete[] el_ptr;
    return 1;
} 

//Para la base de entrenamiento utilizado letras recognition.data sitio http://archive.ics.uci.edu/ml/. 
//La muestra consta de 20.000 elementos. Cada elemento 17 se compone de los atributos: 1 - el miembro de la sección, 
//es decir, el carácter de texto A, B, C, D ... - sólo 26 opciones; 16 atributos - signos de reconocimiento. 
//Criterios para el reconocimiento se pueden encontrar en la descripción de la muestra en este sitio. 
//Se utiliza para el entrenamiento de 10.000 artículos para comprobar también son 10.000 elements.letter_recog.cpp 
//ejemplo se ha simplificado de la siguiente manera:

int build_boost_classifier( char* data_filename )
{
    const int class_count = 26;
    CvMat* data = 0;
    CvMat* responses = 0; //respuestas
    CvMat* var_type = 0;
    CvMat* temp_sample = 0;
    CvMat* weak_responses = 0;

    int ok = read_num_class_data( data_filename, 16, &data, &responses ); //16 caracteristicas - atributos
    int nsamples_all = 0, ntrain_samples = 0;
    int var_count;
    int i, j, k;
    double train_hr = 0, test_hr = 0;
    CvBoost boost;

    if( !ok )
    {
        printf( "Could not read the database %s\n", data_filename );
        return -1;
    }

    printf( "The database %s is loaded.\n", data_filename );
    nsamples_all = data->rows;						//20000
    ntrain_samples = (int)(nsamples_all*0.5);		//50% de todas las muestras se va a entrenar
    var_count = data->cols;							//16

	//data (20.000,16)
	std::cout<< "data (x,y): "<<nsamples_all<< ","<<var_count<<"\n";

	//responses (20.000,1)
	std::cout<< "responses (x,y): "<<responses->rows<< ","<<responses->cols<<"\n";

	//NOTA: Type Mat
	//			Type – Array type. Use CV_8UC1, ..., CV_64FC4 to create 1-4 channel matrices, 
	//			or CV_8UC(n), ..., CV_64FC(n) to create multi-channel (up to CV_CN_MAX channels) matrices.

	//new_data [26*1000,16+1] -> [26.000,17]
    CvMat* new_data = cvCreateMat( ntrain_samples*class_count, var_count + 1, CV_32F );
	//new_responses [26*1000,1] -> [26.000,1]
    CvMat* new_responses = cvCreateMat( ntrain_samples*class_count, 1, CV_32S );
	
	//Print cvMat std::cout << results <<"\n";

    // 1. unroll the database type mask
    printf( "Unrolling the database...\n");
    for( i = 0; i < ntrain_samples; i++ )
    {
        float* data_row = (float*)(data->data.ptr + data->step*i);
        for( j = 0; j < class_count; j++ )
        {
            float* new_data_row = (float*)(new_data->data.ptr + new_data->step*(i*class_count+j));
            for( k = 0; k < var_count; k++ )
                new_data_row[k] = data_row[k];
            new_data_row[var_count] = (float)j;
            new_responses->data.i[i*class_count + j] = responses->data.fl[i] == j+'A';
        }
    }

	//Print
	printf("new_responses\n");
	//std::cout << new_responses <<"\n";
	//std::cout << "M = "<< std::endl << " "  << new_responses << std::endl << std::endl;

	std::cout <<"num cols: "<< new_responses->height<<"\n"; //height = 26000
	//new_responses->data.i[] -> contiene valores entre 0 y 1
	for (j=0;j< 100;j++){
		std::cout << new_responses->data.i[j] << "\t"; 
		
	}

	///*

    // 2. create type mask
    var_type = cvCreateMat( var_count + 2, 1, CV_8U );
    cvSet( var_type, cvScalarAll(CV_VAR_ORDERED) );
    // the last indicator variable, as well
    // as the new (binary) response are categorical
    cvSetReal1D( var_type, var_count, CV_VAR_CATEGORICAL );
    cvSetReal1D( var_type, var_count+1, CV_VAR_CATEGORICAL );
	
	printf("var_type\n");
	std::cout << var_type <<"\n";

    // 3. train classifier
    printf( "Training the classifier (may take a few minutes)...\n");
    boost.train( new_data, CV_ROW_SAMPLE, new_responses, 0, 0, var_type, 0,
        CvBoostParams(CvBoost::REAL, 100, 0.95, 5, false, 0 ));
    cvReleaseMat( &new_data );
    cvReleaseMat( &new_responses );
    printf("\n");
   
    temp_sample = cvCreateMat( 1, var_count + 1, CV_32F );
    weak_responses = cvCreateMat( 1, boost.get_weak_predictors()->total, CV_32F );

    // compute prediction error on train and test data
    for( i = 0; i < nsamples_all; i++ )
    {
        int best_class = 0;
        double max_sum = -DBL_MAX;
        double r;
        CvMat sample;
        cvGetRow( data, &sample, i );
        for( k = 0; k < var_count; k++ )
            temp_sample->data.fl[k] = sample.data.fl[k];

        for( j = 0; j < class_count; j++ )
        {
            temp_sample->data.fl[var_count] = (float)j;
            boost.predict( temp_sample, 0, weak_responses );
            double sum = cvSum( weak_responses ).val[0];
            if( max_sum < sum )
            {
                max_sum = sum;
                best_class = j + 'A';
            }
        }

        r = fabs(best_class - responses->data.fl[i]) < FLT_EPSILON ? 1 : 0;

        if( i < ntrain_samples )
            train_hr += r;
        else
            test_hr += r;
    }

	printf("temp_sample\n");
	std::cout << temp_sample <<"\n";

    test_hr /= (double)(nsamples_all-ntrain_samples);
    train_hr /= (double)ntrain_samples;
    printf( "Recognition rate: train = %.1f%%, test = %.1f%%\n",
            train_hr*100., test_hr*100. );

    printf( "Number of trees: %d\n", boost.get_weak_predictors()->total );
    //*/
	system("PAUSE");
    cvReleaseMat( &temp_sample );
    cvReleaseMat( &weak_responses );
    cvReleaseMat( &var_type );
    cvReleaseMat( &data );
    cvReleaseMat( &responses );

    return 0;
}
 

int main()
{
        
        build_boost_classifier( "letter-recognition.data");
		//build_boost_classifier( "samples.data");
        return 0;
}