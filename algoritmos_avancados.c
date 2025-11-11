#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Sala {
    char *nome;
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// Estrutura para a BST de pistas
typedef struct NoPista {
    char *pista;
    struct NoPista *esq;
    struct NoPista *dir;
} NoPista;

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

// cria um novo nó para a BST de pistas
NoPista *criarNoPista(const char *pista) {
    NoPista *no = malloc(sizeof(NoPista));
    if (!no) {
        fprintf(stderr, "Erro de alocação para pista\n");
        exit(EXIT_FAILURE);
    }
    no->pista = strdup(pista);
    no->esq = NULL;
    no->dir = NULL;
    return no;
}

// insere uma pista na BST em ordem alfabética
NoPista *inserirPista(NoPista *raiz, const char *pista) {
    if (!raiz) {
        return criarNoPista(pista);
    }
    
    int cmp = strcmp(pista, raiz->pista);
    
    if (cmp < 0) {
        raiz->esq = inserirPista(raiz->esq, pista);
    } else if (cmp > 0) {
        raiz->dir = inserirPista(raiz->dir, pista);
    }
    // se cmp == 0, a pista já existe, não insere duplicata
    
    return raiz;
}

// busca uma pista na BST
int buscarPista(NoPista *raiz, const char *pista) {
    if (!raiz) return 0;
    
    int cmp = strcmp(pista, raiz->pista);
    
    if (cmp == 0) return 1;
    else if (cmp < 0) return buscarPista(raiz->esq, pista);
    else return buscarPista(raiz->dir, pista);
}

// lista todas as pistas em ordem alfabética (in-ordem)
void listarPistasEmOrdem(NoPista *raiz) {
    if (!raiz) return;
    
    listarPistasEmOrdem(raiz->esq);
    printf("  • %s\n", raiz->pista);
    listarPistasEmOrdem(raiz->dir);
}

// libera a árvore de pistas recursivamente
void liberarPistas(NoPista *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz->pista);
    free(raiz);
}

// libera a árvore de salas recursivamente
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
        // encontra primeiro caractere não-espaço
        for (int i = 0; buffer[i]; ++i) {
            if (!isspace((unsigned char)buffer[i])) {
                return (char)tolower((unsigned char)buffer[i]);
            }
        }
        printf("Opção inválida. Digite 'e' (esquerda), 'd' (direita) ou 's' (sair): ");
    }
}

// verifica se a sala tem pista e a adiciona à BST
void verificarPista(const char *nomeSala, NoPista **raizPistas) {
    // mapa de salas com pistas
    if (strcmp(nomeSala, "Biblioteca") == 0) {
        printf("🔍 Você encontrou uma pista: 'Carta Misteriosa'\n");
        *raizPistas = inserirPista(*raizPistas, "Carta Misteriosa");
    } else if (strcmp(nomeSala, "Escritório") == 0) {
        printf("🔍 Você encontrou uma pista: 'Chave Enferrujada'\n");
        *raizPistas = inserirPista(*raizPistas, "Chave Enferrujada");
    } else if (strcmp(nomeSala, "Cozinha") == 0) {
        printf("🔍 Você encontrou uma pista: 'Faca de Manteiga'\n");
        *raizPistas = inserirPista(*raizPistas, "Faca de Manteiga");
    } else if (strcmp(nomeSala, "Corredor") == 0) {
        printf("🔍 Você encontrou uma pista: 'Pegadas Estranhas'\n");
        *raizPistas = inserirPista(*raizPistas, "Pegadas Estranhas");
    } else if (strcmp(nomeSala, "Quarto") == 0) {
        printf("🔍 Você encontrou uma pista: 'Diário Secreto'\n");
        *raizPistas = inserirPista(*raizPistas, "Diário Secreto");
    }
}

// explora a mansão interativamente a partir de uma sala
void explorarSalas(Sala *raiz, NoPista **raizPistas) {
    Sala *atual = raiz;
    printf("Exploração iniciada. Digite 'e' para esquerda, 'd' para direita, 'p' para ver pistas, 's' para sair.\n\n");
    
    while (atual) {
        printf("Você está em: %s\n", atual->nome);
        
        // verifica e adiciona pista se houver
        verificarPista(atual->nome, raizPistas);
        
        // se nó-folha, fim do caminho
        if (!atual->esq && !atual->dir) {
            printf("Você chegou ao fim deste caminho (sala-folha).\n");
            break;
        }
        
        printf("Escolha um caminho [e/d], 'p' para ver pistas, ou 's' para sair: ");
        char op = lerOpcao();
        
        if (op == 's') {
            printf("Exploração encerrada pelo jogador.\n");
            break;
        } else if (op == 'p') {
            printf("\n📜 Pistas encontradas (em ordem alfabética):\n");
            if (*raizPistas) {
                listarPistasEmOrdem(*raizPistas);
            } else {
                printf("  Nenhuma pista encontrada ainda.\n");
            }
            printf("\n");
            continue;
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
            printf("Opção inválida. Use 'e', 'd', 'p' ou 's'.\n");
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

    // iniciar BST de pistas
    NoPista *raizPistas = NULL;

    // iniciar exploração
    explorarSalas(hall, &raizPistas);

    // exibir pistas finais
    printf("\n📊 Resumo Final - Todas as pistas encontradas:\n");
    if (raizPistas) {
        listarPistasEmOrdem(raizPistas);
    } else {
        printf("  Nenhuma pista foi encontrada.\n");
    }

    // liberar memória
    liberarSalas(hall);
    liberarPistas(raizPistas);

    return 0;
}