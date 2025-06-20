#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int aposta[10], q, sorteados[6], corretas=0, vetor_corretas[6];

void resetarVetores()
{
    int i;

    for(i=0; i<10; i++){
            aposta[i]=100;
        }
        for(i=0; i<6; i++){
            sorteados[i]=0;
        }
        for(i=0; i<6; i++){
            vetor_corretas[i]=-1;
        }
        corretas=0;
}

void ordenarVetor(int vetor[], int tamanho)
{
    int i, s;

    for(i=0; i<tamanho; i++){
        if(i<tamanho-1){
            if(vetor[i]>vetor[i+1]){
                s=vetor[i];
                vetor[i]=vetor[i+1];
                vetor[i+1]=s;
                i=-1;
            }
        }
        else{
            if(vetor[i]<vetor[i-1]){
                s=vetor[i];
                vetor[i]=vetor[i-1];
                vetor[i-1]=s;
                i=-1;
            }
        }
    }
}

void apostaFunc()
{
    int i, j;

    printf("Digite a quantidade de dezenas para apostar: ");
    scanf("%i", &q);
    while(q<6 || q>10){
        printf("So e permitido apostar entre 6 e 10 dezenas\n");
        printf("Digite a quantidade de dezenas para apostar: ");
        scanf("%i", &q);
    }
    for(i=0; i<q; i++){
        printf("Dezena %i: ", (i+1));
        scanf("%i", &aposta[i]);
        for(j=0; j<q; j++){
            if(i!=j){
                if(aposta[i]<1 || aposta[i]>60){
                    printf("As dezenas devem estar entre 1 e 60, incluindo-os\n");
                    i--;
                    break;
                }
                else if(aposta[i]==aposta[j]){
                    printf("Nao e permitido apostar duas vezes no mesmo numero\n");
                    i--;
                    break;
                }
            }
        }
    }
    printf("\nAs dezenas apostadas foram:\n");
    ordenarVetor(aposta, q);
    for(i=0; i<q-1; i++){
        printf("%i, ", aposta[i]);
    }
    printf("%i\n", aposta[i]);
}

void sorteioFunc(char nome[])
{
    int i, j;

    for(i=0; i<6; i++){
        sorteados[i]=(rand()%60)+1;
        for(j=0; j<6; j++){
            if(sorteados[i]==sorteados[j] && i!=j){
                i--;
                break;
            }
        }
    }
    ordenarVetor(sorteados, 6);
    printf("As dezenas sorteadas foram:\n");
    for(i=0; i<5; i++){
        printf("%i, ", sorteados[i]);
    }
    printf("%i\n", sorteados[i]);

    for(i=0; i<q; i++){
        for(j=0; j<6; j++){
            if(sorteados[j]==aposta[i]){
                vetor_corretas[corretas]=aposta[i];
                corretas++;
                break;
            }
        }
    }
    if(vetor_corretas[0]!=-1){
        printf("\nDezenas corretas: ");
        for(i=0; vetor_corretas[i]!=-1; i++){
            if(vetor_corretas[i+1]!=-1){
                printf("%i, ", vetor_corretas[i]);
            }
            else{
                printf("%i\n", vetor_corretas[i]);
            }
        }
        printf("Quantidade de dezenas corretas: %i\n", corretas);
    }
    else printf("Nenhuma dezena correta\n");

    if(corretas==4){
        printf("\n%s fez a QUADRA_TRO!\n",nome);
    }
    else if(corretas==5){
        printf("\n%s fez a QUINA_TRO!\n",nome);
    }
    else if(corretas==6){
        printf("\n%s fez a SENA_TRO!\n",nome);
    }
}

int main()
{
    char nome[50], v='a';

    srand(time(NULL));
    printf("Digite o seu nome: \n");
    gets(nome);
    printf("\n");
    while(v!=27){
        resetarVetores();

        apostaFunc();
        sorteioFunc(nome);

        printf("\nPressione enter para jogar novamente ou esc para sair\n\n");
        fflush(stdin);
        v = getch();
    }
    printf("Obrigado por jogar!\n\n");

    return 0;
}
