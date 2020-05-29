#include <stdio.h>
#include <stdlib.h>     // Kütüphanelerimizi tanımladık
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define TRAS_ZAMANI 3  	// Traş Süresini Belirledik
#define koltukSayisi 1		// Berber 1 tane ve berber koltuğumuzda 1 tane
#define sandalyeSayisi 5	// 5 tane bekleme sandalyemiz var


int musteriSayisi=0;			// İlk değerlerimizi 0 atadık
int bosBeklemeSandalyeSayisi=0;
int trasEdilecekMusteri =0;
int oturalacakSandalye =0;
int ilkTras=0;
int musteriDurumSayisi=0;

int* koltuk;


sem_t berber_sem;			// Kullanacağımız Semaforları tanımladık
sem_t musteriler_sem;
sem_t mutex_sem;


int Berber()
{
    int sonrakiMusteri, musteriID;
    
    if(ilkTras==0)  	// Berber ilk müşteri grubunda dükkanı açar
    {
    	printf("Berber\tdukkana girdi.\n\n");
    }
    else		// Diğer Müşteri gruplarında ise uyanır.
    {
    	printf("\nBerber\tuyandi.\n\n");
    }

    while(1)  		// Sonsuz Döngü İçerisinde
    {
    
        sem_wait(&berber_sem);      // Müşterinin yoksa koltuğa iletişim engellenir.
        sem_wait(&mutex_sem);

        trasEdilecekMusteri = (++trasEdilecekMusteri) % sandalyeSayisi;  	// Traş edilecek müşteri bekleyen müşterilerden seçilir	
        sonrakiMusteri=trasEdilecekMusteri;
        musteriID=koltuk[sonrakiMusteri];
        koltuk[sonrakiMusteri]=pthread_self();
	musteriDurumSayisi=musteriDurumSayisi-1;
	
        sem_post(&mutex_sem);		// Koltuğa erişim açılır berber müşteri ile ilgilenmeye başlar
        sem_post(&musteriler_sem);

        printf(" Berber \t%d. musterinin trasina basladi. \n\n",musteriID);  // Berber traşa başlar ve belirlediğimiz süre sonunda traşı bitirir
        sleep(TRAS_ZAMANI);
        printf(" Berber \t%d. musterinin trasini bitirdi. \n\n",musteriID);
        
        if(!musteriDurumSayisi)     			// Berber traş edilecek müşteriler bittiğinde uyur.
        {
            printf("Berber uyudu\n\n\n");
        }
    }
    pthread_exit(0);		// Thread sonlandırılır
}

void Musteri(void* sayi)	// Müşteri fonksiyonu müşteri numarasını parametre olarak alır
{
    int kimlik = *(int*)sayi + 1;  // Müşterimim kimliğin alınır
    int oturulanSandalye;

    sem_wait(&mutex_sem);  	    // Koltuğa erişim engellenir

    printf("%d. Musteri\tdukkana geldi. \n",kimlik);  // Gelen müşteri ekrana yazdırılır

    if(bosBeklemeSandalyeSayisi > 0)
    {
        bosBeklemeSandalyeSayisi--;					// Eğer boş bekleme sandalyesi varsa sandalye sayısı bir azaltılır ve
        printf("%d. Musteri\tsandalyede bekliyor.\n\n",kimlik);	// Müşteri beklemeye başlar 

        oturalacakSandalye=(++oturalacakSandalye)%sandalyeSayisi;	// Müşterinin boş sandalyelerden birine oturması sağlanır
        oturulanSandalye = oturalacakSandalye;
        koltuk[oturulanSandalye]=kimlik;

        sem_post(&mutex_sem);		// Koltuğa erişim engeli kaldırlır
        sem_post(&berber_sem);	// Berber uyuyorsa uyandırılır ve koltuğa geçmesi sağlanır

        sem_wait(&musteriler_sem);	// Müşteri traş olmak için bekler
        sem_wait(&mutex_sem);		// Müşterilerin aynı anda traş olması önlenir

        bosBeklemeSandalyeSayisi++; // Müşteri traş olmaya başladığında dükkandaki sandelye sayısı bir arttırılır.

        sem_post(&mutex_sem);		// Erişim engellenir
    }
    else{
        sem_post(&mutex_sem); 	// Engel kaldırılır , eğer boş sandalye yoksa müşteri dükkandan ayrılır
        printf("%d. Musteri\tbeklemek icin sandalye bulamadi. Dukkandan ayriliyor.\n\n",kimlik);  
    }
    pthread_exit(0);	// Thread sonlandırılır
}


int main(int argc , char** args)
{	
		// İlk müşeriler için bilgilerimizi alıyoruz

	    printf("Musteri Sayisi Giriniz: "); 
	    scanf("%d",&musteriSayisi);
	    
	    musteriDurumSayisi =musteriSayisi;
	    bosBeklemeSandalyeSayisi = sandalyeSayisi;
	    koltuk = (int*) malloc(sizeof(int)*sandalyeSayisi);  // Belirlenen Eleman Kadar Koltuk Dizisini Oluşturduk


	    pthread_t berber[koltukSayisi], musteri[musteriSayisi];   // Berber ve müşteri thread değişkenlerimizi oluşturduk

	    sem_init(&berber_sem, 0,0);		// Sem init methoduyla semaforlarımızı başlattık
	    sem_init(&musteriler_sem, 0,0); 
	    sem_init(&mutex_sem, 0,1);

	    printf("\n Berber Dukkani acti. \n\n");

	    for(int i=0; i < koltukSayisi;i++)
	    {
		pthread_create(&berber[i],NULL, (void*)Berber,(void*)&i);  // Berber thread'lerini pthread_create methodu ile oluşturduk
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)
	    {
		pthread_create(&musteri[i],NULL, (void*)Musteri,(void*)&i); // Müşteri thread'lerini pthread_create methodu ile oluşturduk
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)       		// Tüm Müşterilerin işlemlerinin bitmesini bekliyoruz
	    {
		pthread_join(musteri[i],NULL);
		sleep(1);
	    }
	    ilkTras=1;
	    
    do
    {
    				// İlk Müşteri grubundan sonraki her grup için
    	
    	sleep(3);
    	musteriDurumSayisi=0;
    	printf("Musteri Sayisi Giriniz: ");
	scanf("%d",&musteriSayisi);
	
	musteriDurumSayisi=musteriSayisi;

    	if(!musteriSayisi==0 )
    	{
	    bosBeklemeSandalyeSayisi = sandalyeSayisi;
	    koltuk = (int*) malloc(sizeof(int)*sandalyeSayisi);   // Belirlenen Eleman Kadar Koltuk Dizisini Oluşturduk


	    pthread_t berber[koltukSayisi], musteri[musteriSayisi]; // Berber ve müşteri thread değişkenlerimizi oluşturduk

	    sem_init(&berber_sem, 0,0);
	    sem_init(&musteriler_sem, 0,0);  			// Sem init methoduyla semaforlarımızı başlattık
	    sem_init(&mutex_sem, 0,1);


	    for(int i=0; i < koltukSayisi;i++)
	    {
		pthread_create(&berber[i],NULL, (void*)Berber,(void*)&i);  // Berber thread'lerini pthread_create methodu ile oluşturduk
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)
	    {
		pthread_create(&musteri[i],NULL, (void*)Musteri,(void*)&i);  // Müşteri thread'lerini pthread_create methodu ile oluşturduk
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)			// Tüm Müşterilerin işlemlerinin bitmesini bekliyoruz
	    {
		pthread_join(musteri[i],NULL);
		sleep(1);
	    }
    	}
    	else if(musteriSayisi==0 )				// Girilen müşteri sayısı 0 ise berber uyumaya devam eder
    	{
	    printf("\nBerber Uyuyor.\n\n");
    	}
	    
    }while(1);
    
}
