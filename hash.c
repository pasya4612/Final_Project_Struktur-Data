#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXDATA 100
#define MAXDATE 20
#define TABLE_SIZE 1009

typedef struct data{
    int id;
    char name[MAXDATA];
    char city[MAXDATA];
    char email[MAXDATA];
    char date[MAXDATE];
    struct data *next;
} data;

data *hash_table[TABLE_SIZE];
int count = 0;

unsigned int hash(int key){
    return key % TABLE_SIZE;
}

data *createNode(int id, const char *name, const char *city, const char *email, const char *date){
    data *newNode = (data *)malloc(sizeof(data));
    if (newNode == NULL) {
        printf("Gagal.\n");
        exit(1);
    }
    newNode->id = id;
    strcpy(newNode->name, name);
    strcpy(newNode->city, city);
    strcpy(newNode->email, email);
    strcpy(newNode->date, date);
    newNode->next = NULL;
    return newNode;
}

int idExists(int id){
    unsigned int index = hash(id);
    data *curr = hash_table[index];
    while (curr != NULL){
        if (curr->id == id){
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

void loadData(const char *filename){
    FILE *file = fopen(filename, "r");
    char line[500];
    fgets(line, sizeof(line), file); 

    int inserted = 0;
    while (fgets(line, sizeof(line), file)){
        char name[MAXDATA], city[MAXDATA], email[MAXDATA], date[MAXDATE];
        char id_str[20];
        int id;

        int fields = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^\n]",
                            name, id_str, city, date, email);
        if (fields != 5){
            continue;
        }

        id = atoi(id_str);

        if (idExists(id)){
            continue;
        }
        unsigned int index = hash(id);
        data *newNode = createNode(id, name, city, email, date);
        newNode->next = hash_table[index];
        hash_table[index] = newNode;

        count++;
        inserted++;
    }

    fclose(file);
    printf("%d data dimuat dari %s (tanpa duplikasi).\n", inserted, filename);
}

void cariData(){
    char input[20];
    printf("\n=== Cari Data Berdasarkan ID ===\n");
    printf("Masukkan ID: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    int id = atoi(input);

    unsigned int index = hash(id);
    int kompleks = 0;
    data *curr = hash_table[index];

    while (curr != NULL){
        kompleks++;
        if (curr->id == id){
            printf("\nData ditemukan (kompleksitas: %d)\n", kompleks);
            printf("ID              : %d\n", curr->id);
            printf("Nama            : %s\n", curr->name);
            printf("Kota            : %s\n", curr->city);
            printf("Email           : %s\n", curr->email);
            printf("Tanggal Lahir   : %s\n", curr->date);
            return;
        }
        curr = curr->next;
    }

    printf("Data dengan ID %d tidak ditemukan (kompleksitas: %d)\n", id, kompleks);
}

void tampilkanSemuaData(){
    printf("\n=== Semua Data (Total: %d) ===\n", count);
    int total = 0;
    for (int i = 0; i < TABLE_SIZE; i++){
        data *curr = hash_table[i];
        while (curr != NULL){
            printf("\n- ID              : %d\n", curr->id);
            printf("  Nama            : %s\n", curr->name);
            printf("  Kota            : %s\n", curr->city);
            printf("  Email           : %s\n", curr->email);
            printf("  Tanggal Lahir   : %s\n", curr->date);
            curr = curr->next;
            total++;
        }
    }
    printf("\nTotal data: %d\n", total);
}

void cleanup(){
    for (int i = 0; i < TABLE_SIZE; i++){
        data *curr = hash_table[i];
        while (curr){
            data *next = curr->next;
            free(curr);
            curr = next;
        }
        hash_table[i] = NULL;
    }
}

void init(){
    for (int i = 0; i < TABLE_SIZE; i++){
        hash_table[i] = NULL;
    }
    count = 0;
}

int main(){
    int choice;
    char filename[100];
    init();

    printf("PROGRAM HASH MAP DATA\n");

    while (1) {
        printf("\nMENU  :\n");
        printf("1. Muat Data dari File CSV\n");
        printf("2. Cari Data berdasarkan ID\n");
        printf("3. Tampilkan Data\n");
        printf("4. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &choice);
        while (getchar() != '\n');

        switch (choice){
            case 1:
                printf("Masukkan nama file CSV: ");
                scanf("%s", filename);
                while (getchar() != '\n');
                loadData(filename);
                break;
            case 2:
                cariData();
                break;
            case 3:
                tampilkanSemuaData();
                break;
            case 4:
                cleanup();
                printf("Program selesai.\n");
                return 0;
            default:
                printf("Pilihan tidak valid.\n");
        }
    }

    return 0;
}
