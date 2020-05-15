#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MUSTERI_SAYISI 10
#define TRAS_ZAMANI 3
#define koltukSayisi 1
#define sandalyeSayisi 5


int musteriSayisi=0;
int bosBeklemeSandalyeSayisi=0;
int trasEdilecekMusteri =0;
int oturalacakSandalye =0;
int ilkTras=0;
int musteriDurumSayisi=0;

int* koltuk;

sem_t berber_sem;
sem_t musteriler_sem;
sem_t mutex_sem;

int Berber()
{
    int sonrakiMusteri, musteriID;
    
    if(ilkTras==0)
    {
    	printf("Berber\tdukkana girdi.\n\n");
    }
    else
    {
    	printf("\nBerber\tuyandi.\n\n");
    }

    while(1)
    {
        sem_wait(&berber_sem);
        sem_wait(&mutex_sem);

        trasEdilecekMusteri = (++trasEdilecekMusteri) % sandalyeSayisi;
        sonrakiMusteri=trasEdilecekMusteri;
        musteriID=koltuk[sonrakiMusteri];
        koltuk[sonrakiMusteri]=pthread_self();
	musteriDurumSayisi=musteriDurumSayisi-1;
	
        sem_post(&mutex_sem);
        sem_post(&musteriler_sem);

        printf(" Berber \t%d. musterinin trasina basladi. \n\n",musteriID);
        sleep(TRAS_ZAMANI);
        printf(" Berber \t%d. musterinin trasini bitirdi. \n\n",musteriID);
        
        if(!musteriDurumSayisi)
        {
            printf("Berber uyudu\n\n\n");
        }
    }
    pthread_exit(0);
}

void Musteri(void* sayi)
{
    int sandalye = *(int*)sayi + 1;
    int oturulanSandalye;

    sem_wait(&mutex_sem);

    printf("%d. Musteri\tdukkana geldi. \n",sandalye);

    if(bosBeklemeSandalyeSayisi > 0)
    {

        bosBeklemeSandalyeSayisi--;
        printf("%d. Musteri\tsandalyede bekliyor.\n\n",sandalye);

        oturalacakSandalye=(++oturalacakSandalye)%sandalyeSayisi;
        oturulanSandalye = oturalacakSandalye;
        koltuk[oturulanSandalye]=sandalye;

        sem_post(&mutex_sem);
        sem_post(&berber_sem);

        sem_wait(&musteriler_sem);
        sem_wait(&mutex_sem);

        bosBeklemeSandalyeSayisi++;

        sem_post(&mutex_sem);
    }
    else{
        sem_post(&mutex_sem);
        printf("%d. Musteri\tbeklemek icin koltuk bulamadi. Dukkandan ayriliyor.\n\n",sandalye);
    }
    pthread_exit(0);
}

void Bekle()
{
    srand((unsigned int) time(NULL));
    usleep(rand() % (250000 - 50000 +1) +50000);
}

int main(int argc , char** args)
{	
	    printf("Musteri Sayisi Giriniz: ");
	    scanf("%d",&musteriSayisi);
	    
	    musteriDurumSayisi =musteriSayisi;
	    bosBeklemeSandalyeSayisi = sandalyeSayisi;
	    koltuk = (int*) malloc(sizeof(int)*sandalyeSayisi);

	    if(musteriSayisi >MUSTERI_SAYISI)
	    {
		printf("\n Musteri siniri: %d\n\n",MUSTERI_SAYISI);
		return EXIT_FAILURE;
	    }

	    pthread_t berber[koltukSayisi], musteri[musteriSayisi];

	    sem_init(&berber_sem, 0,0);
	    sem_init(&musteriler_sem, 0,0);
	    sem_init(&mutex_sem, 0,1);

	    printf("\n Berber Dukkani acti. \n\n");

	    for(int i=0; i < koltukSayisi;i++)
	    {
		pthread_create(&berber[i],NULL, (void*)Berber,(void*)&i);
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)
	    {
		pthread_create(&musteri[i],NULL, (void*)Musteri,(void*)&i);
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)
	    {
		pthread_join(musteri[i],NULL);
		sleep(1);
	    }
	    ilkTras=1;
	    
    do
    {
    	sleep(3);
    	musteriDurumSayisi=0;
    	printf("Musteri Sayisi Giriniz: ");
	scanf("%d",&musteriSayisi);
	musteriDurumSayisi=musteriSayisi;

    	if(!musteriSayisi==0 )
    	{
	    bosBeklemeSandalyeSayisi = sandalyeSayisi;
	    koltuk = (int*) malloc(sizeof(int)*sandalyeSayisi);

	    if(musteriSayisi >MUSTERI_SAYISI)
	    {
		printf("\n Müsteri siniri: %d\n\n",MUSTERI_SAYISI);
		return EXIT_FAILURE;
	    }

	    pthread_t berber[koltukSayisi], musteri[musteriSayisi];

	    sem_init(&berber_sem, 0,0);
	    sem_init(&musteriler_sem, 0,0);
	    sem_init(&mutex_sem, 0,1);


	    for(int i=0; i < koltukSayisi;i++)
	    {
		pthread_create(&berber[i],NULL, (void*)Berber,(void*)&i);
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)
	    {
		pthread_create(&musteri[i],NULL, (void*)Musteri,(void*)&i);
		sleep(1);
	    }
	    for(int i=0; i < musteriSayisi;i++)
	    {
		pthread_join(musteri[i],NULL);
		sleep(1);
	    }
    	}
    	else if(musteriSayisi==0 )
    	{
	    printf("\nBerber Uyudu.\n\n");
    	}
	    
    }while(1);
    

}
