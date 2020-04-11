#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>    
#include <unistd.h>
#include <errno.h>
#include <time.h>

//Funcio que calcula potencies de base x i exponent y
int pot(int x, int y)
{
	int potencia=1,i=0;
	while ( i < y ) 
	{
		potencia=potencia*x;
		++i;
	} 
	if(x==0)
	{
		return 0;
	}
	return potencia;

}

//Funcio que converteix de binaria a text
int conversor (int nbits, int bits[])
{
	int si[10000],index=0,a=0;
	while(a<=nbits/7)
	{
		int temp=6,num=0;
		while(temp>=0)
		{
			num=num+pot(bits[index++]*2,temp);
			temp--;
		}
		printf("%c",num);
		a++;	
	}
	printf("\n");
}

//Funcio que elimina les repeticions de bits i es queda amb la repetida un major numbre de vegades
int * eliminar_repeticio(int bits[], int nbits , int repeticions)
{
	int buffer[10000];
	int i=0,aux=0;
	while (i<nbits)
	{
		int r=0;
		int zero=0,uns=0;
		while(r<repeticions)
		{
			if (bits[i+r]==0)
			{
				zero++;
			}
			if (bits[i+r]==1)
			{
				uns++;
			}
		r++;
		i++;
		}
		if(zero>uns)
		{
			buffer[aux]=0;
		}

		if(uns>zero)
		{
			buffer[aux]=1;
		}
		aux++;
	}
	i=0;
	aux=nbits/repeticions;
	while (i<aux)
	{	
		bits[i]=buffer[i];
		i++;
	}
	return aux;
	
}

//Funcio que introdueix a bits el bits de test a comparar
int* test(int nbits){
	int bits[10000];
    	int i=0, aux;
    	while(i<nbits)
       	{
        	bits[i]=1;
        	bits[i+1]=0;
        	i=i+2;
        }
    	return bits;
}

//Funcio que compara si dues cadenes son iguals i calcula el persentatge de fallades entre les dues
float comparator(int* bits1,int* bits2,int nbits){
    	float conta=0.0;
    	int i=0;
    	while(i<nbits)
	{
			if(bits1[i]!=bits2[i])
		    	{
		    		conta++;
		    	}

		i++;
    	}
    	return (conta/nbits)*100;
}

int main(int argc, char *argv[])
{

	// Comprovacio del nombre de parametres
	if(argc!=3)
	{
		printf("Usar: \n");
		printf("\t%s <port> -t\n Per al mode TEST",argv[0]);
		printf("\t%s <port> -m\n Per introduir missatge manual",argv[0]);
		exit(0);
	}

	//Definim variables
	double temps,t=0.0;
	int fd, fd2, sin_size, numbytes,port=atoi(argv[1]),bits[1000],nbits=0,index=0,in=0,repeticio=0;
        int * tester,a;
	char buf[100];  
	struct timeval ti, tf;
	struct sockaddr_in server; 
	struct sockaddr_in client;

	//Relitzem la conexio TCP
	if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{  
      		printf("error en socket()\n");
      		return(-1);
   	}

   	server.sin_family = AF_INET;         
   	server.sin_port = htons(port); 
   	server.sin_addr.s_addr = INADDR_ANY; 
   	bzero(&(server.sin_zero),8);

   	if(bind(fd,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1) 
	{
      		printf("error en bind() \n");
      		return(-1);
  	}     

   	if(listen(fd,1) == -1) 
	{
      		printf("error en listen()\n");
      		return(-1);
   	}

      	sin_size=sizeof(struct sockaddr_in);

      	if ((fd2 = accept(fd,(struct sockaddr *)&client, &sin_size))==-1) 
	{
        	printf("error en accept()\n");
        	exit(-1);
      	}

	//Comensem la recepcio
	recv(fd2,buf,100,0);
	printf("Missatge rebut: %s\n",buf);
	t=atoi(buf);      
	t=t*0.001;
	recv(fd2,buf,100,0);
	printf("Missatge rebut: %s\n",buf);
	nbits=atoi(buf);
	recv(fd2,buf,100,0);
	printf("Missatge rebut: %s\n",buf);
	repeticio=atoi(buf);

	//Algorisme de recepcio
   	while(index<=nbits-1) 
	{  
		recv(fd2,buf,100,0);
		printf("Missatge rebut: %s\n",buf);
		gettimeofday(&ti, NULL);   //Instant inicial
		recv(fd2,buf,100,0);
		gettimeofday(&tf, NULL);   //Instant final
		printf("Missatge rebut: %s\n",buf);

		temps= (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0;

		if(temps>=0.0 && temps<t*0.5)
		{
			bits[index++]=0;
			printf("%i\n",bits[index-1]);
		}

		else
		{
		        bits[index++]=1;
		        printf("%i\n",bits[index-1]);
		}
	}

	recv(fd2,buf,100,0);
	close(fd2);
	close(fd);
	//Tractament final de les dades (eleminar repeticions i mostra per pantalla)
	eliminar_repeticio(bits,nbits,repeticio);
	index=0;
	printf("La sequencia de bits entrada és:\n");
	while(index<(nbits/repeticio))
	{
	        printf("%i\n",bits[index]);
		index++;
	}
	if(strcmp(argv[2],"-t")==0)
	{
		tester=test(nbits);
		printf("\nTEMPS= %f ERROR =  %f\n",t,comparator(tester,bits,(nbits/repeticio)));
	}
	
	if(strcmp(argv[2],"-m")==0)
	{
		conversor(nbits,bits);
	}
    	

}

