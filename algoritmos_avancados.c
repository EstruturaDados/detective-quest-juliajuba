#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAMANHO_HASH 17  // Tamanho da tabela hash (número primo)

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

// Estrutura para vincular pista a suspeito (encadeamento de colisões)
typedef struct NoHashPista {
    char *pista;
    char *suspeito;
    struct NoHashPista *proximo;
} NoHashPista;

// Tabela hash para relacionar pistas com suspeitos
typedef struct {
    NoHashPista **tabela;
    int tamanho;
} TabelaHash;

// Função hash simples baseada na soma ASCII dos caracteres
unsigned int funcaoHash(const char *chave, int tamanho) {
    unsigned int hash = 0;
    for (int i = 0; chave[i]; ++i) {
        hash += (unsigned char)chave[i];
    }
    return hash % tamanho;
}

// Cria e inicializa a tabela hash
TabelaHash *criarTabelaHash(int tamanho) {
    TabelaHash *h = malloc(sizeof(TabelaHash));
    if (!h) {
        fprintf(stderr, "Erro ao alocar tabela hash\n");
        exit(EXIT_FAILURE);
    }
    h->tabela = malloc(sizeof(NoHashPista *) * tamanho);
    if (!h->tabela) {
        fprintf(stderr, "Erro ao alocar array da tabela hash\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < tamanho; ++i) {
        h->tabela[i] = NULL;
    }
    h->tamanho = tamanho;
    return h;
}

// Insere uma pista com seu suspeito associado na tabela hash
void inserirNaHash(TabelaHash *h, const char *pista, const char *suspeito) {
    if (!h || !pista || !suspeito) return;
    
    unsigned int indice = funcaoHash(pista, h->tamanho);
    
    // Verificar se já existe
    NoHashPista *atual = h->tabela[indice];
    while (atual) {
        if (strcmp(atual->pista, pista) == 0) {
            // Atualizar suspeito se pista já existe
            free(atual->suspeito);
            atual->suspeito = strdup(suspeito);
            return;
        }
        atual = atual->proximo;
    }
    
    // Criar novo nó e inserir no início da lista
    NoHashPista *novoNo = malloc(sizeof(NoHashPista));
    if (!novoNo) {
        fprintf(stderr, "Erro ao alocar nó da hash\n");
        exit(EXIT_FAILURE);
    }
    novoNo->pista = strdup(pista);
    novoNo->suspeito = strdup(suspeito);
    novoNo->proximo = h->tabela[indice];
    h->tabela[indice] = novoNo;
}

// Busca o suspeito associado a uma pista
char *buscarSuspeitoNaHash(TabelaHash *h, const char *pista) {
    if (!h || !pista) return NULL;
    
    unsigned int indice = funcaoHash(pista, h->tamanho);
    NoHashPista *atual = h->tabela[indice];
    
    while (atual) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// Lista todas as associações pista → suspeito
void listarAssociacoesPistaSuspeito(TabelaHash *h) {
    if (!h) return;
    
    printf("\n🔗 Associações Pista → Suspeito:\n");
    int encontrou = 0;
    
    for (int i = 0; i < h->tamanho; ++i) {
        NoHashPista *atual = h->tabela[i];
        while (atual) {
            printf("  📌 '%s' → 🕵️ %s\n", atual->pista, atual->suspeito);
            encontrou = 1;
            atual = atual->proximo;
        }
    }
    
    if (!encontrou) {
        printf("  Nenhuma associação foi feita ainda.\n");
    }
}

// Encontra o suspeito mais citado nas pistas
char *encontrarSuspeitoMaisCitado(TabelaHash *h) {
    if (!h) return NULL;
    
    typedef struct {
        char *suspeito;
        int contagem;
    } ContadorSuspeito;
    
    ContadorSuspeito *contadores = malloc(sizeof(ContadorSuspeito) * h->tamanho);
    if (!contadores) return NULL;
    
    int totalUnicos = 0;
    
    // Contar ocorrências de cada suspeito
    for (int i = 0; i < h->tamanho; ++i) {
        NoHashPista *atual = h->tabela[i];
        while (atual) {
            int encontrou = 0;
            for (int j = 0; j < totalUnicos; ++j) {
                if (strcmp(contadores[j].suspeito, atual->suspeito) == 0) {
                    contadores[j].contagem++;
                    encontrou = 1;
                    break;
                }
            }
            if (!encontrou && totalUnicos < h->tamanho) {
                contadores[totalUnicos].suspeito = strdup(atual->suspeito);
                contadores[totalUnicos].contagem = 1;
                totalUnicos++;
            }
            atual = atual->proximo;
        }
    }
    
    // Encontrar o mais citado
    char *maisCitado = NULL;
    int maxContagem = 0;
    
    for (int i = 0; i < totalUnicos; ++i) {
        if (contadores[i].contagem > maxContagem) {
            maxContagem = contadores[i].contagem;
            maisCitado = contadores[i].suspeito;
        }
    }
    
    // Liberar memória temporária (mantendo maisCitado válido)
    char *resultado = maisCitado ? strdup(maisCitado) : NULL;
    for (int i = 0; i < totalUnicos; ++i) {
        free(contadores[i].suspeito);
    }
    free(contadores);
    
    return resultado;
}

// Libera a tabela hash
void liberarHash(TabelaHash *h) {
    if (!h) return;
    
    for (int i = 0; i < h->tamanho; ++i) {
        NoHashPista *atual = h->tabela[i];
        while (atual) {
            NoHashPista *temp = atual;
            atual = atual->proximo;
            free(temp->pista);
            free(temp->suspeito);
            free(temp);
        }
    }
    free(h->tabela);
    free(h);
}

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

// verifica se a sala tem pista e a adiciona à BST e Hash
void verificarPista(const char *nomeSala, NoPista **raizPistas, TabelaHash *h) {
    if (strcmp(nomeSala, "Biblioteca") == 0) {
        printf("🔍 Você encontrou uma pista: 'Carta Misteriosa'\n");
        *raizPistas = inserirPista(*raizPistas, "Carta Misteriosa");
        inserirNaHash(h, "Carta Misteriosa", "Dr. Watson");
    } else if (strcmp(nomeSala, "Escritório") == 0) {
        printf("🔍 Você encontrou uma pista: 'Chave Enferrujada'\n");
        *raizPistas = inserirPista(*raizPistas, "Chave Enferrujada");
        inserirNaHash(h, "Chave Enferrujada", "Professora Moriarty");
    } else if (strcmp(nomeSala, "Cozinha") == 0) {
        printf("🔍 Você encontrou uma pista: 'Faca de Manteiga'\n");
        *raizPistas = inserirPista(*raizPistas, "Faca de Manteiga");
        inserirNaHash(h, "Faca de Manteiga", "Professora Moriarty");
    } else if (strcmp(nomeSala, "Corredor") == 0) {
        printf("🔍 Você encontrou uma pista: 'Pegadas Estranhas'\n");
        *raizPistas = inserirPista(*raizPistas, "Pegadas Estranhas");
        inserirNaHash(h, "Pegadas Estranhas", "Jardineiro Gregson");
    } else if (strcmp(nomeSala, "Quarto") == 0) {
        printf("🔍 Você encontrou uma pista: 'Diário Secreto'\n");
        *raizPistas = inserirPista(*raizPistas, "Diário Secreto");
        inserirNaHash(h, "Diário Secreto", "Dr. Watson");
    }
}

// explora a mansão interativamente a partir de uma sala
void explorarSalas(Sala *raiz, NoPista **raizPistas, TabelaHash *h) {
    Sala *atual = raiz;
    printf("Exploração iniciada. Digite 'e' para esquerda, 'd' para direita, 'p' para ver pistas, 's' para sair.\n\n");
    
    while (atual) {
        printf("Você está em: %s\n", atual->nome);
        
        verificarPista(atual->nome, raizPistas, h);
        
        if (!atual->esq && !atual->dir) {
            printf("Você chegou ao fim deste caminho (sala-folha).\n");
            break;
        }
        
        printf("Escolha um caminho [e/d], 'p' para ver pistas, 'a' para ver associações, ou 's' para sair: ");
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
        } else if (op == 'a') {
            listarAssociacoesPistaSuspeito(h);
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
            printf("Opção inválida. Use 'e', 'd', 'p', 'a' ou 's'.\n");
        }
        printf("\n");
    }
}

int main(void) {
    Sala *escritorio = criarSala("Escritório", NULL, NULL);
    Sala *jardim = criarSala("Jardim Interno", NULL, NULL);
    Sala *biblioteca = criarSala("Biblioteca", escritorio, jardim);

    Sala *cozinha = criarSala("Cozinha", NULL, NULL);
    Sala *quarto = criarSala("Quarto", NULL, NULL);
    Sala *banheiro = criarSala("Banheiro", NULL, NULL);
    Sala *corredor = criarSala("Corredor", quarto, banheiro);
    Sala *salaJantar = criarSala("Sala de Jantar", cozinha, corredor);

    Sala *hall = criarSala("Hall de Entrada", biblioteca, salaJantar);

    NoPista *raizPistas = NULL;
    TabelaHash *tabelaSuspeitos = criarTabelaHash(TAMANHO_HASH);

    explorarSalas(hall, &raizPistas, tabelaSuspeitos);

    printf("\n📊 RESUMO FINAL DA INVESTIGAÇÃO\n");
    printf("═════════════════════════════════════════\n");
    
    printf("\n📜 Pistas encontradas:\n");
    if (raizPistas) {
        listarPistasEmOrdem(raizPistas);
    } else {
        printf("  Nenhuma pista foi encontrada.\n");
    }

    listarAssociacoesPistaSuspeito(tabelaSuspeitos);

    char *culpado = encontrarSuspeitoMaisCitado(tabelaSuspeitos);
    printf("\n🕵️ SUSPEITO PRINCIPAL (mais associado às pistas):\n");
    if (culpado) {
        printf("  >>> %s <<<\n", culpado);
        free(culpado);
    } else {
        printf("  Impossível determinar (sem dados suficientes).\n");
    }

    printf("\n═════════════════════════════════════════\n");

    liberarSalas(hall);
    liberarPistas(raizPistas);
    liberarHash(tabelaSuspeitos);

    return 0;
}