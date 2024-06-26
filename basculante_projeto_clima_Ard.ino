// 10/05/24: Atualmente, o codigo tem todas as informações processadas para a melhora da mesma. 

#define PIN_PLUVIOMETRO 2 
#define MEDIDA_BASCULA 7.54 // 7.54ml each time it's activated, 9cm diameter, 4.5cm radius, 63,585 cm2 , 0,118521 L/m2
#define INTERVALO_LEITURA 1000 // 1000 milisegundos que são 1 segundo
#define RAIO_PLUVIOMETRO 4.5 

const int tamanhoLista = 60; // Define o tamanho da lista de contagens por sec do Pluviometro
int listaDeContagemSec[tamanhoLista]; // Lista de Contagem de quantas medições por sec do Pluviometro
int indiceInsercao = 0; // Qual espaço do array de contagem que será preenchida e lida consecutiva

void setup() {
  Serial.begin(9600);
  pinMode(PIN_PLUVIOMETRO, INPUT);
}

int contagemGeral = 0; // A soma de quantas vezes foi lida a passagem do Basculante
int ultimaContagemGeral = 0;
int contagemSec = 0; 
bool reedSwitchAtivado = false;
unsigned long ultimoIntervalo = 0; 

void loop() {
  int valorDigital = digitalRead(PIN_PLUVIOMETRO); // Ele sempre será 1, e só será 0 quando o imã se aproximar no Reed Switch
  modificarContagem(valorDigital);
  if(millis() - ultimoIntervalo >= INTERVALO_LEITURA){
    float mediaContagemMin = contarMlPorMinuto(); // para ser usado mais de uma vez
    pluviometroSerial(valorDigital, mediaContagemMin);
    contagemSec = 0;
    ultimoIntervalo = millis();
  }
}

void modificarContagem(int valorDigital){
  if (valorDigital == LOW && !reedSwitchAtivado) seguraContagem();
  else if (valorDigital == HIGH) reedSwitchAtivado = false;
}

void seguraContagem(){
  if(contagemGeral == ultimaContagemGeral){
      contagemGeral++;
      contagemSec++;
  } else {
    ultimaContagemGeral = contagemGeral;
  }
  reedSwitchAtivado = true;
}

float contarMlPorMinuto(){
  float soma = 0;
  float mediaTotal;
  listaDeContagemSec[indiceInsercao] = contagemSec; 
  indiceInsercao = (indiceInsercao + 1) % tamanhoLista;
  for (int i = 0; i < tamanhoLista; i++) soma += listaDeContagemSec[i];
  return mediaTotal = soma / tamanhoLista;
}

float medidaLporM2() {
  float areaPluviometroM2 = (PI * RAIO_PLUVIOMETRO * RAIO_PLUVIOMETRO) / 10000.0; // convertendo cm² para m²
  float medidaBasculaemM3 = MEDIDA_BASCULA * 1e-6; // transformando mL em m³
  float quantidadeDeAguaPorM2 = medidaBasculaemM3 / areaPluviometroM2;
  return quantidadeDeAguaPorM2;
}

void pluviometroSerial(int valorDigital, float mediaContagemMin) {
  double mlTotais = contagemGeral * MEDIDA_BASCULA;
  double mlMedioPorMin = mediaContagemMin * MEDIDA_BASCULA;

  Serial.print("Basculadas: ");
  Serial.print(contagemGeral);
  Serial.print(" | mL Totais: ");
  Serial.print(mlTotais);
  Serial.print(" | ml Por Min: ");
  Serial.print(mlMedioPorMin);
  Serial.print(" | L/m2 Totais: ");
  Serial.print(medidaLporM2());
  Serial.print(" | ContagemSec ");
  Serial.println(contagemSec);
}
