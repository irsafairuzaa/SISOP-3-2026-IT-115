# LAPORAN PRAKTIKUM SISOP MODUL 3
Nama  : Irsa Fairuza  
NRP   : 5027251115

## Soal 1
Tahap awal untuk soal ini adalah membuat folder maupun file yang diperlukan. Disini terdapat file navi.c sebagai client, wired.c sebagai server, serta protocol.h sebagai penghubung agar server dan client bisa berkomunikasi.

### Membuat protocol.h
```c
#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PORT 8080
#define SERVER_IP "127.0.0.1"

#endif
```
Selanjutnya di dalam protocol.h didefinisikan konstanta #define untuk menetapkan IP address dan port dari masing masing server dan client. Untuk IP address client ditetapkan 127.0.0.1 agar servernya diketahui ada di komputer lokal. Sedangkan untuk port nya ditetapkan sebagai 8080, ini digunakan untuk mengetahui koneksi ini dikirim ke program yang mana. Tahap didalam protocol.h ini dilakukan untuk menghubungkan server dan client.

### Membuat wired.c (server)
```c
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "protocol.h"

#define MAX_CLIENTS 30

int main(int argc, char const *argv[]) {
int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
socklen_t addrlen = sizeof(address);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 20) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
}
```
Untuk kode di wired.c kita didefinisikan file protoocol.h yang sudah kita buat agar isi dari file tersebut juga dikenali oleh file wired.c. Setelah itu kita membuat socket yang disimpan di variabel file descriptor (server_fd). Socket ini digunakan sebagai titik awal komunikasi antara server dan client.  

Lalu setsockopt disini digunakan untuk mengatur izin, ia akan memaksa sistem operasi untuk melepas port yang sudah dimatikan agar bisa dipakai ulang.  

Selanjutnya perintah bind() digunakan untuk mendaftarkan alamat-alamat seperti IP address dan port yang akan digunakan. Disini server akan menerima semua IP komputer yang bisa diakses dengan menggunakan INADDR_ANY.  

Untuk perintah listen() digunakan untuk menampung antrian client yang ingin masuk ke program ./navi di detik yang bersamaan. Disini diatur sebanyak 20 client yang bisa masuk antrian, jika terdapat lebih dari 20, maka sisa dari client itu akan ditolak.  

Fungsi accept() berfungsi sebagai pembuka jalur komunikasi untuk satu client. Program akan diam di baris ini hingga ada request dari client yang sudah antri pada bagian listen dan akan menghasilkan file descriptor. Selanjutnya akan saling kirim pesan menggunakan variabel new_socket.

### Membuat navi.c (client)
```c
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "protocol.h"
#include <pthread.h>

int main(int argc, char const *argv[]) {
        int sock = 0, valread;
        struct sockaddr_in serv_addr;

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                printf("\n Socket creation error \n");
                return -1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr)<=0) {
                printf("\nInvalid address/ Address not supported \n");
                return -1;
        }


        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                printf("\nConnection Failed \n");
                return -1;
        }
```
Tahap awal pembuatan client yaitu dengan membuat socket sebagai titik komunikasi dengan server dengan jalur komunikasi AF_INET dan protokol SOCK_STREAM agar data tidak hilang pada pertengahan proses.  

Selanjutnya langkah keamanan dimana fungsi memset digunakan untuk menghapus isi dari memori serv_addr dengan angka 0. Ini dilakukan agar saat diisi dengan alamat baru nantinya tidak terjadi error. Lalu ditetapkan juga alamat-alamat tujuan yang ingin di akses oleh client.  

Untuk membaca alamat yang sudah ditetapkan, harus diterjemahkan dulu ke format biner  menggunakan inet_pton.  

Selanjutnya fungsi connect akan mengirim sinyal ke alamat server. Jika fungsi accept() yang sudah kita tetapkan di kode server sedang menunggu sinyal, maka server akan menerima request dari client dan mereka saling bertukar pesan. Kalau fungsi ini error seperti server yang lagi mati atau IP maupun port salah, fungsi ini akan mengembalikan angka -1 dan masuk ke blok error.  

### Inisialisasi Identitas
```c
 char buffer[1024] = {0};
char name[50];
printf("Enter your name: ");
scanf("%s", name);
getchar();
send(sock , name , strlen(name) , 0 );
printf("%s", buffer);
```
1. Minta dan intput nama
2. Kirim nama yang udah diinput, ke server
3. Nunggu dan baca balasan dari server
4. Cetak balasan server ke layar

### Multiplexing dengan select():
```c
activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
```
Select digunakan untuk memantau semua soket yang aktif, ini digunakan karena jumlah socket yang ada cukup besar.

### Memvalidasi identitas
```c
int is_duplicate = 0;
for (int j = 0; j < max_clients; j++) {
        if (client_socket[j] > 0 && strcmp(client_names[j], buffer) == 0) {
                is_duplicate = 1;
                break;
        }
}
```
Untuk memastikan input yang dimasukan user tidak ada yang sama atau ter duplikasi kita menggunakan logika iterasi yang mengecek array client_names.

### Implementasi Mekanisme Broadcast
```c
for (j = 0; j < max_clients; j++) {
        int dest_socket = client_socket[j];
        if (dest_socket > 0 && dest_socket != sd) {
                char broadcast_msg[1100];
                sprintf(broadcast_msg, "[%s]: %s", client_names[i], buffer);
                send(dest_socket, broadcast_msg, strlen(broadcast_msg), 0);
        }
}
```
Saat server menerima pesan dari satu client, server akan meneruskannya ke semua client yang aktif menggunakan perulangan for.

### Menjalankan client secara asinkron
```c
pthread_t recv_thread;
if (pthread_create(&recv_thread, NULL, receive_message, (void*)&sock) < 0) {
        perror("could not create thread");
        return 1;
}
```
Untuk menjalankan dua fungsi secara asinkron, program menggunakan thread terpisah agar bisa mendengarkan transmisi dari the wired dan mengirimkan input pengguna secara beriringan.
Disini kita membuat thread baru yang khusus menjalankan fungsi receive_message di background.

## Hasil

<img width="959" height="500" alt="image" src="https://github.com/user-attachments/assets/af5059a2-edde-48e4-a5b9-0034f7b092e9" />  

<img width="1918" height="1000" alt="image" src="https://github.com/user-attachments/assets/3e26bfcc-ad77-4b31-b9a3-1e08d5840ea4" />

Berdasarkan hasil compile dan run server dan client menunjukan bahwa server berhasil dijalankan. Dan setelah client dijalankan serta memaasukan nama server langsung menampilkan bahwa client dengan nama "dora" sudah masuk ke sistem dan menampilkan menu The Knights Console. Hal ini menunjukan bahwa fungsi bind, listen, dan accept pada server serta fungsi connect pada client telah berhasil dijalankan.  

<img width="1918" height="1000" alt="image" src="https://github.com/user-attachments/assets/336d8082-4c06-4266-ab6a-525d3944b37d" />

Untuk validasi apakah nama yang dimasukkan adalah nama yang sama atau beda berjalan dengan baik. Saat client memasukan nama yang sama, server akan memberikan peringatan [System] The identity is already synchronized in The Wired. Akan tetapi pada kasus ini program tidak secara otomatis memutus koneksi dan client masih bisa mengirim pesan.

<img width="959" height="503" alt="image" src="https://github.com/user-attachments/assets/8369d1ba-13c1-4cd3-91a5-6d5658dc3715" />

Dari hasil diatas, komunikasi antar dua arah sudah berhasil. Saat client 1 menuliskan pesan, pesan tersebut akan ditampilkan juga di client lainnya lengkap dengan identitas pengirimnya. Ini menunjukan bahwa aplikasi threading pada client yang memisahkan perintah read dan write telah berhasil, serta perintah select pada server yang berhasil melayani banyak client secara bersamaan.

