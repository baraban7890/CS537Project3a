#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

struct rec {
  int key;
  int values[24];
};

  /*
  Taken from : https://www.geeksforgeeks.org/merge-sort/
  */
  void merge(struct rec arr[], int l, int m, int r)
  {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
 
    /* create temp arrays */

    struct rec *L, *R;
    L = malloc(sizeof(struct rec) * n1);
    R = malloc(sizeof(struct rec) * n2);
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2) {
        if (L[i].key <= R[j].key) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
   }  
       /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
  }

/* l is for left index and r is right index of the
sub-array of arr to be sorted
  Taken from : https://www.geeksforgeeks.org/merge-sort/
  */
void mergeSort(struct rec arr[], int l, int r)
{
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l + (r - l) / 2;
 
        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
 
        merge(arr, l, m, r);
    }
}

int main(int argc, char *argv[])
{
  if(argc < 3)
    {
      printf("Not enough arguments supplied, expected 2");
      return 1;
    }
  if(argc > 3)
    {
      printf("Too many arguments supplied, expected 2");
      return 1;
    }
  FILE *file;
  unsigned long fileLen;
  //int numProcessors = get_nprocs_conf();

  file = fopen(argv[1],"rb");
  
  if( file == NULL)
    {
      fprintf(stderr,"An error has occurred\n");
      exit(0);
    }
  
  fseek(file, 0, SEEK_END);
  fileLen=ftell(file);
  fseek(file, 0, SEEK_SET);
  if(fileLen == 0)
  {
    fprintf(stderr,"An error has occurred\n");
  }

  int buffer;
  struct rec *records;
  records = malloc(sizeof(struct rec) * (fileLen/100));
  int k = 0;
  while(k < fileLen/100)
  {
    (void) !fread(&buffer, 4, 1, file);
    records[k].key = buffer;
    for(int i = 0; i < 24; i ++)
    {
        (void) !fread(&buffer,4,1,file);
        records[k].values[i] = buffer;
    }
    k += 1;
  }
  mergeSort(records,0,(fileLen/100)-1);
  FILE *out_file = fopen(argv[2], "wb");
  if(out_file == NULL)
  {
    exit(1);
  }
  for(int i = 0; i < fileLen/100; i ++)
  {
    fwrite(&(records[i].key),sizeof(records[i].key),1,out_file);
    fwrite(&(records[i].values),sizeof(records[i].values[0]),24, out_file);
  }
  free(records);
  fclose(file);
  fclose(out_file);
}
