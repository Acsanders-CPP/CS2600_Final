// Allows for player versus play or player versus computer tic-tac-toe matches
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"

//user interface for user navagation

void CheckTieCondition(char Slots[9], bool Mode);
void CheckWinCondition(char Slots[9], bool Mode);
void PrintBoard(char Slots[9]);
void PrintMainMenu();
void PvCMatch();
void PvPMatch();
void Quit();
void Replay(bool Mode);
int mqttMessageArrived(void *context, char *topic, int topicLen, MQTTClient_message *message);

MQTTClient client;
int mqttComputerMove = -1;
int lastComputerMove = -1;

int mqttMessageArrived(void *context, char *topic, int topicLen, MQTTClient_message *message) {

    if (message != NULL) {
        printf("\nLength of message: %ld", strlen((*message).payload));
        printf("\nReceived message: %s", (*message).payload);
        mqttComputerMove = atoi((*message).payload);
        fflush(stdout);
        MQTTClient_freeMessage(&message);
        MQTTClient_free(topic);
    }

    return 1;

}


int main(){
  int rc = 0;
  rc = MQTTClient_create(&client, "test.mosquitto.org", "ACS-P2", MQTTCLIENT_PERSISTENCE_NONE, NULL);
  printf("\nCreate rc: %d", rc);
  rc = MQTTClient_setCallbacks(client, NULL, NULL, mqttMessageArrived, NULL);
  printf("\nSet Callback rc: %d", rc);
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  conn_opts.keepAliveInterval = 300;
  conn_opts.cleansession = 0;
  rc = MQTTClient_connect(client, &conn_opts);
  printf("\nConnection rc: %d", rc);
  int qos = 2;
  rc = MQTTClient_subscribe(client, "acs/move", qos);
  printf("\nSubscribe rc: %d\n", rc);

  PrintMainMenu();
}

void CheckTieCondition(char Slots[9], bool Mode){
  int TieCount = 0;
  
  for (int i = 0; i  < 10; i += 4){
    if (i == 0){
      if ((Slots[i] != ' ') && ((Slots[i + 1] != Slots[i]) && (Slots[i + 1] != ' ')) || ((Slots[i + 3] != Slots[i]) && (Slots[i + 3] != ' '))){
        TieCount++;
      }
    }
    else if (i == 4){
      if((Slots[i] != ' ') && ((Slots[i - 3] != Slots[i]) && (Slots[i - 3] != ' ')) || ((Slots[i - 1] != Slots[i]) && (Slots[i - 1] != ' ')) || ((Slots[i - 4] != Slots[i]) && (Slots[i - 4] != ' ')) || ((Slots[i - 2] != Slots[i]) && (Slots[i - 2] != ' '))){
        TieCount++;
      }
    }
    else if (i == 8){
      if ((Slots[i] != ' ') && ((Slots[i - 3] != Slots[i]) && (Slots[i - 3] != ' ')) || ((Slots[i - 1] != Slots[i]) && (Slots[i - 1] != ' '))){
        TieCount++;
      }
    }
    //printf("Tie Count: %d\ni: %d\n", TieCount, i);
  }

  if (TieCount == 3){
    PrintBoard(Slots);
    
    printf("Tie\n");

    Replay(Mode);
  }
}

void CheckWinCondition(char Slots[9], bool Mode){
  for (int i = 0; i  < 10; i += 4){
    if (i == 0){
      if ((Slots[i] == 'X' && Slots[i + 1] == 'X' && Slots[i + 2] == 'X') || (Slots[i] == 'X' && Slots[i + 3] == 'X' && Slots[i + 6] == 'X')){
        PrintBoard(Slots);
        
        printf("Player 1 wins\n");

        Replay(Mode);
      }
      else if ((Slots[i] == 'O' && Slots[i + 1] == 'O' && Slots[i + 2] == 'O') || (Slots[i] == 'O' && Slots[i + 3] == 'O' && Slots[i + 6] == 'O')){
        PrintBoard(Slots);
        
        printf("Player 2 wins\n");
        
        Replay(Mode);
      }
    }
    else if (i == 4){
      if((Slots[i] == 'X' && Slots[i - 3] == 'X' && Slots[i + 3] == 'X') || (Slots[i] == 'X' && Slots[i - 1] == 'X' && Slots[i + 1] == 'X') || (Slots[i] == 'X' && Slots[i - 4] == 'X' && Slots[i + 4] == 'X') || (Slots[i] == 'X' && Slots[i - 2] == 'X' && Slots[i + 2] == 'X')){
        PrintBoard(Slots);
        
        printf("Player 1 wins\n");
        
        Replay(Mode);
      }
      else if((Slots[i] == 'O' && Slots[i - 3] == 'O' && Slots[i + 3] == 'O') || (Slots[i] == 'O' && Slots[i - 1] == 'O' && Slots[i + 1] == 'O') || (Slots[i] == 'O' && Slots[i - 4] == 'O' && Slots[i + 4] == 'O') || (Slots[i] == 'O' && Slots[i - 2] == 'O' && Slots[i + 2] == 'O')){
        PrintBoard(Slots);
        
        printf("Player 2 wins\n");
        
        Replay(Mode);
      }
    }
    else if (i == 8){
      if ((Slots[i] == 'X' && Slots[i -  3] == 'X' && Slots[i - 6] == 'X') || (Slots[i] == 'X' && Slots[i - 1] == 'X' && Slots[i - 2] == 'X')){
        PrintBoard(Slots);
        
        printf("Player 1 wins\n");
        
        Replay(Mode);
      }
      else if ((Slots[i] == 'O' && Slots[i -  3] == 'O' && Slots[i - 6] == 'O') || (Slots[i] == 'O' && Slots[i - 1] == 'O' && Slots[i - 2] == 'O')){
        PrintBoard(Slots);
        
        printf("Player 2 wins\n");
        
        Replay(Mode);
      }
    }
  }
}

void PrintBoard(char Slots[9]){
  printf("|%c|%c|%c|\n|%c|%c|%c|\n|%c|%c|%c|\n", Slots[0], Slots[1], Slots[2], Slots[3], Slots[4], Slots[5], Slots[6], Slots[7], Slots[8]);
}

void PrintMainMenu(){
  int Input;
  
    printf("============================\nTic-Tac-Toe\n\n1.) Player vs. Player\n2.) Player vs. Computer\n3.) Quit\n============================\n\nEnter input:\n");

  scanf("%d", &Input);

  switch (Input){
    case 1:
      PvPMatch();
      break;

    case 2:
      PvCMatch();

    case 3:
      Quit();
  }
}

//cpu match code

void PvCMatch(){
  int ComputerMove;
  int Input;
  int rc;
  char Moves[] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',};
  bool Turn = true;

  while (true){
    PrintBoard(Moves);

    if(Turn){
      Turn = !Turn;
    
      printf("Player 1's turn. Enter a number (1 - 9):\n");

      while (true){
        scanf("%d", &Input);

        if (Moves[Input - 1] == ' '){
          Moves[Input - 1] = 'X';

          int payloadLen = strlen(Moves);
          int qos = 2;
          int retained = 0;
          MQTTClient_deliveryToken dt;
          rc = MQTTClient_publish(client, "acs/board", payloadLen, Moves, qos, retained, &dt);
          printf("\nPublish rc: %d", rc);
          rc = MQTTClient_waitForCompletion(client, dt, 10000L);
          printf("\nWait rc: %d\n", rc);

          CheckWinCondition(Moves, false);

          CheckTieCondition(Moves, false);

          break;
        }
        else{
          printf("Invalid Input\n");
        }
      }

    
    }else{
      Turn = !Turn;

      srand(time(0));
    
      printf("Player 2's turn\n");

      printf("Waiting for Player 2's move...\n");
      printf("mqttComputerMove: %d\n", mqttComputerMove);
      printf("lastComputerMove: %d\n", lastComputerMove);
      while (mqttComputerMove == lastComputerMove) {
          sleep(2);
      }
      printf("Received Player 2's move: %d\n", mqttComputerMove);
      lastComputerMove = mqttComputerMove;

      while (true){
        /* ComputerMove = rand() % 9; */
        /* printf("ComputerMove: %d\n", ComputerMove);*/
        ComputerMove = mqttComputerMove - 1; 
        
        if (Moves[ComputerMove] == ' '){
          Moves[ComputerMove] = 'O';

          int payloadLen = strlen(Moves);
          int qos = 2;
          int retained = 0;
          MQTTClient_deliveryToken dt;
          rc = MQTTClient_publish(client, "acs/board", payloadLen, Moves, qos, retained, &dt);
          printf("\nPublish rc: %d", rc);
          rc = MQTTClient_waitForCompletion(client, dt, 10000L);
          printf("\nWait rc: %d\n", rc);

          CheckWinCondition(Moves, false);

          CheckTieCondition(Moves, false);


          break;
        }
      }
    }
  }
}
//match play code

void PvPMatch(){
  int Input;
  char Moves[] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',};
  bool Turn = true;

  while (true){
    PrintBoard(Moves);

    if(Turn){
      Turn = !Turn;
    
      printf("Player 1's turn. Enter a number (1 - 9):\n");

      while (true){
        scanf("%d", &Input);

        if (Moves[Input - 1] == ' '){
          Moves[Input - 1] = 'X';

          CheckWinCondition(Moves, true);

          CheckTieCondition(Moves, true);

          break;
        }
        else{
          printf("Invalid Input\n");
        }
      }

    
    }else{
      Turn = !Turn;
    
      printf("Player 2's turn. Enter a number (1 - 9):\n");

      while (true){
        scanf("%d", &Input);

        if (Moves[Input - 1] == ' '){
          Moves[Input - 1] = 'O';

          CheckWinCondition(Moves, true);

          CheckTieCondition(Moves, true);

          break;
        }
        else{
          printf("Invalid Input\n");
        }
      }
    }
  }
}

void Quit(){
  exit(0);
}

void Replay(bool Mode){
  char Input;

  printf("Play again? (y/n)\n");

  while (true){
    while ((getchar()) != '\n'){
    };
      
    scanf("%c", &Input);

    if (tolower(Input) == 'y'){
      if (Mode){
        PvPMatch();
      }
      else{
        PvCMatch();
      }
    }
    else if(tolower(Input) == 'n'){
      exit(0);
    }
    else{
      printf("Invalid input\n");
    } 
  }
}
