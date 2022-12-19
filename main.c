#include <stdio.h>
#include <stdlib.h>

typedef struct Produk {
  int kodeproduksi;
  char *kota;
  double harga;
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
} Queue;

typedef struct Stock
{
   Queue *queuePurchase;
   Queue *queueSold;
   Queue *queueHPP;
} Stock;

Produk* create_node(int kodeproduksi, char *kota, double harga) {
  Produk* new_node = (Produk*)malloc(sizeof(Produk));
  new_node->kodeproduksi = kodeproduksi;
  new_node->kota = kota;
  new_node->harga = harga;
  new_node->left = NULL;
  new_node->right = NULL;
  return new_node;
}

QueueProduk* create_queue_node(Produk* node) {
  QueueProduk* new_queue_node = (QueueProduk*)malloc(sizeof(QueueProduk));
  new_queue_node->node = node;
  new_queue_node->next = NULL;
  return new_queue_node;
}

Queue* create_queue() {
  Queue* queue = (Queue*)malloc(sizeof(Queue));
  queue->front = NULL;
  queue->rear = NULL;
  queue->priceSum = 0;
  return queue;
}

int is_empty(Queue* queue) {
  return queue->front == NULL;
}

void enqueue(Queue* queue, Produk* node) {
  QueueProduk* new_queue_node = create_queue_node(node);
  if (is_empty(queue)) {
    queue->front = new_queue_node;
    queue->rear = new_queue_node;
    queue->priceSum += node->harga;
  } else {
    queue->rear->next = new_queue_node;
    queue->rear = new_queue_node;
    queue->priceSum += node->harga;
  }
}

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

void resetQueue(Queue* queue) {
  while (!is_empty(queue)) {
    dequeue(queue);
  }
}

void *inorder(Produk* root,Queue *queue) {
  if (root != NULL) {
    inorder(root->left,queue);
    enqueue(queue, root);
    inorder(root->right,queue);
  }
}


void refresh_queue(Queue *queue, Produk *root) {
  resetQueue(queue);
  inorder(root, queue);
}

void insert(Produk** node, Stock *stock, int kode, char* kota, double harga)
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

void add(Produk** node, Stock *stock, int kode, char* kota, double harga){
    insert(node,stock, kode, kota, harga);
    refresh_queue(stock->queuePurchase, *node);
}

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



void printQueue(Queue* queue) {
  QueueProduk* temp = queue->front;
  while (temp != NULL) {
    printf("%d %s %0.f\n", temp->node->kodeproduksi, temp->node->kota, temp->node->harga);
    temp = temp->next;
  }
}

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

void drop(Produk **root,Stock *stock, int kode){
    removeProduk(root,stock, kode);
    refresh_queue(stock->queuePurchase, *root);
}

Stock *createStock(){
  Stock *stock = (Stock*)malloc(sizeof(Stock));
  stock->queuePurchase = create_queue();
  stock->queueSold = create_queue();
  stock->queueHPP = create_queue();
  return stock;
}

int main() {
  Produk* root = NULL;
  Stock *stock = createStock();

  // insert Production
  add(&root,stock, 10,"Jakarta",12001);
  add(&root,stock, 12,"Jakarta",1250);
  add(&root,stock, 8,"Jakarta",1100);
  add(&root,stock, 190,"Jakarta",1200);
  add(&root,stock, 9,"Jakarta",1400);
  add(&root,stock, 7,"Jakarta",1250);
  add(&root,stock, 11,"Jakarta",1200);
  add(&root,stock, 123,"Jakarta",1300);
  
  // Drop Production
  drop(&root,stock, 11);
  
  // Sell Item
  sell(stock,root,3,1500);
  
  // Print Histori
  printf("Daftar Produksi\n");
  printQueue(stock->queuePurchase);
  
  printf("\nDaftar Penjualan\n");
  printQueue(stock->queueSold);

  printf("\nDaftar HPP\n");
  printQueue(stock->queueHPP);
  
// Mencari Stok di Produksi
  Produk *produk = findProduk(root, 10);
  if (produk != NULL) {
    printf("\nProduk Ditemukan\n");
    printf("%d %s %0.f\n", produk->kodeproduksi, produk->kota, produk->harga);
  } else {
    printf("\nProduk Tidak Ditemukan\n");
  }

  //Rekap 
  printf("\nTotal Pendapatan : %0.f", stock->queueSold->priceSum);
  printf("\nTotal HPP : %0.f", stock->queueHPP->priceSum);
  printf("\nTotal Laba : %0.f", stock->queueSold->priceSum - stock->queueHPP->priceSum);
  return 0;
}

