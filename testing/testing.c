#include "preprocessing.h"
#include "fitting.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include "data.h"


int main(){
	FITPRMS* fitted_prms;
	llist good_ranges, lags;
	int pulse[] = {0,14,22,24,27,31,42,43};
	double real,imag;
	double R,I;
	int i,j;

	DIR* FD;
    struct dirent* in_file;
    FILE  *entry_file;
    char filename[1024],buf[1024];
    char* in_dir = "testing/simulated_acfs";


	good_ranges = llist_create(NULL,NULL,0);
	lags = llist_create(compare_lags,NULL,0);
	fitted_prms = malloc(sizeof(FITPRMS));
	fitted_prms->pulse = pulse;
	fitted_prms->lagfr = 1200;
	fitted_prms->mppul = 8;
	fitted_prms->smsep = 300;
	fitted_prms->txpl = 300;
	fitted_prms->mpinc = 1800;
	fitted_prms->channel = 0;
	fitted_prms->offset = 0;
	fitted_prms->cp = 0;
	fitted_prms->xcf = 1;
	fitted_prms->tfreq = 12444;
	fitted_prms->noise = 0;
	fitted_prms->nrang = 70;
	fitted_prms->nave = 25;
	fitted_prms->mplgs = 23;
	fitted_prms->old = 0;
	fitted_prms->maxbeam = 0;
	fitted_prms->bmsep = 0;
	fitted_prms->phidiff = 0;
	fitted_prms->tdiff = 0;
	fitted_prms->vdir = 0;

	fitted_prms->acfd = malloc(fitted_prms->nrang * fitted_prms->mplgs * sizeof(*fitted_prms->acfd));
	fitted_prms->acfd[0] = malloc(fitted_prms->nrang * fitted_prms->mplgs * 2 * sizeof(**fitted_prms->acfd));
	fitted_prms->pwr0 = malloc(fitted_prms->nrang * sizeof(*fitted_prms->pwr0));

	for(i=0;i<(fitted_prms->nrang * fitted_prms->mplgs);i++){
		fitted_prms->acfd[i] = fitted_prms->acfd[0] + i * 2; 
	}


	Determine_Lags(lags,fitted_prms);
/*	llist_for_each(lags,print_lag_node);*/
/*	for(i=0;i<fitted_prms.mplgs+1;i++){
		printf("lags %d %d\n",lags[i][0],lags[i][1]);
	}
*/

	if((FD = opendir(in_dir)) == NULL){
		fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
	}

	 while ((in_file = readdir(FD))) 
    {
        
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;
        
        sprintf(filename,"%s/%s",in_dir,in_file->d_name);
        entry_file = fopen(filename, "r");
        if (entry_file == NULL)
        {
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            return 1;
        }

        i = 0;
		while (fgets(buf,1024,entry_file) != NULL){
			sscanf(buf,"%lf %lf",&R, &I);
			fitted_prms->acfd[i][0] = R;
			fitted_prms->acfd[i][1] = I;
			i++;
		}

		for(i = 0;i<fitted_prms->nrang;i++){
			real = fitted_prms->acfd[i * fitted_prms->mplgs][0];
			imag = fitted_prms->acfd[i * fitted_prms->mplgs][1];
			fitted_prms->pwr0[i] = sqrt(real * real + imag * imag);
		}

		Filter_Bad_ACFs(fitted_prms,good_ranges,1);
/*		llist_for_each(good_ranges,print_range_node);*/

		/*llist_for_each_arg(good_ranges,Find_CRI,&fitted_prms,NULL);*/
	/*	llist_for_each(good_ranges,print_range_node);*/

		llist_for_each_arg(good_ranges,Find_Alpha,lags,fitted_prms);
/*		llist_for_each(good_ranges,print_range_node);*/

		llist_for_each_arg(good_ranges,Fill_Data_Lists_For_Range,lags,fitted_prms);
/*		llist_for_each(good_ranges,print_range_node);*/

	/*	Filter_TX_Overlap(good_ranges, lags, bad_samples, &fitted_prms);*/
	/*	llist_for_each(good_ranges,print_range_node);*/

		/*llist_for_each_arg(good_ranges,Phase_Unwrap,lags,NULL);
		llist_for_each(good_ranges,print_range_node);*/

		llist_for_each(good_ranges,Power_Fits);
		
		ACF_Phase_Fit(good_ranges,fitted_prms);
		llist_for_each(good_ranges,print_range_node);
        /* When you finish with the file, close it */
        fclose(entry_file);
    }




	return 0;
}
