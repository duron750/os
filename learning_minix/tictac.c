#include <stdio.h>
#include <stdlib.h>

char matrice[3][3];

char verifica(void);
void matrice_init(void);
void muta_jucatorul(void);
void muta_calculatorul(void);
void afis_matrice(void);

void main(void)
{
	char gata;
	printf("Acesta este jocul X si O.\n");
	printf("Veti juca cu calculatorul.\n");

	gata=' ';
	matrice_init();
	do
	{
		afis_matrice();
		muta_jucatorul();
		gata=verifica();
		if(gata!=' ') break;
		muta_calculatorul();
		gata=verifica();
	} while(gata==' ');
	if(gata=='X') printf("Ati castigat!\n");
	else printf("Calculatorul a castigat!\n");
	afis_matrice();
}

void matrice_init(void)
{
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++) matrice[i][j]=' ';
}

void muta_jucatorul(void)
{
	int x,y;
	printf("Introduceti coordonatele pentru X: ");
	scanf("%d%d", &x, &y);
	x--; y--;
	if(matrice[x][y]!=' ')
	{
		printf("Mutare incorecta, incercati din nou. \n");
		muta_jucatorul();
	} else matrice[x][y]='X';
}

void muta_calculatorul(void)
{
	int i,j;
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
			if(matrice[i][j]==' ') break;
		if(matrice[i][j]==' ') break;
	}
	if(i*j==9)
	{
		printf("Gata\n");
		exit(0);
	} else matrice[i][j]='O';
}

void afis_matrice(void)
{
	int t;
	for(t=0;t<3;t++)
	{
		printf(" %c | %c | %c ", matrice[t][0], matrice[t][1], matrice[t][2]);
		if(t!=2) printf("\n---|---|---\n");
	}
	printf("\n");
}

char verifica(void)
{
	int i;
	for(i=0;i<3;i++)
		if(matrice[i][0]==matrice[i][1] && matrice[i][0]==matrice[i][2]) return matrice[i][0];
	for(i=0;i<3;i++)
		if(matrice[0][i]==matrice[1][i] && matrice[0][i]==matrice[2][i]) return matrice[0][i];
	if(matrice[0][0]==matrice[1][1] && matrice[1][1]==matrice[2][2]) return matrice[0][0];
	if(matrice[0][2]==matrice[1][1] && matrice[1][1]==matrice[2][0]) return matrice[0][2];
	return ' ';
}
