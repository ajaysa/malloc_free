#include<stdio.h>

int main()
{
	int *data;
	data = (int *)my_malloc(sizeof(int)*10);

	printf("memory location = %u\n", data);

	data[0] = 0;
	data[9] = 9;

	int *data2 = (int *)my_malloc(sizeof(int)*10);
	printf("New memory location = %u\n", data2);

	data2[0] = 100;
	data2[9] = 900;

	printf("data1 = %d and data2 = %d\n", data[9], data2[9]);

	my_free(data);
	my_free(data2);
	return 0;
}
