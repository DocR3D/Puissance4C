#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(x) close(x)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
//Socket permettant d'accepter les connexions pour le serveur
SOCKET sock;

//Socket permettant d'envoyer des informations à l'autre joueur
SOCKET csock;


/*Partie Serveur*/
//Fonction permettant de créer un socket
SOCKET * creationSocket(){
	// TODO Faire des allocations mémoires
	//socket(int domain, int type, int protocol)
	sock = socket(AF_INET, SOCK_STREAM,0);
	if(sock<0){
		return 0;
	}
	return &sock;
}

//fonction permettant de configurer le socket
SOCKADDR_IN parametrageSocket(unsigned int ip,unsigned short port){
	SOCKADDR_IN sin;
	sin.sin_addr.s_addr = htonl(ip);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	return sin;
}

//Fonction permettant de créer un serveur sur le port souhaité
int creationServeur(unsigned short port){
	//Créeer le socket
	creationSocket();
	//Configures le socket
	SOCKADDR_IN sin = parametrageSocket(htonl(INADDR_ANY),port);
	int i = bind(sock,(SOCKADDR*)&sin, sizeof(sin));
	if(i<0){
		return 0;
	}
	//Attends une connexion sur le socket
	int j = listen(sock, 1);
	/*Attente du client*/
	SOCKADDR_IN csin;
	socklen_t crecsize=sizeof(csin);
	//Acceptes la prochaine connexion et définit le socket de communication
	csock = accept(sock, (SOCKADDR*)&csin, &crecsize);
	if(csock == -1) return 0;
	else return 1;
}

//Ferme le socket TODO Utiliser cette fonction
int fermerSocketServeur(SOCKET sock){
	int i = closesocket(sock);
	return i;
}

/*Partie client*/
//Fonction permettant de se connecter à un socket à l'ip et le port choisis
int connection(unsigned int ip,unsigned short port){
	//Crééer le socket
	creationSocket();
	//Configures le socket
	SOCKADDR_IN sin = parametrageSocket(ip,port);
	//Tentes de se connecter au socket choisis 
	if(connect(sock, (SOCKADDR*)&sin, sizeof(sin))!= SOCKET_ERROR){
		csock = sock;
		return 1;
	}else{
		return 0;
	}
}

//Fonction permettant d'envoyer des informations sur le socket de communication
int envoieInfo(char * info){
	int i = send(csock, info, sizeof(info), 0);
	return i;
}

//Fonction permettant de recevoir des informations sur le socket de communication
char * receptionInfo(){
        unsigned char * data = malloc(sizeof(unsigned char)*512);
	int n = 0;
		if(n = recv(csock,data,sizeof(data), 0) == SOCKET_ERROR){
		    fprintf(stderr, "recv: %s (%d)\n", strerror(errno), errno);	
		}else{	
			return data;
		}
}

