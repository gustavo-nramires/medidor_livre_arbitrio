#define N_HISTORICO 4
#define N_ESTATISTICAS (1<<N_HISTORICO)
#define N_BOTOES 2

#define PERIODO_ATUALIZACAO 50
#define ATRASO_BOUNCE 150 / PERIODO_ATUALIZACAO

#define intervalo_treinamento 8

int estatisticas[N_ESTATISTICAS];
int historico[N_HISTORICO];
int acertos = 0;
int n_escolhas = 0;
float taxa_acertos = 0.5;

const int DEBUG = 1;

struct botao {
    int n;
    int estado;
    int atraso;
};

struct botao botoes[N_BOTOES];
enum estado_botao { b_apertado, b_solto, b_ativar, b_desapertado };

char print_buffer[40];
#define DEBUG_PRINT(...)                   \
  do {                                     \
      if(DEBUG) {                          \
        sprintf (print_buffer,__VA_ARGS__);\
        Serial.print(print_buffer);        \
      }                                    \
  } while(0)

int pinoBotao[N_BOTOES] = { 2, 3 };
// the number of the pushbutton pin

const int pinoVoltimetro = 5;

void atualiza_botoes ();
void atualiza_displays ();
float f_livre_arbitrio();

void setup() {
  Serial.begin(9600);

  for( int i = 0; i < N_BOTOES; i++ ) {
    pinMode(pinoBotao[i], INPUT);
  }
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

  delay(PERIODO_ATUALIZACAO);

  atualiza_botoes();
  
  if ( botoes[0].estado == b_ativar || botoes[1].estado == b_ativar ) {

    int escolha;
    if ( botoes[0].estado == b_ativar ) {
      escolha = 0;
    }
    else {
      escolha = 1;
    }

    DEBUG_PRINT("Escolha: %d",  escolha);

    n_escolhas++;

    // escolha = 0 ou 1
    // 0: esquerda, 1: direita

    // Geracao de previsao
    // Possiveis estados: escolha = 0 | historico ou escolha = 1 | historico

    // Exemplo
    // historico
    // 0010
    // 10010 - 7 -> previsao
    // 00010 - 4

    int prox_estado0, prox_estado1;
    prox_estado0 = 0;
    prox_estado0 += 0 * 1<<(N_HISTORICO-1);
    for ( int i = 0; i < N_HISTORICO-1; i++ ) {
      prox_estado0 += historico[i] * 1<<(N_HISTORICO-1-(i+1));
    }

    prox_estado1 = 0;
    prox_estado1 += 1 * 1<<(N_HISTORICO-1);
    for ( int i = 0; i < N_HISTORICO-1; i++ ) {
      prox_estado1 += historico[i] * 1<<(N_HISTORICO-1-(i+1));
    }

    int previsao;
    if ( estatisticas[prox_estado0] > estatisticas[prox_estado1] ) {
      previsao = 0;
    }
    else {
      previsao = 1;
    }

    //
    //
    if ( escolha == previsao ) {
      acertos++;
      taxa_acertos = acertos / n_escolhas;
    }

    // Acomodacao da nova decisao no historico
    for( int i = N_HISTORICO-1; i > 0; i-- ) {
      historico[i] = historico[i-1];
    }
    historico[0] = escolha;

    // Estado associado ao novo historico
    int novo_estado = 0;
    for ( int i = 0; i < N_HISTORICO; i++ ) {
        DEBUG_PRINT("historico %d : %d\n" , i, historico[i] );
        novo_estado += historico[i] * 1<<(N_HISTORICO-1-i);
    }

    // Incementa estatistica do novo estado
    estatisticas[novo_estado]++;

    atualiza_displays();

    DEBUG_PRINT("Previsibilidade: %f", taxa_acertos);
  }
}

// TODO
void atualiza_botoes () {
  for( int i = 0; i < N_BOTOES; i++ ) {
    atualiza_botao(&botoes[i]);
    DEBUG_PRINT("Estado botao %d: %d\n",  i, botoes[i].estado);
  }
}

int leitura_botao(int n_botao)
{
    return digitalRead(pinoBotao[n_botao]);
}

void atualiza_botao(botao* b)
{
    switch(b->estado) {
    case b_apertado:
        b->atraso++;
        if( b->atraso > ATRASO_BOUNCE && leitura_botao(b->n) == 0 ) {
            b->estado = b_solto;
            b->atraso = 0;
        }
        break;
    case b_ativar:
        b->estado = b_apertado;
        b->atraso = 0;
        break;
    case b_solto:
        b->atraso++;
        if( b->atraso > ATRASO_BOUNCE && leitura_botao(b->n) == 1 ) {
            b->estado = b_ativar;
            b->atraso = 0;
        }
        break;
    default:
        break;
    }
}

void atualiza_displays () {
  // Display atual: Voltimetro (0-5V)
  // TODO: movimentar agulha de acordo com variancia

  float saida = f_livre_arbitrio();
  int saida_analogica = (int) saida * 255;

  analogWrite(pinoVoltimetro, saida);

}

// Use sua habilidade artistico-matematica-filosofica
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
