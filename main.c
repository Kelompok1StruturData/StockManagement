#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Produk {
  int kodeproduksi;
  char *kota;
  double harga;
  int stok;
  struct Produk* left;
  struct Produk* right;
} Produk;


typedef struct QueueProduk {
  Produk* node;
  struct QueueProduk* next;
} QueueProduk;


typedef struct Queue {
  QueueProduk* front;
  QueueProduk* rear;
  double priceSum;
  int stokSum;
} Queue;


typedef struct Stock
{
   Queue *queuePurchase;
   Queue *queueSold;
   Queue *queueHPP;
} Stock;


void printAccent(char *message){
  printf("\033[1;36m");
  printf("%s", message);
  printf("\033[0m");
}


void printBold(char *message) {
  printf("\033[1m");
  printf("%s", message);
  printf("\033[0m");
}

void printError(char *message) {
  printf("\033[0;31m");
  printf("%s", message);
  printf("\033[0m");
}

void printSuccess(char *message) {
  printf("\033[0;32m");
  printf("%s", message);
  printf("\033[0m");
}

void printWarning(char *message) {
  printf("\033[0;33m");
  printf("\033[1m");
  printf("\nPeringatan : ");
  printf("\033[0;33m");
  printf("%s", message);
  printf("\033[0m");
}

void printInfo(char *message) {
  printf("\033[0;34m");
  printf("%s", message);
  printf("\033[0m");
}

void clearScreen() {
  system("cls");
}

void pause() {
  system("pause");
}


//-------------------------------------------------------------------
 Produk* create_node(int kodeproduksi, char *kota, double harga, int stok) {
  Produk* new_node = (Produk*)malloc(sizeof(Produk));
  new_node->kodeproduksi = kodeproduksi;
  new_node->kota = kota;
  new_node->harga = harga;
  new_node->stok = stok;
  new_node->left = NULL;
  new_node->right = NULL;
  return new_node;
}

//-------------------------------------------------------------------
QueueProduk* create_queue_node(Produk* node) {
  QueueProduk* new_queue_node = (QueueProduk*)malloc(sizeof(QueueProduk));
  new_queue_node->node = node;
  new_queue_node->next = NULL;
  return new_queue_node;
}

//-------------------------------------------------------------------
Queue* create_queue() {
  Queue* queue = (Queue*)malloc(sizeof(Queue));
  queue->front = NULL;
  queue->rear = NULL;
  queue->stokSum = 0;
  queue->priceSum = 0;
  return queue;
}
//-------------------------------------------------------------------

void saveToFileRecursive(Produk* node, FILE* file) {
  // Base case: jika node kosong, kembali dari rekursi
  if (node == NULL) return;
  // Gunakan traversal in-order untuk mengunjungi node secara terurut
  saveToFileRecursive(node->left, file);
  // Tulis informasi dari node ke file
  fprintf(file, "%d,%s,%0.f,%d\n", node->kodeproduksi, node->kota, node->harga, node->stok);
  saveToFileRecursive(node->right, file);
}

void saveFileProduk(Produk* root) {
  char fileName[11] ="produk.dat";
  // Buka file dengan mode menulis
  FILE* file = fopen(fileName, "w");
  // Jika file tidak dapat dibuka, keluar dari fungsi
  if (file == NULL) {
    printError("Error opening file!\n");
    return;
  }
  // Panggil rekursif untuk menuliskan setiap node ke file
  saveToFileRecursive(root, file);
  // Tutup file setelah selesai menulis
  fclose(file);
}

void savePenjualan(Produk *node, double hpp){
  char fileName[15] ="penjualan.dat";
  // Buka file dengan mode menulis
  FILE* file = fopen(fileName, "a");
  // Jika file tidak dapat dibuka, keluar dari fungsi
  if (file == NULL) {
    printError("Error opening file!\n");
    return;
  }
  // Tambahkan pada baris baru
  fprintf(file, "%d,%s,%0.f,%d,%0.f\n", node->kodeproduksi, node->kota, node->harga, node->stok,hpp);
  // Tutup file setelah selesai menulis
  fclose(file);
}

//-------------------------------------------------------------------
int is_empty(Queue* queue) {
  return queue->front == NULL;
}

//-------------------------------------------------------------------
void enqueue(Queue* queue, Produk* node) {
  QueueProduk* new_queue_node = create_queue_node(node);
  if (is_empty(queue)) {
    queue->front = new_queue_node;
    queue->rear = new_queue_node;
  } else {
    queue->rear->next = new_queue_node;
    queue->rear = new_queue_node;
  }
  queue->priceSum += node->harga * node->stok;
  queue->stokSum += node->stok;
}

//-------------------------------------------------------------------
Produk* dequeue(Queue* queue) {
  if (is_empty(queue)) {
    return NULL;
  }

  Produk* node = queue->front->node;
  queue->priceSum -= node->harga;
  queue->stokSum --;
  node->stok --;
  if (node->stok == 0) {
    QueueProduk* temp = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
  }
  return node;
}

Produk* dequeueAll(Queue* queue) {
  if (is_empty(queue)) {
    return NULL;
  }

  Produk* node = queue->front->node;
  queue->priceSum -= node->harga * node->stok;
  queue->stokSum -= node->stok;
    QueueProduk* temp = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
  
  return node;
}
//-------------------------------------------------------------------
void resetQueue(Queue* queue) {
  while (!is_empty(queue)) {
    dequeueAll(queue);
  }
}

//-------------------------------------------------------------------
void *inorder(Produk* root,Queue *queue) {
  if (root != NULL) {
    inorder(root->left,queue);
    if (root->stok > 0)
      enqueue(queue, root);
    inorder(root->right,queue);
  }
}

//-------------------------------------------------------------------
void refresh_queue(Queue *queue, Produk *root) {
  resetQueue(queue);
  inorder(root, queue);
}


//-------------------------------------------------------------------
void insert(Produk** node, Stock *stock, int kode, char* kota, double harga, int stok)
{
  Produk *produk = *node; 
    if (produk == NULL){
        *node = create_node(kode, kota,harga,stok);
    }
    else
    {
        if (kode <= produk->kodeproduksi){
            insert(&(produk->left),stock, kode, kota, harga, stok);
        }else{
            insert(&(produk->right), stock, kode, kota, harga,stok);
        }   
    }
}


//-------------------------------------------------------------------
void add(Produk** node, Stock *stock, int kode, char* kota, double harga, int stok){
    insert(node,stock, kode, kota, harga,stok);
    refresh_queue(stock->queuePurchase, *node);
    saveFileProduk(*node);
}

//-------------------------------------------------------------------
int checkIsExists(Produk* root, int kodeproduksi) {
  if (root == NULL) {
    return 0;
  }
  
  if (root->kodeproduksi == kodeproduksi) {
    return 1;
  }
  
  int left = checkIsExists(root->left, kodeproduksi);
  int right = checkIsExists(root->right, kodeproduksi);
  return left || right;
}


//-------------------------------------------------------------------

void printQueue(Queue* queue) {
  QueueProduk* temp = queue->front;
  printBold("| \tKode\t | \tKota\t | \tHarga\t | \tStok\t |\n");
  printf("----------------------------------------------------\n");
  while (temp != NULL) {
    printf("| \t%d\t | \t%s\t | \t%0.f\t  | \t%d\t |\n", temp->node->kodeproduksi, temp->node->kota, temp->node->harga,temp->node->stok);
    temp = temp->next;
  }
}

//-------------------------------------------------------------------
Produk *findProduk(Produk *root,int kode){
    if(root == NULL){
        return NULL;
    }
    if(root->kodeproduksi == kode){
        return root;
    }
    if(root->kodeproduksi > kode){
        return findProduk(root->left,kode);
    }else{
        return findProduk(root->right,kode);
    }
}

//-------------------------------------------------------------------

int isExistInQueue(Queue *queue, int kode){
    QueueProduk *temp = queue->front;
    while(temp != NULL){
        if(temp->node->kodeproduksi == kode){
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// -------------------------------------------------------------------
void removeFromTree(Produk *root, Produk *target){
    if (root == NULL) return;
    if (root->left == target){
        root->left = NULL;
    } else if (root->right == target) {
        root->right = NULL;
    } else {
        removeFromTree(root->left, target);
        removeFromTree(root->right, target);
    }
}

//-------------------------------------------------------------------
void sell(Stock *stock, Produk *root, int amount, double price){
  int i;
  int total = 0;
  if(amount > 0){
    for(i=0;i<amount;i++){
      Produk *produk = dequeue(stock->queuePurchase); 
      total++;
      if(produk->stok == 0 || i == amount-1){
        Produk *produkHPP = create_node(produk->kodeproduksi, produk->kota, produk->harga,total);
        enqueue(stock->queueHPP, produkHPP);
        Produk *produkJual = create_node(produk->kodeproduksi, produk->kota, price,total);
        enqueue(stock->queueSold, produkJual);
        savePenjualan(produkJual, produk->harga);
        total = 0;
      }
      
    }
    saveFileProduk(root);
  }
  
}

//-------------------------------------------------------------------
// remove from bst with kode and free the memory
void removeProduk(Produk **root,Stock *stock, int kode) {
  Produk *produk = *root;
  if (produk == NULL) {
    return;
  }
  if (kode < produk->kodeproduksi) {
    removeProduk(&(produk->left),stock, kode);
  } else if (kode > produk->kodeproduksi) {
    removeProduk(&(produk->right),stock, kode);
  } else {
    if (produk->left == NULL && produk->right == NULL) {
      free(produk);
      *root = NULL;
      refresh_queue(stock->queuePurchase, *root);
    } else if (produk->left == NULL) {
      *root = produk->right;
      free(produk);
      refresh_queue(stock->queuePurchase, *root);
    } else if (produk->right == NULL) {
      *root = produk->left;
      free(produk);
      refresh_queue(stock->queuePurchase, *root);
    } else {
      Produk *temp = produk->right;
      while (temp->left != NULL) {
        temp = temp->left;
      }
      produk->kodeproduksi = temp->kodeproduksi;
      produk->harga = temp->harga;
      produk->kota = temp->kota;
      removeProduk(&(produk->right),stock, temp->kodeproduksi);
    }
  }
}

//-------------------------------------------------------------------
void drop(Produk **root,Stock *stock, int kode){
    removeProduk(root,stock, kode);
    refresh_queue(stock->queuePurchase, *root);
    saveFileProduk(*root);
}

Stock *createStock(){
  Stock *stock = (Stock*)malloc(sizeof(Stock));
  stock->queuePurchase = create_queue();
  stock->queueSold = create_queue();
  stock->queueHPP = create_queue();
  return stock;
}

void menuTambah(Produk **root,Stock *stock){
    char *kota = (char*)malloc(sizeof(char)*100);
    int kodeproduksi;
    double harga;
    int stok;
    clearScreen();
    printBold("-----------------------------------------------------");
    printBold("\n                MASUKKAN DATA  : ");
    printBold("\n-----------------------------------------------------\n");
    printInfo("Masukkan Kode Produksi (Angka) : ");
    scanf("%d",&kodeproduksi);
    if(checkIsExists(*root,kodeproduksi)){
        printError("Kode Produksi telah dipakai\n");
        pause();
        return;
    }
    printInfo("Masukkan Kota Asal Produk : ");
    scanf("%s",kota);
    printInfo("Masukkan Harga Beli / Biaya Produksi : ");
    scanf("%lf",&harga);
    printInfo("Masukkan Jumlah Stok : ");
    scanf("%d",&stok);
    add(root,stock,kodeproduksi,kota,harga, stok);
    printSuccess("\nBerhasil Menambahkan Produk\n");
    pause();
}

void menuHapus(Produk **root,Stock *stock){
    if(stock->queuePurchase->stokSum == 0){
        printError("Stok Kosong\n");
        pause();
        return;
    }
    int kodeproduksi;
    clearScreen();
    printBold("--------------------------");
    printBold("\n      HAPUS DATA   ");
    printBold("\n--------------------------\n");
    printInfo("Masukkan Kode Produksi (Angka) : ");
    scanf("%d",&kodeproduksi);
    
    if(checkIsExists(*root,kodeproduksi) == 0){
        printError("Kode Produksi tidak ditemukan\n");
        pause();
        return;
    }else if(isExistInQueue(stock->queueSold,kodeproduksi)){
        printError("Tidak dapat menghapus produk yang telah terjual\n");
        pause();
        return;
    }
    
    char pilihan[1];
    printInfo("Apakah anda ingin menghapus produk ini ? (y/n) : ");
    scanf("%s",pilihan);
    if(strcmp(pilihan,"n") != 0 && strcmp(pilihan,"N")!=0){
      drop(root,stock,kodeproduksi);
      printSuccess("\nBerhasil Menghapus Produk\n");
      pause();  
    }
}

void menuJual(Produk *root,Stock *stock){
  clearScreen();
    if(stock->queuePurchase->stokSum == 0){
        printError("Stok Kosong\n");
        pause();
        return;
    }
    int amount;
    double price;
    clearScreen();
    printBold("--------------------------");
    printBold("\n      JUAL PRODUK     ");
    printBold("\n--------------------------\n");
    printInfo("Masukkan Jumlah Produk yang dijual : ");
    scanf("%d",&amount);
    if(amount > stock->queuePurchase->stokSum){
        printError("Stok tidak mencukupi\n");
        pause();
        return;
    }else if(amount <= 0){
        printError("Jumlah tidak valid\n");
        pause();
        return;
    }
    printInfo("Masukkan Harga Jual : ");
    scanf("%lf",&price);
    char pilihan [1];
    printf("\nKonfirmasi penjualan (y/n)? ");
    scanf("%s",&pilihan);
    if(strcmp(pilihan,"n") != 0 && strcmp(pilihan,"N")!=0){
      sell(stock,root,amount,price);        
      printSuccess("\nBerhasil Menjual Produk\n");
      pause();
    }
}

void menuSisaStok(Stock *stock){
    clearScreen();
    printBold("-----------------------------------------------------");
    printBold("\n         SISA STOK  : ");
    printBold("\n-----------------------------------------------------\n");
    if (stock->queuePurchase->stokSum == 0) {
        printError("Data Kosong\n");
        pause();
        return;
    }
    printQueue(stock->queuePurchase);
    pause();
}

void menuHistoriPenjualan(Stock *stock){
    clearScreen();
    printBold("-----------------------------------------------------");
    printBold("\n         HISTORI PENJUALAN  : ");
    printBold("\n-----------------------------------------------------\n");
    if(stock->queueSold->stokSum == 0){
        printError("Data Kosong\n");
        pause();
        return;
    }
    printQueue(stock->queueSold);
    pause();
}

void menuHistoriHPP(Stock *stock){
    clearScreen();
    printBold("-----------------------------------------------------");
    printBold("\n         HISTORI HPP  : ");
    printBold("\n-----------------------------------------------------\n");
    if(stock->queueHPP->stokSum == 0){
        printError("Data Kosong\n");
        pause();
        return;
    }
    printQueue(stock->queueHPP);
    pause();
}

void menuCariProduk(Produk *root){
    int kodeproduksi;
    clearScreen();
    printBold("-----------------------------------------------------");
    printBold("\n         CARI PRODUK BERDASARAKAN KODE PRODUK  : ");
    printBold("\n-----------------------------------------------------\n");
    printInfo("Masukkan Kode Produk : ");
    scanf("%d",&kodeproduksi);
    Produk *produk = findProduk(root,kodeproduksi);
    if(produk == NULL){
        printError("Kode Produk tidak ditemukan\n");
        pause();
        return;
    }
    printf("Kode Produk : %d\n",produk->kodeproduksi);
    printf("Kota : %s\n",produk->kota);
    printf("Harga Beli / Biaya Produksi : %.0f\n",produk->harga);
    pause();
}

void menuUtama(Produk *root,Stock *stock){
    int pilihan;
    do
    {
      // clearScreen(); 
      printBold("=========================================================================|\n");
      printBold("                       FINAL PROJECT KELOMPOK 1                          |\n");
      printBold("-------------------------------------------------------------------------|\n");
      printBold("                          ANGGOTA KELOMPOK :                             |\n");
      printBold("                                                                         |\n");
      printBold("                  M SULTAN ARDIANSYAH   (19081010174)                    |\n");
      printBold("                  M FADZILLAH ZAIN      (19081010155)                    |\n");
      printBold("                  M HIBATUL HAQQI       (19081010182)                    |\n");
      printBold("                  NOVANDI KEVIN P       (20081010005)                    |\n");
      printBold("                  RIZKY RAMADHAN        (20081010043)                    |\n");
      printBold("=========================================================================|\n");
      printAccent("                                MENU                                     |\n");
      
      printAccent("-------------------------------------------------------------------------|");
      if (stock->queuePurchase->stokSum == 0)
        printWarning("Harap tambahkan stok karena stok kosong\n");
      printf("\n Total Pendapatan : %0.f                                                    |", stock->queueSold->priceSum);
      printf("\n Total HPP : %0.f                                                           |", stock->queueHPP->priceSum);
      printf("\n Total Laba : %0.f                                                          |", stock->queueSold->priceSum - stock->queueHPP->priceSum);
      printf("\n Sisa Stok : %d                                                           |", stock->queuePurchase->stokSum);
      
      printf("\n=========================================================================|\n");
      printAccent(" 1. Tambah Barang di Gudang                                              |\n");
      printAccent(" 2. Hapus Barang di Gudang                                               |\n");
      printAccent(" 3. Cari Barang di Gudang dengan Kode Produksi                           |\n");
      printAccent(" 4. Jual Barang                                                          |\n");
      printAccent(" 5. Daftar Stok                                                          |\n");
      printAccent(" 6. Daftar Penjualan                                                     |\n");
      printAccent(" 7. Daftar HPP                                                           |\n");
      printAccent(" 8. Keluar                                                               |\n");
      printAccent("=========================================================================|\n");
      printAccent(" Masukkan pilihan Anda: ");
      
      scanf("%d",&pilihan);

      switch (pilihan)
      {
          case 1:
            menuTambah(&root,stock);
            break;
          case 2:
            menuHapus(&root,stock);
            break;
          case 3:
            menuCariProduk(root);
            break;
          case 4:
            menuJual(root,stock);
            break;
          case 5:
            menuSisaStok(stock);
            break;
          case 6:
            menuHistoriPenjualan(stock);
            break;
          case 7:
            menuHistoriHPP(stock);
            break;  
          case 8:
            clearScreen();
            printAccent("Terima kasih telah menggunakan aplikasi ini\n");
            pause();
            break;
          default:
            printError("Pilihan tidak tersedia\n");
            pause();
            break;
      }
    } while (pilihan != 8);  
}

void createFileIfNotExists(char *filename){
    FILE *fptr;
    fptr = fopen(filename, "rb+");
    if(fptr == NULL) 
    {
        fptr = fopen(filename, "wb");
    }
    fclose(fptr);
}


void readProdukFromFile(Produk** root, Stock *stock) {
  char * filename = "produk.dat";
  // Buka file dengan mode membaca
  FILE* file = fopen(filename, "r");
  // Jika file tidak dapat dibuka, keluar dari fungsi
  if (file == NULL) {
    printError("Error opening file!\n");
    return;
  }
  // Inisialisasi variabel untuk menyimpan informasi dari file
  int kodeproduksi;
  char *kota = malloc(100);
  double harga;
  int stok;

  // Baca informasi dari file satu baris per satu
  while (fscanf(file, "%d,%[^,],%lf,%d", &kodeproduksi, kota, &harga, &stok) == 4) {
    // Alokasi memori untuk node baru
    insert(root,stock,kodeproduksi,kota,harga,stok);
  }
  refresh_queue(stock->queuePurchase, *root);
  // Tutup file setelah selesai membaca
  fclose(file);
}


void readPenjualan(Stock * stock){
  char * filename = "penjualan.dat";
  // Buka file dengan mode membaca
  FILE* file = fopen(filename, "r");
  // Jika file tidak dapat dibuka, keluar dari fungsi
  if (file == NULL) {
    printError("Error opening file!\n");
    return;
  }
  // Inisialisasi variabel untuk menyimpan informasi dari file
  int kodeproduksi;
  int jumlah;
  char *kota = malloc(100);
  double hargajual;
  double hpp;
  // Baca informasi dari file satu baris per satu
  while (fscanf(file, "%d,%[^,],%lf,%d,%lf", &kodeproduksi, kota, &hargajual, &jumlah, &hpp) == 5) {
    // Alokasi memori untuk node baru
      Produk *produkJual = create_node(kodeproduksi,kota,hargajual,jumlah);
      enqueue(stock->queueSold, produkJual);
      Produk *produkHPP = create_node(kodeproduksi,kota,hpp,jumlah);
      enqueue(stock->queueHPP, produkHPP);
  }
  // Tutup file setelah selesai membaca
  fclose(file);
}

void createDatabase(Produk **root, Stock *stock) {
    createFileIfNotExists("produk.dat");
    createFileIfNotExists("penjualan.dat");
    readProdukFromFile(root,stock);
    readPenjualan(stock);
}


int main()
{
  Produk* root = NULL;
  Stock *stock = createStock();
  createDatabase(&root,stock);
  menuUtama(root,stock);
  return 0;
}
