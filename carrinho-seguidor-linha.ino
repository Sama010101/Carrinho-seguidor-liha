#include <MFRC522.h>
#include <SPI.h>
#include <Ultrasonic.h>
#include <StackArray.h>

int motorA_VM = 10;  //Controle de velocidade do motor A
int motorB_VM = 11;  //Controle de velocidade do motor B
int motorA_DM = 12;  //Controle de direção do motor A
int motorB_DM = 13;  //Controle de direção do motor B

int sensorDireito = 5; 
int sensorCentro = 6;
int sensorEsquerdo = 7;

const int echoPin= 9;
const int trigPin= 8;
const int led = 4;

Ultrasonic ultrasonic(8, 9);

MFRC522 mfrc522(10, 9); // Configuração do leitor RFID

StackArray<String> cartoesRFID; // Pilha para armazenar os números dos cartões

//Leitura de sensores
int leituraDireito;
int leituraCentro;
int leituraEsquerdo;

//Velocidade Motores
int veloc0 = 0;  //Velocidade para parar os motores
int velocA1 = 120;
int velocB1 = 120;
int velocA2 = 100;
int velocB2 = 100;

// variaveis de controle
long microsec = 0;
float distanciaCM = 0;

void setup() {
  Serial.begin(9600); //Inicializa o monitor serial

  delay(5000); // delay de 5 segundos para inicializar o programa

//Declarando os motores como saída
  pinMode(motorA_VM, OUTPUT);
  pinMode(motorA_DM, OUTPUT);
  pinMode(motorB_VM, OUTPUT);
  pinMode(motorB_DM, OUTPUT);
//Declarando os sensores como saída
  pinMode(sensorDireito, INPUT);
  pinMode(sensorCentro, INPUT);
  pinMode(sensorEsquerdo, INPUT);
//Declarando o led como saída
  pinMode(led, OUTPUT);


  SPI.begin();      // Inicializa a interface SPI
  mfrc522.PCD_Init(); // Inicializa o leitor RFID

  cartoesRFID.push("1234567890"); // Exemplo de cartão RFID já na pilha

  }
void loop() {
  
  leitura();
  seguidor();
  ledDistancia();
  //Serial.println(sensorDireito);//printar a leitura do sensor no monitor serial
  
  //Lendo o valor do sensor
  microsec = ultrasonic.timing();

  //Convertendo a distância em CM
  distanciaCM = ultrasonic.convert(microsec, Ultrasonic::CM);


  Serial.print(distanciaCM);// mostrar a distancia na porta serial
  Serial.println(" cm");// colocar unidade de medida

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String cartaoID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cartaoID.concat(String(mfrc522.uid.uidByte[i]));
    }
    cartoesRFID.push(cartaoID); // Adiciona o cartão à pilha
  }


  if (!cartoesRFID.isEmpty()) {
    String cartaoLido = cartoesRFID.pop();
    Serial.print("Cartão lido: ");
    Serial.println(cartaoLido);
    // Adicione o código para ação com base no cartão lido aqui
  }
  
 
}
//leitura do sensor infravermelho
void leitura() {
  leituraDireito = digitalRead(sensorDireito);
  leituraCentro = digitalRead(sensorCentro);
  leituraEsquerdo = digitalRead(sensorEsquerdo);

}
//Condicionais para a movimentação do carrinho
void seguidor() {

  if ((leituraDireito == 1) && (leituraCentro == 0) && (leituraEsquerdo == 1)) {
    frente();
    Serial.println(sensorCentro);

  }
  if ((leituraDireito == 0) && (leituraCentro == 1) && (leituraEsquerdo == 1)) {
    direita();
    Serial.println(sensorDireito);
  }
  if ((leituraDireito == 1) && (leituraCentro == 1) && (leituraEsquerdo == 0)) {
    esquerda();
    Serial.println(sensorEsquerdo);
  }
  }
  //Funções para a movimentação do carrinho
  void frente() {
    digitalWrite(motorA_DM, HIGH);
    digitalWrite(motorB_DM, HIGH);
    analogWrite(motorA_VM, velocA1);
    analogWrite(motorB_VM, velocB1);
  }
  void direita() {
    digitalWrite(motorA_DM, HIGH);
    digitalWrite(motorB_DM, LOW);
    analogWrite(motorA_VM, velocA2);
    analogWrite(motorB_VM, veloc0);
  }
  void esquerda() {
    digitalWrite(motorA_DM, LOW);
    digitalWrite(motorB_DM, HIGH);
    analogWrite(motorA_VM, veloc0);
    analogWrite(motorB_VM, velocB2);
  }
  void parar() {
    digitalWrite(motorA_DM, HIGH);
    digitalWrite(motorB_DM, HIGH);
    analogWrite(motorA_VM, veloc0);
    analogWrite(motorB_VM, veloc0);
  }
  void ledDistancia() {
  //Desliga todos os LEDs
  digitalWrite(led, LOW);
  //Condicional para acionamento do LED
  if (distanciaCM <= 8) { // se a distancia for menor ou igual a 8 cm o carrinho para
    digitalWrite(led, HIGH); // liga LED vermelho
    parar();
    
  }
}