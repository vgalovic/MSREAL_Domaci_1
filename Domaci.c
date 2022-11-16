//Vladimir Galović EE 210/2018

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//definisane funkcije
int led_f(short led_pos);
int switch_f();
int button_f(short sw, short led_pos);

int main(){  

  short sw = 0; //broj za koliko se pomera pozicija led u zavisnosti od pozicija sviceva
  short led_pos = 1; //pozicija led koja je upaljena
  long int per = 20000L; //period

  while(1){
    printf("led_pos %d", led_pos);
    
    //Pokrece funkciju led_f i upisuje koja dioda treba da bude upaljena u zadatom trenutku
    if(led_f(led_pos) == -1) //prvra da li ima problema pri radu funkcije
      return -1;
    
    usleep(per);

    //pokrece funkciju switch_f i cita u kojim polozajima se nalaze SWITCH0 i SWITCH1
    sw = switch_f();
    if(sw == -1) //prvra da li ima problema pri radu funkcije
      return -1;

    //pokrece funkciju button_f i cita da li je neki taster pritisnut
    led_pos = button_f(sw, led_pos); 
    if(led_pos == -1) //prvra da li ima problema pri radu funkcije
      return -1;
    
    if(led_pos == -2){ //Ako su BUTTON0 i BUTTON3 pritisnuti u isto vreme, program se gasi
      //Ugasi sve diode
      led_pos = 0;
      if(led_f(led_pos) == -1) //prvra da li ima problema pri radu funkcije
      return -1;
       
      puts("Uspesno ste izasli iz programa!");
      return 0; // Ako nije prethodno bilo ikakvih problema, program se uspesno zatvara
    }
  }	
}

int led_f(short led_pos){
  FILE* fp;
  
  //upis u fajl /dev/led
  fp = fopen("/dev/led", "w");
  if(fp == NULL){
    puts("Problem pri otvaranju /dev/led");
    return -1;
  }

  fprintf(fp, "0x%x", led_pos);
  if(fclose(fp)){
    puts("Problem pri zatvaranju /dev/led");
    return -1;
  }

  return 0;
}

int switch_f(){
  FILE *fp;
  char *str;
  char chr1, chr2;
  size_t nob = 6; // number of byts
  
  //citanje vrednosti sa sviceva
  fp = fopen("/dev/switch", "r");
  if(fp == NULL){
    puts("Problem pri otvaranju /dev/switch");
    return -1;
  }

  str = (char *)malloc(nob +1);
  getline(&str, &nob, fp);

  if(fclose(fp)){
    puts("problem pri zatvaranju /dev/switch");
    return -1;
  }

  chr1 = str[4] -48;
  chr2 = str[5] -48;
  free(str);

  //U zavisnosti od pozicija sviceva SWITCH0 i SWITCH1, za toliko ce se pomerati led_pos(vrednost se vraca u sw)
  if((chr1 == 0) && (chr2 == 0))
    return 0;
  else if((chr1 == 0) && (chr2 == 1))
    return 1;
  else if((chr1 == 1) && (chr2 == 0))
    return 2;
  else
    return 3;
}

int button_f(short sw, short led_pos){
  FILE *fp;
  char *str;
  char chr1, chr2, chr3, chr4;
  size_t nob = 6; // number of byts
  short rt = led_pos; //Vraca vrednos koja se upisuje u led_pos
  short i;
  
  //Citanje vrednosti sa tastera
  fp = fopen("/dev/button", "r");
  if(fp == NULL){
    puts("Problem pri otvaranju /dev/button");
    return -1;
  }

  str = (char *)malloc(nob +1);
  getline(&str, &nob, fp);

  if(fclose(fp)){
    puts("problem pri zatvaranju /dev/button");
    return -1;
  }

  chr1 = str[2] -48;
  chr2 = str[3] -48;
  chr3 = str[4] -48;
  chr4 = str[5] -48;
  free(str);

  if((chr1 == 1) && (chr4 == 1)) //Ako su BUTTON0 i BUTTON3 pritisnuti u isto vreme, program se gasi
    return -2;
  
  if(chr1 == 1) //Ako je pritisnut taster BUTTON4, pali se dioda LED4
    rt = 8;

  else if(chr2 == 1){ //Ako je pritisnut taster BUTTON2, ukljucena dioda na poziciji led_pos ce se pomeriti za sw mesta u levo

    for(i = 0; i < sw; i++){
      if(rt == 8)
	rt = 1;
      else
	rt = rt *2;
    }
  }
  
  else if(chr3 == 1){ //Ako je pritisnut taster BUTTON1, ukljucena dioda na poziciji led_pos ce se pomeriti za sw mesta u desno

    for(i = 0; i < sw; i++){
      if(rt == 1)
	rt = 8;
      else
	rt = rt /2;
    }
  }
  
  else if(chr4 == 1) //Ako je pritisnut taster BUTTON0, pali se dioda LED0
    rt = 1;

  return rt;
}
