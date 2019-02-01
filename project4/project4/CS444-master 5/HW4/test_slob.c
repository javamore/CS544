#include <stdio.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
int main(){
	srand(time(NULL));
	/*
	int i = 0;
	int** p = (int**)malloc(sizeof(int*)*5000);
	int val = 0;

	for(i=0;i<5000;i++){
		val = rand()%100+50;
		p[i] = (int*)malloc(sizeof(int)*val);
	}
	for(i=0;i<5000;i++){
		free(p[i]);
		val = rand()%200+100;
		p[i] = (int*)malloc(sizeof(int)*val);
	}
	*/

	// 353: get total pages size
	long int total_page_size = syscall(353);
	if( total_page_size == -1)
 		printf("Hull Breach!");
	else
		printf("Total page memory size: %ld\n", total_page_size);
	// 354: get total mem unused
	long int total_mem_unused = syscall(354);
	if (total_mem_unused == -1)
		printf("Hull breach!");
	else 
		printf("Total memory holes: %ld\n", total_mem_unused);

	printf("Total Fragmentation: %.2f \% \n", 
		(100*(float)total_mem_unused/total_page_size)); 
/*
	for(i=0;i<5000;i++){
		free(p[i]);
	}
	free(p);
*/
	return 0;
}
