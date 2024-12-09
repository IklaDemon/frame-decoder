#ifndef DECODERLIB_H_INCLUDED
#define DECODERLIB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

#define BYTE unsigned char
#define WORD unsigned short
#define buffcleaner while((getchar())!='\n')

#define MTU 1500
#define HEADER_FRAME 14
#define FCS 4

typedef struct
{
    BYTE llc[3];
    BYTE oui[3];
    BYTE protocolType[2];
    BYTE data[MTU];
    int dataLength;
} payload_t;

typedef struct
{
    BYTE b[6];
} macAddr_t;

typedef struct
{
    macAddr_t destinatario;
    macAddr_t mittente;
    BYTE typeLength[2];
    payload_t data;
} frame_t;

void getXY(int* x, int* y);
void gotoXY(int XPos, int YPos);

frame_t* alloc(int n);
int numOfFrame(char* filename);
int fgetFrame(BYTE* buffer, char* filename, int num);//Read frame from file and save in a buffer; return number of byte readed from file
void buff2Frame(frame_t* vect, BYTE* buffer, int i);  //convert buffer in a frame struct
void printMac(macAddr_t mac);      //stampa il mac e lo caratterizza (U/M, G/L, ...)
int htoi(BYTE strHex[]);
int getEthType(BYTE* framebuf);	//return EthType/Length field
void printEtherType(BYTE* ethertype);
void decodeLLC(frame_t* frame); //decodifica LLC stampando le info (SAP, OUI, ecc) e ritorna DSAP o EthType se presente
void printInfoFrame(frame_t frame);//funzione completa che restituisce la descrizione completa del frame
void addFrame(frame_t* vect, BYTE* buffer);

#endif // DECODERLIB_H_INCLUDED
