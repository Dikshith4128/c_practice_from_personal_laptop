#include <stdio.h>

int main() {
    FILE *fp;
    char item[50];
    int quantity;
    float price, total;
    
    fp = fopen("invoice.txt", "w"); 
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    
    fprintf(fp, "Invoice\n");
    fprintf(fp, "-----------------------------\n");
    fprintf(fp, "Item\tQuantity\tPrice\tTotal\n");
    
    for (int i = 0; i < 2; i++) { 
        printf("Enter item name: ");
        scanf("%s", item);
        printf("Enter quantity: ");
        scanf("%d", &quantity);
        printf("Enter price: ");
        scanf("%f", &price);
        
        total = quantity * price;
        fprintf(fp, "%s\t%d\t%.2f\t%.2f\n", item, quantity, price, total);
    }
    
    fprintf(fp, "-----------------------------\n");
    fclose(fp);
    
    printf("Invoice generated successfully in invoice.txt\n");
    return 0;
}
