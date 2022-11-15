//Vladimir Galović EE 210/2018

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int led_f(int led_pos);
int switch_f();
int button_f(int sw, int led_pos);

int main(){  

  int sw = 0; //broj za koliko se pomera pozicija led u zavisnosti od pozicija sviceva
  int led_pos = 1; //pozicija led koja je upaljena
  long int per = 20000L; //period

  while(1){
    if(led_f(led_pos) == -1)
      return -1;

    usleep(per);
    
    sw = switch();
    if(sw == -1)
      return -1;

    led_pos = button_f(sw, led_pos);
    if(led_pos == -1)
      return -1;
    if(led_pos == -2){ //Ako su BUTTON0 i BUTTON3 pritisnuti u isto vreme, program se gasi
      puts("Uspesno ste izasli iz programa!");
      return 0;
    }
  }	
}

int led_f(int led_pos){
  FILE* fp;
  
  //Upali diodu
  fp = fopen("/dev/led", "w");
  if(fp == NULL){
    puts("Problem pri otvaranju /dev/led");
    return -1;
  }

  fprintf(fp, "0x%d", led_pos);
  if(fclose(fp)){
    puts("Problem pri zatvaranju /dev/led");
    return -1;
  }
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

  if((chr1 == 0) && (chr2 == 0))
    return 0;
  else if((chr1 == 0) && (chr2 == 1))
    return 1;
  else if((chr1 == 1) && (chr2 == 0))
    return 2;
  else
    return 3;
}

it button_f(int sw, int led_pos){
  FILE *fp;
  char *str;
  char chr1, chr2, chr3, chr4;
  size_t nob = 6; // number of byts
  short rt = 0;
  
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

  if((chr1 == 1) && (chr4 == 1))
    return -2;
  
  if(chr1 == 1)
    rt = 8;

  else if(chr2 == 1){
    rt = led_pos;

    for(int i = 0; i < sw; i++){
      if(rt == 8)
	rt = 1;
      else
	rt = rt *2;
    }
  }
  
  else if(chr3 == 1){
    rt = led_pos;

    for(int i = 0; i < sw; i++){
      if(rt == 1)
	rt = 8;
      else
	rt = rt /2;
    }
  }
  
  else if(chr4 == 1)
    rt = 1;

  return rt;
}
