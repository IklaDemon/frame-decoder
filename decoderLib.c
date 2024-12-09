#include "decoderLib.h"

void getXY(int* x, int* y) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(h, &bufferInfo);
    *x=bufferInfo.dwCursorPosition.X;
    *y=bufferInfo.dwCursorPosition.Y;
}

void gotoXY(int XPos, int YPos) {
    COORD coord;
    coord.X = XPos; coord.Y = YPos;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}

/******************************************************************************
    Funzione:  alloc
    --------------------
    Genera un array di strutture
    Parametri:
        n: numero di strutture da allocare/numero di elementi dell'array
    return:
        frame_t* : l'indirizzo del primo elemento dell'array.
	Autore:
		Aleksandr Furlan
    Tempo impiegato:
        ~1 ore
******************************************************************************/

frame_t* alloc(int n)
{
    return (frame_t*)calloc(n, sizeof(frame_t));
}

/******************************************************************************
    Funzione:  numOfFrame
    --------------------
    Calcola il numero di frame dentro un file di testo.
    Parametri:
        filename: nome del file
    return:
        int: numero di frame dentro il file di testo
	Autore:
		Aleksandr Furlan
    Tempo impiegato:
        ~1 ore
******************************************************************************/

int numOfFrame(char* filename)
{
    int cnt = 0;
    FILE* ptr;
    ptr = fopen(filename, "r");
    char c;
    do
    {
        c = fgetc(ptr);
        if(c == '\n') cnt++;
    }
    while(c != EOF);
    fclose(ptr);
    return cnt+1;
}

/******************************************************************************
    Funzione:  fgetFrame
    --------------------
    Riempie l'array con il frame dipendendo da num
    Parametri:
        buffer: array in cui viene salvato il frame
        filename: nome del file
        num: numero del frame da salvare
    return:
        int: length del frame
	Autore:
		Aleksandr Furlan
    Tempo impiegato:
        ~2:30 ore
******************************************************************************/

int fgetFrame(BYTE* buffer, char* filename, int num)
{
    FILE* ptr;
    char c;
    unsigned int byteInt;
    int len = 0;
    int cnt = 0;

    ptr = fopen(filename, "r");
    if(num == 0)
    {
        do
        {
            fscanf(ptr, "%2x", &byteInt);
            buffer[len] = (unsigned char)byteInt;
            c = fgetc(ptr);
            len++;
        }
        while(c != '\n');
    }
    else
    {
        do
        {
            c = fgetc(ptr);
            if(c == '\n') cnt++;
        }
        while(cnt != num);

        do
        {
            fscanf(ptr, "%2x", &byteInt);
            buffer[len] = (unsigned char)byteInt;
            c = fgetc(ptr);
            len++;
        }
        while(c != '\n' && c != EOF);
    }
    fclose(ptr);
    return len;
}

/******************************************************************************
    Funzione:  buff2Frame
    --------------------
    Formatta il frame (contenuto in buffer) nella struttura vect
    Parametri:
        vect: indirizzo del primo elemento dell'array
        buffer: array di byte da formattare nella struct
    return:
        void
	Autore:
		Michael 80%, Furlan 20%
    Tempo impiegato:
        Furlan: ~1:30 ore Michael: ~2:30 ore
******************************************************************************/

void buff2Frame(frame_t* vect, BYTE* buffer, int i)
{
    int len, j, m, am;
    WORD typelenght;

    len = fgetFrame(buffer, "frameeth.txt", i);

    am = 0;
    for(m = 0; m < 6; m++, am++)vect[i].destinatario.b[m] = buffer[am];

    for(m = 0; m < 6; m++, am++) vect[i].mittente.b[m] = buffer[am];

    for(m = 0; m < 2; m++, am++) vect[i].typeLength[m] = buffer[am];

    typelenght = vect[i].typeLength[1] + (vect[i].typeLength[0] << 8);

    if (typelenght < 1500)
    {

        for(m = 0; m < 3; m++, am++) vect[i].data.llc[m] = buffer[am]; //LLC

        if ((vect[i].data.llc[0] == 0xAA) && (vect[i].data.llc[2] == 0x03))
        {
            vect[i].data.dataLength = len - 22;

            for(m = 0; m < 3; m++, am++) vect[i].data.oui[m] = buffer[am]; //OUI

            for(m = 0; m < 2; m++, am++) vect[i].data.protocolType[m] = buffer[am]; //protocolType

            for(m = 0; m < (typelenght - 8); m++, am++) vect[i].data.data[m] = buffer[am]; //data
        } else
        {
            vect[i].data.dataLength = len - 17;

            for(m = 0; m < 3; m++, am++) vect[i].data.oui[m] = 0x00; //OUI = NULL

            for(m = 0; m < 2; m++, am++) vect[i].data.protocolType[m] = 0x00; //protocolType = NULL

            for(m = 0, am = am - 5; m < (typelenght - 2); m++, am++) vect[i].data.data[m] = buffer[am]; //data
        }
    } else
    {
        vect[i].data.dataLength = len - 14;

        for(m = 0; m < 3; m++, am++) vect[i].data.llc[m] = 0x00; //LLC = NULL

        for(m = 0; m < 3; m++, am++) vect[i].data.oui[m] = 0x00; //OUI = NULL

        for(m = 0; m < 2; m++, am++) vect[i].data.protocolType[m] = 0x00; //protocolType = NULL

        for(m = 0, am = am - 8; m < (len - 14); m++, am++) vect[i].data.data[m] = buffer[am];
    }

    for(j = 0; j < MTU; j++) buffer[j] = 0x00;
}

/******************************************************************************
    Funzione:  printMac
    --------------------
    Controlla il bit meno significativo del primo byte dell'indirizzo MAC per determinare
    la sua tipologia.
	Parametri:
        mac : struttura contenente un array 6 byte.
    Return:
        void
	Autore:
		Montecchi 90%, Meza 10% (rivisionati da Furlan)
    Tempo impiegato:
        Montecchi: ~1:30 ore Meza: ~30 minuti
******************************************************************************/

void printMac(macAddr_t mac){
    int i;
    int flag = 1;
    for(i = 0; i < 6; i++){
        printf("%x ", mac.b[i]);
    }
    if((mac.b[0] & 0x01) == 0x01){
        for(i = 0; i < 6; i++){
            if(mac.b[i] != 0xff){
                if((mac.b[0] & 0x01) == 0x01){ // 0x03
                    flag = 0;
                    if(((mac.b[0]>>1) & 0x01) == 0){
                        printf("\n\t\t\t - S/M:  Multicast \n\t\t\t - L/U:  Universal");
                    }else{
                        printf("\n\t\t\t - S/M:  Multicast \n\t\t\t - L/U:  Local");
                    }
                }
                break;
            }
        }
        if (flag == 1){
            printf("\n\t\t\t - Tipo T/X:  Broadcast");
        }
    }
    else{
        printf("\n\t\t\t - S/M:  Singlecast");
    }
}

/******************************************************************************
    Funzione:  htoi
    --------------------
    La funzione serve per convertire da esadecimale a decimale
    Parametri:
        strHex: un array che contiene due Byte in esadecimale
    return:
        int: ritorna il numero decimale
	Autore:
		Leonardo Montecchi(riadattato da Covone)
    Tempo impiegato:
        ~2 ore
******************************************************************************/

int htoi(BYTE arr[])
{
    int res = 0;
    res = (res<<4) | ((arr[0]>>4) & 0x0f);
    res = (res<<4) | ((arr[0]) & 0x0f);
    res = (res<<4) | ((arr[1]>>4) & 0x0f);
    res = (res<<4) | ((arr[1]) & 0x0f);
    return res;
}

/******************************************************************************
    Funzione:  getFrameType
    --------------------
    ritorna 0 se ethernet 2, 1 se 802.3
    Parametri:
        framebuf: array contenente typelength
    return:
        int: 1 o 0
    Autore:
       Leonardo Montecchi(riadattato da Covone)
    Tempo impiegato:
        ~1 ora
******************************************************************************/

int getFrameType(BYTE* framebuf)
{
    if (htoi(framebuf)>1500) return 0;
    else return 1;
}

/******************************************************************************
    Funzione:  printEtherType
    --------------------
    stampa il tipo di frame
    Parametri:
        ethertype: array contenente l'ethertype
    return:
        void
    Autore:
       Leonardo Montecchi(riadattato da Covone)
    Tempo impiegato:
        ~1 ore
******************************************************************************/

void printEtherType(BYTE* ethertype)
{
    switch(htoi(ethertype))
    {
        case 0x0800:
        printf("IPv4");
        break;

        case 0x0806:
        printf("ARP");
        break;

        case 0x8035:
        printf("RARP");
        break;

        case 0x809B:
        printf("Ethertalk");
        break;

        case 0x8100:
        printf("EEE 802.1Q");
        break;

        case 0x814C:
        printf("SNMP");
        break;

        case 0x86DD:
        printf("IPv6");
        break;

        case 0x8808:
        printf("MAC Control");
        break;

        case 0x8809:
        printf("LACP");
        break;

        case 0x8847:
        printf("MPLS");
        break;

        case 0x8863:
        printf("PPPoE");
        break;

        case 0x8870:
        printf("Jumbo Frames");
        break;

        default:
        break;
    }
}

/******************************************************************************
    Funzione:  decodeLLC
    --------------------
    stampa il tipo dell'llc
    Parametri:
        frame_t*: struttura frame
    return:
        void
    Autore:
       Covone Alessio
    Tempo impiegato:
        ~1,5 ore
******************************************************************************/

void decodeLLC(frame_t* frame)
{
        if((*frame).data.llc[0] == 0xaa && (*frame).data.llc[1] == 0xaa && (*frame).data.llc[2] == 0x03){
              if((*frame).data.oui[0]== 0x00 && (*frame).data.oui[1]== 0x00 && (*frame).data.oui[2]== 0x00){
                     printf("SNAP\n");
              }
              else{
                     printf("SNAP RFC1042\n");
              }
       }
       else if((*frame).data.llc[0] == 0xf0  && (*frame).data.llc[1] == 0xf0){
              printf("Netbios\n");
       }
       else if((*frame).data.llc[0] == 0xe0 && (*frame).data.llc[1] == 0xe0){
              printf("IPX Novell\n");
       }
       else if((*frame).data.llc[0] == 0x06 && (*frame).data.llc[0] == 0x06){
              printf("IP\n");
       }
       else if((*frame).data.llc[0] == 0x42 && (*frame).data.llc[0] == 0x42){
                printf("Spanning Tree Protocol\n");
       }
}

/******************************************************************************
    Funzione:  printInfoFrame
    --------------------
    stampa con una determinata formattazione il frame passato come parametro
    Parametri:
        frame_t: struttura frame
    return:
        void
    Autore:
       Jafari Michael
    Tempo impiegato:
        ~3:00 ore
******************************************************************************/

void printInfoFrame(frame_t frame)
{
    int typelength;
    int m;

    printf("Frame: ");
    for(m = 0; m < 26; m++) printf("%02x ", frame.destinatario.b[m]);

    if (getFrameType(frame.typeLength)) printf("... ...\n\n==> frame Ethernet 802.3 LLC\n");
    else printf("... ...\n\n==> frame Ethernet 2\n");

    printf("\nMAC address dst: ");
    printMac(frame.destinatario);

    printf("\nMAC address src: ");
    printMac(frame.mittente);

    printf("\n\t\t\t - OUI:  %02X-%02X-%02X", frame.mittente.b[0], frame.mittente.b[1], frame.mittente.b[2]);

    printf("\nLength: ");
    for(m = 0; m < 2; m++) printf("%02x", frame.typeLength[m]);

    typelength = htoi(frame.typeLength);
    printf(" (%dB)", typelength);

    if (getFrameType(frame.typeLength)) {

        printf("\nLLC: ");
        for(m = 0; m < 3; m++) printf("%02x ", frame.data.llc[m]);

        if (frame.data.llc[0] == 0xAA && frame.data.llc[2] == 0x03) {
            printf("(SNAP)\nSNAP:\n   ");


            printf("OUI: ");
            for(m = 0; m < 3; m++) printf("%02x ", frame.data.oui[m]);


            printf("\n   Type ID: ");
            for(m = 0; m < 2; m++) printf("%02x", frame.data.protocolType[m]);
            printEtherType(frame.data.protocolType);
        }


    } else {

        printf("\n");
        printEtherType(frame.typeLength);

    }

    printf("\nPayload:\n   ");
    for(m = 0; m < 6; m++) printf("%02x ", frame.data.data[m]);
    printf("... ...");

    if (getFrameType(frame.typeLength)) printf("\n\nLength field corresponds to packet length");
}

/******************************************************************************
    Funzione:  addFrame
    --------------------
    salva frame su file di testo, aggiunge il frame sul array di strutture, stampa la info.
    Parametri:
        vect: array di strutture
        buffer: array con il frame da aggiungere
    return:
        void
    Autore:
       Aleksandr Furlan
    Tempo impiegato:
        ~4:00 ore
******************************************************************************/

void addFrame(frame_t* vect, BYTE* buffer)
{
    int i, len, num;
    FILE* fp;
    fp = fopen("frameeth.txt", "a");

    fprintf(fp, "\n");
    for(len = 0; buffer[len] != '\0'; len++);

    for(i = 0; i < len; i += 2)
    {
        fprintf(fp, "%c%c", buffer[i], buffer[i+1]);
        if(i < len - 2) fprintf(fp, " ");
    }
    fclose(fp);
    for(i = 0; i < MTU; i++) buffer[i] = 0x00;

    num = numOfFrame("frameeth.txt") - 1;

    buff2Frame(vect, buffer, num);

    printf("------------------------------------------------------------\n");
    printInfoFrame(vect[num]);
    printf("\n------------------------------------------------------------\n");
}
