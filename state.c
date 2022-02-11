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
  char* basic_state;
  int counter;
  int start_index;

  basic = malloc(sizeof(game_state_t));
  basic->x_size = strlen("##############");
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
  // STORED AS STRING LITERAL, not an actual array that can be modified
  for (counter = 0; counter < basic->y_size; counter += 1) {
    basic->board[counter] = malloc(sizeof(char)*basic->x_size + 2);
    start_index = counter*(basic->x_size + 1);
    strncpy(basic->board[counter], &basic_state[start_index], basic->x_size + 1);
  }


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
  int rows;
  for (rows = 0; rows < state->y_size; rows += 1) {
    fprintf(fp, "%s", state->board[rows]);
  }
  fprintf(fp, "\n");
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
  // TODO: Implement this function.
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
  // TODO: Implement this function.
  int t_x_pos;
  int t_y_pos;
  char tail;
  char next; 
  int rows;
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
  // TODO: Implement this function.
  int counter;
  int fruit_eaten;
  int total_sneks;
  int rows;

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

/* Task 5 */
/*
game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
  game_state_t* state;
  FILE *fptr;
  int num_cols;
  int num_rows;
  int start_of_row;
  char str[] = {'\0', '\0'};

  state = malloc(sizeof(game_state_t));
  fptr = fopen(filename, "r");

  // go row 0
  state->board = malloc(sizeof(char*));
  state->board[num_rows] = malloc(sizeof(char)*2);
  
  for (num_cols = 0, num_rows = 0; num_cols < 1 && feof(fptr) == 0; num_cols += 1) {
    str[0] = fgetc(fptr);
    strcat(state->board[num_rows], str[0]);
    state->board[num_rows] = realloc(sizeof(state->board[num_rows]) + sizeof(char));
    if (str[0] == '\n') {
      num_rows += 1;
    }



  }


  return state;

}
*/


game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
  game_state_t* basic;
  FILE *fptr;
  int row;
  int counter;
  int size;
  int iterator;
  int row_size;
  int new_row;
  char str[2] = {'\0', '\0'};

  basic = malloc(sizeof(game_state_t));
  fptr = fopen(filename, "r");

  if (fptr == NULL) {
    printf("File DNE.\n");
    exit(1);
  }
  
  /* fseek(fptr, 0L, SEEK_END);
  size = ftell(fptr);
  rewind(fptr); */

  basic->board = malloc(sizeof(char*));
  basic->board[0] = malloc(sizeof(char)*2);

  for (counter = 0, row = 0; feof(fptr) == 0 && row == 0; counter += 1) {
    str[0] = fgetc(fptr);
    //printf("Char: %c, counter: %d\n", str[0], counter);
    //printf("size so far: %ld\n", sizeof(basic->board[0]));
    //basic->board[row] = realloc(basic->board[row], sizeof(basic->board[row]) + sizeof(char));
    basic->board[row][counter] = str[0];
    //strcat(basic->board[row], str);
    if (str[0] == '\n') {
      // basic->board[row] = realloc(basic->board[row], sizeof(basic->board[row]) + sizeof(char));
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
      //printf("ddddd before malloc row 0: %s\n", basic->board[0]);
      basic->board = realloc(basic->board, sizeof(char*)*(row + 1));
      basic->board[row] = malloc(row_size*sizeof(char));
      //printf("ddddd after malloc row 0: %s\n", basic->board[0]);
      new_row = 0;
    }

    str[0] = fgetc(fptr);
    strcat(basic->board[row], str);
    if (str[0] == '\n') {
      //printf("row #: %d, %s\n", row, basic->board[row]);
      //basic->board[row] = realloc(basic->board[row], sizeof(basic->board[row]) + sizeof(char));
      basic->board[row][basic->x_size + 1] = '\0';
      //strcat(basic->board[row], "\0");
      row += 1;
      new_row = 1;
    } 
  }
  //printf("load_board checkpoint G (rest of rows completed)\n");

  

  basic->y_size = row; 
  // change to iterator

  
  //fclose(fptr);
  //printf("at end load_board\n");
  return basic;
}


/* Task 6.1 */
static void find_head(game_state_t* state, int snum) {
  // TODO: Implement this function.
  int curr_x;
  int curr_y;
  char next;
  char curr;

  //printf("at find_head\n");
  curr_x = state->snakes[snum].tail_x;
  curr_y = state->snakes[snum].tail_y;
  curr = get_board_at(state, curr_x,  curr_y);
  next = get_board_at(state, curr_x + incr_x(curr), curr_y + incr_y(curr));

  while (next != ' ' && next != '#') {
    curr_x += incr_x(next);
    curr_y += incr_y(next);
    curr = get_board_at(state, curr_x, curr_y);
    next = get_board_at(state, curr_x + incr_x(curr), curr_y + incr_y(curr));
  }

  state->snakes[snum].head_x = curr_x;
  state->snakes[snum].head_y = curr_y;

  if (get_board_at(state, curr_x, curr_y) == 'x') {
    state->snakes[snum].live = 0;
  }

  //printf("at end find_head\n");
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  int row;
  int col;
  char curr;


  //printf("at initialize_snakes\n");
  state->num_snakes = 0;
  state->snakes = malloc(sizeof(snake_t));

  for (row = 0; row < state->y_size; row += 1) {
    for (col = 0; col < state->x_size; col += 1) {
      curr = get_board_at(state, col, row);
      if (is_tail(curr)) {
        state->snakes = realloc(state->snakes, sizeof(state->snakes) + sizeof(snake_t));
        state->snakes[state->num_snakes].tail_x = col;
        state->snakes[state->num_snakes].tail_y = row;
        state->num_snakes += 1;
        state->snakes[state->num_snakes].live = 1;
      }
    }
  }

  //printf("at end initialize_snakes\n");
  return state;
}

