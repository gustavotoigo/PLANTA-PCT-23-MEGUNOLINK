#include <Arduino.h>
#include "MegunoLink.h"
#include "CommandHandler.h"
#include <max6675.h>
#include <pins.h>
#include <declarations.h>

//CONSTANTE DE CALIBRAÇÃO
#define CONSTANTE_CALIB      2

//NUMERO DE AMOSTRAS DO FILTRO
#define N               20

CommandHandler<> SerialCommandHandler;    //CRIA INSTANCIA DE COMUNICACAO
InterfacePanel MyPanel;                   //CRIA INSTANCIA DO PAINEL
TimePlot MyPlot;                          //CRIA INSTANCIA DO PLOTTER DO SUPERVISRIO

void setup(){
  //INICIAL PINOS DO RESISTOR 1
  pinMode(31, OUTPUT);                        
  pinMode(32, OUTPUT);
  //INICIA PINOS DO RESISTOR 2
  pinMode(30, OUTPUT);
  pinMode(2 , OUTPUT);

  //INICIA PINOS PWM DOS MOTORES
  pinMode(MOT_1, OUTPUT);
  pinMode(MOT_2, OUTPUT);

  //INICIA PINOS DOS SOLENOIDES
  pinMode(SOLENOIDE_1, OUTPUT);
  pinMode(SOLENOIDE_2, OUTPUT);
  pinMode(SOLENOIDE_3, OUTPUT);
  pinMode(SOLENOIDE_4, OUTPUT);
  pinMode(SOLENOIDE_5, OUTPUT);

  pinMode(13, OUTPUT);
  Serial.begin(9600);

  //MENSAGENS SERIAL DOS SOLENOIDES
  //SerialCommandHandler.AddCommand(F("mensagem a ser recebida"), funcao de atulização)
  //CONFIGURA MENSAGEM E FUNCAO QUE ATUALIZA O VALOR RECEBIDO PELA SERIAL
  SerialCommandHandler.AddCommand(F("Enable1"), GetSol_1);     //MENSAGEM SOLENOIDE 1 
  SerialCommandHandler.AddCommand(F("Enable2"), GetSol_2);     //MENSAGEM SOLENOIDE 2
  SerialCommandHandler.AddCommand(F("Enable3"), GetSol_3);     //MENSAGEM SOLENOIDE 3 
  SerialCommandHandler.AddCommand(F("Enable4"), GetSol_4);     //MENSAGEM SOLENOIDE 4 
  SerialCommandHandler.AddCommand(F("Enable5"), GetSol_5);     //MENSAGEM SOLENOIDE 5 

  //MENSAGENS SERIAL DOS RESISTORES
  SerialCommandHandler.AddCommand(F("Enable6"), GetRes_1);      //MENSAGEM RESISTOR 1
  SerialCommandHandler.AddCommand(F("Enable7"), GetRes_2);      //MENSAGEM RESISTOR 1
  
  //MENSAGENS SERIAL PARA ATIVAR OU DESATIVAR SISTEMA
  SerialCommandHandler.AddCommand(F("StartOP"), GetStartOP);
  SerialCommandHandler.AddCommand(F("StopOP"), GetStopOP);

  //MENSAGEM SERIAL PARA ATUALIZAR setpoint
  SerialCommandHandler.AddCommand(F("setpoint"), GetSetPoint);

  //MENSAGEM SERIAL PARA ATUALIZAR VALORES DE POTENCIA DAS BOMBAS
  SerialCommandHandler.AddCommand(F("bomba1"), GetBomba1);
  SerialCommandHandler.AddCommand(F("bomba2"), GetBomba2);
}

void loop() {
   /*
  .
  .
  .
  .
  */
  RunSys();
  delay(200);
}

void RunSys(void){
  SerialCommandHandler.Process();
  //VERIFICA O ESTADO DO SISTEMA, SE FOR 1 ENTAO O SISTEMA É ATIVADO E RODA TODAS FUNCOES
  if(state_sys == 1){
    
                                         
    MyPanel.SetText(F("DynamicLabel9"), "Operando");   //ENVIA AO SUPERVISORIO DIZENDO QUE O SISTEMA ESTA EM OPERACAO

    //ENVIA GRAFICO DO setpoint
    MyPlot.SendData(F("Setpoint"), setpoint + CONSTANTE_CALIB, TimePlot::Red, TimePlot::Solid, 1.5, TimePlot::NoMarker);                                   
    

    ReadTemps();                          //LE OS SENSORES E ATUALIZA VALORES DE TEMPERATURA                  
    PWMotors();                           //ATUALIZA O VALOR DO PWM DOS MOTORES    

    //VARRE SENSORES CALCULANDO A MEDIA MOVEL DE TODOS ELES
    for(int i = 0; i <= 5; i++){
      MovingAverage(i);           
    }
    
    WatchTemperature();                   //OBSERVA A TEMPERATURA E REGULA DE ACORDO COM O STPOINT
    EnableResistance_1();                 //ATUALIZA ESTADO DO RESISTOR 1     
    EnableResistance_2();                 //ATUALIZA ESTADO DO RESISTOR 1  
    EnableSolenoid();                     //ATUALIZA ESTADO DOS SOLENOIDES
    SendDataToTimePlot();

    //PrintData2();                       //(OBSOLETA) FUNCAO QUE FOI USADA NO DEBUG E PRINT DIRETO NA SERIAL  

  //VERIFICA ESTADO DO SISTEMA, SE NAO FOR 1 ENTAO DESATIVA
  }
  if(state_sys != 1){  
    MyPanel.SetText(F("DynamicLabel9"), "Parada");  //ENVIA AO SUPERVISORIO DIZENDO QUE O SISTEMA ESTA PARADO
    
  }
}

//READ ALL TEMPERATURES
float ReadTemps(void){
  MyPanel.SetText(F("DynamicLabel8"), temp_1_f);    
  MyPanel.SetNumber(F("IPGauge1"), temp_1_f);
  temp_1 = ktc_1.readCelsius();
  temp_2 = ktc_2.readCelsius();
  temp_3 = ktc_3.readCelsius();
  temp_4 = ktc_4.readCelsius();
  temp_5 = ktc_5.readCelsius();
  
}

//FILTES OF ALL SENSORS
float MovingAverage(int sensor_num){
  if(sensor_num == 1){
    for(int i = N-1; i > 0; i--){ 
      samples_1[i] = samples_1[i-1];
    }
    
    samples_1[0] = temp_1;
      
    float acc = 0;
      
    for (int i = 0; i < N; i++){ 
      acc += samples_1[i];
    }
  
    temp_1_f = acc/N;
  }

  if(sensor_num == 2){
    for(int i = N-1; i > 0; i--){ 
      samples_2[i] = samples_2[i-1];
    }
    samples_2[0] = temp_2;
      
    float acc = 0;
      
    for (int i = 0; i < N; i++){ 
      acc += samples_2[i];
    }
    temp_2_f = acc/N;
  }

  if(sensor_num == 3){
    for(int i = N-1; i > 0; i--){ 
      samples_3[i] = samples_3[i-1];
    }
    samples_3[0] = temp_3;
      
    float acc = 0;
      
    for (int i = 0; i < N; i++){ 
      acc += samples_3[i];
    }
    temp_3_f = acc/N;
  }
  
  if(sensor_num == 4){
    for(int i = N-1; i > 0; i--){ 
      samples_4[i] = samples_4[i-1];
    }
    samples_4[0] = temp_4;
      
    float acc = 0;
      
    for (int i = 0; i < N; i++){ 
      acc += samples_4[i];
    }
    temp_4_f = acc/N;
  }

  if(sensor_num == 5){
    for(int i = N-1; i > 0; i--){ 
      samples_5[i] = samples_5[i-1];
    }
    samples_5[0] = temp_5;
      
    float acc = 0;
      
    for (int i = 0; i < N; i++){ 
      acc += samples_5[i];
    }
    temp_5_f = acc/N;
  }
}

//COMPARE THE TEMPERATURE VALUE AND UPDATE THE STATE VALUE OF THE RESISTORS
int WatchTemperature(void){  

  if((temp_1_f > setpoint)){
    state = HIGH;
  }else{
    state = LOW;
  }
  return 0;
}


void EnableResistance_1(void){
  if(state == HIGH){
    digitalWrite(RES_1A, HIGH);
    digitalWrite(RES_1B, HIGH);
    MyPanel.SetText(F("DynamicLabel0"), "ON");
    
  }

  if(state != HIGH){
    digitalWrite(RES_1A, LOW);
    digitalWrite(RES_1B, LOW);
    MyPanel.SetText(F("DynamicLabel0"), "OFF");
  }
}

void EnableResistance_2(void){
  if(state == HIGH){
    digitalWrite(RES_2A, HIGH);
    digitalWrite(RES_2B, HIGH);
    MyPanel.SetText(F("DynamicLabel1"), "ON");
  }

  if(state != HIGH){
    digitalWrite(RES_2A, LOW);
    digitalWrite(RES_2B, LOW);
    MyPanel.SetText(F("DynamicLabel1"), "OFF");
  }
}

//SEND TO PINS THE VALUE RECEIVED
void PWMotors(void){
  bomba1 = map(bomba1, 0, 100, 0, 255);
  analogWrite(MOT_1, bomba1);
  
  bomba2 = map(bomba2, 0, 100, 0, 255);
  analogWrite(MOT_2, bomba2);
}

void EnableSolenoid(void){
  
  if(state_sol1 == 1){                          //THEN MUST BE ACTIVATED
    MyPanel.SetText(F("DynamicLabel1"), "ON");  //SEND ON
    digitalWrite(SOLENOIDE_1, HIGH);            //ACTIVATED SOLENOID 1
  }

  if(state_sol1 != 1){                          //THEN MUST BE DEACTIVATED
    MyPanel.SetText(F("DynamicLabel1"), "OFF"); //SEND OFF
    digitalWrite(SOLENOIDE_1, LOW);             //DEACTIVATED SOLENOID 1
  }

  if(state_sol2 == 1){
    MyPanel.SetText(F("DynamicLabel2"), "ON");
    digitalWrite(SOLENOIDE_2, HIGH);
  }

  if(state_sol2 != 1){
    MyPanel.SetText(F("DynamicLabel2"), "OFF");
    digitalWrite(SOLENOIDE_2, LOW);
  }

  if(state_sol3 == 1){
    MyPanel.SetText(F("DynamicLabel3"), "ON");
    digitalWrite(SOLENOIDE_3, HIGH);
  }

  if(state_sol3 != 1){
    MyPanel.SetText(F("DynamicLabel3"), "OFF");
    digitalWrite(SOLENOIDE_3, LOW);
  }

  if(state_sol4 == 1){
    MyPanel.SetText(F("DynamicLabel4"), "ON");
    digitalWrite(SOLENOIDE_4, HIGH);
  }

  if(state_sol4 != 1){
    MyPanel.SetText(F("DynamicLabel4"), "OFF");
    digitalWrite(SOLENOIDE_4, LOW);
  }

  if(state_sol5 == 1){
    MyPanel.SetText(F("DynamicLabel5"), "ON");
    digitalWrite(SOLENOIDE_5, HIGH);
  }

  if(state_sol5 != 1){
    MyPanel.SetText(F("DynamicLabel5"), "OFF");
    digitalWrite(SOLENOIDE_5, LOW);
  }
}

//JUST DEBUG INITIAL DEVELOP
void PrintData(void){
  Serial.print(ktc_1.readCelsius());
  Serial.print(";");
  Serial.print(ktc_2.readCelsius());
  Serial.print(";");
  Serial.print(ktc_3.readCelsius());
  Serial.print(";");
  Serial.print(ktc_4.readCelsius());
  Serial.print(";");
  Serial.print(ktc_5.readCelsius());
  Serial.println();
}
//JUST DEBUG INITIAL DEVELOP
void PrintData2(void){
  Serial.print(temp_1);
  Serial.print(";");
  Serial.print(temp_1_f);
  Serial.println();
}

//SEND TO SERIAL ALL VALUES OS TEMPERATURE TO PLOT
void SendDataToTimePlot(void)
{ 
  MyPlot.SendData(F("Sensor 1"), temp_1_f, TimePlot::Green, TimePlot::Solid, /* Line width = */ 1,  TimePlot::NoMarker);
  MyPlot.SendData(F("Sensor 2"), temp_2_f, TimePlot::Magenta, TimePlot::Solid, /* Line width = */ 1,  TimePlot::NoMarker);
  MyPlot.SendData(F("Sensor 3"), temp_3_f, TimePlot::Blue, TimePlot::Solid, /* Line width = */ 1,  TimePlot::NoMarker);
  MyPlot.SendData(F("Sensor 4"), temp_4_f, TimePlot::Yellow, TimePlot::Solid, /* Line width = */ 1,  TimePlot::NoMarker);
  MyPlot.SendData(F("Sensor 5"), temp_5_f, TimePlot::White, TimePlot::Solid, /* Line width = */ 1,  TimePlot::NoMarker);
}


//THE FUNCTIONS BELOW ARE RESPONSIBLE FOR UPDATING THE VALUES OF THE VARIABLES RECEIVED BY THE SERIAL
//WHEN THE EVENT WITH THE CORRECT MESSAGE IS RECEIVED THE VALUE IS UPDATED

//SOLENOID 1
void GetSol_1(CommandParameter &Parameters){
  state_sol1 = Parameters.NextParameterAsInteger(state_sol1);
}
//SOLENOID 2
void GetSol_2(CommandParameter &Parameters){
  state_sol2 = Parameters.NextParameterAsInteger(state_sol2);
}
//SOLENOID 3
void GetSol_3(CommandParameter &Parameters){
  state_sol3 = Parameters.NextParameterAsInteger(state_sol3);
}
//SOLENOID 4
void GetSol_4(CommandParameter &Parameters){
  state_sol4 = Parameters.NextParameterAsInteger(state_sol4);
}
//SOLENOID 5
void GetSol_5(CommandParameter &Parameters){
  state_sol5 = Parameters.NextParameterAsInteger(state_sol5);
}
//RESISTOR 1
void GetRes_1(CommandParameter &Parameters){
  state_res1 = Parameters.NextParameterAsInteger(state_res1);
}
//RESISTOR 2
void GetRes_2(CommandParameter &Parameters){
  state_res2 = Parameters.NextParameterAsInteger(state_res2);
}
//START SYSTEM
void GetStartOP(CommandParameter &Parameters){
  state_sys = Parameters.NextParameterAsInteger(state_sys);
}
//STOP SYSTEM
void GetStopOP(CommandParameter &Parameters){
  state_sys = Parameters.NextParameterAsInteger(state_sys);
}
//SETPOINT
void GetSetPoint(CommandParameter &Parameters){
  setpoint = Parameters.NextParameterAsInteger(setpoint) - CONSTANTE_CALIB;
}
//BOMB 1
void GetBomba1(CommandParameter &Parameters){
  bomba1 = Parameters.NextParameterAsInteger(bomba1);
}
//BOM 2
void GetBomba1(CommandParameter &Parameters){
  bomba2 = Parameters.NextParameterAsInteger(bomba2);
}