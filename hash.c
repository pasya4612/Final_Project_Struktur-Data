#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXDATA 100
#define MAXDATE 20
#define TABLE_SIZE 1100

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
    newNode->id = id;
    strcpy(newNode->name, name);
    strcpy(newNode->city, city);
    strcpy(newNode->email, email);
    strcpy(newNode->date, date);
    newNode->next = NULL;
    return newNode;
}

int id_ada(int id){
    unsigned int index = hash(id);
    data *curr = hash_table[index];
    while (curr){
        if (curr->id == id){
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

void load_data(const char *filename){
    FILE *file = fopen(filename, "r");
    if (!file){
        printf("File tidak ditemukan.\n");
        return;
    }
    char line[500];
    fgets(line, sizeof(line), file); 

    int inserted = 0;
    while (fgets(line, sizeof(line), file)){
        char name[MAXDATA], city[MAXDATA], email[MAXDATA], date[MAXDATE];
        char id_str[20];
        int id;

        int fields = sscanf(line, "%[^,],%[^,],%[^,],%[^,],%[^\n]", name, id_str, city, date, email);
        if (fields != 5){
            continue;
        }
        id = atoi(id_str);
        if (id_ada(id)) {
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
    printf("%d data dimuat dari %s.\n", inserted, filename);
}

void cari_data(){
    char input[20];
    printf("\nCari Data Berdasarkan ID \nMasukkan ID: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    int id = atoi(input);

    unsigned int index = hash(id);
    int kompleks = 0;
    data *curr = hash_table[index];

    while (curr){
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

    printf("Data dengan ID %d tidak ditemukan (kompleksitas: %d).\n", id, kompleks);
}

void update_data(){
    char input[20];
    printf("\nUpdate Data \nMasukkan ID: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    int id = atoi(input);

    unsigned int index = hash(id);
    data *curr = hash_table[index];
    int kompleks = 0;

    while (curr){
        kompleks++;
        if (curr->id == id){
            printf("Data ditemukan (kompleksitas: %d). Masukkan data baru:\n", kompleks);

            printf("Nama baru: ");
            fgets(curr->name, MAXDATA, stdin);
            curr->name[strcspn(curr->name, "\n")] = 0;

            printf("Kota baru: ");
            fgets(curr->city, MAXDATA, stdin);
            curr->city[strcspn(curr->city, "\n")] = 0;

            printf("Email baru: ");
            fgets(curr->email, MAXDATA, stdin);
            curr->email[strcspn(curr->email, "\n")] = 0;

            printf("Tanggal lahir baru: ");
            fgets(curr->date, MAXDATE, stdin);
            curr->date[strcspn(curr->date, "\n")] = 0;

            printf("Data berhasil diperbarui.\n");
            return;
        }
        curr = curr->next;
    }

    printf("Data dengan ID %d tidak ditemukan (kompleksitas: %d).\n", id, kompleks);
}

void hapus_data(){
    char input[20];
    printf("\nHapus Data \nMasukkan ID: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    int id = atoi(input);

    unsigned int index = hash(id);
    data *curr = hash_table[index];
    data *prev = NULL;
    int kompleks = 0;

    while (curr){
        kompleks++;
        if (curr->id == id){
            if (prev) prev->next = curr->next;
            else hash_table[index] = curr->next;

            free(curr);
            count--;

            printf("Data dengan ID %d berhasil dihapus (kompleksitas: %d).\n", id, kompleks);
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    printf("Data dengan ID %d tidak ditemukan (kompleksitas: %d).\n", id, kompleks);
}

void tampilkan_data(){
    for (int i = 0; i < TABLE_SIZE; i++){
        data *curr = hash_table[i];
        while (curr){
            printf("\n- ID              : %d\n", curr->id);
            printf("  Nama            : %s\n", curr->name);
            printf("  Kota            : %s\n", curr->city);
            printf("  Email           : %s\n", curr->email);
            printf("  Tanggal Lahir   : %s\n", curr->date);
            curr = curr->next;
        }
    }
    printf("\nTotal data : %d \n", count);

}

void clear(){
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
    int cmd;
    char filename[100];
    init();

    printf("\nPROGRAM HASH MAP DATA\n");

    while (1){
        printf("\nMENU:\n");
        printf("1. Muat Data dari File CSV\n");
        printf("2. Cari Data berdasarkan ID\n");
        printf("3. Tampilkan Semua Data\n");
        printf("4. Update Data\n");
        printf("5. Hapus Data\n");
        printf("6. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &cmd);
        while (getchar() != '\n');

        switch (cmd){
            case 1:
                printf("Masukkan nama file CSV: ");
                scanf("%s", filename);
                while (getchar() != '\n');
                load_data(filename);
                break;
            case 2:
                cari_data();
                break;
            case 3:
                tampilkan_data();
                break;
            case 4:
                update_data();
                break;
            case 5:
                hapus_data();
                break;
            case 6:
                clear();
                printf("Program selesai.\n");
                return 0;
            default:
                printf("Pilihan tidak valid.\n");
        }
    }
}
