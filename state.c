#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "snake_utils.h"
#include "state.h"

/* Helper function definitions */
static char get_board_at(game_state_t* state, int x, int y);
static void set_board_at(game_state_t* state, int x, int y, char ch);
static bool is_tail(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static int incr_x(char c);
static int incr_y(char c);
static void find_head(game_state_t* state, int snum);
static char next_square(game_state_t* state, int snum);
static void update_tail(game_state_t* state, int snum);
static void update_head(game_state_t* state, int snum);

/* Helper function to get a character from the board (already implemented for you). */
static char get_board_at(game_state_t* state, int x, int y) {
  return state->board[y][x];
}

/* Helper function to set a character on the board (already implemented for you). */
static void set_board_at(game_state_t* state, int x, int y, char ch) {
  state->board[y][x] = ch;
}

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.

  game_state_t* basic;

  basic = malloc(sizeof(game_state_t));
  basic->board = malloc(sizeof(char*)*(10));
  basic->x_size = strlen("##############");
  basic->y_size = 10;
  basic->board[0] = "##############";
  basic->board[1] = "#            #";
  basic->board[2] = "#        *   #";
  basic->board[3] = "#            #";
  basic->board[4] = "#   d>       #";
  basic->board[5] = "#            #";
  basic->board[6] = "#            #";
  basic->board[7] = "#            #";
  basic->board[8] = "#            #";
  basic->board[9] = "##############";

  basic->snakes = malloc(sizeof(snake_t));
  
  basic->snakes[0].head_x = 5;
  basic->snakes[0].head_y = 4;
  basic->snakes[0].tail_x = 4;
  basic->snakes[0].tail_y = 4;
  basic->snakes[0].live = 1;

  basic->num_snakes = 1;

  return basic;

}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  // remember to put newline at end of it
  int rows;
  for (rows = 0; rows < state->y_size; rows += 1) {
    fprintf(fp, state->board[rows]);
    fprintf(fp, "\n");
  }
  return;
}

/* Saves the current state into filename (already implemented for you). */
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */
static bool is_tail(char c) {
  // TODO: Implement this function.
  char snek_set[] = "wasd";
  if (strchr(snek_set, c) == NULL) {
    return false;
  }
  return true;
}

static bool is_snake(char c) {
  // TODO: Implement this function.
  char snek_set[] = "wasd^<>vx";
  if (strchr(snek_set, c) == NULL) {
    return false;
  }
  return true;
}

static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch (c) {
    case '^':
      return 'w';
    case '<':
      return 'a';
    case '>':
      return 'd';
    case 'v':
      return 's';
  }
  return '?';
}

static int incr_x(char c) {
  // TODO: Implement this function.
  switch (c) {
    case 'd':
    case '>':
      return 1;
    case 'a':
    case '<':
      return -1;
  }
  return 0;
}

static int incr_y(char c) {
  // TODO: Implement this function.
  switch (c) {
    case 's':
    case 'v':
      return 1;
    case 'w':
    case '^':
      return -1;
  }
  return 0;
}

/* Task 4.2 */
static char next_square(game_state_t* state, int snum) {
  // TODO: Implement this function.
  // how to deal with out of bounds snake number?
  int x_pos;
  int y_pos;
  char head;
  printf("at next_square:\n");
  x_pos = state->snakes[snum].head_x;
  y_pos = state->snakes[snum].head_y;
  head = state->board[y_pos][x_pos];
  printf("at end next_square\n");
  printf("here is head: %c", head);
  printf("here is next: %c", state->board[y_pos + incr_y(head)][x_pos + incr_x(head)]);
  return state->board[y_pos + incr_y(head)][x_pos + incr_x(head)];
  // head is a pointer; tail is a letter indicating where it is going forward
}

/* Task 4.3 */
static void update_head(game_state_t* state, int snum) {
  // TODO: Implement this function.
  char next;
  int h_x_pos;
  int h_y_pos;
  char head;
  printf("at update_head\n");
  
  h_x_pos = state->snakes[snum].head_x;
  h_y_pos = state->snakes[snum].head_y;
  head = state->board[h_x_pos][h_y_pos];

  next = next_square(state, snum);

  if (next == '#' || is_snake(next)) {
    // set head to be an x and update the snake to be dead
    state->board[h_x_pos][h_y_pos] = 'x';
    state->snakes[snum].live = 0;
    printf("at end update_head\n");
    // state->num_snakes -= 1;
  }
  else if (next == ' ') {
    // change board, board representation, snake values, and tail
    state->board[h_y_pos + incr_y(head)][h_x_pos + incr_x(head)] = head;
    state->snakes[snum].head_x += incr_x(head);
    state->snakes[snum].head_y += incr_y(head);
    printf("at end update_head, going into update_tail\n");
    update_tail(state, snum);
  }
  else if (next == '*') {
    state->board[h_y_pos + incr_y(head)][h_x_pos + incr_x(head)] = head;
    state->snakes[snum].head_x += incr_x(head);
    state->snakes[snum].head_y += incr_y(head);
    printf("at end update_head\n");
  }

}

/* Task 4.4 */
static void update_tail(game_state_t* state, int snum) {
  // TODO: Implement this function.
  
  int t_x_pos;
  int t_y_pos;
  char tail;
  printf("at update_tail\n");
  t_x_pos = state->snakes[snum].tail_x;
  t_y_pos = state->snakes[snum].tail_y;
  tail = state->board[t_x_pos][t_y_pos];

  state->board[t_x_pos][t_y_pos] = ' ';

  state->snakes[snum].tail_x += incr_x(tail);
  state->snakes[snum].tail_y += incr_y(tail);

  state->board[t_x_pos][t_y_pos] = body_to_tail(state->board[t_x_pos][t_y_pos]);

  printf("at end update_tail\n");
  // always shrinks tail if called

  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  int counter;
  int fruit_eaten;
  int total_sneks;

  printf("at update_state\n");
  fruit_eaten = 0;
  
  total_sneks = state->num_snakes;

  for (counter = 0; counter < total_sneks; state += 1) {
    if (next_square(state, counter) == '*') {
      fruit_eaten = 1;
    }
    update_head(state, counter);
    if (fruit_eaten) {
      add_food(state);
    }
  }

  printf("at update_state\n");
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
  game_state_t* basic;
  FILE *fptr;
  int row;
  int counter;
  int size;
  int updated_x;
  char str[2] = "\0";

  basic = malloc(sizeof(game_state_t));
  fptr = fopen(filename, "r");
  if (fptr == NULL) {
    printf("File DNE.");
    exit(1);
  }
  fseek(fptr, 0L, SEEK_END);
  size = ftell(fptr);
  rewind(fptr);

  basic->board = malloc(size);

  updated_x = 0;

  for (counter = 0, row = 0; counter < size && !feof(fptr); counter += 1) {
    str[0] = fgetc(fptr);
    if (str[0] == '\n') {
      row += 1;
      if (updated_x == 0) {
        basic->x_size = counter;
        updated_x = 1;
      }
    } 
    else {
      strcat(basic->board[row], str);
    }
  }
  fclose(fptr);
  basic->y_size = row;
  

  return basic;
}

/* Task 6.1 */
static void find_head(game_state_t* state, int snum) {
  // TODO: Implement this function.
  int curr_x;
  int curr_y;
  char next;
  char curr;

  curr_x = state->snakes[snum].tail_x;
  curr_y = state->snakes[snum].tail_y;
  curr = state->board[curr_y][curr_x];
  next = state->board[curr_y + incr_y(curr)][curr_x + incr_x(curr)];

  while (next != ' ' && next != '#') {
    curr_x += incr_x(next);
    curr_y += incr_y(next);
    curr = state->board[curr_y][curr_x];
    next = state->board[curr_y + incr_y(curr)][curr_x + incr_x(curr)];
  }

  state->snakes[snum].head_x = curr_x;
  state->snakes[snum].head_y = curr_y;

  if (state->board[curr_y][curr_x] == 'x') {
    state->snakes[snum].live = 0;
  }

  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  int row;
  int col;
  char curr;
  state->num_snakes = 0;
  state->snakes = malloc(sizeof(snake_t));

  for (row = 0; row < state->y_size; row += 1) {
    for (col = 0; col < state->x_size; col += 1) {
      curr = state->board[row][col];
      if (is_tail(curr)) {
        state->snakes = realloc(state->snakes, sizeof(state->snakes) + sizeof(snake_t));
        state->snakes[state->num_snakes].tail_x = col;
        state->snakes[state->num_snakes].tail_y = row;
        state->num_snakes += 1;
        state->snakes[state->num_snakes].live = 1;
      }
    }
  }
  return state;
}

