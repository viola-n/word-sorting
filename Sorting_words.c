
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#define HASH_TABLE_SIZE 30000
#define MAX_SANA 500

////sana ja sanan lukum‰‰r‰
typedef struct sanat{
    char *sana;
    int kplmaara;
    struct sanat *next;
} Sanat;

//hash taulu sanoille
Sanat * hash_table[HASH_TABLE_SIZE]= {NULL, 0, NULL};


//puunsolmu
typedef struct puun_solmu {
	Sanat *data;
	struct puun_solmu* vanhempi;
	struct puun_solmu* vasen;
	struct puun_solmu* oikea;
} Puun_solmu;

//puu
typedef struct puu {
	Puun_solmu * juuri;
} Puu;

//funktiot
void insert(const char *ptrsana);
void init_hash_table();
int hash (const char *sana);
void tulosta();
void lisaa_puuhun(Puu *puu, Sanat *sana);
void laheta_puuhun();


void tulosta_solmu(Puun_solmu * solmu) {
    static int i = 0;

    //tulostaa 100 ensimm‰ist‰ solmua
    if(i<101){
        if (solmu != NULL){
            if(i>0){//ei tulosta nolla rivi‰, joka laskee tyhji‰ v‰lej‰
                printf(" %s %d\n",solmu->data->sana, solmu->data->kplmaara);
            }
            i++;
        }
    }else {
        return;
    }
}
void tulosta_puu(Puun_solmu * solmu){

        //k‰y puun l‰pi
        if (solmu != NULL) {
            tulosta_puu(solmu->oikea);
            tulosta_solmu(solmu);
            tulosta_puu(solmu->vasen);
        }
}

void tulosta_koko_puu(Puu puu) {
	tulosta_puu(puu.juuri);
}

void laheta_puuhun(){

    Puu tree;
	tree.juuri=0;

	//k‰y l‰pi hash taulua ja kutsuu funktiota, joka lis‰‰ sanan puuhun
    for (int i=0; i<HASH_TABLE_SIZE; i++){
            Sanat *ptr;
            ptr = hash_table[i];
            while(ptr != NULL){
                lisaa_puuhun(&tree, ptr);
                ptr = ptr->next;
            }
    }
    //kun kaikki sanat lis‰tty puuhun, kutsuu funktiota, joka tulostaa puun
    tulosta_koko_puu(tree);

}

//lis‰‰ sanan puuhun oikeaan paikkaan sanan esiintymiskertojen mukaan
void lisaa_puuhun(Puu *puu, Sanat *sana) {

	Puun_solmu * uusisolmu = (Puun_solmu *)malloc(sizeof(Puun_solmu));
	uusisolmu->data = sana;
	uusisolmu->vanhempi = uusisolmu->oikea = uusisolmu->vasen = NULL;
	Puun_solmu * nykyinen = puu->juuri;
	Puun_solmu * vanhempi = NULL;
	while (nykyinen != NULL) {
		vanhempi = nykyinen;
		if (sana->kplmaara < nykyinen->data->kplmaara)
			nykyinen = nykyinen->vasen;
		else if (sana->kplmaara >= nykyinen->data->kplmaara)
			nykyinen = nykyinen->oikea;
		else
			return;
	}

	uusisolmu->vanhempi = vanhempi;
	if (vanhempi == NULL)
		puu->juuri= uusisolmu;
	else if (sana->kplmaara < vanhempi->data->kplmaara)
		vanhempi->vasen = uusisolmu;
	else
		vanhempi->oikea = uusisolmu;
}



//insert lis‰‰ sanan ja sen kpl m‰‰r‰n hash tauluun
void insert(const char *ptrsana){
    Sanat *ptr;

    int paikka = hash(ptrsana); // laskee sanalle hash arvon, joka tallennetaan paikka muuttujaan

     for(ptr = hash_table[paikka]; ptr != NULL; ptr = ptr -> next){//etsii lˆytyykˆ sama sana jo, jos lˆytyy lis‰‰ kplm‰‰r‰ laskuria yhdell‰
            if((strcmp(ptrsana, ptr -> sana) == 0)){
                (ptr->kplmaara)++;
                return;
            }
     }
            //jos sanaa ei lˆydy varaa muistia uudelle sanalle
            ptr = (Sanat*)malloc(sizeof(Sanat));
            if (ptr == NULL) {
                printf("virhe \n");
                exit(0);
            }
            ptr ->kplmaara = 1;//laitetaan kpl m‰‰r‰‰n 1
            ptr ->sana =(char*)malloc(strlen(ptrsana)+1);
            strcpy(ptr->sana, ptrsana);//lis‰‰ sanan
            ptr ->next = hash_table[paikka];//siirt‰‰ pointteria seuraavaan, jotta saman hash arvon saavaa sanaa ei kirjoiteta p‰‰lle
            hash_table[paikka]= ptr; // lis‰‰ sanan ja sanan kpl m‰‰r‰n hash arvon m‰‰r‰‰m‰‰n paikkaan taulussa


    return;
}


//laskee sanasta hash arvon ja palauttaa sen
int hash (const char *sana){
    int g=26;
    int hash = 0;

    for(int i=0; i<strlen(sana); i++){
        hash = (g*hash + sana[i]) % HASH_TABLE_SIZE;
    }

    return hash;
}


int main( void ){
    FILE *tiedosto;
    char tiedostonnimi[57]={'\0'};
    char temp [30] = {'\0'};
    char ch;
    int i =0;

    //kysyy k‰ytt‰j‰lt‰ tiedoston nimen
    printf("Anna tiedostonnimi >");
    fgets(tiedostonnimi, sizeof(tiedostonnimi), stdin);

    if( tiedostonnimi[strlen(tiedostonnimi)-1] == '\n')
        tiedostonnimi[strlen(tiedostonnimi)-1] = '\0';
    else
        printf("virhe");

    //ohjelman suoritusajan mittaamista varten
    double time_spent = 0.0;
    clock_t begin = clock();

    //avaa tiedoston jos sellainen on olemassa
    if((tiedosto = fopen(tiedostonnimi, "r")) == NULL){
        printf("Virhe tiedoston avaamisessa");
    }
    else{

        //lukee sanoja tiedostosta yksi kerrallaan
        do {
            strcpy(temp,"");
            fscanf(tiedosto, "%30[a-zA-Z']", temp);
            for(i=0; i<strlen(temp); i++){
                temp[i]=tolower(temp[i]); //kirjaimet pieniksi
            }
            char *sana = "";
            sana = temp;
            insert(sana); //l‰hett‰‰ sanan hash tauluun

        }while((ch = fgetc(tiedosto)) != EOF);

    }
    fclose(tiedosto);

    //k‰y l‰pi hash taulun ja l‰hett‰‰ sanat puuhun j‰rjestett‰v‰ksi.
    //kun hash taulu k‰yty l‰pi, kutsuu funktiota, joka tulostaa puun
    //j‰rjestykses‰ (100 ensimm‰ist‰ sanaa)
    laheta_puuhun();

    //ohjelman suoritusajan laskeminen ja tulostus
    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time elapsed is %f seconds", time_spent);

    return 0;
}




