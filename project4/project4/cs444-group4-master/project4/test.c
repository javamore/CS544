#include <stdio.h>
#include <unistd.h>

#define mem_claimed syscall(347)
#define mem_free syscall(348)

int main(){
	int x;
	float frag;

	printf("Running 5 tests:\n");

	for (x=0;x<5;x++){
		frag = (float)mem_free / (float)mem_claimed;
		printf("CLAIMED: %lu\n", mem_claimed);
		printf("FREE: %lu\n", mem_free);
		printf("FRAGMENTATION: %f\n", frag);
		printf("-----\n");
		sleep(1);
	}	


}
