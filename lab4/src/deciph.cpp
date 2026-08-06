#include "ciph.hpp"
#include "deciph.hpp"

int main()
{   /* Main program */
   FILE *  		infile;
   int 	   		i, j;
   int      		desflg, ksflg, cyflg;
   gl32array    	ipp;
   gl48array    	ipc2, iet;
   gl56array    	glicd, ipc1;
   gl64array    	ip, ipm, inpt, key, jotput;
   gl16_4_8array	is;
   gl4_16array 		ibin;
   gl4_16array 		hexinbin;
   char         	htext[17], hkey[17];
   int a;
//   char         	oper;

   system("clear"); // clrscr();
   glopen(&infile,"hex.dat");
   for (i = 0; i <= 15; i++)
     for (j = 0; j <= 3; j++) fscanf(infile,"%d",&(hexinbin[j][i]));
   fclose(infile); printf("\n");
   printf("*** PROGRAM DECIPHER ***\n"); printf("\n");
   printf("Type ciphertext <16 hexadecimal characters [0..f] + ENTER>:\n");
   for (i=0; i<=15; i++) htext[i]=48;
   htext[16]=0;
   printf("Text hex:  \n");
   printf("%s\r",htext);
   scanf("%s",&htext[0]);
   HEXTOBIN(htext,inpt,hexinbin);
   printf("Text bin:   "); for (i = 0; i <= 63; i++) printf("%d",inpt[i]); printf("\n"); printf("\n");
   printf("Type Key <16 hexadecimal characters [0..f] + ENTER>:\n");
   for (i=0; i<=15; i++) hkey[i]=48;
   hkey[16]=0;
   printf("Key hex:  \n");
   printf("%s\r",hkey);
   scanf("%s",&hkey[0]);
   HEXTOBIN(hkey,key,hexinbin);
   printf("Key bin:    "); for (i = 0; i <= 63; i++) printf("%d",key[i]); printf("\n"); printf("\n");
   ksflg = 1; cyflg = 1; desflg = 1;
   printf("DeCiphering is running...");
   d_des(inpt,key,jotput,desflg,ip,ipm,ksflg,ipc1,ipc2,glicd,cyflg,iet,ipp,is,ibin);
   printf("Done.\n"); printf("\n");
   BINTOHEX(jotput);
   printf("\n");
   printf("DeCipher bin: "); for (i = 0; i <= 63; i++) printf("%d",jotput[i]); printf("\n");
   printf("\n"); printf("Press ENTER\n");
   getchar();
}



