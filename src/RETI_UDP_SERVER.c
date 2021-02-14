#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define MAXBUFFER 512
#define PORT 9999

void clearWSA();

int main(void) {

#ifdef WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		fprintf(stderr, "Si è verificato un errore in WSAStartup()\n");
		return EXIT_FAILURE;
	}
#endif

	// creazione socket
	int serverSocket;
	serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket < 0) {
		fprintf(stderr, "Si è verificato un errore durante l'inizializzazione della socket.\n");
		clearWSA();
		return EXIT_FAILURE;
	}

	// costruzione dell'indirizzo del server
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

	// bind della socket
	if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
		fprintf(stderr, "Si è verificato un errore durante il binding.\n");
		clearWSA();
		return EXIT_FAILURE;
	}

	struct sockaddr_in clientAddress;
	unsigned int clientAddressLength;

	printf("In attesa del client...\n");

	char buffer[MAXBUFFER];
	int recvMessageSize;

	// ricezione della stringa echo dal client
	while (1) {
		clientAddressLength = sizeof(clientAddress);
		recvMessageSize = recvfrom(serverSocket, buffer, MAXBUFFER, 0, (struct sockaddr*) &clientAddress, &clientAddressLength);

		printf("\nRicevuto da %s: %s\n", inet_ntoa(clientAddress.sin_addr), buffer);

		// rinvia la stringa echo al client
		if (sendto(serverSocket, buffer, recvMessageSize, 0, (struct sockaddr*) &clientAddress, sizeof(clientAddress)) != recvMessageSize) {
			fprintf(stderr, "Si è verificato un errore durante l'invio dei dati.\n");
			clearWSA();
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

void clearWSA() {
#ifdef WIN32
	WSACleanup();
#endif

}
