#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Sala {
    char *nome;
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// cria dinamicamente uma sala
Sala *criarSala(const char *nome, Sala *esq, Sala *dir) {
    Sala *s = malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro de alocação\n");
        exit(EXIT_FAILURE);
    }
    s->nome = strdup(nome);
    s->esq = esq;
    s->dir = dir;
    return s;
}

// libera a árvore recursivamente
void liberarSalas(Sala *r) {
    if (!r) return;
    liberarSalas(r->esq);
    liberarSalas(r->dir);
    free(r->nome);
    free(r);
}

// lê uma opção do usuário: 'e', 'd' ou 's'
char lerOpcao() {
    char buffer[64];
    while (1) {
        if (!fgets(buffer, sizeof(buffer), stdin)) return 's';
        // encontra primeiro caractere não-espaco
        for (int i = 0; buffer[i]; ++i) {
            if (!isspace((unsigned char)buffer[i])) {
                return (char)tolower((unsigned char)buffer[i]);
            }
        }
        printf("Opção inválida. Digite 'e' (esquerda), 'd' (direita) ou 's' (sair): ");
    }
}

// explora a mansão interativamente a partir de uma sala
void explorarSalas(Sala *raiz) {
    Sala *atual = raiz;
    printf("Exploração iniciada. Digite 'e' para esquerda, 'd' para direita, 's' para sair.\n\n");
    while (atual) {
        printf("Você está em: %s\n", atual->nome);
        // se nó-folha, fim do caminho
        if (!atual->esq && !atual->dir) {
            printf("Você chegou ao fim deste caminho (sala-folha).\n");
            break;
        }
        printf("Escolha um caminho [e/d] ou 's' para sair: ");
        char op = lerOpcao();
        if (op == 's') {
            printf("Exploração encerrada pelo jogador.\n");
            break;
        } else if (op == 'e') {
            if (atual->esq) {
                atual = atual->esq;
            } else {
                printf("Não há caminho à esquerda a partir daqui. Tente outra opção.\n");
            }
        } else if (op == 'd') {
            if (atual->dir) {
                atual = atual->dir;
            } else {
                printf("Não há caminho à direita a partir daqui. Tente outra opção.\n");
            }
        } else {
            printf("Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
        printf("\n");
    }
}

int main(void) {
    // montar a árvore estaticamente (estrutura imutável em tempo de execução)
    // Exemplo de mapa da mansão:
    //                    Hall de Entrada
    //                   /               \
    //            Biblioteca          Sala de Jantar
    //            /       \            /         \
    //        Escritório  Jardim   Cozinha     Corredor
    //                             (folha)     /     \
    //                                       Quarto  Banheiro

    Sala *escritorio = criarSala("Escritório", NULL, NULL);
    Sala *jardim = criarSala("Jardim Interno", NULL, NULL);
    Sala *biblioteca = criarSala("Biblioteca", escritorio, jardim);

    Sala *cozinha = criarSala("Cozinha", NULL, NULL);
    Sala *quarto = criarSala("Quarto", NULL, NULL);
    Sala *banheiro = criarSala("Banheiro", NULL, NULL);
    Sala *corredor = criarSala("Corredor", quarto, banheiro);
    Sala *salaJantar = criarSala("Sala de Jantar", cozinha, corredor);

    Sala *hall = criarSala("Hall de Entrada", biblioteca, salaJantar);

    // iniciar exploração
    explorarSalas(hall);

    // liberar memória
    liberarSalas(hall);

    return 0;
}