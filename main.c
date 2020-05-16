#include <stdio.h>
#include <stdlib.h>     // Kütüphanelerimizi tanýmladýk
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define TRAS_ZAMANI 3  	// Traþ Süresini Belirledik
#define koltukSayisi 1		// Berber 1 tane ve berber koltuðumuzda 1 tane
#define sandalyeSayisi 5	// 10 tane bekleme sandalyemiz var


int musteriSayisi=0;			// Ýlk deðerlerimizi 0 atadýk
int bosBeklemeSandalyeSayisi=0;
int trasEdilecekMusteri =0;
int oturalacakSandalye =0;
int ilkTras=0;
int musteriDurumSayisi=0;

int* koltuk;


sem_t berber_sem;			// Kullanacaðýmýz Semaforlarý tanýmladýk
sem_t musteriler_sem;
sem_t mutex_sem;


int Berber()
{
    int sonrakiMusteri, musteriID;
    
    if(ilkTras==0)  	// Berber ilk müþteri grubunda dükkaný açar
    {
    	printf("Berber\tdukkana girdi.\n\n");
    }
    else		// Diðer Müþteri gruplarýnda ise uyanýr.
    {
    	printf("\nBerber\tuyandi.\n\n");
    }

    while(1)  		// Sonsuz Döngü Ýçerisinde
    {
    
        sem_wait(&berber_sem);      // Müþterinin yoksa koltuða iletiþim engellenir.
        sem_wait(&mutex_sem);

        trasEdilecekMusteri = (++trasEdilecekMusteri) % sandalyeSayisi;  	// Traþ edilecek müþteri bekleyen müþterilerden seçilir	
        sonrakiMusteri=trasEdilecekMusteri;
        musteriID=koltuk[sonrakiMusteri];
        koltuk[sonrakiMusteri]=pthread_self();
	musteriDurumSayisi=musteriDurumSayisi-1;
	
        sem_post(&mutex_sem);		// Koltuða eriþim açýlýr berber müþteri ile ilgilenmeye baþlar
        sem_post(&musteriler_sem);

        printf(" Berber \t%d. musterinin trasina basladi. \n\n",musteriID);  // Berber traþa baþlar ve belirlediðimiz süre sonunda traþý bitirir
        sleep(TRAS_ZAMANI);
        printf(" Berber \t%d. musterinin trasini bitirdi. \n\n",musteriID);
        
        if(!musteriDurumSayisi)     			// Berber traþ edilecek müþteriler bittiðinde uyur.
        {
            printf("Berber uyudu\n\n\n");
        }
    }
    pthread_exit(0);		// Thread sonlandýrýlýr
}

void Musteri(void* sayi)	// Müþteri fonksiyonu müþteri numarasýný parametre olarak alýr
{
    int kimlik = *(int*)sayi + 1;  // Müþterimim kimliðin alýnýr
    int oturulanSandalye;

    sem_wait(&mutex_sem);  	    // Koltuða eriþim engellenir

    printf("%d. Musteri\tdukkana geldi. \n",kimlik);  // Gelen müþteri ekrana yazdýrýlýr

    if(bosBeklemeSandalyeSayisi > 0)
    {
        bosBeklemeSandalyeSayisi--;					// Eðer boþ bekleme sandalyesi varsa sandalye sayýsý bir azaltýlýr ve
        printf("%d. Musteri\tsandalyede bekliyor.\n\n",kimlik);	// Müþteri beklemeye baþlar 

        oturalacakSandalye=(++oturalacakSandalye)%sandalyeSayisi;	// Müþterinin boþ sandalyelerden birine oturmasý saðlanýr
        oturulanSandalye = oturalacakSandalye;
        koltuk[oturulanSandalye]=kimlik;

        sem_post(&mutex_sem);		// Koltuða eriþim engeli kaldýrlýr
        sem_post(&berber_sem);	// Berber uyuyorsa uyandýrýlýr ve koltuða geçmesi saðlanýr

        sem_wait(&musteriler_sem);	// Müþteri traþ olmak için bekler
        sem_wait(&mutex_sem);		// Müþterilerin ayný anda traþ olmasý önlenir

        bosBeklemeSandalyeSayisi++; // Müþteri traþ olmaya baþladýðýnda dükkandaki sandelye sayýsý bir arttýrýlýr.

        sem_post(&mutex_sem);		// Eriþim engellenir
    }
    else{
        sem_post(&mutex_sem); 	// Engel kaldýrýlýr , eðer boþ sandalye yoksa müþteri dükkandan ayrýlýr
        printf("%d. Musteri\tbeklemek icin sandalye bulamadi. Dukkandan ayriliyor.\n\n",kimlik);  
    }
    pthread_exit(0);	// Thread sonlandýrýlýr
}


int main(int argc , char** args)
{	
		// Ýlk müþeriler için bilgilerimizi alýyoruz

	    printf("Musteri Sayisi Giriniz: "); 
	    scanf("%d",&musteriSayisi);
	    
	    musteriDurumSayisi =musteriSayisi;
	    bosBeklemeSandalyeSayisi = sandalyeSayisi;
	    koltuk = (int*) malloc(sizeof(int)*sandalyeSayisi);  // Belirlenen Eleman Kadar Koltuk Dizisini Oluþturduk


	    pthread_t berber[koltukSayisi], musteri[musteriSayisi];   // Berber ve müþteri thread deðiþkenlerimizi oluþturduk

	    sem_init(&berber_sem, 0,0);		// Sem init methoduyla semaforlarýmýzý baþlattýk
	    sem_init(&musteriler_sem, 0,0); 
	    sem_init(&mutex_sem, 0,1);

	    printf("\n Berber Dukkani acti. \n\n");

	    for(int i=0; i < koltukSayisi;i++)
	    {
		pthread_create(&berber[i],NULL, (void*)Berber,(void*)&i);  // Berber thread'lerini pthread_create methodu ile oluþturduk
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)
	    {
		pthread_create(&musteri[i],NULL, (void*)Musteri,(void*)&i); // Müþteri thread'lerini pthread_create methodu ile oluþturduk
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)       		// Tüm Müþterilerin iþlemlerinin bitmesini bekliyoruz
	    {
		pthread_join(musteri[i],NULL);
		sleep(1);
	    }
	    ilkTras=1;
	    
    do
    {
    				// Ýlk Müþteri grubundan sonraki her grup için
    	
    	sleep(3);
    	musteriDurumSayisi=0;
    	printf("Musteri Sayisi Giriniz: ");
	scanf("%d",&musteriSayisi);
	
	musteriDurumSayisi=musteriSayisi;

    	if(!musteriSayisi==0 )
    	{
	    bosBeklemeSandalyeSayisi = sandalyeSayisi;
	    koltuk = (int*) malloc(sizeof(int)*sandalyeSayisi);   // Belirlenen Eleman Kadar Koltuk Dizisini Oluþturduk


	    pthread_t berber[koltukSayisi], musteri[musteriSayisi]; // Berber ve müþteri thread deðiþkenlerimizi oluþturduk

	    sem_init(&berber_sem, 0,0);
	    sem_init(&musteriler_sem, 0,0);  			// Sem init methoduyla semaforlarýmýzý baþlattýk
	    sem_init(&mutex_sem, 0,1);


	    for(int i=0; i < koltukSayisi;i++)
	    {
		pthread_create(&berber[i],NULL, (void*)Berber,(void*)&i);  // Berber thread'lerini pthread_create methodu ile oluþturduk
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)
	    {
		pthread_create(&musteri[i],NULL, (void*)Musteri,(void*)&i);  // Müþteri thread'lerini pthread_create methodu ile oluþturduk
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)			// Tüm Müþterilerin iþlemlerinin bitmesini bekliyoruz
	    {
		pthread_join(musteri[i],NULL);
		sleep(1);
	    }
    	}
    	else if(musteriSayisi==0 )				// Girilen müþteri sayýsý 0 ise berber uyumaya devam eder
    	{
	    printf("\nBerber Uyuyor.\n\n");
    	}
	    
    }while(1);
    
}
