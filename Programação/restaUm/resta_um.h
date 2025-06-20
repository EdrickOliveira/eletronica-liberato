/**********************************************************
* @file     resta_um.h
* @brief    Resta Um - defines, tipos dados e prototipos
* @author   Marcos Zuccolotto
* @date     abr/2024
* @version  1.0
*
*
**********************************************************/

#ifndef _RESTA_UM_H_   // Declaracao de guarda
#define _RESTA_UM_H_

// Dimensoes da matriz que guarda o tabuleiro
#define NCOL 7
#define NLIN 7

// Marcadores do tabuleiro
#define  NU 'X'  // Ponto invalido/proibido
#define  OC '*'  // Ponto ocupado
#define  VZ 'O'  // Ponto livre/vazio

/* Tipos de dados */

// Posicao peca no tabuleiro
typedef struct
    {
    int lin; // Posicao linha
    int col; // Posicao destino
    }posTab_t;
// Movimento a ser realizado
typedef struct
    {
        posTab_t origem;   // Posicao inicio/origem
        posTab_t destino; // Posicao fim/
    }movimento_t;

typedef enum { OK=0 , // movimento executado, segue o jogo
               INVALIDO, // nao existe peca a pular sobre/retira
                         // fora tabuleiro
                         // movimento longo, curto ou em diagonal
               OCUPADO,  // posicao destino ocupada
               VAZIO,    // posicao origem vazia
               VITORIA,  // vitoria - fim jogo
               DERROTA   // nao tem mais o que fazer
             } status_t;
/* Prototipos */

/**
@brief Inicializa tabuleiro
@param tabuleiro
@retval none
*/
void inicTab(char *tab);
/**
@brief Exibe o tabuleiro
@param tabuleiro
@retval none
*/
void showTab(char *tab);

/**
@brief Entrada dados da jogada
@param movimentacao a realizar
@retval OK - jogador definiu a movimentacao
        DERROTA - jogador desistiu do jogo
*/
status_t qualJogada(movimento_t *jogada)
{
    //conferir se o jogador deseja continuar ou desistir
    printf("\nD - desistir\nOutro - continuar\nProximo passo: ");
    if(getch() == 'd')
    {
        printf("Desistiu...\n");
        return DERROTA;
    }
    printf("Continuando...\n\n");

    printf("Digite as coordenadas (em numeros de 1 a 7) da origem (lin col): ");
    scanf("%i %i", &jogada->origem.lin, &jogada->origem.col);

    printf("Digite as coordenadas (em numeros de 1 a 7) do destino (lin col): ");
    scanf("%i %i", &jogada->destino.lin, &jogada->destino.col);

    //ajustar para que o usuario digite de 1 a 7 ao inves de 0 a 6
    jogada->origem.lin--;
    jogada->origem.col--;
    jogada->destino.lin--;
    jogada->destino.col--;

    return OK;
}

/**
@brief Executa a movimentacao solicitada
@param tabuleiro
@param movimentacao a realizar
@retval OK - movimento executado, segue o jogo
        INVALIDO - movimento invalido
        OCUPADO - posicao destino ocupada
        VAZIO  posicao origem vazia
*/
status_t movimenta(char *tab, movimento_t jog)
{
    //para melhor legibilidade, assinalando os elementos (pino, espaço vazio ou invalido) de origem e destino a ponteiros
    char *elem_origem = tab + NCOL * jog.origem.lin + jog.origem.col;
    char *elem_destino = tab + NCOL * jog.destino.lin + jog.destino.col;

    //vazio se a origem estiver vazia
    if (*elem_origem == VZ)     return VAZIO;
    //ocupado se o destino estiver ocupado
    if (*elem_destino == OC)    return OCUPADO;
    //invalido se fora do tabuleiro
    if (*elem_origem == NU    || *elem_destino == NU  ||
        jog.origem.lin < 0     || jog.origem.col < 0    ||
        jog.destino.lin < 0    || jog.destino.col < 0   ||
        jog.origem.lin > NLIN  || jog.origem.col > NCOL ||
        jog.destino.lin > NLIN || jog.destino.col > NCOL)    return INVALIDO;

    //armazena se a peca se movera horizontal/verticalmente (H/V, indice 0)
    //e se o sentido e positivo/negativo (cima/baixo, direita/esquerda) (P/N, indice 1)
    char sentido_mov[2];

    //invalido se origem e destino nao estiverem alinhados
    if      (jog.origem.lin == jog.destino.lin) sentido_mov[0] = 'H';
    else if (jog.origem.col == jog.destino.col) sentido_mov[0] = 'V';
    else    return INVALIDO;

    //se o movimento e para cima ou para a direita o sentido e positivo
    if ((sentido_mov[0]=='H' && jog.origem.col < jog.destino.col) ||
        (sentido_mov[0]=='V' && jog.origem.lin > jog.destino.lin))  sentido_mov[1] = 'P';
    //se o movimento e para baixo ou para a esquerda o sentido e negativo
    else    sentido_mov[1] = 'N';

    //se o alinhamento for horizontal
    if (sentido_mov[0]=='H')
    {
        //invalido se o destino nao estiver dois a direita ou a esquerda da origem
        if (jog.destino.col+2 != jog.origem.col && jog.destino.col-2 != jog.origem.col) return INVALIDO;

        //se o movimento for para a direita
        if (sentido_mov[1]=='P')
        {
            //invalido se o espaco a direita estiver vazio
            if (*(elem_origem+1) == VZ)    return INVALIDO;

            //passou em todos os requisitos, executar o movimento para a direita
            *(elem_origem)   = VZ;
            *(elem_origem+1) = VZ;
            *(elem_destino)  = OC;
        }

        //se o movimento for para a esquerda
        else
        {
            //invalido se o espaco a esquerda estiver vazio
            if (*(elem_origem-1) == VZ)    return INVALIDO;

            //passou em todos os requisitos, executar o movimento para a esquerda
            *(elem_origem)   = VZ;
            *(elem_origem-1) = VZ;
            *(elem_destino)  = OC;
        }
    }

    //se o alinhamento for vertical
    else
    {
        //invalido se o destino nao estiver dois acima ou abaixo da origem
        if (jog.destino.lin+2 != jog.origem.lin && jog.destino.lin-2 != jog.origem.lin) return INVALIDO;

        //se o movimento for para cima
        if (sentido_mov[1]=='P')
        {
            //invalido se o espaco acima estiver vazio
            if (*(elem_origem-NCOL) == VZ)    return INVALIDO;

            //passou em todos os requisitos, executar o movimento para cima
            *(elem_origem)      = VZ;
            *(elem_origem-NCOL) = VZ;
            *(elem_destino)     = OC;
        }

        //se o movimento for para baixo
        else
        {
            //invalido se o espaco abaixo estiver vazio
            if (*(elem_origem+NCOL) == VZ)    return INVALIDO;

            //passou em todos os requisitos, executar o movimento para baixo
            *(elem_origem)      = VZ;
            *(elem_origem+NCOL) = VZ;
            *(elem_destino)     = OC;
        }
    }

    return OK;
}

/**
@brief Confere possibilidades de continuacao
@param tabuleiro
@retval OK -  segue o jogo
        VITORIA - restou um, acabou
        DERROTA - nada mais a fazer, fim de jogo
*/
status_t confereJogo(char *tab)
{
    char *elem;
    int qtd_pecas=0;

    //itera atraves dos elementos no tabuleiro
    for(int i=0; i<NLIN; i++)
    {
        for(int j=0; j<NCOL; j++)
        {
            //para melhor legibilidade, assinala o elemento atual a um ponteiro
            elem = tab + NCOL*i + j;

            //se o elemento for uma peca:
            if (*elem == '*')
            {
                //somar 1 a quantidade
                qtd_pecas++;
                //OK se houver uma peça adjacente seguida por um espaço vazio, sem que um destes ultrapasse a borda do tabuleiro
                if ((*(elem+1)=='*' && *(elem+2)=='O' && j<5) ||
                    (*(elem-1)=='*' && *(elem-2)=='O' && j>1) ||
                    (*(elem-NCOL)=='*' && *(elem-NCOL*2)=='O' && i>1) ||
                    (*(elem+NCOL)=='*' && *(elem+NCOL*2)=='O' && i<5)
                )   return OK;
            }
        }
    }

    //se nao e possivel executar mais jogadas:
        //VITORIA se ainda resta apenas 1 peca
        if(qtd_pecas==1)    return VITORIA;

        //DERROTA caso contrario
        return DERROTA;
}

#endif // _RESTA_UM_H_
