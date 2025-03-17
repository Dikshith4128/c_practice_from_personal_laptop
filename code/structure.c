#include <stdio.h>

struct Student {
    int id;
    char name[50];
    float marks[3];
};

int main() {
    int n;
    printf("Enter the number of students: ");
    scanf("%d", &n);

    struct Student students[n];

    for (int i = 0; i < n; i++) {
        printf("\nEnter details for student %d\n", i + 1);
        printf("ID: ");
        scanf("%d", &students[i].id);
        printf("Name: ");
        scanf("%s", students[i].name);
        printf("Enter marks in 3 subjects: ");
        for (int j = 0; j < 3; j++) {
            scanf("%f", &students[i].marks[j]);
        }
    }

    printf("\nStudent Details:\n");
    for (int i = 0; i < n; i++) {
        float total = students[i].marks[0] + students[i].marks[1] + students[i].marks[2];
        float avg = total / 3.0;
        printf("ID: %d, Name: %s, Total Marks: %.2f, Average: %.2f\n", students[i].id, students[i].name, total, avg);
    }

    return 0;
}

