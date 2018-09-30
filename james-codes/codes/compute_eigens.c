#include <stdio.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>


// TODO:
//1.) read in data from covariance matrix file
//2.) output eigenvalues and vectors in friendly form

//---> cast data along feature vector. 
//---> cluster!

const int nDimensions = 511;

int main (int argc, char **argv) {

  	printf("\n");
    printf("******************************\n");
    printf("Start...\n");
    printf("******************************");
    printf("\n");
    printf("\n");
 
	char * inPath;
    char * outPath;
  
  	char INfile[1000];
	char OUTfile[1000];

	inPath = argv[1]; 
	outPath = argv[2];
	
	FILE * input_file = NULL;
    FILE * output_file = NULL;
 
 	double * covariance_list;
	covariance_list = (double*)calloc(nDimensions * nDimensions, sizeof(double));
	
 	sprintf(INfile, inPath);
    input_file = fopen(INfile,"r"); 
       
    int dim1, dim2;
    double covar;
    
    printf("Here 1. \n");
    
    int idx = 0;
    while (fscanf(input_file,"%i  %i  %lf \n",&dim1,&dim2,&covar) != EOF) {
 
    	covariance_list[idx] = covar;
    	idx++;
 	}
 
    printf("Here 2. \n");
  
	gsl_matrix_view m = gsl_matrix_view_array (covariance_list, nDimensions, nDimensions);
	gsl_vector *eval = gsl_vector_alloc (nDimensions);
  	gsl_matrix *evec = gsl_matrix_alloc (nDimensions, nDimensions);
	
	gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc (nDimensions);
	gsl_eigen_symmv (&m.matrix, eval, evec, w);
	gsl_eigen_symmv_free (w);
	gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_ABS_ASC);

	double * eigenvalues;
	eigenvalues = (double*)calloc(nDimensions, sizeof(double));
  	
  	double * eigenvectors;
	eigenvectors = (double*)calloc(nDimensions * nDimensions, sizeof(double));
		
	int eigenIdx = 0;
	int eigenValueIdx =0;
	
    for (int i=0; i<nDimensions; i++) {
        
        eigenValueIdx = nDimensions - i - 1;
        double eval_i = gsl_vector_get (eval, eigenValueIdx);  // value
        eigenvalues[i] = eval_i;
       	
        gsl_vector_view evec_i = gsl_matrix_column (evec, eigenValueIdx);  // vector
		
        //printf ("eigenvalue = %g\n", eval_i);
        //printf ("eigenvector = \n");
        //gsl_vector_fprintf (stdout, &evec_i.vector, "%g");
        
		for (int j=0; j<nDimensions; j++) {
		
			double eigen_elem = gsl_vector_get(&evec_i.vector, j);
			eigenvectors[eigenIdx] = eigen_elem;
			eigenIdx++;
		}
	}

	gsl_vector_free (eval);
	gsl_matrix_free (evec);

	printf("Here 3. \n");
	// write out eigenvalues
	sprintf(OUTfile, "%s_%s.dat", outPath, "eigenvalues");
	output_file = fopen(OUTfile,"w");
	
	double acc = 0;
	for (int i=0; i<nDimensions; i++) {
	
		acc = acc + eigenvalues[i];
		fprintf(output_file, "%lf  %lf \n", eigenvalues[i], acc);
	}
	fclose(output_file);

	// write out eigenvectors
	sprintf(OUTfile, "%s_%s.dat", outPath, "eigenvectors");
	output_file = fopen(OUTfile,"w");
	
	printf("Here 4. \n");
	
	eigenIdx = 0;
	for (int i=0; i<nDimensions; i++) {
	
		for (int j=0; j<nDimensions; j++) {
		
			fprintf(output_file, "%lf  ", eigenvectors[eigenIdx]);
			eigenIdx++;
		}
		fprintf(output_file, "\n");
	}
	fclose(output_file);

	printf("Finished! \n");
	
	return 0;
}