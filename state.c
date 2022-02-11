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
static void show_board(game_state_t* state);

/* Helper function to get a character from the board (already implemented for you). */
static char get_board_at(game_state_t* state, int x, int y) {
  return state->board[y][x];
}

/* Helper function to set a character on the board (already implemented for you). */
static void set_board_at(game_state_t* state, int x, int y, char ch) {
  state->board[y][x] = ch;
}

static void show_board(game_state_t* state) {
  int rows;
  for (rows = 0; rows < state->y_size; rows += 1) {
    printf("%s", state->board[rows]);
  }
  return;
}

/* Task 1 */
game_state_t* create_default_state() {
  game_state_t* basic;
  char* basic_state;
  int counter;
  int start_index;

  basic = malloc(sizeof(game_state_t));
  basic->x_size = 14;
  basic->y_size = 10;

  basic_state =
    "##############\n"
    "#            #\n"
    "#        *   #\n"
    "#            #\n"
    "#   d>       #\n"
    "#            #\n"
    "#            #\n"
    "#            #\n"
    "#            #\n"
    "##############\n";

  basic->board = malloc(sizeof(char*)*basic->y_size);
  for (counter = 0; counter < basic->y_size; counter += 1) {
    basic->board[counter] = malloc(sizeof(char)*basic->x_size + 2);
    start_index = counter*(basic->x_size + 1);
    strncpy(basic->board[counter], &basic_state[start_index], basic->x_size + 1);
    basic->board[counter][basic->x_size + 1] = '\0';
  }
  basic->snakes = malloc(sizeof(snake_t*));
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
  int counter;
  for (counter = 0; counter < state->y_size; counter += 1) {
    free(state->board[counter]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  int rows;
  for (rows = 0; rows < state->y_size; rows += 1) {
    fprintf(fp, "%s", state->board[rows]);
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
  char snek_set[] = "wasd";
  if (strchr(snek_set, c) == NULL) {
    return false;
  }
  return true;
}

static bool is_snake(char c) {
  char snek_set[] = "wasd^<>vx";
  if (strchr(snek_set, c) == NULL) {
    return false;
  }
  return true;
}

static char body_to_tail(char c) {
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
  int x_pos;
  int y_pos;
  char head;
  x_pos = state->snakes[snum].head_x;
  y_pos = state->snakes[snum].head_y;
  head = get_board_at(state, x_pos, y_pos);
  return get_board_at(state, x_pos + incr_x(head), y_pos + incr_y(head));
}

/* Task 4.3 */
static void update_head(game_state_t* state, int snum) {
  char next;
  int h_x_pos;
  int h_y_pos;
  char head;
  
  h_x_pos = state->snakes[snum].head_x;
  h_y_pos = state->snakes[snum].head_y;
  head = get_board_at(state, h_x_pos, h_y_pos);
  next = next_square(state, snum);

  if (next == '#' || is_snake(next)) {
    set_board_at(state, h_x_pos, h_y_pos, 'x');
    state->snakes[snum].live = 0;
  }
  else if (next == ' ') {
    set_board_at(state, h_x_pos + incr_x(head), h_y_pos + incr_y(head), head);
    state->snakes[snum].head_x += incr_x(head);
    state->snakes[snum].head_y += incr_y(head);
  }
  else if (next == '*') {
    set_board_at(state, h_x_pos + incr_x(head), h_y_pos + incr_y(head), head);
    state->snakes[snum].head_x += incr_x(head);
    state->snakes[snum].head_y += incr_y(head);
  }

}

/* Task 4.4 */
static void update_tail(game_state_t* state, int snum) {
  int t_x_pos;
  int t_y_pos;
  char tail;
  char next; 
  t_x_pos = state->snakes[snum].tail_x;
  t_y_pos = state->snakes[snum].tail_y;
  tail = get_board_at(state, t_x_pos, t_y_pos);
  set_board_at(state, t_x_pos, t_y_pos, ' ');

  state->snakes[snum].tail_x += incr_x(tail);
  state->snakes[snum].tail_y += incr_y(tail);
  t_x_pos = state->snakes[snum].tail_x;
  t_y_pos = state->snakes[snum].tail_y;
  next = get_board_at(state, t_x_pos, t_y_pos);
  set_board_at(state, t_x_pos, t_y_pos, body_to_tail(next));
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  int counter;
  int fruit_eaten;
  int total_sneks;

  fruit_eaten = 0;
  total_sneks = state->num_snakes;
  for (counter = 0; counter < total_sneks; counter += 1) {
    if (next_square(state, counter) == '*') {
      fruit_eaten = 1;
    }
    update_head(state, counter);
    if (fruit_eaten == 0 && state->snakes[counter].live) {
      update_tail(state, counter);
    } 
    else if (state->snakes[counter].live) {
      fruit_eaten = 0;
      add_food(state);
    }
  }
}


game_state_t* load_board(char* filename) {
  game_state_t* basic;
  FILE *fptr;
  int row;
  int counter;
  int row_size;
  int new_row;
  char str[2] = {'\0', '\0'};

  basic = malloc(sizeof(game_state_t));
  fptr = fopen(filename, "r");

  if (fptr == NULL) {
    printf("File DNE.\n");
    exit(1);
  }
  
  rewind(fptr);

  basic->board = malloc(sizeof(char*));
  basic->board[0] = malloc(sizeof(char)*2);

  for (counter = 0, row = 0; feof(fptr) == 0 && row == 0; counter += 1) {
    str[0] = fgetc(fptr);
    basic->board[row][counter] = str[0];
    if (str[0] == '\n') {
      basic->board[row][counter + 1] = '\0';
      row += 1;
      basic->x_size = counter;
    }   
  }
  
  row_size = basic->x_size + 1;
  basic->board[row] = malloc(row_size * sizeof(char));

  basic->board = realloc(basic->board, sizeof(char*)*(row + 1));
  new_row = 0;

  for (; feof(fptr) == 0; counter += 1) {
    if (new_row) {
      basic->board = realloc(basic->board, sizeof(char*)*(row + 1));
      basic->board[row] = malloc(row_size*sizeof(char));
      new_row = 0;
    }

    str[0] = fgetc(fptr);
    strcat(basic->board[row], str);
    if (str[0] == '\n') {
      basic->board[row][basic->x_size + 1] = '\0';
      row += 1;
      new_row += 1;
    } 
  }

  basic->y_size = row; 
  fclose(fptr);

  return basic;
}


/* Task 6.1 */
static void find_head(game_state_t* state, int snum) {
  int curr_x;
  int curr_y;
  char next;
  char curr;
  
  curr_x = state->snakes[snum].tail_x;
  curr_y = state->snakes[snum].tail_y;
  curr = get_board_at(state, curr_x,  curr_y);
  next = get_board_at(state, curr_x + incr_x(curr), curr_y + incr_y(curr));

  while (is_snake(next)) {
    curr_x += incr_x(curr);
    curr_y += incr_y(curr);
    curr = next;
    next = get_board_at(state, curr_x + incr_x(curr), curr_y + incr_y(curr));
  }

  state->snakes[snum].head_x = curr_x;
  state->snakes[snum].head_y = curr_y;

  if (get_board_at(state, curr_x, curr_y) == 'x') {
    state->snakes[snum].live = 0;
  }
  else {
    state->snakes[snum].live = 1;
  }

  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  int row;
  int col;
  char curr;

  state->num_snakes = 0;
  state->snakes = malloc(sizeof(snake_t)*(state->num_snakes));

  for (row = 0; row < state->y_size; row += 1) {
    for (col = 0; col < state->x_size; col += 1) {
      curr = get_board_at(state, col, row);
      if (is_tail(curr)) {
        state->snakes = realloc(state->snakes, sizeof(snake_t*)*(state->num_snakes + 1));
        state->snakes[state->num_snakes].tail_x = col;
        state->snakes[state->num_snakes].tail_y = row;
        find_head(state, state->num_snakes);
        state->num_snakes += 1;
      }
    }
  }

  return state;
}

