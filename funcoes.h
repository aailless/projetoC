#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USUARIO 50
#define MAX_SENHA 50
#define MAX_POST 280
#define USERS_FILE "usuarios.txt"
#define POSTS_FILE "posts.txt"

typedef struct User {
    char usuario[MAX_USUARIO];
    char senha[MAX_SENHA];
    struct User *next;
} User;

typedef struct Post {
    char usuario[MAX_USUARIO];
    char conteudo[MAX_POST];
    time_t timestamp;
    struct Post *next;
} Post;

extern User *listaUsuarios;
extern Post *listaPosts;
extern User *usuarioAtual;

void registro();
User* login();
void criarPost();
void verPosts();
void salvarUsersArquivo();
void carregarUsuariosArquivo();
void salvarPostsArquivo();
void carregarPostsArquivo();
char* formatTime(time_t timestamp);
void limparEntrada();
void liberarMemoria();
void exibirMenu();

#endif