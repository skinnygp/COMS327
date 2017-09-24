#include<pthread.h>
#include<stdio.h>
void *hello(void) { 
		printf( "hello " );
		pthread_exit(0);
} 
void *world(void) { 
		printf( "world" );
		pthread_exit(0);
}
int main(int argc, char *argv[])
{
pthread_t t1, t2;
pthread_create(&t1, NULL , &hello, NULL);
pthread_create(&t2, NULL , &world, NULL);
pthread_join(t1, NULL);
pthread_join(t2, NULL);
printf("\n"); 
}

