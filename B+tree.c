#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MIN_DEGREE 3
#define MAX_NAME 50
#define MAX_CITY 50
#define MAX_EMAIL 50
#define MAX_DATE 20

typedef struct {
    int id;
    char name[MAX_NAME];
    char city[MAX_CITY];
    char date[MAX_DATE];
    char email[MAX_EMAIL];
} Mahasiswa;

typedef struct BPTreeNode {
    int keys[2 * MIN_DEGREE - 1];
    Mahasiswa *data[2 * MIN_DEGREE - 1];
    struct BPTreeNode *children[2 * MIN_DEGREE];
    struct BPTreeNode *next;
    int leaf;
    int n;
} BPTreeNode;

BPTreeNode *root = NULL;

BPTreeNode* createNode(int leaf) {
    BPTreeNode *node = (BPTreeNode*)malloc(sizeof(BPTreeNode));
    node->leaf = leaf;
    node->n = 0;
    node->next = NULL;
    for (int i = 0; i < 2 * MIN_DEGREE; i++) node->children[i] = NULL;
    for (int i = 0; i < 2 * MIN_DEGREE - 1; i++) node->data[i] = NULL;
    return node;
}

typedef void (*Callback)(Mahasiswa*, int*);
void traverse(BPTreeNode *node, Callback cb, int *j) {
    if (node == NULL) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            traverse(node->children[i], cb, j);
        cb(node->data[i], j);
        (*j)++;
    }
    if (!node->leaf)
        traverse(node->children[i], cb, j);
}

void splitChild(BPTreeNode *parent, int i) {
    BPTreeNode *y = parent->children[i];
    BPTreeNode *z = createNode(y->leaf);
    z->n = MIN_DEGREE - 1;

    for (int j = 0; j < MIN_DEGREE - 1; j++) {
        z->keys[j] = y->keys[j + MIN_DEGREE];
        z->data[j] = y->data[j + MIN_DEGREE];
    }

    if (!y->leaf) {
        for (int j = 0; j < MIN_DEGREE; j++)
            z->children[j] = y->children[j + MIN_DEGREE];
    } else {
        z->next = y->next;
        y->next = z;
    }

    y->n = MIN_DEGREE - 1;

    for (int j = parent->n; j >= i + 1; j--)
        parent->children[j + 1] = parent->children[j];
    parent->children[i + 1] = z;

    for (int j = parent->n - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j];
        parent->data[j + 1] = parent->data[j];
    }
    parent->keys[i] = y->keys[MIN_DEGREE - 1];
    parent->data[i] = y->data[MIN_DEGREE - 1];
    parent->n++;
}


void insertNonFull(BPTreeNode *node, Mahasiswa *mhs) {
    int i = node->n - 1;
    if (node->leaf) {
        while (i >= 0 && mhs->id < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->data[i + 1] = node->data[i];
            i--;
        }
        node->keys[i + 1] = mhs->id;
        node->data[i + 1] = mhs;
        node->n++;
    } else {
        while (i >= 0 && mhs->id < node->keys[i])
            i--;
        i++;
        if (node->children[i]->n == 2 * MIN_DEGREE - 1) {
            splitChild(node, i);
            if (mhs->id > node->keys[i])
                i++;
        }
        insertNonFull(node->children[i], mhs);
    }
}

void insert(Mahasiswa *mhs) {
    if (root == NULL) {
        root = createNode(1);
        root->keys[0] = mhs->id;
        root->data[0] = mhs;
        root->n = 1;
    } else {
        if (root->n == 2 * MIN_DEGREE - 1) {
            BPTreeNode *s = createNode(0);
            s->children[0] = root;
            splitChild(s, 0);
            int i = 0;
            if (mhs->id > s->keys[0])
                i++;
            insertNonFull(s->children[i], mhs);
            root = s;
        } else {
            insertNonFull(root, mhs);
        }
    }
}

Mahasiswa* search(BPTreeNode *node, int id) {
    int i = 0;
    while (i < node->n && id > node->keys[i])
        i++;
    if (i < node->n && id == node->keys[i])
        return node->data[i];
    if (node->leaf)
        return NULL;
    return search(node->children[i], id);
}

int deletedata(BPTreeNode *node, int id) {
    for (int i = 0; i < node->n; i++) {
        if (node->keys[i] == id) {
            free(node->data[i]);
            for (int j = i; j < node->n - 1; j++) {
                node->keys[j] = node->keys[j + 1];
                node->data[j] = node->data[j + 1];
            }
            node->n--;
            return 1;
        }
    }
    if (node->leaf) return 0;
    for (int i = 0; i <= node->n; i++) {
        if (deletedata(node->children[i], id)) return 1;
    }
    return 0;
}

void printMahasiswa(Mahasiswa *mhs, int *j) {
    printf("%d. ID: %d | Nama: %s | Kota: %s | Tanggal Lahir: %s | Email: %s\n",
           *j, mhs->id, mhs->name, mhs->city, mhs->date, mhs->email);
}

typedef char str100[100];
void loadCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Gagal membuka file %s\n", filename);
        return;
    }

    char line[256];
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        Mahasiswa *mhs = (Mahasiswa*)malloc(sizeof(Mahasiswa));
        sscanf(line, "%49[^,],%d,%49[^,],%19[^,],%49[^\n]",
               mhs->name, &mhs->id, mhs->city, mhs->date, mhs->email);
        insert(mhs);
    }
    fclose(file);
    printf("Data berhasil dimuat!\n");
}

void searchRange(BPTreeNode *node, int low, int high) {
    if (node == NULL) return;

    int i = 0;
    while (!node->leaf) {
        while (i < node->n && low > node->keys[i])
            i++;
        node = node->children[i];
        i = 0;
    }

    int done = 0;
    while (node && !done) {
        for (i = 0; i < node->n; i++) {
            if (node->keys[i] > high) {
                done = 1;
                break;
            }
            if (node->keys[i] >= low)
                printf("ID: %d | Nama: %s\n", node->keys[i], node->data[i]->name);
        }
        node = node->next;
    }
}


int main() {
    int pilihan;
    int id;
    char buffer[100];

    do {
        printf("\n=== Menu ===\n");
        printf("1. Muat Data dari File CSV\n");
        printf("2. Tambah Data Baru\n");
        printf("3. Cari Data berdasarkan ID\n");
        printf("4. Tampilkan Semua Data\n");
        printf("5. Update Data\n");
        printf("6. Hapus Data\n");
        printf("7. Search Range ID\n");
        printf("8. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);
        getchar();

        if(pilihan == 1){
            printf("Masukkan Nama Data: ");
            scanf("%s", buffer);
            loadCSV(buffer);
        } else if (pilihan == 2) {
            Mahasiswa *mhs = (Mahasiswa*)malloc(sizeof(Mahasiswa));
            printf("Nama: "); fgets(mhs->name, MAX_NAME, stdin); strtok(mhs->name, "\n");
            printf("ID: "); scanf("%d", &mhs->id); getchar();
            printf("Kota: "); fgets(mhs->city, MAX_CITY, stdin); strtok(mhs->city, "\n");
            printf("Tanggal Lahir: "); fgets(mhs->date, MAX_DATE, stdin); strtok(mhs->date, "\n");
            printf("Email: "); fgets(mhs->email, MAX_EMAIL, stdin); strtok(mhs->email, "\n");
            insert(mhs);
            printf("Mahasiswa berhasil ditambahkan.\n");
        } else if (pilihan == 3) {
            printf("Masukkan ID Mahasiswa: ");
            scanf("%d", &id);
            Mahasiswa *mhs = search(root, id);
            if (mhs)
                printf("Ditemukan: %s, %s, %s, %s\n", mhs->name, mhs->city, mhs->date, mhs->email);
            else
                printf("Mahasiswa tidak ditemukan.\n");
        } else if (pilihan == 4) {
            int j = 1;
            traverse(root, printMahasiswa, &j);
        } else if (pilihan == 5) {
            printf("Masukkan ID Mahasiswa yang ingin diupdate: ");
            scanf("%d", &id); getchar();
            Mahasiswa *mhs = search(root, id);
            if (mhs) {
                printf("Nama baru: "); fgets(mhs->name, MAX_NAME, stdin); strtok(mhs->name, "\n");
                printf("Kota baru: "); fgets(mhs->city, MAX_CITY, stdin); strtok(mhs->city, "\n");
                printf("Tanggal Lahir baru: "); fgets(mhs->date, MAX_DATE, stdin); strtok(mhs->date, "\n");
                printf("Email baru: "); fgets(mhs->email, MAX_EMAIL, stdin); strtok(mhs->email, "\n");
                printf("Data berhasil diupdate.\n");
            } else {
                printf("Mahasiswa tidak ditemukan.\n");
            }
        } else if (pilihan == 6) {
            printf("Masukkan ID Mahasiswa yang ingin dihapus: ");
            scanf("%d", &id);
            if (deletedata(root, id))
                printf("Mahasiswa berhasil dihapus.\n");
            else
                printf("Mahasiswa tidak ditemukan.\n");
        } else if (pilihan == 7) {
            int low, high;
            printf("Masukkan batas bawah ID: ");
            scanf("%d", &low);
            printf("Masukkan batas atas ID: ");
            scanf("%d", &high);
            searchRange(root, low, high);
        }
    } while (pilihan != 8);

    return 0;
}
