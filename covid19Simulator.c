#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

//Change this for a bigger or smaller classroom
#define ROWS 30
#define COLLS 30

enum BOOLEAN {False, True};
enum STATUS_CELL{LIBRE,OCUPADA};
enum STATUS_COVID{SANO,ENFERMO,RECUPERADO};

int contadorSanos=0;
int contEnfermos=0;
int contRecuperados=0;

typedef struct CELL
{
    enum STATUS_CELL statusCell;
    int numID;
    int age;
    enum STATUS_COVID statusCovid;
    int sickDays;
    enum BOOLEAN mask;

}CELL;


int randInt(int n) //Return a random int
{
    return rand()%n ;
}


int probPercent(int probSuccess) //Return true if the random int <= probSuccess
{
    return randInt(100) <= probSuccess? 1:0;
}

void matrixNoneValues(CELL *matrix)//Set all the value to 0 on the matrix
{
    CELL *mtx;
    for(int i=0;i<ROWS*COLLS;i++)
    {
        mtx=matrix+i;
        mtx->statusCell=LIBRE;
        mtx->numID=-1;
        mtx->age=-1;
        mtx->statusCovid=-1;
        mtx->mask=-1;
        mtx->sickDays=0;
    }
}

//Establece aleatoriamente en estado ocupado y sano en la matriz principal al porcentaje de celdas
//que se le indique y a la mitad les asigna cubrebocas.
void initializer( CELL *matrix, int probability)//Set random cells into occupied and non-infected by the given percentage of cells
                                                // and set the half with a mask
{
    CELL *mtx;
    int totalCells=ROWS*COLLS;
    int totalCellOccupied= probability*totalCells/100;
    int occCellsCount=0;
    int position = 0;

    while(occCellsCount<totalCellOccupied){ 

        mtx=matrix+position;
        int condForOcc=probPercent(probability/2);
        if(condForOcc==1 && mtx->statusCell==LIBRE)
        {   
            mtx->statusCell=OCUPADA;
            mtx->numID=occCellsCount+1;
            mtx->age=(randInt(90)+1);
            mtx->statusCovid=SANO;
            contadorSanos+=1;
            
            if((occCellsCount+1)%2==0)
            {
                mtx->mask=True;
            }
            else
            {
                mtx->mask=False;
            } 
            occCellsCount+=1;
        }
        if(position>=totalCells)
        {
            position=0;
        }
        else
        {
            position+=1;
        }
        
    }

}

void printerMat(CELL *matrix) //Print the matrix with the correct letter to show the state of the cell
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    printf("\n ");
    CELL *mtx;
    char cellStatus;
    int totalCells=ROWS*COLLS;
    for(int i=0;i<totalCells;i++)
    {
        mtx=matrix+i;
        if(mtx->statusCell==OCUPADA)
        {
            if(mtx->statusCovid==SANO)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
                if(mtx->mask==False)
                {
                    cellStatus='S';
                }
                else
                {
                    cellStatus='s';
                }

            }
            else if(mtx->statusCovid==ENFERMO)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
                if(mtx->mask==False)
                {
                    cellStatus='E';
                }
                else
                {
                    cellStatus='e';
                }

            }
            else
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
                cellStatus='r';
            }

        }
        else
        {
            cellStatus=' ';
        }
        if((i+1)%COLLS==0)
        {
            
            printf(" %c\n ",cellStatus);
        }
        else
        {
            printf(" %c ",cellStatus);
        }
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    }
}

void randomMatElementChange(CELL *matrix) //Start of the infection by selection one occupied random cell
{
    CELL *mtx;
    int totalCells=ROWS*COLLS;
    
    int cond=0; 
    while(cond==0)
    {
        int randElement= randInt(totalCells-1);
        mtx=matrix+randElement;
        
        if(mtx->statusCell==OCUPADA) 
        {
            mtx->statusCovid=ENFERMO;
            contadorSanos-=1;
            contEnfermos+=1;
            cond+=1;
        }
    }
}


void oneDayPlus(CELL *matrix) //Add one day into the infected cell counter, and if this counter reaches 7 turns it into a recovered cell
{
    CELL *mtx;
    int totalCells=ROWS*COLLS;
    for(int i=0;i<totalCells;i++)
    {
        mtx=matrix+i;
        if(mtx->statusCovid==ENFERMO)
        {   
            if(mtx->sickDays==7)
            {
                mtx->statusCovid=RECUPERADO;
                contEnfermos-=1;
                contRecuperados+=1;
                mtx->sickDays=0;
            }
            else{
                mtx->sickDays+=1;
            }
        }
    }
}

void conditionSpread(CELL *mtx, CELL *objChange) //Will try to infect the objChange cell by the conditions
{
    if(objChange->statusCell==OCUPADA) 
    {
        if(objChange->statusCovid==SANO) 
        {
            if(objChange->mask==True && mtx->mask==True)
            {
                int probContagio=probPercent(6); 
                if(probContagio==1)
                {
                    objChange->statusCovid=ENFERMO;
                    contadorSanos-=1;
                    contEnfermos+=1;
                    
                }
            }
            else if(objChange->mask==False && mtx->mask==False)
            {
                int probContagio=probPercent(60); 
                if(probContagio==1)
                {
                    objChange->statusCovid=ENFERMO;
                    contadorSanos-=1;
                    contEnfermos+=1;
                }
            }
            else
            {
                int probContagio=probPercent(20); 
                if(probContagio==1)
                {
                    objChange->statusCovid=ENFERMO;
                    contadorSanos-=1;
                    contEnfermos+=1;
                }
            }             
        }
    }
}

void spreadVirus(CELL *matrix) //Select the cell that can be infected with the percentage of probability of each 
{
    CELL *mtx;
    CELL *objChange;
    int totalCells=ROWS*COLLS;
    for(int i=0;i<totalCells;i++)
    {
        mtx=matrix+i;
        if(mtx->statusCovid==ENFERMO && mtx->sickDays!=0)
        {                                                        
            if((i-COLLS-1)>=0 && ((i-COLLS)%COLLS)!=0)
            {
                objChange=matrix+i-COLLS-1;
                conditionSpread((CELL *) mtx,(CELL *) objChange);
            }

            if((i-ROWS)>=0) 
            {
                objChange=matrix+i-COLLS;
                conditionSpread((CELL *) mtx,(CELL *) objChange);
            }

            if((i-COLLS+1)>=0 &&(i-COLLS+1)%COLLS!=0) 
            {
                objChange=matrix+i-COLLS+1;
                conditionSpread((CELL *) mtx,(CELL *) objChange);
            }

            if((i%COLLS)!=0)  
            {                
                objChange=matrix+i-1;
                conditionSpread((CELL *) mtx,(CELL *) objChange);
            }

            if(((i+1)%COLLS)!=0) 
            {                   
                objChange=matrix+i+1;
                conditionSpread((CELL *) mtx,(CELL *) objChange);
            }

            if((i+COLLS-1)<totalCells &&(i+COLLS)%COLLS!=0)
            {
                objChange=matrix+i+COLLS-1;
                conditionSpread((CELL *) mtx,(CELL *) objChange);
            }

            if((i+COLLS)<totalCells) 
            {
                objChange=matrix+i+COLLS;
                conditionSpread((CELL *) mtx,(CELL *) objChange);
            }

            if((i+COLLS+1)<totalCells&&(i+COLLS+1)%COLLS!=0) 
            {
                objChange=matrix+i+COLLS+1;
                conditionSpread((CELL *) mtx,(CELL *) objChange);
            }
        }
    }
}


int main()
{   
    int probCellOccu;  //Percentage of occupied cells
    int days;  //Days of the experiments
    setbuf(stdout,NULL);
    printf("\nDime el porcentaje de celdas que se deberan ocupar(0-100): ");
    scanf("%d",&probCellOccu);
    printf("\nDime la cantidad de dias de infeccion: ");
    scanf("%d",&days);

    srand(time(NULL)); 

    CELL matrix[ROWS][COLLS];  //Creation of the CELL matrix
    matrixNoneValues((CELL *) matrix); 

    int totalCells=ROWS*COLLS; 
    
    initializer((CELL *) matrix, probCellOccu); 
    printerMat((CELL *) matrix); 

    randomMatElementChange((CELL *) matrix); 
    printf("\n\nDia de infeccion numero:0\n"); //Print the first instance of the matrix 
    printerMat((CELL *) matrix); 
    printf("\n  Total de Sanos:%d\n  Total de Enfermos:%d\n  Total de recuperados:%d\n",contadorSanos,contEnfermos,contRecuperados);
    
    for(int i=0;i<days;i++) //Start the game by changing the days
    {
        printf("\n\nDia de infeccion numero:%d\n",i+1); 
        oneDayPlus((CELL *) matrix); 
        spreadVirus((CELL *) matrix); 
        printerMat((CELL *) matrix);  
        
        printf("\n  Total de Sanos:%d\n  Total de Enfermos:%d\n  Total de recuperados:%d\n",contadorSanos,contEnfermos,contRecuperados);
    }

    int infectedPercent=(contadorSanos*100)/(contadorSanos+contEnfermos+contRecuperados); //We can se the stats of this experiment.
    printf("\n\n\tAproximadamente el %d%% de la poblacion nunca se infecto.\n",infectedPercent);

    return 0;
}