//Miembros del equipo: Carlos Rafael Flores Gallardo, Oscar Ruiz Ramírez,
//Alejandro Gras Olea

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define ROWS 30
#define COLLS 30


enum BOOLEAN {FALSE,TRUE};
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

//Regresa un número random en el rango de 0 a n
int randNum(int n)
{
    int numRand;
    numRand=rand();
    return numRand%n ;
}

//Hace un tiro aleatorio del 1 al 100 y devuelve 1 si el número obtenido
//es menor o igual a probSuccess
int probPercent(int probSuccess)
{
    int probNum;
    probNum=randNum(100);    
    if(probNum<=probSuccess)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//Pone en estado default a todas las celdas de la matriz principal para no
//tener valores basura.
void matrixNoneValues( CELL *matrix)
{
    CELL *mtx;
    int totalCells=ROWS*COLLS;

    for(int i=0;i<totalCells;i++)
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
void initializer( CELL *matrix, int probability)
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
            mtx->age=(randNum(90)+1);
            mtx->statusCovid=SANO;
            contadorSanos+=1;
            
            if((occCellsCount+1)%2==0)
            {
                mtx->mask=TRUE;
            }
            else
            {
                mtx->mask=FALSE;
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

//Imprime la matriz de celdas, seleccionando la letra adecuada para hacerlo según su estatus
//de ocupación, de infección y de uso de máscara.
void printerMat(CELL *matrix)
{
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
                if(mtx->mask==FALSE)
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
                if(mtx->mask==FALSE)
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
        
    }
}

//Escoge una celda ocupada aleatoria en la matriz principal, y la vuelve el
//primer paciente infectado.
void randomMatElementChange(CELL *matrix)
{
    CELL *mtx;
    int totalCells=ROWS*COLLS;
    
    int cond=0; //Esta condición romperá el ciclo while cuando se infecte a la primera persona.
    while(cond==0)
    {
        int randElement= randNum(totalCells-1);
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

//Le agrega un día a todos los contadores de días de infección a los que
//actualmente se encuentren infectados, y si llegan a siete días, los vuelve
//recuperados.
void oneDayPlus(CELL *matrix)
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


//Revisa si es posible que una celda sea infectada, y si es el caso, entonces 
//decidirá si se infecta basándose en probabilidad y dependiendo del estado de la
//celda emisora y receptora de la infección.
void conditionSpread(CELL *mtx, CELL *objChange) //objChange es la celda que se infectará
{
    if(objChange->statusCell==OCUPADA) 
    {
        if(objChange->statusCovid==SANO) 
        {
            if(objChange->mask==TRUE && mtx->mask==TRUE)
            {
                int probContagio=probPercent(6); 
                if(probContagio==1)
                {
                    objChange->statusCovid=ENFERMO;
                    contadorSanos-=1;
                    contEnfermos+=1;
                    
                }
            }
            else if(objChange->mask==FALSE && mtx->mask==FALSE)
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

//Esta función se encarga de encontrar aquellas celdas que estén en riesgo de infectarse
//y las envía a la función conditionSpread junto con la celda emisora de la infección.
void spreadVirus(CELL *matrix)
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
    int probCellOccu;  //Porcentaje de celdas que estarán ocupadas.
    int days;  //Días que durará el experimento.
    setbuf(stdout,NULL);
    printf("\nDime el porcentaje de celdas que se deberan ocupar(0-100): ");
    scanf("%d",&probCellOccu);
    printf("\nDime la cantidad de dias de infeccion: ");
    scanf("%d",&days);

    srand(time(NULL)); //Obtenemos la semilla para poder obtener números aleatorios.

    CELL matrix[ROWS][COLLS];  //Creamos una matriz compuesta de elementos del struct CELL.
    matrixNoneValues((CELL *) matrix); 

    int totalCells=ROWS*COLLS; //Calculamos la cantidad de celdas en la matriz principal.
    
    initializer((CELL *) matrix, probCellOccu); 
    printerMat((CELL *) matrix); 

    randomMatElementChange((CELL *) matrix); 
    printf("\n\nDia de infeccion numero:0\n"); //Imprimimos el estado inicial de la matriz tras inicializarla y tener el paciente cero.
    printerMat((CELL *) matrix); 
    printf("\n  Total de Sanos:%d\n  Total de Enfermos:%d\n  Total de recuperados:%d\n",contadorSanos,contEnfermos,contRecuperados);
    
    for(int i=0;i<days;i++) //Avanzamos los días del experimento con un for e imprimimos el estado de la matriz cada ciclo.
    {
        printf("\n\nDia de infeccion numero:%d\n",i+1); 
        oneDayPlus((CELL *) matrix); 
        spreadVirus((CELL *) matrix); 
        printerMat((CELL *) matrix);  
        
        printf("\n  Total de Sanos:%d\n  Total de Enfermos:%d\n  Total de recuperados:%d\n",contadorSanos,contEnfermos,contRecuperados);
    }

    int infectedPercent=(contadorSanos*100)/(contadorSanos+contEnfermos+contRecuperados); //Se obtiene el porcentaje final de aquellos que no se infectaron nunca.
    printf("\n\n\tAproximadamente el %d%% de la poblacion nunca se infecto.\n",infectedPercent);

    return 0;
}