//Vladimir Galović EE 210/2018

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//definisane funkcije
int led_f(int led_pos);
int switch_f();
int *  button_f(int sw, int pos[2]);

int main(){  

  int sw = 0; //broj za koliko se pomera pozicija led u zavisnosti od pozicija sviceva
  int pos[2] = {1,0}; //pozicija led koja je upaljena
  int *point;
  short button_presed = 0;
  long int per = 20000L; //period

  while(1){
    
    //Pokrece funkciju led_f i upisuje koja dioda treba da bude upaljena u zadatom trenutku
    if(pos[1] != button_presed){
      if(led_f(pos[0]) == -1) //prvra da li ima problema pri radu funkcije
	return -1;
      button_presed = pos[1];
    }
    usleep(per);

    //pokrece funkciju switch_f i cita u kojim polozajima se nalaze SWITCH0 i SWITCH1
    sw = switch_f();
    if(sw == -1) //prvra da li ima problema pri radu funkcije
      return -1;

    //pokrece funkciju button_f i cita da li je neki taster pritisnut
    point = button_f(sw, pos);
    *(point) = pos[0];
    *(point +1) = pos[1];
    if(pos[0] == -1) //prvra da li ima problema pri radu funkcije
      return -1;
    
    if(pos[0] == -2){ //Ako su BUTTON0 i BUTTON3 pritisnuti u isto vreme, program se gasi
      //Ugasi sve diode
      pos[0] = 0;
      if(led_f(pos[0]) == -1) //prvra da li ima problema pri radu funkcije
      return -1;
       
      puts("Uspesno ste izasli iz programa!");
      return 0; // Ako nije prethodno bilo ikakvih problema, program se uspesno zatvara
    }
  }	
}

int led_f(int led_pos){
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

int *  button_f(int sw, int pos[2]){
  FILE *fp;
  char *str;
  char chr1, chr2, chr3, chr4;
  size_t nob = 6; // number of byts
  static int rt[2]; //Vraca vrednos koja se upisuje u led_pos
  short i;
  rt[0] = pos[0];
  
  //Citanje vrednosti sa tastera
  fp = fopen("/dev/button", "r");
  if(fp == NULL){
    puts("Problem pri otvaranju /dev/button");
    rt[0] = -1;
    rt[1] = 5;
    return rt;
  }

  str = (char *)malloc(nob +1);
  getline(&str, &nob, fp);

  if(fclose(fp)){
    puts("problem pri zatvaranju /dev/button");
    rt[0] = -1;
    rt[1] = 5;
    return rt;
  }

  chr1 = str[2] -48;
  chr2 = str[3] -48;
  chr3 = str[4] -48;
  chr4 = str[5] -48;
  free(str);

  if((chr1 == 1) && (chr4 == 1)){ //Ako su BUTTON0 i BUTTON3 pritisnuti u isto vreme, program se gasi
    rt[0] =-2;
    rt[1] =5;
  }
  
  if(chr1 == 1){ //Ako je pritisnut taster BUTTON3, pali se dioda LED4
    rt[0] = 8;
    rt[1] = 1;
  }

  else if(chr2 == 1){ //Ako je pritisnut taster BUTTON2, ukljucena dioda na poziciji led_pos ce se pomeriti za sw mesta u levo
    rt[1] = 2;
    
    for(i = 0; i < sw; i++){
      if(rt[0] == 8)
	rt[0] = 1;
      else
	rt[0] = rt[0] *2;
    }
  }
  
  else if(chr3 == 1){ //Ako je pritisnut taster BUTTON1, ukljucena dioda na poziciji led_pos ce se pomeriti za sw mesta u desno
    rt[1] = 3;
    
    for(i = 0; i < sw; i++){
      if(rt[0] == 1)
	rt[0] = 8;
      else
	rt[0] = rt[0] /2;
    }
  }
  
  else if(chr4 == 1){ //Ako je pritisnut taster BUTTON0, pali se dioda LED0
    rt[0] = 1;
    rt[1] = 4;
  }

  return rt;
}
