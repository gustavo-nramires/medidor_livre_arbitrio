#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <stdlib.h>

#include <math.h>

#define CHAR_0 48
#define CHAR_1 49

#define intervalo_treinamento 12

#define N_HISTORICO 4
#define N_ESTATISTICAS (1<<N_HISTORICO)

#define DEBUG_PRINT(x) printf(x);

int estatisticas[N_ESTATISTICAS];
int historico[N_HISTORICO];
int acertos = 0;
int n_escolhas = 0;
float taxa_acertos = 0.5;

const int pinoBotao = 2;    // the number of the pushbutton pin

void loop(int botao);
void f_previsao();
void f_atualiza_display();

float f_livre_arbitrio();

int main() {

    srand(time(NULL));

    char botao_char;
    int botao = 0;
    int i = 0;

    //Inicializa estatisticas
    for ( i = 0; i < N_ESTATISTICAS; i++ ) {
        estatisticas[i] = 0;
    }
    // Inicializa historico
    for ( i = 0; i < N_HISTORICO; i++ ) {
        historico[i] = 0;
    }

    while(1) {
        scanf("%c", &botao_char);
        if ( botao_char == CHAR_0 ) {
            #ifdef DEBUG
            printf("Botao = 0\n");
            #endif // DEBUG
            botao = 0;
            loop(botao);
        }
        else if ( botao_char == CHAR_1 ) {
            #ifdef DEBUG
            printf("Botao = 1\n");
            #endif // DEBUG
            botao = 1;
            loop(botao);
        }
        else {

        }
    }
    return 0;
}

void loop(int botao) {

	int i = 0;
  // put your main code here, to run repeatedly:

  //if ( botao_apertado ) {

    // botao = 0 ou 1
    // 0: esquerda, 1: direita

    // Geracao de previsao
    // Possiveis estados: botao = 0 | historico ou botao = 1 | historico

    // Exemplo
    // historico
    // 0010
    // 10010 - 7 -> previsao
    // 00010 - 4

    #ifdef DEBUG
    for( i = 0; i < N_ESTATISTICAS; i++ ) {
        printf("estatisticas %d : %d\n" , i, estatisticas[i] );
    }
    #endif // DEBUG

    int prox_estado0, prox_estado1;
    prox_estado0 = 0;
    prox_estado0 += 0 * 1<<(N_HISTORICO-1);
    for ( i = 0; i < N_HISTORICO-1; i++ ) {
      prox_estado0 += historico[i] * 1<<(N_HISTORICO-1-(i+1));
    }

    prox_estado1 = 0;
    prox_estado1 += 1 * 1<<(N_HISTORICO-1);
    for ( i = 0; i < N_HISTORICO-1; i++ ) {
      prox_estado1 += historico[i] * 1<<(N_HISTORICO-1-(i+1));
    }

    #ifdef DEBUG
    printf("prox_estado0 : %d\n" , prox_estado0 );
    printf("prox_estado1 : %d\n" , prox_estado1 );
    #endif // DEBUG

    int previsao;
    float livre_arbitrio;
    if ( estatisticas[prox_estado0] == estatisticas[prox_estado1]) {
        previsao = rand() % 2;
    }
    if ( estatisticas[prox_estado0] > estatisticas[prox_estado1] ) {
      previsao = 0;
    }
    else {
      previsao = 1;
    }

    printf("previsao : %d\n" , previsao );


    //
    //
    n_escolhas++;
    if ( n_escolhas > intervalo_treinamento ) {

        int n_escolhas_teste = n_escolhas - intervalo_treinamento;

        if ( botao == previsao ) {
            #ifdef DEBUG
            printf(" Acertou !!! \n");
            #endif // DEBUG
            acertos++;
        }

        #ifdef DEBUG
        printf("Acertos : %d\n" , acertos );
        #endif // DEBUG

        taxa_acertos = (float) acertos / (float) n_escolhas_teste;

        f_atualiza_display();

        if ( 1==1 ) {
            int k = n_escolhas - intervalo_treinamento;
            printf("|");
            for( int w = 1; w <= 10; w++ ) {
                if ( w <= k ) {
                    printf("=");
                }
                else {
                    printf(".");
                }
            }
            printf("|\n");
        }

        printf("N = %d\n", n_escolhas - intervalo_treinamento );

        printf("Previsibilidade: %f\n", taxa_acertos);

        livre_arbitrio = f_livre_arbitrio();

        printf("Livre arbitrio: %f\n", livre_arbitrio);

    }


    // Acomodacao da nova decisao no historico
    for( i = N_HISTORICO-1; i > 0; i-- ) {
      historico[i] = historico[i-1];
    }
    historico[0] = botao;

    // Estado associado ao novo historico
    #ifdef DEBUG
    printf("Novo historico\n");
    #endif // DEBUG

    int novo_estado = 0;
    for ( i = 0; i < N_HISTORICO; i++ ) {
        #ifdef DEBUG
        printf("historico %d : %d\n" , i, historico[i] );
        #endif // DEBUG
        novo_estado += historico[i] * 1<<(N_HISTORICO-1-i);
    }

    // Incementa estatistica do novo estado
    estatisticas[novo_estado]++;

    // Previsao da proxima escolha
    //f_previsao();

    //Serial.print("Previsibilidade: %f", taxa_acertos);
}

void f_previsao() {

    int prox_estado0, prox_estado1;
    int i;
    prox_estado0 = 0;
    prox_estado0 += 0 * 1<<(N_HISTORICO-1);
    for ( i = 0; i < N_HISTORICO-1; i++ ) {
      prox_estado0 += historico[i] * 1<<(N_HISTORICO-2-i);
    }

    prox_estado1 = 0;
    prox_estado1 += 1 * 1<<(N_HISTORICO-1);
    for ( i = 0; i < N_HISTORICO-1; i++ ) {
      prox_estado1 += historico[i] * 1<<(N_HISTORICO-2-i);
    }

    #ifdef DEBUG
    printf("prox_estado0 : %d\n" , prox_estado0 );
    printf("prox_estado1 : %d\n" , prox_estado1 );
    #endif // DEBUG

    int previsao;
    if ( estatisticas[prox_estado0] == estatisticas[prox_estado1] ) {
        previsao = rand() % 2;
    }
    if ( estatisticas[prox_estado0] > estatisticas[prox_estado1] ) {
      previsao = 0;
    }
    else {
      previsao = 1;
    }

    printf("previsao proxima: %d\n" , previsao );

    return;
}

float f_livre_arbitrio() {
	float livre_arbitrio;
	if ( taxa_acertos < 0.5 ) {
		livre_arbitrio = 1;
	}
	else {
		livre_arbitrio = pow((1-2*(taxa_acertos-0.5)),3);
	}
	return livre_arbitrio;
}

void f_atualiza_display() {

}
