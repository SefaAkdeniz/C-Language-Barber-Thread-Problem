#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MUSTERI_SAYISI 10
#define TRAS_SURESI 3

const int koltuk_sayisi =1;
const int sandalye_sayisi=5;
int musteri_sayisi=0;
int bos_beklemeSandalye_sayisi=0;
int tras_edilecek_musteri =0;
int oturalacak_sandalye =0;
int ilk_tras=0;
int musteri_durum_sayisi=0;

int* koltuk;

sem_t berber_sem;
sem_t musteriler_sem;
sem_t mutex_sem;

int Berber()
{
    int sonrakiMusteri, musteri_id;
    
    if(ilk_tras==0)
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

        tras_edilecek_musteri = (++tras_edilecek_musteri) % sandalye_sayisi;
        sonrakiMusteri=tras_edilecek_musteri;
        musteri_id=koltuk[sonrakiMusteri];
        koltuk[sonrakiMusteri]=pthread_self();
	musteri_durum_sayisi=musteri_durum_sayisi-1;
	
        sem_post(&mutex_sem);
        sem_post(&musteriler_sem);

        printf(" Berber \t%d. musterinin trasina basladi. \n\n",musteri_id);
        sleep(TRAS_SURESI);
        printf(" Berber \t%d. musterinin trasini bitirdi. \n\n",musteri_id);
        
        if(!musteri_durum_sayisi)
        {
            printf("Berber uyudu\n\n\n");
        }
    }
    pthread_exit(0);
}

void Musteri(void* sayi)
{
    int s = *(int*)sayi + 1;
    int oturulanSandalye,berber_kimligi;

    sem_wait(&mutex_sem);

    printf("%d. Musteri\tdukkana geldi. \n",s);

    if(bos_beklemeSandalye_sayisi > 0)
    {

        bos_beklemeSandalye_sayisi--;
        printf("%d. Musteri\tsandalyede bekliyor.\n\n",s);

        oturalacak_sandalye=(++oturalacak_sandalye)%sandalye_sayisi;
        oturulanSandalye = oturalacak_sandalye;
        koltuk[oturulanSandalye]=s;

        sem_post(&mutex_sem);
        sem_post(&berber_sem);

        sem_wait(&musteriler_sem);
        sem_wait(&mutex_sem);

        berber_kimligi = koltuk[oturulanSandalye];
        bos_beklemeSandalye_sayisi++;

        sem_post(&mutex_sem);
    }
    else{
        sem_post(&mutex_sem);
        printf("%d. Musteri\tbeklemek icin koltuk bulamadi. Dukkandan ayriliyor.\n\n",s);
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
	    scanf("%d",&musteri_sayisi);
	    
	    musteri_durum_sayisi =musteri_sayisi;
	    bos_beklemeSandalye_sayisi = sandalye_sayisi;
	    koltuk = (int*) malloc(sizeof(int)*sandalye_sayisi);

	    if(musteri_sayisi >MUSTERI_SAYISI)
	    {
		printf("\n Musteri siniri: %d\n\n",MUSTERI_SAYISI);
		return EXIT_FAILURE;
	    }

	    pthread_t berber[koltuk_sayisi], musteri[musteri_sayisi];

	    sem_init(&berber_sem, 0,0);
	    sem_init(&musteriler_sem, 0,0);
	    sem_init(&mutex_sem, 0,1);

	    printf("\n Berber Dukkani acti. \n\n");

	    for(int i=0; i < koltuk_sayisi;i++)
	    {
		pthread_create(&berber[i],NULL, (void*)Berber,(void*)&i);
		sleep(1);
	    }
	    for(int i=0; i < musteri_sayisi;i++)
	    {
		pthread_create(&musteri[i],NULL, (void*)Musteri,(void*)&i);
		sleep(1);
	    }
	    for(int i=0; i < musteri_sayisi;i++)
	    {
		pthread_join(musteri[i],NULL);
		sleep(1);
	    }
	    ilk_tras=1;
	    
    do
    {
    	sleep(3);
    	musteri_durum_sayisi=0;
    	printf("Musteri Sayisi Giriniz: ");
	scanf("%d",&musteri_sayisi);
	musteri_durum_sayisi=musteri_sayisi;

    	if(!musteri_sayisi==0 )
    	{
	    bos_beklemeSandalye_sayisi = sandalye_sayisi;
	    koltuk = (int*) malloc(sizeof(int)*sandalye_sayisi);

	    if(musteri_sayisi >MUSTERI_SAYISI)
	    {
		printf("\n Müsteri siniri: %d\n\n",MUSTERI_SAYISI);
		return EXIT_FAILURE;
	    }

	    pthread_t berber[koltuk_sayisi], musteri[musteri_sayisi];

	    sem_init(&berber_sem, 0,0);
	    sem_init(&musteriler_sem, 0,0);
	    sem_init(&mutex_sem, 0,1);


	    for(int i=0; i < koltuk_sayisi;i++)
	    {
		pthread_create(&berber[i],NULL, (void*)Berber,(void*)&i);
		sleep(1);
	    }
	    for(int i=0; i < musteri_sayisi;i++)
	    {
		pthread_create(&musteri[i],NULL, (void*)Musteri,(void*)&i);
		sleep(1);
	    }
	    for(int i=0; i < musteri_sayisi;i++)
	    {
		pthread_join(musteri[i],NULL);
		sleep(1);
	    }
    	}
    	else if(musteri_sayisi==0 )
    	{
	    printf("\nBerber Uyudu.\n\n");
    	}
	    
    }while(1);
    

}
