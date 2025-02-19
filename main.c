#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <ncurses.h>
#include <signal.h>

static void finish(int sig);
static void save_text(const char *filename, char *buffer, int size);
static void save(char *filename, char *buffer);
static void toggle_window(int width, int height);
static void save_as_window();
static void read_file(const char *filename);

WINDOW *win = NULL; // Variável global para rastrear a janela
char buffer[1024] = {0}; // Buffer global para armazenar o texto

int main(int argc, char *argv[])
{
    int num = 0;
    int x = 0, y = 0;        // Coordenadas do cursor
    int buffer_index = 0;

    // Tratamento de sinal para finalizar o programa corretamente
    struct sigaction sa;
    sa.sa_handler = finish;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    initscr();            // Inicializa a biblioteca ncurses
    keypad(stdscr, TRUE); // Habilita mapeamento de teclado
    nonl();               // Desabilita NL -> CR/NL na saída
    cbreak();             // Modo de leitura de caractere por caractere
    noecho();             // Não ecoa a entrada

    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        attrset(COLOR_PAIR(1));
    }

    // Barra com os atalhos de teclado
    mvprintw(LINES - 1, 0, "CTRL + X -> Save | CTRL + D -> Toggle Window | CTRL + O -> Open file | CTRL + C -> Exit");

    move(y, x); // Move o cursor para a posição inicial

    for (;;)
    {
        int c = getch(); // Aceita um único caractere de entrada
        num++;

        switch (c)
        {
        case KEY_LEFT:
            if (x > 0)
            {
                x--;
            }
            break;
        case KEY_RIGHT:
            if (x < COLS - 1)
            {
                x++;
            }
            break;
        case KEY_UP:
            if (y > 0)
            {
                y--;
            }
            break;
        case KEY_DOWN:
            if (y < LINES - 1)
            {
                y++;
            }
            break;
        case KEY_BACKSPACE:
        case 127: // Algumas configurações de teclado usam 127 para o backspace
            if (x > 0)
            {
                mvaddch(y, --x, ' ');
                move(y, x);
                if (buffer_index > 0)
                {
                    buffer[--buffer_index] = '\0';
                }
            }
            break;
        case '\n': // Quebra de linha ao pressionar Enter
        case '\r':
            buffer[buffer_index++] = '\n';
            x = 0;
            if (y < LINES - 1)
            {
                y++;
            }
            break;
        case 24: // Ctrl+X para abrir a janela de salvar como
            save_as_window();
            break;
        case 15: // Ctrl+O para abrir arquivo
        {
          char filename[256] = {0};
          echo();
          mvprintw(LINES - 2, 0, "Open:");
          getnstr(filename, 255);
          noecho();

          read_file(filename);

          clear();
          mvprintw(LINES - 1, 0, "CTRL + X -> Save | CTRL + D -> Toggle Window | CTRL + O -> Open file | CTRL + C -> Exit");
          buffer_index = strlen(buffer);
          y = buffer_index / COLS;
          x = buffer_index % COLS;
          mvprintw(0, 0, "%s", buffer);
        }
        case 4: // Ctrl+D para alternar a janela
            toggle_window(40, 10);
            break;
        default:
            addch(c);
            buffer[buffer_index++] = c;
            x++;
            break;
        }
        move(y, x);
        refresh();
    }

    finish(0); // Finaliza o programa corretamente
}

static void finish(int sig)
{
    endwin();
    // printf("Finalizando o programa...\n");
    exit(0);
}

static void read_file(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (file)
  {
    size_t length = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[length] = '\0';
    fclose(file);
  }
  else
  {
    mvprintw(LINES - 2, 0, "Error on open the file %s\n", filename);
  }
}

// Criado por IA
static void save_text(const char *filename, char *buffer, int size)
{
    FILE *file = fopen(filename, "w");
    if (file)
    {
        size_t written = fwrite(buffer, 1, size, file);
        if (written == size)
        {
            printf("Texto salvo em %s\n", filename);
        }
        else
        {
            printf("Erro ao escrever no arquivo %s\n", filename);
        }
        fclose(file);
    }
    else
    {
        printf("Erro ao abrir o arquivo %s para escrita\n", filename);
    }
}

// Eu que fiz
static void save(char *filename, char *buffer)
{
    const char *name = filename;
    const char *content = buffer;

    int fd = open(name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    assert(fd != -1);
    int rd = write(fd, content, strlen(content));
    assert(rd == strlen(content));
    close(fd);
}

static void toggle_window(int width, int height)
{
    if (win == NULL)
    {
        int starty = (LINES - height) / 2; // Calcula a posição vertical da janela
        int startx = (COLS - width) / 2;   // Calcula a posição horizontal da janela
        win = newwin(height, width, starty, startx);
        box(win, 0, 0); // Adiciona uma borda à janela
        mvwprintw(win, 1, 1, "Janela Centralizada");
        wrefresh(win);
    }
    else
    {
        werase(win); // Limpa a janela
        wrefresh(win);
        delwin(win); // Deleta a janela
        win = NULL;  // Define a janela como NULL
        clear();     // Limpa a tela
        // mvprintw(LINES - 1, 0, "CTRL + X -> Save | CTRL + D -> Toggle Window | CTRL + C -> Exit"); // Redesenha a barra de atalhos
        mvprintw(LINES - 1, 0, "CTRL + X -> Save | CTRL + D -> Toggle Window | CTRL + O -> Open file | CTRL + C -> Exit");
        refresh();   // Atualiza a tela
    }
}

static void save_as_window()
{
    int width = 40;
    int height = 5;
    int starty = (LINES - height) / 2; // Calcula a posição vertical da janela
    int startx = (COLS - width) / 2;   // Calcula a posição horizontal da janela

    WINDOW *save_win = newwin(height, width, starty, startx);
    box(save_win, 0, 0); // Adiciona uma borda à janela
    mvwprintw(save_win, 1, 1, "Nome do Arquivo:");
    wrefresh(save_win);

    char filename[256] = {0};
    echo(); // Habilita o eco para permitir a entrada do usuário
    mvwgetnstr(save_win, 2, 1, filename, 255); // Lê o nome do arquivo do usuário
    noecho(); // Desabilita o eco novamente

    save(filename, buffer); // Salva o texto no arquivo com o nome fornecido
    werase(save_win); // Limpa a janela
    wrefresh(save_win);
    delwin(save_win); // Deleta a janela

    // Redesenha a barra de atalhos
    // mvprintw(LINES - 1, 0, "CTRL + X -> Save | CTRL + D -> Toggle Window | CTRL + C -> Exit");
    mvprintw(LINES - 1, 0, "CTRL + X -> Save | CTRL + D -> Toggle Window | CTRL + O -> Open file | CTRL + C -> Exit");
    refresh();
}
