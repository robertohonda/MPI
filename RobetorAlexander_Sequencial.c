#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include <mpi.h>

#define TAM 1024

int** alocaMatriz();

int* alocaVetor();

void inicializacao(int** mat, int* vet_entrada);

void produto_sequencial(int** mat, int* vet_entrada, int* vet_saida);

void imprime_resultado(int* vet);

void desalocaMatriz(int**mat);

void desalocaVetor(int* vet);



int main(int argc, char* argv[]){

	int* vet_entrada, *vet_saida;
	int** mat;

	double tempo_inicio, tempo_fim, tempo_total;

	MPI_Init(&argc, &argv);
	
	srandom(time(NULL));
	mat = alocaMatriz();
	vet_entrada = alocaVetor();
	vet_saida = alocaVetor();
	inicializacao(mat, vet_entrada);//inicializa matriz e vetor

	tempo_inicio = MPI_Wtime();

	produto_sequencial(mat, vet_entrada, vet_saida);

	tempo_fim = MPI_Wtime();
	tempo_total = tempo_fim - tempo_inicio;

	/*
	printf("Resultado:\n");
	imprime_resultado(vet_saida);
	*/

	printf("Tempo gasto: %e microssegundos\n", tempo_total*1000000);

	MPI_Finalize();

	desalocaMatriz(mat);
	desalocaVetor(vet_entrada);
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

void produto_sequencial(int** mat, int* vet_entrada, int* vet_saida)
{
	int i, j, soma;

	for(i=0;i<TAM;i++)
	{
		soma = 0;
		for(j=0;j<TAM;j++)
		{
			soma += mat[i][j] * vet_entrada[j];
		}
		vet_saida[i] = soma;
	}

}

void imprime_resultado(int* vet)
{
	int i;

	for(i=0;i<TAM;i++)		
		printf("%d ", vet[i]);
	printf("\n");
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