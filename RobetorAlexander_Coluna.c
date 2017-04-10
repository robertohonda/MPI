#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define TAM 1024

int** alocaMatriz();

int* alocaVetor();

void inicializacao(int** mat, int* vet_entrada);

void multiplicacao_local(int** mat_local, int* vet_local, int coluna_local, int* mult_local);

void desalocaMatriz(int**mat);

void desalocaVetor(int* vet);

int main(int argc, char* argv[]){
	int rank, size, i, j;

	int** mat;//matriz do problema todo
	int* mult_local;//resultados locais
	int* vet;//vetor multiplicador
	int* vet_local;//vetor local
	int** mat_saida, pos, dest;
	int coluna_local, destino, origem;
	
	int** mat_local;///matriz local
	int* vet_saida;

	double tempo_inicio, tempo_fim, tempo_total;

	/*alocação*/
	mat = alocaMatriz();
	mult_local = alocaVetor();
	vet = alocaVetor();
	vet_local = alocaVetor();
	mat_saida = alocaMatriz();
	mat_local = alocaMatriz();
	vet_saida = alocaVetor();

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	coluna_local = TAM/size;

	if(rank == 0)
	{
		srandom(time(NULL));
		/* Gerando Matriz e vetor (Inicialização) */
		inicializacao(mat, vet);//inicializa matriz e vetor

		tempo_inicio = MPI_Wtime();
	
		/* Comunicação */
		for(i=0;i<TAM;i++)
		{
			for(j=coluna_local;j<TAM;j++)
			{
				dest = j/coluna_local;
				MPI_Send(&mat[i][j], 1, MPI_INT, dest, 0, MPI_COMM_WORLD);//distribuindo os elementos da matriz entre os processos
			}
		}
		MPI_Scatter(vet, coluna_local, MPI_INT, vet_local, coluna_local, MPI_INT, 0, MPI_COMM_WORLD);//dividindo o vetor entre os processos

		/* Computação */
		multiplicacao_local(mat, vet_local, coluna_local, mult_local);
	}
	else
	{
		/* Comunicação */
		origem = 0;
		for(i=0;i<TAM;i++)
		{
			for(j=0;j<coluna_local;j++)
			{
				MPI_Recv(&mat_local[i][j], 1, MPI_INT, origem, 0, MPI_COMM_WORLD, NULL);//recebe a matriz local de cada processo
			}
		}
		MPI_Scatter(vet, coluna_local, MPI_INT, vet_local, coluna_local, MPI_INT, 0, MPI_COMM_WORLD);//recebe o vetor local de cada processos

		/* Computação */
		multiplicacao_local(mat_local, vet_local, coluna_local, mult_local);
	}
	MPI_Allreduce(mult_local, vet_saida, TAM, MPI_INT, MPI_SUM, MPI_COMM_WORLD);//envio dos resultados parciais

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

	/*desalocação*/
	desalocaMatriz(mat);
	desalocaVetor(mult_local);
	desalocaVetor(vet);
	desalocaVetor(vet_local);
	desalocaMatriz(mat_saida);
	desalocaMatriz(mat_local);
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

void multiplicacao_local(int** mat_local, int* vet_local, int coluna_local, int* mult_local)
{
	int i, j, pos;
	for(i=0;i<TAM;i++)
	{
		for(j=0;j<coluna_local;j++)
		{
			pos = j;
			mult_local[i] += mat_local[i][j]*vet_local[pos];
		}
		
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