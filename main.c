#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MUSTERI_SAYISI 8
#define TRAS_SURESI 3

int koltuk_sayisi =0;
int musteri_sayisi=0;
int sandalye_sayisi=0;
int bos_sandalye_sayisi=0;
int hizmet_edilecek_musteri =0;
int oturalacak_sandalye =0;

int* koltuk;

sem_t berber_sem;
sem_t musteriler_sem;
sem_t mutex_sem;

int Berber(void* sayi)
{
    int berber_id = *(int*)sayi + 1;
    int sonrakiMusteri, musteri_id;

    printf("Berber \tdükkana girdi.\n");

    while(1)
    {
        if(!musteri_id)
        {
            printf("Berber uyudu\n\n");
        }

        sem_wait(&berber_sem);
        sem_wait(&mutex_sem);

        hizmet_edilecek_musteri = (++hizmet_edilecek_musteri) % sandalye_sayisi;
        sonrakiMusteri=hizmet_edilecek_musteri;
        musteri_id=koltuk[sonrakiMusteri];
        koltuk[sonrakiMusteri]=pthread_self();

        sem_post(&mutex_sem);
        sem_post(&musteriler_sem);

        printf(" Berber \t%d. musterinin trasina basladý. \n\n",musteri_id);
        sleep(TRAS_SURESI);
        printf(" Berber \t%d. musterinin trasini bitirdi. \n\n",musteri_id);
    }
    pthread_exit(0);
}

void Musteri(void* sayi)
{
    int s = *(int*)sayi + 1;
    int oturulanSandalye,berber_kimligi;

    sem_wait(&mutex_sem);

    printf("%d. Musteri \tdukkana geldi. \n",s);

    if(bos_sandalye_sayisi > 0)
    {

        bos_sandalye_sayisi--;
        printf("%d. Musteri\t bekliyor.\n\n",s);

        oturalacak_sandalye=(++oturalacak_sandalye)%sandalye_sayisi;
        oturulanSandalye = oturalacak_sandalye;
        koltuk[oturulanSandalye]=s;

        sem_post(&mutex_sem);
        sem_post(&berber_sem);

        sem_wait(&musteriler_sem);
        sem_wait(&mutex_sem);

        berber_kimligi = koltuk[oturulanSandalye];
        bos_sandalye_sayisi++;

        sem_post(&mutex_sem);
    }
    else{
        sem_post(&mutex_sem);
        printf("%d. Musteri\t beklemek için koltuk bulamadý. Dukkandan ayriliyor.\n\n",s);
    }
    pthread_exit(0);
}

void Bekle()
{
    srand((unsigned int) time(NULL));
    usleep(rand() % (250000 - 50000 +1) +50000);
}

int Main()
{
    printf("Müsteri Sayisi Giriniz: ");
    scanf("%d",&musteri_sayisi);
    printf("Sandalye Sayisi Giriniz: ");
    scanf("%d",&sandalye_sayisi);
    printf("Koltuk Sayisi Giriniz: ");
    scanf("%d",&koltuk_sayisi);

    bos_sandalye_sayisi = sandalye_sayisi;
    koltuk = (int*) malloc(sizeof(int)*sandalye_sayisi);

    if(musteri_sayisi >MUSTERI_SAYISI)
    {
        printf(printf("\n Müsteri siniri: %d\n\n"),MUSTERI_SAYISI);
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
        pthread_join(&musteri[i],NULL);
        sleep(1);
    }

    printf("\n Tum musterilerin trasi bitti. Berber dukkaný kapattý \n\n");
}
