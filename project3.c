
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <math.h>
#include <time.h>

#define handle_error(msg)\
    do{perror(msg); exit(EXIT_FAILURE);} while(0)
typedef struct task {
   int i;
   char* dst;
   int miThnk;
   int maThnk;
   int miDin;
   int maDin;
   int numDin;
   int numbPhil;
} WT;
sem_t* Quato;
sem_t* forks;

double Exp(double l)
{
     double Rand = rand()/(1.0 + RAND_MAX); 
    int range = 100 - 1 + 1;
    int scaledRand = (Rand * range) +1;
	int	result = scaledRand;
  double a;                     
  double mean;           

  a = (double) result / 101.0;
  mean = -l * log(a);
  return (int) mean;
}


void ThreadWork(void *i) {
    struct timespec start;
    struct timespec end;
    double elapsed;
    WT *x = ((WT *) i);
	int thinkTime = 0;
	int dineTime = 0;

	if(x->dst == "uniform") {
	      double Rand = rand()/(1.0 + RAND_MAX); 
    int range = x->maThnk - x->miThnk + 1;
    int scaledRand = (Rand * range) + x->miThnk;
		thinkTime = scaledRand;
	  double Rand1 = rand()/(1.0 + RAND_MAX); 
    int range1 = x->maDin -x-> miDin + 1;
    int scaledRand1 = (Rand1 * range1) +x-> miDin;
		dineTime = scaledRand1;
	} else {
	    
		thinkTime = Exp((double) (x->miThnk + x->maThnk) / 2);
		dineTime = Exp( (double) ((x->miDin + x->maDin) / 2));
	}
clock_gettime(CLOCK_MONOTONIC, &start);
	while(x->numDin > 0) {
		usleep(thinkTime*1000);


		sem_wait(&Quato);
		sem_wait(&forks[x->i]);
		sem_wait(&forks[(x->i+1)%x->numbPhil]);

		x->numDin--;

		usleep(dineTime*1000);

		sem_post(&forks[x->i]);
		sem_post(&forks[(x->i+1)%x->numbPhil]);
		sem_post(&Quato);
	}
	 clock_gettime(CLOCK_MONOTONIC, &end);
	  elapsed = (end.tv_sec - start.tv_sec);
    elapsed += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("philosopher #%d is done\n in %f milliseconds \n", x->i, elapsed);
    
    free(x);

    pthread_exit(NULL);
  
}

int main(int argc, char *argv[]) {

	pid_t pid;
	int i;

	int numbPhil = atoi(argv[1]);
	if(numbPhil>27){
	    handle_error("philosopher number should be smaller than 27");
	}
	if(numbPhil%2==0){
	    handle_error("not odd number");
	}
	int miThnk = atoi(argv[2]);
		
	int maThnk = atoi(argv[3]);
	
	int miDin = atoi(argv[4]);
	int maDin = atoi(argv[5]);
    if(miThnk<1 || miDin<1){
        handle_error("Minimum Time Parameter is 1ms");
    }
  if(maThnk>60000 || maDin > 60000){
        handle_error("Maximum Time Parameter is 60s");
    }
	char* probability = argv[6];
	int numDin = atoi(argv[7]);
	int tid[numbPhil];

	sem_t s[numbPhil];
    forks = s;
	
    sem_init(&Quato, 0, numbPhil / 2);

	for (i = 0; i <= numbPhil; i++) {
		sem_init(&forks[i], 0, 1);
	}



	for (i = 0; i < numbPhil; ++i) {

        WT* arg = (WT * ) malloc(sizeof(WT));

        arg->maThnk = maThnk;
        arg->miDin = miDin;
        arg->miThnk = miThnk;
        arg->dst = probability;
        arg->maDin = maDin;
   
        arg->numDin = numDin;
        arg->numbPhil = numbPhil;
        arg->i = i;
        pthread_create(&tid[i], NULL, &ThreadWork, (void *)arg);
    }

	pthread_exit(NULL);
}
