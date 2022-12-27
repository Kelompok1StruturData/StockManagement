# Pengelolaan Stok Gudang Dengan Metode FIFO
![Gudang](https://github.com/Kelompok1StruturData/StockManagement/blob/main/assets/doc.png?raw=true)
[Image by macrovector on Freepik](https://www.freepik.com/free-vector/warehouse-industrial-space-storage-pick-pack-orders-shipping-delivering-logistic-services-isometric-web-banner_13869096.htm#query=warehouse&position=11&from_view=search&track=sph)
### Anggota
1. 	M. Sultan Ardiansyah    (19081010174)
2. 	M. Fadzillah Zain    (19081010155)
3. 	M. Hibatul Haqqi    (19081010182)
4. 	Novandi Kevin P    (20081010005)
5.   Rizky Ramadhan    (20081010043)
### Algoritma
Algoritma yang digunakan pada studi kasus
- Binary Search Tree
- Queue
### Studi Kasus
Pengelolaan stok barang dengan metode FIFO *(First In First Out)* adalah pengelolaan stok yang memiliki aturan barang yang memiliki kode produksi paling kecil adalah barang yang masuk pertama dan dikeluarkan pertama. Barang-barang tersebut membentuk suatu antrean. Setiap barang memiliki spesifikasi sebagai berikut :
- Kode Produksi (Berbentuk Angka dan bersifat **unik**)
- Kota Asal
- Harga
- Jumlah Stok
- Antrean barang dapat  terdiri dari beberapa barang bersifat dinamis. Antrean barang berurutan berdasarkan kode produksi untuk mempermudah melacak HPP
- Selain itu, kode Produksi yang pernah diproduksi tidak dapat diproduksi kembali. Barang yang telah dijual tidak dapat dihapus namun jika belum dijual dapat dihapus. 
- Barang dijual secara berurutan dari kode produksi yang  paling kecil terlebih dahulu.
- Data disimpan menjadi file sehingga ketika program ditutup, dapat menggunakan data sebelumnya.