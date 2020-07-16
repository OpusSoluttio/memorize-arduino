//A função do código é mandar uma requisição GET via DNS, com um gateway fixo.
// Junho/2020 opussoluttio@gmail.com Opus Soluttio -  MemoRize

#include <EtherCard.h>
#define REQUEST_RATE 5000 // taxa de req 5000ms ou 5s

// endereço mac do ethernet
static byte mymac[] = { 0x76,0x68,0x69,0x4D,0x40,0x21 };
// endereco ip do ethernet
static byte myip[] = { 192,168,0,120 };
// ip netmask do ethernet
static byte mask[] = { 255,255,255,0 };
// porta de entrada do endereco ip
static byte gwip[] = { 192,168,0,1 };
// nome do website remoto
const char website[] PROGMEM = "memorize.southcentralus.cloudapp.azure.com";
     
// variável para ler o estado do botão
int estadobotao = 0;
// o número do pino do botão
const int botao = 4;   
// o número do pino do led 1
const int led1 = 10;
// o número do pino do led 2
const int led2 = 7;         

// variáveis usadas no loop
int loopReq = 1;
int loopBotaoUnpressed = 1;
int repetidor = 0;

byte Ethernet::buffer[300];   
static long timer;


// chamado quando a requisição é completada, mostra o retorno da requisição
static void my_result_cb (byte status, word desligado, word tamanho) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println("ms");
  Serial.println((const char*) Ethernet::buffer + desligado);
}

// chamado depois de clicar no botão, serve para o led2 piscar
static void blink(int qnt){
  repetidor = 1;
  while(repetidor <= qnt){
    digitalWrite(led2, HIGH); 
    delay(80);
    digitalWrite(led2, LOW);
    delay(80);
    repetidor = repetidor + 1;
  }
}

void setup () {
  // inicializa o pino do botão como entrada:
  pinMode(botao, INPUT);
  // inicializa o pino do led 1 como saída:
  pinMode(led1, OUTPUT);
  // inicializa o pino do led 2 como saída:
  pinMode(led2, OUTPUT);
  
  Serial.begin(57600);
  Serial.println("\n[getViaDNS]");

  // condicional para identificar se o módulo ethernet está conectado
  if (ether.begin(sizeof Ethernet::buffer, mymac, 8) == 0)
    Serial.println( "Falha ao acessar Ethernet controller");

  ether.staticSetup(myip, gwip, NULL, mask);

 // condicional para identificar se o módulo ethernet acessa ao servidor
  if (!ether.dnsLookup(website))
    Serial.println("DNS falhou");
  ether.printIp("Server: ", ether.hisip);

  timer = - REQUEST_RATE; // começa a diminuir o tempo
}


void loop () {
  ether.packetLoop(ether.packetReceive());
    // condicional para quando apertar o botão e o tempo de request
    if (digitalRead(botao) == HIGH && millis() > timer + REQUEST_RATE) {
      // condicional para saber se já foi enviada a requisição
      if(loopReq <= 1){
        Serial.println("\n>>> REQ");
        ether.browseUrl(PSTR("/api/sessao/arduino/"), "1", website, my_result_cb);
        digitalWrite(led1, HIGH);
        loopReq = loopReq + 1;
        delay(1500);

        blink(3);
      }
     }
     else{
      if(loopBotaoUnpressed >= 1){
        digitalWrite(led1, LOW);
        digitalWrite(led2, HIGH);
        loopReq = 1;
        loopBotaoUnpressed = loopBotaoUnpressed + 1;
      }else{
        digitalWrite(led1, LOW);
        loopReq = 1;
        loopBotaoUnpressed = 1;
      }
    }
    delay(50);

}
