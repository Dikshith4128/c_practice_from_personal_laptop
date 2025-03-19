#include <stdio.h>
#include <string.h>
#pragma pack(8)

typedef struct student{
    char name[20];
    char usn[10];
    long long int mob_no;
    int age;
    float cgpa;
}SD;


int main (){

    SD a[10];

    strcpy(a[0].name,"UGO");
    strcpy(a[0].usn,"1AH20CS001");
    a[0].mob_no = 8073999562;
    a[0].age = 22;
    a[0].cgpa = 8.9;


    strcpy(a[1].name,"UGO1");
    strcpy(a[1].usn,"1AH20CS002");
    a[1].mob_no = 8073999562;
    a[1].age = 22;
    a[1].cgpa = 8.9;


    strcpy(a[2].name,"UGO2");
    strcpy(a[2].usn,"1AH20CS003");
    a[2].mob_no = 8073999562;
    a[2].age = 22;
    a[2].cgpa = 8.9;


    strcpy(a[3].name,"UGO3");
    strcpy(a[3].usn,"1AH20CS004");
    a[3].mob_no = 8073999562;
    a[3].age = 22;
    a[3].cgpa = 8.9;


    strcpy(a[4].name,"UGO4");
    strcpy(a[4].usn,"1AH20CS005");
    a[4].mob_no = 8073999562;
    a[4].age = 22;
    a[4].cgpa = 8.9;


    strcpy(a[5].name,"UGO");
    strcpy(a[5].usn,"1AH20CS006");
    a[5].mob_no = 8073999562;
    a[5].age = 22;
    a[5].cgpa = 8.9;


    strcpy(a[6].name,"UGO");
    strcpy(a[6].usn,"1AH20CS007");
    a[6].mob_no = 8073999562;
    a[6].age = 22;
    a[6].cgpa = 8.9;


    strcpy(a[7].name,"UGO");
    strcpy(a[7].usn,"1AH20CS008");
    a[7].mob_no = 8073999562;
    a[7].age = 22;
    a[7].cgpa = 8.9;


    strcpy(a[8].name,"UGO");
    strcpy(a[8].usn,"1AH20CS009");
    a[8].mob_no = 8073999562;
    a[8].age = 22;
    a[8].cgpa = 8.9;

 

    strcpy(a[9].name,"UGO");
    strcpy(a[9].usn,"1AH20CS010");
    a[9].mob_no = 8073999562;
    a[9].age = 22;
    a[9].cgpa = 8.9;

 

    printf("Enter the name  to find\n");
    char find[10];
    int j = 0;
    scanf("%s",find);


    for(int i = 0; i < 10; i++){
        if(strcmp(a[i].name,find)==0){
            printf("USN_EXISTS\n");
            printf("NAME:%s\n",a[i].name);
            printf("USN:%s\n",a[i].usn);
            printf("MOB_NO:%lld\n",a[i].mob_no);
            printf("AGE:%d\n",a[i].age);
            printf("CGPA:%0.1f\n",a[i].cgpa);
            printf("\n");
            j++;
            //break;
        }
    }

    printf("%ld\n",sizeof(SD));
if(j==0)
    printf("USN_NOT_IN_STRUCT\n");
else
    printf("%d students with same name %s\n", j, find);
return 0;
}
