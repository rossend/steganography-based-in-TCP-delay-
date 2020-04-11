#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>    
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>

//Funcio que introdueix a bits el bits de test a enviar
int test(int bits[])
{
	int i=0,nbits=0;
	printf("Introduir el numero de bits a usar per al test:\n");
	scanf("%i",&nbits);
	while(i<nbits)
	{
		bits[i]=1;
		bits[i+1]=0;
		i=i+2;
	}
	return nbits;
}

//Funcio que converteix el text a la seva representacio binaria
int conversor(int bits[])
{
	char text[300];
	int in=0;
	int contador=0, num,resul,zerouns[20],i;
	printf("Introduir el text sense espais ha enviar:\n");
	scanf("%s",text);
	while(contador < strlen(text))
	{
		i=1;
		num = text[contador];
		while(num>0)
		{
			resul = num%2;
			if(resul==0)
			{
				zerouns[i]=0;
				i=i+1;
			}
			else
			{
				zerouns[i]=1;
				i=i+1;
				num=num-1;
			}
			num=num/2;
		}
		i=i-1;
		while(i>0)
		{
			bits[in]=zerouns[i];
			i=i-1;
			in++;
		}
		contador++;
	}
	return in;
}

//Funcio que afegeix les repeticions de cada bit
int repeticio(int bits[], int nbits , int repeticions)
{
	int buffer[10000];
	int i=0,aux=0;
	while (i<nbits)
	{
		int r=0;
		while(r<repeticions)
		{	
			buffer[aux++]=bits[i];
			r++;
		}
		i++;
	}
	i=0;
	while (i<aux)
	{	
		bits[i]=buffer[i];
		i++;
	}
	return aux;
	
}

int main(int argc, char *argv[])
{
	//Controlem la entrada

	if(argc!=6)
	{
		printf("Usar: \n");
		printf("\t%s <destinacio> <port> <time> <repeticions> -t Per ussar el mode TEST\n",argv[0]);
		printf("\t%s <destinacio> <port> <time> <repeticions> -m Per introduir missatge manual\n",argv[0]);
		return(-1);
	}
	
	//Definim Variables	
	
	int nbits=0,t=atoi(argv[3]),port=atoi(argv[2]),fd,repeticions=atoi(argv[4]),aux=0, bits[10000];
	char *buffer = malloc(5), buffero[100];
	struct hostent *he;
	struct sockaddr_in server;  

	//Aconseguim el missatge

	if(strcmp(argv[5],"-t")==0)
	{
		nbits=test(bits);
	}
	
	if(strcmp(argv[5],"-m")==0)
	{
		nbits=conversor(bits);
	}

	nbits=repeticio(bits,nbits,repeticions);
	
	//Relitzem la conexio TCP

	if ((he=gethostbyname(argv[1]))==NULL)
	{
     		printf("gethostbyname() error\n");
      		return(-1);
   	}

   	if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
      		printf("socket() error\n");
      		return(-1);
   	}

   	server.sin_family = AF_INET;
   	server.sin_port = htons(port);
   	server.sin_addr = *((struct in_addr *)he->h_addr);
   	bzero(&(server.sin_zero),8);

   	if(connect(fd, (struct sockaddr *)&server,sizeof(struct sockaddr))==-1)
	{ 
      		printf("connect() error\n");
      		return(-1);
   	}

	//Comensem l'enviament
	//Algorisme d'enviament
	sprintf(buffero, "%i", t);
	send(fd,buffero,100,0);
	usleep(t);
	sprintf(buffero, "%i", nbits);
	send(fd,buffero,100,0);
	usleep(t);
	sprintf(buffero, "%i", repeticions);
	send(fd,buffero,100,0);
	usleep(t);
	buffer="Hoot\0";
	while(nbits>0)
	{
		if(bits[aux]==0)
		{
			printf("ENVIA un 0\n");
			send(fd,buffer,20,0);
			usleep(1);	
			send(fd,buffer,20,0);
		}
		else
		{
			printf("ENVIA un 1\n");
			send(fd,buffer,20,0);
			usleep(t); 
			send(fd,buffer,20,0);
		}
		aux++;
		nbits--;
		//Per a que no envii abanç de k acabi de transmetre
		usleep(t); 
	}
	buffer="/q\0";
	send(fd,buffer,20,0);
	//printf("Final de transmisió\n");
   	close(fd);

}
