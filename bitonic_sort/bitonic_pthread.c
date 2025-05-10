#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>
#include <omp.h>


struct timeval startwtime, endwtime;
double seq_time;


int N,n;          // data array size
int *a;         // data array to be sorted
int threadCount=0;
int threadlayers;
const int ASCENDING  = 1;
const int DESCENDING = 0;
void init(void);
void print(void);
void test(void);
void exchange(int i, int j);
void compare(int i, int j, int dir);
void Psort( void );
void * PrecBitonicSort( void * arg );
void * PbitonicMerge( void * arg );
void PimpBitonicSort( void );

int desc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 > *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}
int asc( const void *a, const void *b ){
    int* arg1 = (int *)a;
    int* arg2 = (int *)b;
    if( *arg1 < *arg2 ) return -1;
    else if( *arg1 == *arg2 ) return 0;
    return 1;
}

int main( int argc, char **argv ) {
    if (argc != 3 || atoi( argv[ 2 ] ) > 256 ) {
        printf( "Usage: %s q t\n  where n=2^q is problem size (power of two), and t is the number of threads, <=256, to use.\n", argv[ 0 ] );
        printf( "Usage: The first argument you pass is used to calculate number of elements in the array \n(number of elements = 2^first argument)\n" );
        printf( "Usage: Similarly second argument decides how many threads to be created\n (NUmber of threads = 2^ second argument)\n" );
        exit( 1 );
    }
    N = 1 << atoi( argv[ 1 ] ); // Size of aray. 2^First argument.
    n = atoi( argv[ 2 ] );
    threadlayers = atoi( argv[ 2 ] ); // number of threads you want. 2^second argument
    if( threadlayers != 0 && threadlayers != 1 ) {
	--threadlayers;
    }
    a = (int *) malloc( N * sizeof( int ) );
    init();
    gettimeofday( &startwtime, NULL );
    // printf("Before\n");
    //print();
    Psort();
	// printf("After\n");
    //print();
    gettimeofday( &endwtime, NULL );
    seq_time = (double)( ( endwtime.tv_usec - startwtime.tv_usec ) / 1.0e6 + endwtime.tv_sec - startwtime.tv_sec );
    printf( "Total time = %f\n s", seq_time );
    printf("Total threads used: %d\n",threadCount);
    test();
}

//Test
void test() {
  int pass = 1;
  int i;
  for (i = 1; i < N; i++) {
    pass &= (a[i-1] <= a[i]);
  }
  printf(" TEST %s\n",(pass) ? "PASSED" : "FAILED!");
}
//initializing the array
void init() {
  int i;
  for (i = 0; i < N; i++) {
    a[i] = rand() % N; // (N - i);
  }
}
//Display
void print() {
  int i;
  for (i = 0; i < N; i++) {
    printf("%d\n", a[i]);
  }
  printf("\n");
}
//Swap method
void exchange(int i, int j) {
  int t;
  t = a[i];
  a[i] = a[j];
  a[j] = t;
}
//Comparisions
void compare(int i, int j, int dir) {
  if (dir==(a[i]>a[j])) 
    exchange(i,j);
}
//Merge
void bitonicMerge(int lo, int cnt, int dir) {
  if (cnt>1) {
    int k=cnt/2;
    int i;
    for (i=lo; i<lo+k; i++)
      compare(i, i+k, dir);
    bitonicMerge(lo, k, dir);
    bitonicMerge(lo+k, k, dir);
  }
}
//Structure
typedef struct{
    int lo, cnt, dir, layer;
} sarg;
//Merge
void * PbitonicMerge( void * arg ){
    int lo = ( ( sarg * ) arg ) -> lo;
    int cnt = ( ( sarg * ) arg ) -> cnt;
    int dir = ( ( sarg * ) arg ) -> dir;
    int layer = ( ( sarg * ) arg ) -> layer;
    if( cnt > 1 ){
        int k = cnt / 2;
        int i;
        for( i = lo; i < lo + k; ++i ){
            compare( i, i + k, dir );
        }
        if( layer <= 0 ){
            bitonicMerge( lo, k, dir );
            bitonicMerge( lo + k, k, dir );
            return 0;
        }
        sarg arg1, arg2;
        pthread_t thread1, thread2;
        arg1.lo = lo;
        arg1.cnt = k;
        arg1.dir = dir;
        arg1.layer = layer - 1;
        arg2.lo = lo + k;
        arg2.cnt = k;
        arg2.dir = dir;
        arg2.layer = layer - 1;
        pthread_create( &thread1, NULL, PbitonicMerge, &arg1 );
        pthread_create( &thread2, NULL, PbitonicMerge, &arg2 ); 
        
        pthread_join( thread1, NULL );
        pthread_join( thread2, NULL );
    }
    return 0;
}
//Bitonic sort
void * PrecBitonicSort( void * arg ){
    int lo = ( ( sarg * ) arg ) -> lo;
    int cnt = ( ( sarg * ) arg ) -> cnt;
    int dir = ( ( sarg * ) arg ) -> dir;
    int layer = ( ( sarg * ) arg ) -> layer;
    if ( cnt > 1 ) {
        int k = cnt / 2;
        if( layer >= threadlayers ) {
            qsort( a + lo, k, sizeof( int ), asc );
            qsort( a + ( lo + k ) , k, sizeof( int ), desc );
        }
        else{
            sarg arg1;
            pthread_t thread1;
            arg1.lo = lo;
            arg1.cnt = k;
            arg1.dir = ASCENDING;
            arg1.layer = layer + 1;
            //printf("Creating a new Accending thread\n");
            threadCount++;
            pthread_create( &thread1, NULL, PrecBitonicSort, &arg1 );
            
            sarg arg2;
            pthread_t thread2;
            arg2.lo = lo + k;
            arg2.cnt = k;
            arg2.dir = DESCENDING;
            arg2.layer = layer + 1;
            //printf("Creating a new Decending thread\n");
            threadCount++;
            pthread_create( &thread2, NULL, PrecBitonicSort, &arg2 );
                       
            pthread_join( thread1, NULL );
            pthread_join( thread2, NULL );
        }
        sarg arg3;
        arg3.lo = lo;
        arg3.cnt = cnt;
        arg3.dir = dir;
        arg3.layer = threadlayers - layer;
        PbitonicMerge( &arg3 );
    }
    return 0;
}
//setting up the arguments and first call to the sorter
void Psort() {
    sarg arg;
    arg.lo = 0;
    arg.cnt = N;
    arg.dir = ASCENDING;
    arg.layer = 0;    
    PrecBitonicSort( &arg );
}