#include "funcoes.h"

User *listaUsuarios = NULL;
Post *listaPosts = NULL;
User *usuarioAtual = NULL;

void registro() {
    User *novoUsuario = (User*)malloc(sizeof(User));
    if (!novoUsuario) {
        printf("Erro ao alocar memoria.\n");
        return;
    }

    printf("\n--- Registro ---\n");
    printf("Nome de usuario: ");
    scanf("%49s", novoUsuario->usuario);
    limparEntrada();

    User *temp = listaUsuarios;
    while (temp != NULL) {
        if (strcmp(temp->usuario, novoUsuario->usuario) == 0) {
            printf("Nome de usuario ja existe!\n");
            free(novoUsuario);
            return;
        }
        temp = temp->next;
    }

    printf("Senha: ");
    scanf("%49s", novoUsuario->senha);
    limparEntrada();

    novoUsuario->next = listaUsuarios;
    listaUsuarios = novoUsuario;

    salvarUsersArquivo();
    printf("Registro realizado com sucesso!\n");
}

User* login() {
    char usuario[MAX_USUARIO];
    char senha[MAX_SENHA];

    printf("\n--- Login ---\n");
    printf("Nome de usuario: ");
    scanf("%49s", usuario);
    limparEntrada();

    printf("Senha: ");
    scanf("%49s", senha);
    limparEntrada();

    User *temp = listaUsuarios;
    while (temp != NULL) {
        if (strcmp(temp->usuario, usuario) == 0 &&
            strcmp(temp->senha, senha) == 0) {
            printf("Login bem-sucedido! Bem-vindo, %s!\n", usuario);
            return temp;
        }
        temp = temp->next;
    }

    printf("Nome de usuario ou senha incorretos.\n");
    return NULL;
}

void criarPost() {
    if (usuarioAtual == NULL) {
        printf("Voce precisa estar logado para postar.\n");
        return;
    }

    Post *novoPost = (Post*)malloc(sizeof(Post));
    if (!novoPost) {
        printf("Erro ao alocar memoria.\n");
        return;
    }

    printf("\n--- Novo Post ---\n");
    printf("Digite sua mensagem (max %d caracteres):\n", MAX_POST);

    fgets(novoPost->conteudo, MAX_POST, stdin);
    novoPost->conteudo[strcspn(novoPost->conteudo, "\n")] = '\0';

    strcpy(novoPost->usuario, usuarioAtual->usuario);
    novoPost->timestamp = time(NULL);
    novoPost->next = listaPosts;
    listaPosts = novoPost;

    salvarPostsArquivo();
    printf("Post publicado com sucesso!\n");
}

void verPosts() {
    printf("\n--- Feed ---\n");

    if (listaPosts == NULL) {
        printf("Nenhum post disponivel.\n");
        return;
    }

    Post *temp = listaPosts;
    while (temp != NULL) {
        printf("\n@%s - %s\n", temp->usuario, formatTime(temp->timestamp));
        printf("%s\n", temp->conteudo);
        printf("-------------------\n");
        temp = temp->next;
    }
}

void salvarUsersArquivo() {
    FILE *file = fopen(USERS_FILE, "w");
    if (!file) {
        printf("Erro ao abrir arquivo de usuarios.\n");
        return;
    }

    User *temp = listaUsuarios;
    while (temp != NULL) {
        fprintf(file, "%s;%s\n", temp->usuario, temp->senha);
        temp = temp->next;
    }

    fclose(file);
}

void carregarUsuariosArquivo() {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) return;

    char linha[200];
    while (fgets(linha, sizeof(linha), file)) {
        User *novoUsuario = (User*)malloc(sizeof(User));
        if (!novoUsuario) {
            printf("Erro ao alocar memoria.\n");
            fclose(file);
            return;
        }

        sscanf(linha, "%49[^;];%49s", novoUsuario->usuario, novoUsuario->senha);
        novoUsuario->next = listaUsuarios;
        listaUsuarios = novoUsuario;
    }

    fclose(file);
}

void salvarPostsArquivo() {
    FILE *file = fopen(POSTS_FILE, "w");
    if (!file) {
        printf("Erro ao abrir arquivo de posts.\n");
        return;
    }

    Post *temp = listaPosts;
    while (temp != NULL) {
        fprintf(file, "%s;%ld;%s\n", temp->usuario, temp->timestamp, temp->conteudo);
        temp = temp->next;
    }

    fclose(file);
}

void carregarPostsArquivo() {
    FILE *file = fopen(POSTS_FILE, "r");
    if (!file) return;

    char linha[400];
    char conteudoTemp[MAX_POST];
    while (fgets(linha, sizeof(linha), file)) {
        Post *novoPost = (Post*)malloc(sizeof(Post));
        if (!novoPost) {
            printf("Erro ao alocar memoria.\n");
            fclose(file);
            return;
        }

        sscanf(linha, "%49[^;];%ld;%[^\n]", novoPost->usuario, &novoPost->timestamp, conteudoTemp);
        strncpy(novoPost->conteudo, conteudoTemp, MAX_POST);
        novoPost->conteudo[MAX_POST - 1] = '\0';
        novoPost->next = listaPosts;
        listaPosts = novoPost;
    }

    fclose(file);
}

char* formatTime(time_t timestamp) {
    static char buffer[80];
    struct tm *timeinfo = localtime(&timestamp);
    strftime(buffer, 80, "%d/%m/%Y %H:%M:%S", timeinfo);
    return buffer;
}

void limparEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void liberarMemoria() {
    User *userTemp;
    while (listaUsuarios != NULL) {
        userTemp = listaUsuarios;
        listaUsuarios = listaUsuarios->next;
        free(userTemp);
    }

    Post *postTemp;
    while (listaPosts != NULL) {
        postTemp = listaPosts;
        listaPosts = listaPosts->next;
        free(postTemp);
    }
}

void exibirMenu() {
    printf("\n--- cmdwitter ---\n");
    if (usuarioAtual) {
        printf("Usuario: @%s\n", usuarioAtual->usuario);
        printf("1. Criar post\n");
        printf("2. Ver feed\n");
        printf("3. Logout\n");
    } else {
        printf("1. Login\n");
        printf("2. Registrar\n");
    }
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
}