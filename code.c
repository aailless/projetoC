#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_POST 280
#define USERS_FILE "usuarios.txt"
#define POSTS_FILE "posts.txt"

typedef struct User {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    struct User *next;
} User;

typedef struct Post {
    char username[MAX_USERNAME];
    char content[MAX_POST];
    time_t timestamp;
    struct Post *next;
} Post;

User *userList = NULL;
Post *postList = NULL;
User *currentUser = NULL;

void registerUser();
User* login();
void createPost();
void viewAllPosts();
void saveUsersToFile();
void loadUsersFromFile();
void savePostsToFile();
void loadPostsFromFile();
char* formatTime(time_t timestamp);
void clearInputBuffer();
void freeMemory();
void displayMenu();

int main() {
    loadUsersFromFile();
    loadPostsFromFile();

    int option;
    do {
        displayMenu();
        scanf("%d", &option);
        clearInputBuffer();

        if (currentUser) {
            switch (option) {
                case 1: createPost(); break;
                case 2: viewAllPosts(); break;
                case 3:
                    currentUser = NULL;
                    printf("Logout realizado.\n");
                    break;
                case 0: printf("Saindo...\n"); break;
                default: printf("Opcao invalida.\n");
            }
        } else {
            switch (option) {
                case 1: currentUser = login(); break;
                case 2: registerUser(); break;
                case 0: printf("Saindo...\n"); break;
                default: printf("Opcao invalida.\n");
            }
        }
    } while (option != 0);

    saveUsersToFile();
    savePostsToFile();
    freeMemory();

    return 0;
}

void registerUser() {
    User *newUser = (User*)malloc(sizeof(User));
    if (!newUser) {
        printf("Erro ao alocar memoria.\n");
        return;
    }

    printf("\n--- Registro ---\n");
    printf("Nome de usuario: ");
    scanf("%49s", newUser->username);
    clearInputBuffer();

    User *temp = userList;
    while (temp != NULL) {
        if (strcmp(temp->username, newUser->username) == 0) {
            printf("Nome de usuario ja existe!\n");
            free(newUser);
            return;
        }
        temp = temp->next;
    }

    printf("Senha: ");
    scanf("%49s", newUser->password);
    clearInputBuffer();

    newUser->next = userList;
    userList = newUser;

    saveUsersToFile();
    printf("Registro realizado com sucesso!\n");
}

User* login() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("\n--- Login ---\n");
    printf("Nome de usuario: ");
    scanf("%49s", username);
    clearInputBuffer();

    printf("Senha: ");
    scanf("%49s", password);
    clearInputBuffer();

    User *temp = userList;
    while (temp != NULL) {
        if (strcmp(temp->username, username) == 0 &&
            strcmp(temp->password, password) == 0) {
            printf("Login bem-sucedido! Bem-vindo, %s!\n", username);
            return temp;
        }
        temp = temp->next;
    }

    printf("Nome de usuario ou senha incorretos.\n");
    return NULL;
}

void createPost() {
    if (currentUser == NULL) {
        printf("Voce precisa estar logado para postar.\n");
        return;
    }

    Post *newPost = (Post*)malloc(sizeof(Post));
    if (!newPost) {
        printf("Erro ao alocar memoria.\n");
        return;
    }

    printf("\n--- Novo Post ---\n");
    printf("Digite sua mensagem (max %d caracteres):\n", MAX_POST);

    fgets(newPost->content, MAX_POST, stdin);
    newPost->content[strcspn(newPost->content, "\n")] = '\0';

    strcpy(newPost->username, currentUser->username);
    newPost->timestamp = time(NULL);
    newPost->next = postList;
    postList = newPost;

    savePostsToFile();
    printf("Post publicado com sucesso!\n");
}

void viewAllPosts() {
    printf("\n--- Feed ---\n");

    if (postList == NULL) {
        printf("Nenhum post disponivel.\n");
        return;
    }

    Post *temp = postList;
    while (temp != NULL) {
        printf("\n@%s - %s\n", temp->username, formatTime(temp->timestamp));
        printf("%s\n", temp->content);
        printf("-------------------\n");
        temp = temp->next;
    }
}

void saveUsersToFile() {
    FILE *file = fopen(USERS_FILE, "w");
    if (!file) {
        printf("Erro ao abrir arquivo de usuarios.\n");
        return;
    }

    User *temp = userList;
    while (temp != NULL) {
        fprintf(file, "%s;%s\n", temp->username, temp->password);
        temp = temp->next;
    }

    fclose(file);
}

void loadUsersFromFile() {
    FILE *file = fopen(USERS_FILE, "r");
    if (!file) return;

    char linha[200];
    while (fgets(linha, sizeof(linha), file)) {
        User *newUser = (User*)malloc(sizeof(User));
        if (!newUser) {
            printf("Erro ao alocar memoria.\n");
            fclose(file);
            return;
        }

        sscanf(linha, "%49[^;];%49s", newUser->username, newUser->password);
        newUser->next = userList;
        userList = newUser;
    }

    fclose(file);
}

void savePostsToFile() {
    FILE *file = fopen(POSTS_FILE, "w");
    if (!file) {
        printf("Erro ao abrir arquivo de posts.\n");
        return;
    }

    Post *temp = postList;
    while (temp != NULL) {
        fprintf(file, "%s;%ld;%s\n", temp->username, temp->timestamp, temp->content);
        temp = temp->next;
    }

    fclose(file);
}

void loadPostsFromFile() {
    FILE *file = fopen(POSTS_FILE, "r");
    if (!file) return;

    char linha[400];
    // Adicione esta linha para declarar conteudoTemp
    char conteudoTemp[MAX_POST]; // ou um tamanho um pouco maior se houver risco de ler além de MAX_POST + 1 para o nulo
                                 // considerando MAX_POST 280, 281 seria seguro para o '\0'
    while (fgets(linha, sizeof(linha), file)) {
        Post *newPost = (Post*)malloc(sizeof(Post));
        if (!newPost) {
            printf("Erro ao alocar memoria.\n");
            fclose(file);
            return;
        }

        // Linha 250 (agora 253 no código original, mas o sscanf é o que nos importa)
        sscanf(linha, "%49[^;];%ld;%[^\n]", newPost->username, &newPost->timestamp, conteudoTemp);
        // Linha 251 (agora 254 no código original)
        strncpy(newPost->content, conteudoTemp, MAX_POST);
        newPost->content[MAX_POST - 1] = '\0'; // Garante terminação nula para segurança
        newPost->next = postList;
        postList = newPost;
    }

    fclose(file);
}

char* formatTime(time_t timestamp) {
    static char buffer[80];
    struct tm *timeinfo = localtime(&timestamp);
    strftime(buffer, 80, "%d/%m/%Y %H:%M:%S", timeinfo);
    return buffer;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void freeMemory() {
    User *userTemp;
    while (userList != NULL) {
        userTemp = userList;
        userList = userList->next;
        free(userTemp);
    }

    Post *postTemp;
    while (postList != NULL) {
        postTemp = postList;
        postList = postList->next;
        free(postTemp);
    }
}

void displayMenu() {
    printf("\n--- Rede Social ---\n");
    if (currentUser) {
        printf("Usuario: @%s\n", currentUser->username);
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
