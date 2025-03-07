#include <stdio.h>

int main(){

	int arr_2d[5][3];
	int i, j;

	arr_2d[3][1] = 0;
	for (i = 0; i < 5; i++){
		for(j = 0; j < 3; j++){
			printf("Enter for row_num = %d and colum_num = %d\n",i,j);
			scanf("%d",&arr_2d[i][j]);
		}
	}
	printf("\n");
	printf("2d_array ::\n");
	for (i = 0; i < 5; i++){
		for(j = 0; j < 3; j++){
			printf("%d ",arr_2d[i][j]);
		}
	printf("\n");
	}

return 0;
}
