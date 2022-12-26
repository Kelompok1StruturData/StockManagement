#include <stdio.h>
#include <stdlib.h>

typedef struct Produk {
  int kodeproduksi;
  char *kota;
  double harga;
  double stok;
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
  double stokSum;
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
  printf("Peringatan : ");
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
 Produk* create_node(int kodeproduksi, char *kota, double harga) {
  Produk* new_node = (Produk*)malloc(sizeof(Produk));
  new_node->kodeproduksi = kodeproduksi;
  new_node->kota = kota;
  new_node->harga = harga;
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
  queue->priceSum += node->harga;
}

//-------------------------------------------------------------------
Produk* dequeue(Queue* queue) {
  if (is_empty(queue)) {
    return NULL;
  }

  Produk* node = queue->front->node;
  queue->priceSum -= node->harga;
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
    dequeue(queue);
  }
}

//-------------------------------------------------------------------
void inorder(Produk root,Queue *queue) {
  if (root != NULL) {
    inorder(root->left,queue);
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
void insert(Produk** node, Stock stock, int kode, char kota, double harga)
{
  Produk *produk = *node; 
    if (produk == NULL){
        *node = create_node(kode, kota,harga);
    }
    else
    {
        if (kode <= produk->kodeproduksi){
            insert(&(produk->left),stock, kode, kota, harga);
        }else{
            insert(&(produk->right), stock, kode, kota, harga);
        }   
    }
}


//-------------------------------------------------------------------
void add(Produk** node, Stock stock, int kode, char kota, double harga){
    insert(node,stock, kode, kota, harga);
    refresh_queue(stock->queuePurchase, *node);
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
  printBold("| \tKode\t | \tKota\t | \tHarga\t |\n");
  printf("----------------------------------------------------\n");
  while (temp != NULL) {
    printf("| \t%d\t | \t%s\t | \t%0.f\t |\n", temp->node->kodeproduksi, temp->node->kota, temp->node->harga);
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
void sell(Stock *stock, Produk *root, int amount, int price){
  int i;
  for(i=0;i<amount;i++){
    Produk *produk = dequeue(stock->queuePurchase); 
    enqueue(stock->queueHPP, produk);
    Produk *produk2 = create_node(produk->kodeproduksi, produk->kota, price);
    enqueue(stock->queueSold, produk2);
    removeFromTree(root, produk);
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
}

Stock *createStock(){
  Stock stock = (Stock)malloc(sizeof(Stock));
  stock->queuePurchase = create_queue();
  stock->queueSold = create_queue();
  stock->queueHPP = create_queue();
  return stock;
}

void menuTambah(Produk **root,Stock *stock){
    char kota = (char)malloc(sizeof(char)*100);
    int kodeproduksi;
    double harga;
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
    add(root,stock,kodeproduksi,kota,harga);
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
    Produk *produk = findProduk(*root,kodeproduksi);
    if(produk == NULL){
        printError("Kode Produksi tidak ditemukan\n");
        pause();
        return;
    }
    char pilihan[1];
    printInfo("Apakah anda ingin menjual produk ini ? (y/n) : ");
    scanf("%s",pilihan);
    if(pilihan != "n" && pilihan != "N"){
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
    int price;
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
    }
    printInfo("Masukkan Harga Jual : ");
    scanf("%d",&price);
    char pilihan [1];
    printf("\nKonfirmasi penjualan (y/n)? ");
    scanf("%s",&pilihan);
    if(pilihan != "n" && pilihan != "N"){
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
      clearScreen();
      
      
      printBold("=========================================================================|\n");
      printBold("                       FINAL PROJECT KELOMPOK 1                          |\n");
      printBold("-------------------------------------------------------------------------|\n");
      printBold("                          ANGGOTA KELOMPOK :                             |\n");
      printBold("                                                                         |\n");
      printBold("                  M SULTAN ARDIANSYAH   (19081010174)                    |\n");
      printBold("                  NOVANDI KEVIN P       (20081010005)                    |\n");
      printBold("                  RIZKY RAMADHAN        (20081010043)                    |\n");
      printBold("=========================================================================|\n");
      printAccent("                                MENU                                     |\n");
      
      printAccent("-------------------------------------------------------------------------|");
      if (stock->queuePurchase->stokSum == 0)
        printf("\n");
        printWarning("Harap tambahkan stok karena stok kosong\n");
      printf("\n Total Pendapatan : %0.f                                                    |", stock->queueSold->priceSum);
      printf("\n Total HPP : %0.f                                                           |", stock->queueHPP->priceSum);
      printf("\n Total Laba : %0.f                                                          |", stock->queueSold->priceSum - stock->queueHPP->priceSum);
      printf("\n Sisa Stok : %0.f                                                           |", stock->queuePurchase->stokSum);
      
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


int main()
{
  Produk* root = NULL;
  Stock *stock = createStock();
  menuUtama(root,stock);
  return 0;
}
