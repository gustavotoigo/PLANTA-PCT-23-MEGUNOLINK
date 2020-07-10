#ifndef _DECLARATIONS_H_
#define _DECLARATIONS_H_

#include <max6675.h>
#include <declarations.h>

//CONSTANTE DE CALIBRAÇÃO
#define CONSTANTE_CALIB      2

//NUMERO DE AMOSTRAS DO FILTRO
#define N                    20

//VETORES DE CADA SENSOR PARA O CALCULO DO FILTRO
float samples_1[N];
float samples_2[N];
float samples_3[N];
float samples_4[N];
float samples_5[N];

//VARIAVEIS DE TEMPERATURA, TANTO COM FILTRO E SEM FILTRO
float temp_1,
      temp_1_f,
      temp_2,
      temp_2_f,
      temp_3,
      temp_3_f,
      temp_4,
      temp_4_f,
      temp_5,
      temp_5_f;

//ESTADO DO SISTEMA, USADO NO CONTROLE DA TEMPERATURA E ACIONAMENTO DOS RESISTORES
bool state;

//VALORES E ESTADOS RECEBIDOS E ATUALIZADOS PELO SUPERVISORIO
int state_sol1 = 0,     //ESTADO DO SOLENOIDE 1
    state_sol2 = 0,     //ESTADO DO SOLENOIDE 2
    state_sol3 = 0,     //ESTADO DO SOLENOIDE 2  
    state_sol4 = 0,     //ESTADO DO SOLENOIDE 2
    state_sol5 = 0,     //ESTADO DO SOLENOIDE 2
    state_res1 = 0,     //ESTADO DO SOLENOIDE 2
    state_res2 = 0,     //ESTADO DO SOLENOIDE 2
    state_sys = 0,      //ESTADO DO SISTEMA (OPERACAO OU PARADO)
    bomba1 = 0,
    bomba2 = 0,
    setpoint;       //VALOR DO setpoint
    

//PROTÓTIPOS DAS FUNCOES  
float ReadTemps(void);                //COLETA A TEMPERATURA DE TODOS OS SENSORES
int WatchTemperature(void);           //VERIFICA O VALOR DE TEMPERATURA
void EnableResistance_1(void);        //ATIVA RESISTOR 1
void EnableResistance_2(void);        //ATIVA RESISTOR 1
void PrintData(void);                 //IMPRIME NA SERIAL VALORES DE TEMPERATURA
void PWMotors(void);                  //ATUALIZA O VALOR DE POTENCIA NOS MOTORES
void EnableSolenoid(void);            //ATUALIZA ESTADO DOS SOLENOIDES
float MovingAverage(int sensor_num);  //CALCULA A MEDIA MOVEL DE TODOS OS SENSORES
void PrintData2(void);
void SendDataToTimePlot(void);        //ENVIA PARA O SUPERVISORIO CONFIGURAÇÃO DOS DADOS A SEREM PLOTADOS    
void RunSys(void);                    //CONTEM TODAS FUNCOES DE IMPORTANCIA DO SISTEMA, BASICAMENTE FOI FEITA POR ORGANIZACAO DO LOOP            

//PROTOTIPOS DAS FUNCOES DE ATUALIZAÇÃO E VALORES E ESTADOS DO SISTEMA VINDOS DO SUPERVISORIO
void GetSol_1(CommandParameter &Parameters);    //RECEBE E ATUALIZA O VALOR DO ESTADO DO SOLENOIDE 1
void GetSol_2(CommandParameter &Parameters);    //RECEBE E ATUALIZA O VALOR DO ESTADO DO SOLENOIDE 2
void GetSol_3(CommandParameter &Parameters);    //RECEBE E ATUALIZA O VALOR DO ESTADO DO SOLENOIDE 3
void GetSol_4(CommandParameter &Parameters);    //RECEBE E ATUALIZA O VALOR DO ESTADO DO SOLENOIDE 4
void GetSol_5(CommandParameter &Parameters);    //RECEBE E ATUALIZA O VALOR DO ESTADO DO SOLENOIDE 5

void GetRes_1(CommandParameter &Parameters);    //RECEBE E ATUALIZA O VALOR DO ESTADO DO RESISTOR 1
void GetRes_2(CommandParameter &Parameters);    //RECEBE E ATUALIZA O VALOR DO ESTADO DO RESISTOR 2

void GetSetPoint(CommandParameter &Parameters); //RECEBE E ATUALIZA O VALOR DO setpoint

void GetStartOP(CommandParameter &Parameters);  //RECEBE A ATUALIZA O ESTADO INATIVO DO SISTEMA
void GetStopOP(CommandParameter &Parameters);   //RECEBE E ATUALIZA O ESTADO ATIVO DO SISTEMA

//CREAT AN INSTANCE USING THE PINS (CLK, CS, SO)
MAX6675 ktc_1(CLK_1, CS_1, SO_1);               //SENSOR 1
MAX6675 ktc_2(CLK_2, CS_2, SO_2);               //SENSOR 2
MAX6675 ktc_3(CLK_3, CS_3, SO_3);               //SENSOR 3
MAX6675 ktc_4(CLK_4, CS_4, SO_4);               //SENSOR 4
MAX6675 ktc_5(CLK_5, CS_5, SO_5);               //SENSOR 5  




#endif