#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define TAM 1024

int** alocaMatriz();

int* alocaVetor();

void inicializacao(int** mat, int* vet_entrada);

void multiplicacao_local(int** mat_local, int* vet_global, int linha_local, int* mult_local);

void desalocaMatriz(int**mat);

void desalocaVetor(int* vet);

int main(int argc, char* argv[]){
	int rank, size, i, j;

	int** mat;//matriz do problema todo
	int* mult_local;//resultados locais
	int tam_local;//tamanho local de cada matriz local
	int* vet_global;
	
	int** mat_local;///matriz local
	int* vet_saida;
	int linha_local;
	double tempo_inicio, tempo_fim, tempo_total;

	/*alocação*/
	mat = alocaMatriz();
	mat_local = alocaMatriz();
	mult_local = alocaVetor();
	vet_global = alocaVetor();
	vet_saida = alocaVetor();

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	/* Gerando Matriz e vetor (Inicialização)*/
	if(rank == 0)
	{
		srandom(time(NULL));
		inicializacao(mat, vet_global);//inicializa matriz e vetor
		tempo_inicio = MPI_Wtime();
	}

	//Communicação
	linha_local = TAM/size;
	tam_local = TAM*TAM/size;//Balanceamento de cargas
	MPI_Scatter(*mat, tam_local, MPI_INT, *mat_local, tam_local, MPI_INT, 0, MPI_COMM_WORLD);//Divide a matriz em matrizes locais
	MPI_Bcast(vet_global, TAM, MPI_INT, 0, MPI_COMM_WORLD);//Envia o vetor para todos processos
	
	//Computação local
	multiplicacao_local(mat_local, vet_global, linha_local, mult_local);

	//Comunicação
	MPI_Allgather(mult_local, TAM/size, MPI_INT, vet_saida, TAM/size, MPI_INT, MPI_COMM_WORLD);


	/*Fim da computação*/
	if(rank==0)
	{	
		tempo_fim = MPI_Wtime();
		tempo_total = tempo_fim - tempo_inicio;
		
		/*
		printf("Resultado:\n");
		for(i=0;i<TAM;i++)		
			printf("%d ", vet_saida[i]);
		printf("\n");
		*/
		
		
		printf("Tempo gasto: %e microssegundos\n", tempo_total*1000000);
	}
	
	MPI_Finalize();

	desalocaMatriz(mat);
	desalocaMatriz(mat_local);
	desalocaVetor(mult_local);
	desalocaVetor(vet_global);
	desalocaVetor(vet_saida);

	return 0;
}


int** alocaMatriz()
{
	int* vetTemp;//vetorTemporário
	int** mat;
	int i;

	mat = (int**)calloc(TAM, sizeof(int*));
	vetTemp = (int*) calloc(TAM*TAM, sizeof(int));
	for(i=0;i<TAM;i++)
	{
		mat[i] = &vetTemp[i*TAM];
	}

	return mat;
}

int* alocaVetor()
{
	return (int*) calloc(TAM, sizeof(int));
}

void inicializacao(int** mat, int* vet_entrada)
{
	int i, j;

	for(i=0;i<TAM;i++)
	{
		for(j=0;j<TAM;j++)
		{	
			mat[i][j] = random();///Inicializa matriz
		}
		vet_entrada[i] = random();
	}
}

void multiplicacao_local(int** mat_local, int* vet_global, int linha_local, int* mult_local)
{
	int i, j, pos;
	for(i=0;i<linha_local;i++)
	{
		for(j=0;j<TAM;j++)
		{
			mult_local[i] += mat_local[i][j] * vet_global[j];
		}
		//printf("processo: %d, Resultado: %d\n", rank, mult_local[i]);
	}
}

void desalocaMatriz(int**mat)
{
	free(mat[0]);
	free(mat);
}

void desalocaVetor(int* vet)
{
	free(vet);
}