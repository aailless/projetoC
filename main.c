#include "funcoes.h"

int main() {
    carregarUsuariosArquivo();
    carregarPostsArquivo();

    int opcao;
    do {
        exibirMenu();
        scanf("%d", &opcao);
        limparEntrada();

        if (usuarioAtual) {
            switch (opcao) {
                case 1: criarPost(); break;
                case 2: verPosts(); break;
                case 3:
                    usuarioAtual = NULL;
                    printf("Logout realizado.\n");
                    break;
                case 0: printf("Saindo...\n"); break;
                default: printf("Opcao invalida.\n");
            }
        } else {
            switch (opcao) {
                case 1: usuarioAtual = login(); break;
                case 2: registro(); break;
                case 0: printf("Saindo...\n"); break;
                default: printf("Opcao invalida.\n");
            }
        }
    } while (opcao != 0);

    salvarUsersArquivo();
    salvarPostsArquivo();
    liberarMemoria();

    return 0;
}