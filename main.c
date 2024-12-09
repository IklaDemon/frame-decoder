#include "decoderLib.h"

int main()
{
    int i, flag = 0;
    int x = 0, y = 0;
    char comando;
    int scelta;

    BYTE buffer[MTU] = {0x00};
    frame_t* vect = alloc(numOfFrame("frameeth.txt"));
    for(i = 0; i < numOfFrame("frameeth.txt"); i++) buff2Frame(vect, buffer, i);

    while(scelta != 3)
    {
        //grafica
        system("CLS");
        gotoXY(1, 0);
        printf("Stampa Info");
        gotoXY(1, 1);
        printf("Aggiungi Frame");
        gotoXY(1, 2);
        printf("Esci");

        //settaggi
        scelta = 0;
        comando = 'z';
        flag = 0;

        //interfaccia
        gotoXY(x, y);
        getXY(&x, &y);
        do
        {
            if(kbhit())
            {
                comando = getch();
                switch(comando)
                {
                    case 'w':
                    if(y > 0)
                    {
                        y--;
                        gotoXY(x, y);
                    }
                    break;

                    case 's':
                    if(y < 2)
                    {
                        y++;
                        gotoXY(x, y);
                    }
                    break;

                    case ' ':
                    switch(y)
                    {
                        case 0:
                        scelta = 1;
                        break;
                        case 1:
                        scelta = 2;
                        break;
                        case 2:
                        scelta = 3;
                        break;
                    }
                    break;

                    default:
                    break;
                }
            }

        }while(comando != ' ');

        switch(scelta)
        {
            case 1:
            system("CLS");
            printf("\nNumero di Frame Trovati: %d\n", numOfFrame("frameeth.txt"));
            printf("------------------------------------------------------------\n");
            for (i = 0; i < numOfFrame("frameeth.txt"); i++) {
                printInfoFrame(vect[i]);
                printf("\n------------------------------------------------------------\n");
            }
            printf("Premi spazio per tornare indietro: \n");
            while(flag != 1)
            {
                if(kbhit())
                {
                    scelta = getch();
                    if(scelta == ' ') flag = 1;
                }
            }
            break;

            case 2:
            system("CLS");
            vect = (frame_t*)realloc(vect, (numOfFrame("frameeth.txt") + 1) * sizeof(frame_t));
            printf("Frame: ");
            scanf("%s", buffer);
            printf("\n%s", buffer);
            printf("\nAnalizzando...\n");
            addFrame(vect, buffer);
            printf("\nPremi spazio per tornare indietro: \n");
            while(flag != 1)
            {
                if(kbhit())
                {
                    scelta = getch();
                    if(scelta == ' ') flag = 1;
                }
            }
            break;

            default:
            break;
        }
    }
    gotoXY(0, 29);
    free(vect);
    return 0;
}
