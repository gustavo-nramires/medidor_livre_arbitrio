#define N_HISTORICO 4
#define N_ESTATISTICAS (1<<N_HISTORICO)
#define N_BOTOES 2

#define ATRASO_BOUNCE 100

#define intervalo_treinamento 8

int estatisticas[N_ESTATISTICAS];
int historico[N_HISTORICO];
int acertos = 0;
int n_escolhas = 0;
float taxa_acertos = 0.5;

struct botao {
    int n;
    int estado;
    int atraso;
};
struct botao botoes[N_BOTOES];
enum estado_botao { b_apertado, b_solto, b_ativar, b_desapertado };

const int pinoBotao0 = 2;    // the number of the pushbutton pin
const int pinoBotao1 = 3;
const int pinoVoltimetro = 4;

void atualiza_botoes ();
void atualiza_displays ();
float f_livre_arbitrio();

void setup() {

  pinMode(pinoBotao0, INPUT);
  pinMode(pinoBotao1, INPUT);
  pinMode(pinoVoltimetro, OUTPUT);

  //Inicializa estatisticas
  for ( int i = 0; i < N_ESTATISTICAS; i++ ) {
    estatisticas[i] = 0;
  }
  // Inicializa historico
  for (int i = 0; i < N_HISTORICO; i++ ) {
    historico[i] = 0;
  }
  // put your setup code here, to run once:

}

// put your main code here, to run repeatedly
void loop() {

  atualiza_botoes();
  
  if ( botoes[0].estado == b_ativar || botoes[1].estado == b_ativar ) {

    n_escolhas++;

    // botao = 0 ou 1
    // 0: esquerda, 1: direita

    // Geracao de previsao
    // Possiveis estados: botao = 0 | historico ou botao = 1 | historico

    // Exemplo
    // historico
    // 0010
    // 10010 - 7 -> previsao
    // 00010 - 4

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

    if ( estatistica[prox_estado0] > estatistica[prox_estado1] ) {
      previsao = 0;
    }
    else {
      previsao = 1;
    }

    //
    //
    if ( botao == previsao ) {
      acertos++;
      taxa_acertos = acertos / n_escolhas;
    }

    // Acomodacao da nova decisao no historico
    for( i = N_HISTORICO-1; i > 0; i-- ) {
      historico[i] = historico[i-1];
    }
    historico[0] = botao; 

    // Estado associado ao novo historico
    int novo_estado = 0;
    for ( i = 0; i < N_HISTORICO; i++ ) {
        #ifdef DEBUG
        printf("historico %d : %d\n" , i, historico[i] );
        #endif // DEBUG
        novo_estado += historico[i] * 1<<(N_HISTORICO-1-i);
    }

    // Incementa estatistica do novo estado
    estatisticas[novo_estado]++;

    atualiza_display();

    Serial.print("Previsibilidade: %f", taxa_acertos);
  }
}

void atualiza_botoes () {
  if ( solto ) {
        
  }
}

void atualiza_displays () {
  // Display atual: Voltimetro (0-5V)
  // TODO: movimentar agulha de acordo com variancia

  saida = f_livre_arbitrio();

  analogWrite(pinoVoltimetro, saida);

}

// Use sua habilidade artistico-matematica-filosofica
float f_livre_arbitrio() {
  float livre_arbitrio;
  if ( taxa_acertos < 0.5 ) {
    livre_arbitrio = 1;
  }
  else {
    livre_arbitrio = ??? //pow((1-2*(taxa_acertos-0.5)),3);
  }
  return livre_arbitrio;
}
