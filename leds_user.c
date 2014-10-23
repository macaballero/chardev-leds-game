/*
*	leds_user.c
*
*	Copyright (C) Miguel A. Caballero 
*
*
*	A game center based on our own designed module.
*	You can choose 5 different games to have fun with the leds on your keyboard.
*	"modoAlternate" and "partyTime" are multithreaded.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>


/*Prototipos*/
void contadorBinario(int fd);
void escritorBinario(int fd);
void simon(int fd);
void modoAlternante(int fd);
void partyTime(int fd);
char traducirNumero(int num);
void *funcionDelHiloAlternante(void *);
void *funcionDelHiloParty(void *);

/*Variables globales*/
static int delay=200000;
static char msg[3];
static int intHilo=-1;

#define _REENTRANT

int main()
{
	srand(time(NULL));
	int opMenu=-1;
	printf("----Centro de Operaciones KazMig V1.0----\n");

	int fichero = open("/dev/leds", O_WRONLY);
	if(fichero < 0)
	{
		printf("No se ha podido abrir /dev/leds \n");
		exit(-1); 
	}

	do
	{
		printf("1-Contador Binario\n");
		printf("2-Escritor Binario\n");
		printf("3-Jugar a Simon\n");
		printf("4-Modo Alternante\n");
		printf("5-Party Time\n");
		printf("0-Salir\n------\n");
		printf("Elige una opcion: ");
		scanf("%d",&opMenu);
		
		switch(opMenu)
		{
			case 1:
				contadorBinario(fichero);
				break;
			case 2:
				escritorBinario(fichero);
				break;
			case 3:
				simon(fichero);
				break;
			case 4:
				modoAlternante(fichero);
				break;
			case 5:
				partyTime(fichero);
				break;
			case 0:
				break;
			default:
				printf("Opcion incorrecta\n");
				break;
		}
	}while(opMenu != 0);

	close(fichero);
	return 0;
}

void contadorBinario(int fd)
{
	int opBinario;
	printf("Contador Binario.\n");
	printf("1: Cuenta normal, 2: Cuenta atras, 0: Salir: ");
	scanf("%d",&opBinario);
	if(opBinario != 0)
	{
		switch(opBinario)
		{
			case 1:
				msg[0]='4';
				write(fd,msg,1);
				usleep(500000);
				msg[0]='3';
				write(fd,msg,1);
				usleep(500000);
				msg[0]='2';
				write(fd,msg,1);
				usleep(500000);
				msg[0]='2';
				msg[1]='3';
				write(fd,msg,2);
				usleep(500000);
				msg[0]='1';
				write(fd,msg,1);
				usleep(500000);
				msg[0]='1';
				msg[1]='3';
				write(fd,msg,2);
				usleep(500000);
				msg[0]='1';
				msg[1]='2';
				write(fd,msg,2);
				usleep(500000);
				msg[0]='1';
				msg[1]='2';
				msg[2]='3';
				write(fd,msg,3);
				usleep(500000);
				msg[0]='4';
				write(fd,msg,1);
				break;
			case 2:
				msg[0]='4';
				write(fd,msg,1);
				usleep(500000);
				msg[0]='1';
				msg[1]='2';
				msg[2]='3';
				write(fd,msg,3);
				usleep(500000);
				msg[0]='1';
				msg[1]='2';
				write(fd,msg,2);
				usleep(500000);
				msg[0]='1';
				msg[1]='3';
				write(fd,msg,2);
				usleep(500000);
				msg[0]='1';
				write(fd,msg,1);
				usleep(500000);
				msg[0]='2';
				msg[1]='3';
				write(fd,msg,2);
				usleep(500000);
				msg[0]='2';
				write(fd,msg,1);
				usleep(500000);
				msg[0]='3';
				write(fd,msg,1);
				usleep(500000);
				msg[0]='4';
				write(fd,msg,1);
				usleep(500000);
				break;
			case 0:
				break;
			default:
				printf("Opcion incorrecta\n");
				break;
		}
	}
}

void escritorBinario(int fd)
{
	int opBinario;
	int salir=0;
	printf("Escritor Binario.\n");
	printf("Escribe un numero del 0 al 7 y los leds lo representaran en binario: ");
	scanf("%d",&opBinario);
	switch(opBinario)
	{
		case 0:
			msg[0]='4';
			write(fd,msg,1);
			break;
		case 1:
			msg[0]='3';
			write(fd,msg,1);
			break;
		case 2:
			msg[0]='2';
			write(fd,msg,1);
			break;
		case 3:
			msg[0]='2';
			msg[1]='3';
			write(fd,msg,2);
			break;
		case 4:
			msg[0]='1';
			write(fd,msg,1);
			break;
		case 5:
			msg[0]='1';
			msg[1]='3';
			write(fd,msg,2);
			break;
		case 6:
			msg[0]='1';
			msg[1]='2';
			write(fd,msg,2);
			break;
		case 7:
			msg[0]='1';
			msg[1]='2';
			msg[2]='3';
			write(fd,msg,3);
			break;
		default:
			printf("Opcion incorrecta\n");
			break;
	}
}

void simon(int fd)
{
	int numeroLeds, opMenu1=3, temp, dificultad=1, correcto =1, frecuencia,i=0,j=0;
	char borrar='4';
	printf("Bienvenido a Simon led-umina!\n");
	while(opMenu1 !=0)
	{
		printf("1: Jugar, 2: Cambiar dificultad, 0: Salir.\n");
		scanf("%d",&opMenu1);
		switch(opMenu1)
		{
			case 1://Jugar
				if(dificultad==1)
				{
					frecuencia=500000;
					numeroLeds=3;
				}
				else
				{
					frecuencia=250000;
					numeroLeds=5;
				}
				
				while(correcto == 1)
				{
					char msg[numeroLeds];
					for(i;i<numeroLeds;i++)//Generar leds aleatorios
						msg[i]=traducirNumero(rand()%3+1);
					
					i=0;
					printf("Atento...\n");
					usleep(frecuencia);
					for(i;i<numeroLeds;i++)//Mostrar en Led
					{
						write(fd,&msg[i],1);
						usleep(frecuencia);
						write(fd,&borrar,1);
						usleep(frecuencia);
					}

					i=0;

					for(i;i<numeroLeds && correcto == 1;i++)//Comprobacion
					{
						printf("Escribe el led nº%d :",i);
						scanf("%d",&temp);
						if(traducirNumero(temp) != msg[i])
						{
							printf("Te has confundido. HAS PERDIDO\n");
							msg[0]='1';
							msg[1]='2';
							msg[2]='3';
							while(j<3)
							{
								write(fd,&msg,3);
								usleep(250000);
								write(fd,&borrar,1);
								usleep(250000);
								j++;
							}
							j=0;
							correcto=0;
						}
					}
					i=0;
					if(correcto!=0)
					{
						printf("Todo correcto. Aumentando en 1 los leds...\n");
						usleep(frecuencia*2);
						numeroLeds++;
					}
				}
				correcto=1;//Reseteamos el indicador de poder jugar, para futuros reintentos
				break;
			case 2://Cambiar Dificultad
				printf("Elige dificultad (1-Facil, 2-Dificil): ");
				scanf("%d",&temp);
				if(temp <1 || temp >2)
					printf("Valor no valido. Dificultad no cambiada");
				else
					dificultad=temp;
				break;
			case 0://salir
				break;
			default://retraso
				printf("Opcion incorrecta\n");
				break;
		}
	}
	//Dificultad solamente cambiar la frecuencia
	//por cada fase superada, se aumenta en uno el numero de leds a recordar	
}

void modoAlternante(int fd)
{
	pthread_t idHilo;
	if(pthread_create(&idHilo,NULL,funcionDelHiloAlternante,NULL) != 0)
		printf("Error al crear el Hilo");

	printf("Modo alternante\n");
	printf("1: aumentar frecuencia, 2: disminuir frecuencia, 0: salir\n");
	if(intHilo == 0)//Reseteamos la variable señal si no es la primera vez que entramos
		intHilo=-1;
	while(intHilo != 0)
	{
		msg[0]='1';
		write(fd,msg,1);
		usleep(delay);
		msg[0]='2';
		write(fd,msg,1);
		usleep(delay);
		msg[0]='3';
		write(fd,msg,1);
		usleep(delay);
	}
	msg[0]='4';
	write(fd,msg,1);
}

void partyTime(int fd)
{
	int partyLuces=rand()%3+1;
	pthread_t idHilo;
	if(pthread_create(&idHilo,NULL,funcionDelHiloParty,NULL) != 0)
		printf("Error al crear el Hilo");

	printf("Bienvenido a la fiesta!\n");
	printf("0: Salir\n");
	if(intHilo == 0)//Reseteamos la variable señal si no es la primera vez que entramos
		intHilo=-1;
	while(intHilo != 0)
	{
		msg[0]=traducirNumero(partyLuces);
		write(fd,msg,1);
		usleep(delay);
		partyLuces=rand()%3+1;
		delay=rand()%500000+1;
	}
	msg[0]='4';
	write(fd,msg,1);
}

char traducirNumero(int num)
{
	if(num == 1)
		return '1';
	else if(num ==2)
		return '2';
	else if(num ==3)
		return '3';
}

void *funcionDelHiloAlternante(void *parametro)
{
	while(intHilo != 0)
	{
		scanf("%d",&intHilo);
		switch(intHilo)
		{
			case 1:
				delay += 15000;
				break;
			case 2:
				if(delay >= 14999)
					delay -= 15000;
				else
					printf("No puedes bajar mas la frecuencia\n");
				break;
			default:
				break;
		}
	}
}

void *funcionDelHiloParty(void *parametro)
{
	scanf("%d",&intHilo);
}
