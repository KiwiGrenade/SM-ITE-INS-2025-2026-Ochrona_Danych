#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ciph.hpp"

typedef int gl32array[32];
typedef int gl48array[48];
typedef int gl56array[56];
typedef int gl64array[64];
typedef int gl16_4_8array[16][4][8];
typedef int gl4_16array[4][16];

void         d_des(gl64array	inpt,
		 gl64array	key,
		 gl64array  	jotput,
		 int		desflg,
		 gl64array	ip,
		 gl64array	ipm,
		 int		ksflg,
		 gl56array	ipc1,
		 gl48array	ipc2,
		 gl56array	glicd,
		 int	    	cyflg,
		 gl48array	iet,
		 gl32array	ipp,
		 gl16_4_8array	is,
		 gl4_16array	ibin)

  /* The values of ip and ipm are read from the file desinp.dat.
 The procedure GLOPEN is used to assign desinp.dat
 to infile and open the file for reading. */
{
   int      	j, ii, ic, i;
   gl32array    titmp, icf;
   gl64array    itmp;
   int      	kns[48][16];
   gl48array    tkns;
   FILE *       infile;

   if (desflg) {
      desflg = 0;
      glopen(&infile,"desinp.dat");
      for (i = 0; i <= 63; i++) fscanf(infile,"%d",&(ip[i]));
      for (i = 0; i <= 63; i++) fscanf(infile,"%d",&(ipm[i]));
      fclose(infile);
   }
   for (i = 0; i <= 15; i++) {
      ks(key,i,tkns,ksflg,ipc1,ipc2,glicd);
      for (j = 0; j <= 47; j++) kns[j][15-i] = tkns[j];   // zmiana kolejnosci kluczy z i na 15-i
   }
   for (j = 0; j <= 63; j++) itmp[j] = inpt[ip[j]-1];
   for (i = 0; i <= 15; i++) {
      ii = i;
      for (j = 0; j <= 47; j++) tkns[j] = kns[j][ii];
      for (j = 0; j <= 31; j++) titmp[j] = itmp[32 + j];
      cyfun(titmp,tkns,icf,cyflg,iet,ipp,is,ibin);
      for (j = 0; j <= 31; j++) {
	 ic = icf[j] + itmp[j];
	 itmp[j] = itmp[j + 32];
	 itmp[j + 32] = abs(ic % 2);
      }
   }
   for (j = 0; j <= 31; j++) {
      ic = itmp[j];
      itmp[j] = itmp[j + 32];
      itmp[j + 32] = ic;
   }
   for (j = 0; j <= 63; j++) jotput[j] = itmp[ipm[j]-1]; }
