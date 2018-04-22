
/*---------------------------------------------------------------------
 *REF below*
**http://dudevictor.github.io/projects/programacao-concorrente/shared-memory-with-openmp/**
 *Function:  Gen_bins
 * Purpose:   Compute max value for each bin, and store 0 as the
 *            number of values in each bin
 * In args:   min_meas:   the minimum possible measurement
 *            max_meas:   the maximum possible measurement
 *            bin_count:  the number of bins
 * Out args:  bin_maxes:  the maximum possible value for each bin
 *            bin_counts: the number of data values in each bin
 */
void Gen_bins(
      float min_meas      /* in  */, 
      float max_meas      /* in  */, 
      float bin_maxes[]   /* out */, 
      int   bin_counts[]  /* out */, 
      int   bin_count     /* in  */) {
   float bin_width;
   int   i;

   bin_width = (max_meas - min_meas)/bin_count;

   #pragma omp parallel for num_threads(thread_count) \
      default(none) \
      shared(min_meas, max_meas, bin_maxes, bin_counts, bin_count, bin_width) \
      private(i)
   for (i = 0; i < bin_count; i++) {
      bin_maxes[i] = min_meas + (i+1)*bin_width;
      bin_counts[i] = 0;
   }

#  ifdef DEBUG
   printf("bin_maxes = ");
   for (i = 0; i < bin_count; i++) 
      printf("%4.3f ", bin_maxes[i]);
   printf("\n");
#  endif
}  /* Gen_bins */

/*---------------------------------------------------------------------
 * Function:  Gen_data
 * Purpose:   Generate random floats in the range min_meas <= x < max_meas
 * In args:   min_meas:    the minimum possible value for the data
 *            max_meas:    the maximum possible value for the data
 *            data_count:  the number of measurements
 * Out arg:   data:        the actual measurements
 */
void Gen_data(
        float   min_meas    /* in  */, 
        float   max_meas    /* in  */, 
        float   data[]      /* out */,
        int     data_count  /* in  */) {
   int i;

   srandom(0);
   #pragma omp parallel for num_threads(thread_count) \
      default(none) shared(data, min_meas, max_meas, data_count)
      for (i = 0; i < data_count; i++) {
         data[i] = min_meas + (max_meas - min_meas) * random() / ((double) RAND_MAX);
      }

#  ifdef DEBUG
   printf("data = ");
   for (i = 0; i < data_count; i++)
      printf("%4.3f ", data[i]);
   printf("\n");
#  endif
}  /* Gen_data */

/* Count number of values in each bin */
   #pragma omp parallel for num_threads(thread_count) default(none) \
      shared(data_count, data, bin_maxes, bin_count, min_meas, bin_counts) \
      private(bin, i)
   for (i = 0; i < data_count; i++) {
      bin = Which_bin(data[i], bin_maxes, bin_count, min_meas);
      #pragma omp critical
      bin_counts[bin]++;
   }
