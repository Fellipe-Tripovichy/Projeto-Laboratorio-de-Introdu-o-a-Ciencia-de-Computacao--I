#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    int id;
    char nome[100];
    int idade;
    double saldo;
    bool excluido;
} Cliente;

void adicionarUmCliente(FILE* arquivo) {
    Cliente* cliente = (Cliente*) malloc(sizeof(Cliente));
    if (cliente == NULL) {
        printf("Erro na alocacao de memoria.\n");
        return;
    }

    char input[200];
    scanf(" %[^\n]%*c", input);

    char *token = strtok(input, ",");
    if (token == NULL) {
        printf("Entrada invalida.\n");
        free(cliente);
        return;
    }

    strncpy(cliente->nome, token, sizeof(cliente->nome) - 1);
    cliente->nome[sizeof(cliente->nome) - 1] = '\0';

    token = strtok(NULL, ",");
    if (token == NULL) {
        printf("Entrada invalida.\n");
        free(cliente);
        return;
    }

    sscanf(token, "%d", &(cliente->idade));

    token = strtok(NULL, ",");
    if (token == NULL) {
        printf("Entrada invalida.\n");
        free(cliente);
        return;
    }
    
    sscanf(token, "%lf", &(cliente->saldo));

    cliente->excluido = false;

    fseek(arquivo, 0, SEEK_END);
    long int posicao = ftell(arquivo);

    cliente->id = 1 + posicao/sizeof(Cliente);
    printf("Usuario inserido com id %d.\n", cliente->id);

    fwrite(cliente, sizeof(Cliente), 1, arquivo);

    free(cliente);
}

void adicionarClientes(FILE* arquivo){

    int nInsercoes;

    scanf("%d", &nInsercoes);
    for(int i = 0; i < nInsercoes; i++){
        adicionarUmCliente(arquivo);
    }
}

void pesquisarCliente(FILE* arquivo, int id) {
    fseek(arquivo, id * sizeof(Cliente), SEEK_SET);
    Cliente cliente;

    if (fread(&cliente, sizeof(Cliente), 1, arquivo) == 1 && !cliente.excluido) {
        printf("\n");
        printf("id: %d\n", cliente.id);
        printf("Nome: %s\n", cliente.nome);
        printf("Idade: %d\n", cliente.idade);
        printf("Saldo atual: %.2lf\n", cliente.saldo);
        printf("\n");
    } else {
        printf("Erro: Usario %d nao encontrado.\n", id+1);
    }
}

void transferirSaldo(FILE* arquivo, int idOrigem, int idDestino, double valor) {
    fseek(arquivo, idOrigem * sizeof(Cliente), SEEK_SET);
    Cliente clienteOrigem;

    if (fread(&clienteOrigem, sizeof(Cliente), 1, arquivo) == 1 && !clienteOrigem.excluido) {
        if (clienteOrigem.saldo >= valor) {
            fseek(arquivo, idDestino * sizeof(Cliente), SEEK_SET);
            Cliente clienteDestino;

            if (fread(&clienteDestino, sizeof(Cliente), 1, arquivo) == 1 && !clienteDestino.excluido) {
                clienteOrigem.saldo -= valor;
                clienteDestino.saldo += valor;

                fseek(arquivo, idOrigem * sizeof(Cliente), SEEK_SET);
                fwrite(&clienteOrigem, sizeof(Cliente), 1, arquivo);

                fseek(arquivo, idDestino * sizeof(Cliente), SEEK_SET);
                fwrite(&clienteDestino, sizeof(Cliente), 1, arquivo);

                printf("Transferencia de R$%.2lf do usuario %d para usuario %d realizada com sucesso.\n", valor, idOrigem+1, idDestino+1);
            } else {
                printf("Erro: Usario %d nao encontrado.\n", idDestino+1);
            }
        } else {
            printf("Saldo insuficiente para transferencia.\n");
        }
    } else {
        printf("Erro: Usario %d nao encontrado.\n", idOrigem+1);
    }
}

void excluirCliente(FILE* arquivo, int id) {
    Cliente cliente;
    fseek(arquivo, id * sizeof(Cliente), SEEK_SET);

    if (fread(&cliente, sizeof(Cliente), 1, arquivo) == 1 &&cliente.excluido == false) {
        fseek(arquivo, id * sizeof(Cliente), SEEK_SET);
        cliente.excluido = true;
        fwrite(&cliente, sizeof(Cliente), 1, arquivo);

        printf("Usuario %d removido com sucesso.\n", id+1);
    } else {
        printf("Erro: Usario %d nao encontrado.\n", id+1);
    }
}

int main() {
    FILE* arquivo = fopen("clientes.txt", "r+");

    if (arquivo == NULL) {
        arquivo = fopen("clientes.txt", "w+");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            return 1;
        }
    }

    int opcao;
    do {
        printf("\n");
        printf("1 - Adicionar um cliente\n");
        printf("2 - Adcionar carios clientes\n");
        printf("3 - Buscar cliente por id\n");
        printf("4 - Transferir saldo\n");
        printf("5 - Excluir cliente\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        printf("\n");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                adicionarUmCliente(arquivo);
                break;
            case 2:
                adicionarClientes(arquivo);
                break;
            case 3:
                printf("");
                int id;
                scanf("%d", &id);
                pesquisarCliente(arquivo, id-1);
                break;
            case 4:
                printf("");
                int idOrigem;
                scanf("%d", &idOrigem);

                int idDestino;
                scanf("%d", &idDestino);

                double valor;
                scanf("%lf", &valor);

                transferirSaldo(arquivo, idOrigem-1, idDestino-1, valor);
                break;
            case 5:
                printf("");
                int idExclusao;
                scanf("%d", &idExclusao);
                excluirCliente(arquivo, idExclusao-1);
                break;
            case 0:
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opcao invalida.\n");
                break;
        }
    } while (opcao != 0);

    fclose(arquivo);
    return 0;
}