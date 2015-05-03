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
  ff = fopen("HOPFIELD1-3-17.txt", "w");
}


void WriteNet(NET* Net)
{
  int i,j;
   
  for (i=0; i<Y; i++) {
    for (j=0; j<X; j++) {
      fprintf(ff, "%c", BINARY(Net->Output[i*X+j]) ? 'O' : ' ');
    }
  
       fprintf(ff, "\n");
  
  }
  fprintf(ff, "\n");

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
  Net->Temp     = (int*) calloc(N,sizeof(int));     
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
/*Εδώ αρχίζουν οι αλλαγές στον κώδικα.
 * Αν έχει επιλεγεί Ασύγχρονη ενημέρωση, ο κώδικας δεν χρειάζεται καμμιά αλλαγή, τρέχει ο αρχικός  κώδικας
 * Αν έχει επιλεγεί Σύγχρονη ενημέρωση, τότε "περνάμε" στο  else και εκτελείτε ο νέος κώδικας.
 */
BOOL PropagateUnit(NET* Net, int i)
{
  int  j,f;
  int  Sum, Out;
  BOOL Changed;

  Changed = FALSE;
  Sum = 0;
  if(ASYNCHRONOUS ){
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
  }
  /* Καταρχην ο υπολογισμός του διανύσματος εξόδου δεν χρησιμοποιεί την γεννήτρια τυχαίων αριθμών αφού θα χρησιμοποιηθεί ολόκληρος ο
   * πίνακας βαρών για τον υπολογισμό του νέου διανύσματος εισόδου.
   * Έτσι κάνω τον πολλαπλασιασμό της κάθε γραμμής του πίνακα βαρών με το εισερχόμενο διάνυσμα και την τιμή που προκύπτει την περνάμε
   * σαν όρισμα στην συνάρτηση προσήμου (εδώ αν η τιμή της συνάρτησης είναι μηδέν η είσοδος μεταφέρεται στην έξοδο) , αλλάζω το flag Changed
   * σε TRUE και καταχωρώ την αλλαγμένη τιμή στον προσωρινό πίνακα Temp
  
    */
      else{
                    for (f=0; f< N;  f++) {
                        Sum=0;
                            for(j=0; j< N; j++) {
                                    Sum += Net->Weight[f][j] * Net->Output[j];
                                }
                   if (Sum != Net->Threshold[f]) {
                        if (Sum < Net->Threshold[f]) Out = LO;
                        if (Sum > Net->Threshold[f]) Out = HI;
                        if (Out != Net->Output[f]) {
                             Changed = TRUE;
                            Net->Temp[f]=Out;
                            }
                    }  
                }
         }
         return Changed; 
  }
  
/*
 * Συνεχίζονται οι αλλαγές και σε αυτή την συνάρτηση.
 * Το "μοντέλο" είναι το ίδιο με πριν, αν έχει επιλεγεί Ασύγχρονη ενημέρωση όλα κυλούν όπως πριν, αλλιώς περνάει στο else του κώδικα.
 */


void PropagateNet(NET* Net)
{
  int Iteration, IterationOfLastChange,f;

  Iteration = 0;
  IterationOfLastChange = 0;
   if(ASYNCHRONOUS){
  do {
    Iteration++;
    if (PropagateUnit(Net, RandomEqualINT(0, Net->Units-1)))
      IterationOfLastChange = Iteration;
    }while (Iteration-IterationOfLastChange < 10*(Net->Units));
   }
  /*Ξεκινάω με αρχικοποίηση του προσωρινού πίνακα Temp με τις τιμές του διανύσματος εισόδου.
   * Αν η συνάρτηση PropagateUnit επιστρέψει TRUE τότε αντιγράφω στο διάνυσμα εισόδου Output τον προσωρινό πίνακα Temp.
   */
    else{
          for(f=0;f<Net->Units;f++){
               Net->Temp[f]=Net->Output[f];
          }
             if (PropagateUnit(Net, N)){ 
                     for (f=0;f<Net->Units;f++){
                        Net->Output[f]=Net->Temp[f];  
                        }
                } 
            }       
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

