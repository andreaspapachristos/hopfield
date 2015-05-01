/* 
 * File:   main.c
 * Author: maria
 *
 * Created on 23 Απριλίου 2015, 11:13 μμ
 */

#include "hopfield.h"

void InitializeRandoms()
{
  srand(4711);
}


int RandomEqualINT(int Low, int High)
{
  return rand() % (High-Low+1) + Low;
}   


void InitializeApplication(NET* Net)
{
  int n,i,j;

  for (n=0; n<NUM_DATA; n++) {
    for (i=0; i<Y; i++) {
      for (j=0; j<X; j++) {
        Input [n][i*X+j] = BIPOLAR(Pattern [n][i][j] == 'O');
        Input_[n][i*X+j] = BIPOLAR(Pattern_[n][i][j] == 'O');
      }
    }
  }
  ff = fopen("HOPFIELD1.txt", "w");
}


void WriteNet(NET* Net)
{
  int i,j;
   
  for (i=0; i<Y; i++) {
    for (j=0; j<X; j++) {
      fprintf(ff, "%i", Net->Output[i*X+j]);
    }
  
       fprintf(ff, "\n");
  
  }
   if (!(i%2)){
       fprintf(ff, "%s","->");}
  fprintf(ff, "\t");

}

void FinalizeApplication(NET* Net)
{
  fclose(ff);
}

void GenerateNetwork(NET* Net)
{
  int i;

  Net->Units     = N;
  Net->Output    = (int*)  calloc(N, sizeof(int));
  Net->Threshold = (int*)  calloc(N, sizeof(int));
  Net->Weight    = (int**) calloc(N, sizeof(int*));

  for (i=0; i<N; i++) {
    Net->Threshold[i] = 0;
    Net->Weight[i]    = (int*) calloc(N, sizeof(int));
  }
}


void CalculateWeights(NET* Net)
{
  int i,j,n;
  int Weight;

  for (i=0; i<Net->Units; i++) {
    for (j=0; j<Net->Units; j++) {
      Weight = 0;
      if (i!=j) {
        for (n=0; n<NUM_DATA; n++) {
          Weight += Input[n][i] * Input[n][j];
        }
      }
      Net->Weight[i][j] = Weight;
    }
  }
}


void SetInput(NET* Net, int* Input)
{
  int i;
   
  for (i=0; i<Net->Units; i++) {
    Net->Output[i] = Input[i];
  }
  WriteNet(Net);
}


void GetOutput(NET* Net, int* Output)
{
  int i;
   
  for (i=0; i<Net->Units; i++) {
    Output[i] = Net->Output[i];
  }
  WriteNet(Net);
}
BOOL PropagateUnit(NET* Net, int i)
{
  int  j;
  int  Sum, Out;
  BOOL Changed;

  Changed = FALSE;
  Sum = 0;
  for (j=0; j<Net->Units; j++) {
    Sum += Net->Weight[i][j] * Net->Output[j];
  }
  if (Sum != Net->Threshold[i]) {
    if (Sum < Net->Threshold[i]) Out = LO;
    if (Sum > Net->Threshold[i]) Out = HI;
    if (Out != Net->Output[i]) {
      Changed = TRUE;
      Net->Output[i] = Out;
    }
  }
  return Changed;
}


void PropagateNet(NET* Net)
{
  int Iteration, IterationOfLastChange;

  Iteration = 0;
  IterationOfLastChange = 0;
  do {
    Iteration++;
    if (PropagateUnit(Net, RandomEqualINT(0, Net->Units-1)))
      IterationOfLastChange = Iteration;
  } while (Iteration-IterationOfLastChange < 10*Net->Units);
}

void SimulateNet(NET* Net, int* Input)
{
  int Output[N];
   
  SetInput(Net, Input);
  PropagateNet(Net);
  GetOutput(Net, Output);
}


int main(int argc, char** argv) {
    
  NET Net;
  int n;

  InitializeRandoms();
  GenerateNetwork(&Net);
  InitializeApplication(&Net);
  CalculateWeights(&Net);
   
  for (n=0; n<NUM_DATA; n++) {
    SimulateNet(&Net, Input[n]);
  }
  for (n=0; n<NUM_DATA; n++) {
    SimulateNet(&Net, Input_[n]);
  }
   
  FinalizeApplication(&Net);

    return (0);
}

