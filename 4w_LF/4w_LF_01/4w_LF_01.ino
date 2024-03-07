#include<NewPing.h>
#include<AFMotor.h>

#define TRIGGER_PIN_RIGHT A1
#define ECHO_PIN_RIGHT A0
#define TRIGGER_PIN_LEFT A3
#define ECHO_PIN_LEFT A5
#define MAX_DISTANCE 200

NewPing sonar_right(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE); //Função da biblioteca NewPing.h a qual se responsabiliza pela conversão das unidades
NewPing sonar_left(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE);    //Função da biblioteca NewPing.h a qual se responsabiliza pela conversão das unidades

//DEFININDO VARIÁVEIS MOTORES
AF_DCMotor motorFL(1);
AF_DCMotor motorFR(4);
AF_DCMotor motorBL(2);
AF_DCMotor motorBR(3);
int lumi_esq = 0;           //Necesidade de ser uma variável global
int lumi_dir = 0;          //Necesidade de ser uma variável global
int lumi_mark = 0;
bool action = false;        // indica a decisao de direita ou esquerda de acordo com marcador
bool flag = false;          // auxilia action p/ nao confundir marcador com linha a ser seguida

int dist_sensor_right = 0;  //Necesidade de ser uma variável global
int dist_sensor_left = 0;   //Necesidade de ser uma variável global

/* Devido ao fato de terem muitos sensores conectados ao Vcc e o arduíno possuir apenas um pino para isto
defini-se o pino 13 em nível lógico alto, apenas para distribuir as cargas */

void setup() {

  //VELOCIDADE DOS MOTORES -> AJUSTÁVEL CASO NÃO ESTEJAM SINCRONIZADOS (255 = Vmáx)
  motorFR.setSpeed(255);
  motorBR.setSpeed(255);
  motorFL.setSpeed(255);
  motorBL.setSpeed(255);

  halt(); //OS MOTORES INICIAM PARADOS

  Serial.begin(9600);

  delay(3000);

}

void loop() {
  lumi_esq = digitalRead(A4);           //Variável de medida do sensor de luminosidade esquerdo (vista traseira)
  lumi_dir = digitalRead(A2);           //Variável de medida do sensor de luminosidade direito (vista traseira)
  lumi_mark = digitalRead(9);          //Variável digital de detecção de marcador (sensor de decisao)

  dist_sensor_right = sonar_right.ping_cm();  //Variável de medida do sensor ultrassônico direito (vista traseira)
  dist_sensor_left = sonar_left.ping_cm();    //Variável de medida do sensor ultrassônico esquerdo (vista traseira)

  // Serial.print("esquerda: ");
  // Serial.println(lumi_esq);
  // Serial.print("direita: ");
  // Serial.println(lumi_dir);

  //if(ultrassonic_action() == 0){ //Caso os sensores ultrassônicos não estejam identificando objetos
    lumi_action();               //O comportamento do seguidor de linha atua
  //}

  // OS PRINTS A SEGUIR DEVEM SER UTILIZADOS PARA CALIBRAÇÃO DOS SENSORES DE DISTÂNCIA */

  // Serial.print("dist_sensor_right = ");
  // Serial.println(dist_sensor_right);
  // Serial.print("dist_sensor_left = ");
  // Serial.println(dist_sensor_left);
  // delay(500);

  //delay(30);
  //halt();
  //delay(10);

}

void lumi_action(){

  /*************** !leitura equivale a região luminosa ***************/
  /***************  leitura equivale a região sem luz  ***************/
  if(lumi_esq && lumi_dir){ //Caso ambos sensores estejam identificando luminosidade
    moveForward();                   //O carrinho se locomove para frente
  }
  else if(!lumi_esq && lumi_dir){ //Caso o sensor da direita identifique luminosidade
    moveRight();                        //O carrinho se move para a esquerda, afim de encontrar a linha
  }
  else if(lumi_esq && !lumi_dir){ //Caso o sensor da esquerda identifique luminosidade
    moveLeft();                       //O carrinho se move para a direita, afim de encontrar a linha
  }
  else if(!lumi_esq && !lumi_dir){ //Caso ambos sensores não estejam identificando luminosidade
    halt();
    decision();                   //O carrinho toma uma decisao
  }

  if(lumi_mark == 1 && flag == false){
    flag = true;
  }
  else if(lumi_mark == 0 && flag == true){ //lumi_mark == 0
    action = true;
  }

  //ultrassonic_action();

}

int ultrassonic_action(){

//*****CASO NÃO LEIA NADA NO RAIO DA DISTÂNCIA MÁXIMA, A DISTÂNCIA É "INFINITA"*****
  if(dist_sensor_right == 0){
    dist_sensor_right = 1000;
  }
  if(dist_sensor_left == 0){
    dist_sensor_left = 1000;
  }
//*******************************************************************

  if(dist_sensor_right < 15 || dist_sensor_left < 15){ //Se há algum objeto à frente
    halt();                                            //Para os motores
    return 1;                                          //Retorna 1, ou seja, os sensores atuaram
  }
  else{                                                //Se não há objetos à frente
    return 0;                                          //Retorna 0, liberando o comportamento do seguidor de linha
  }

}

void moveForward(){

  motorBL.run(FORWARD);
  motorFL.run(FORWARD);
  motorBR.run(FORWARD);
  motorFR.run(FORWARD);
  action = false;

  Serial.println("forward");

}

void moveBackward(){

  motorBL.run(BACKWARD);
  motorFL.run(BACKWARD);
  motorBR.run(BACKWARD);
  motorFR.run(BACKWARD);

  Serial.println("backward");

}

void moveLeft(){

  motorBL.run(RELEASE);
  motorFL.run(RELEASE);
  motorBR.run(FORWARD);
  motorFR.run(FORWARD);

  Serial.println("left");

}

void moveRight(){

  motorBR.run(RELEASE);
  motorFR.run(RELEASE);
  motorBL.run(FORWARD);
  motorFL.run(FORWARD);

  Serial.println("right");

}

void decision(){
  if(action == false){ // quando true, indica decisao a direita // por algum cacete de motivo tá sempre entrando como TRUE >:/
    goLeft();
  }
  else{
    goRight();
  }

  flag = false;
  action = false;

}

void goLeft(){
  while(lumi_dir == 0){
    lumi_dir = digitalRead(A4);           //Variável de medida do sensor de luminosidade esquerdo (vista traseira)

    motorBL.run(BACKWARD);
    motorFL.run(BACKWARD);
    // motorBL.run(RELEASE);
    // motorFL.run(RELEASE);
    motorBR.run(FORWARD);
    motorFR.run(FORWARD);

    Serial.println("Decision Left");
  }
}

void goRight(){
  while(lumi_esq == 0){
    lumi_esq = digitalRead(A2);         //Variável de medida do sensor de luminosidade direito (vista traseira)

    motorBR.run(BACKWARD);
    motorFR.run(BACKWARD);
    // motorBR.run(RELEASE);
    // motorFR.run(RELEASE);
    motorBL.run(FORWARD);
    motorFL.run(FORWARD);

    Serial.println("Decision Right");
  }
}

void halt(){

  motorBR.run(RELEASE);
  motorFR.run(RELEASE);
  motorBL.run(RELEASE);
  motorFL.run(RELEASE);

}