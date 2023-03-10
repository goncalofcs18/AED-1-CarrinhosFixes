//
// AED, August 2022 (Tomás Oliveira e Silva)
//
// First practical assignement (speed run)
//
// Compile using either
//   cc -Wall -O2 -D_use_zlib_=0 solution_speed_run.c -lm
// or
//   cc -Wall -O2 -D_use_zlib_=1 solution_speed_run.c -lm -lz
//
// Place your student numbers and names here
//   N.Mec. 108713  Name: Liliana Ribeiro
//   N.Mec. 108133  Name: Gonçalo Sousa

//
// static configuration
//

#define _max_road_size_ 800 // the maximum problem size
#define _min_road_speed_ 2  // must not be smaller than 1, shouldnot be smaller than 2
#define _max_road_speed_ 9  // must not be larger than 9 (only because of the PDF figure)

//
// include files --- as this is a small project, we include the PDF generation code directly from make_custom_pdf.c
//

#include <math.h>
#include <stdio.h>
#include <string.h> //usado para podermos escolher que funcção vamos correr
#include "../P02/elapsed_time.h"
#include "make_custom_pdf.c"

//
// road stuff
//

static int max_road_speed[1 + _max_road_size_]; // positions 0.._max_road_size_

static void init_road_speeds(void)
{
  double speed;
  int i;

  for (i = 0; i <= _max_road_size_; i++)
  {
    speed = (double)_max_road_speed_ * (0.55 + 0.30 * sin(0.11 * (double)i) + 0.10 * sin(0.17 * (double)i + 1.0) + 0.15 * sin(0.19 * (double)i));
    max_road_speed[i] = (int)floor(0.5 + speed) + (int)((unsigned int)random() % 3u) - 1;
    if (max_road_speed[i] < _min_road_speed_)
      max_road_speed[i] = _min_road_speed_;
    if (max_road_speed[i] > _max_road_speed_)
      max_road_speed[i] = _max_road_speed_;
  }
}

//
// description of a solution
//

typedef struct
{
  int n_moves;                        // the number of moves (the number of positions is one more than the number of moves)
  int positions[1 + _max_road_size_]; // the positions (the first one must be zero)
} solution_t;

//
// the (very inefficient) recursive solution given to the students
//

static solution_t solution_1, solution_1_best;
static double solution_1_elapsed_time; // time it took to solve the problem
static unsigned long solution_1_count; // effort dispended solving the problem

static void solution_1_recursion(int move_number, int position, int speed, int final_position)
{
  int i, new_speed;

  // record move
  solution_1_count++;
  solution_1.positions[move_number] = position;
  // is it a solution?
  if (position == final_position && speed == 1)
  {
    // is it a better solution?
    if (move_number < solution_1_best.n_moves)
    {
      solution_1_best = solution_1;
      solution_1_best.n_moves = move_number;
    }
    return;
  }
  // no, try all legal speeds
  for (new_speed = speed - 1; new_speed <= speed + 1; new_speed++)
    if (new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for (i = 0; i <= new_speed && new_speed <= max_road_speed[position + i]; i++)
        ;
      if (i > new_speed)
        solution_1_recursion(move_number + 1, position + new_speed, new_speed, final_position);
    }
}

static solution_t solution_2v1, solution_2v1_best;
static double solution_2v1_elapsed_time; // time it took to solve the problem
static unsigned long solution_2v1_count; // effort dispended solving the problem

static void solution_2v1_recursionTeste(int move_number, int position, int speed, int final_position)
{

  if (solution_2v1_best.positions[move_number] > position)
    return;

  int i, new_speed;

  // record move
  solution_2v1_count++;
  solution_2v1.positions[move_number] = position;
  // is it a solution?
  if (position == final_position && speed == 1)
  {
    if (move_number < solution_2v1_best.n_moves)
    {
      solution_2v1_best = solution_2v1;
      solution_2v1_best.n_moves = move_number;
    }

    return;
  }
  // no, try all legal speeds

  for (new_speed = speed + 1; new_speed >= speed - 1; new_speed--)
    if (new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position)
    {
      for (i = 0; i <= new_speed && new_speed <= max_road_speed[position + i]; i++)
        ;
      if (i > new_speed)
      {
        solution_2v1_recursionTeste(move_number + 1, position + new_speed, new_speed, final_position);
      }
    }
}

static solution_t solution_3_best;
static double solution_3_elapsed_time; // time it took to solve the problem
static unsigned long solution_3_count; // effort dispended solving the problem

static void solution_3_SmartWay(int move_number, int position, int speed, int final_position)
{
  int speed_test, md_position, new_speed, position_test, brk;
  solution_3_best.n_moves = 0;

  while (position < final_position)
  {

    for (new_speed = speed + 1; new_speed >= speed - 1; new_speed--)
    {
      brk = 0;

      if (new_speed > _max_road_speed_) // testa se ultrapassa a velocidade maxima
      {
        continue;
      }

      position_test = position;

      for (speed_test = new_speed; speed_test >= 1; speed_test--) // i percorre todos os valores desde speed+1 (movimento ideal) até 1... --> velocidades assumidas na travagem
      {

        if ((position_test + speed_test) > final_position)
        {
          brk = 1;
          break;
        }
        for (md_position = 0; md_position <= speed_test; md_position++) // testar posicao de travagem
        {
          if (speed_test > max_road_speed[position_test + md_position]) // passa a velocidade da casa?
          {
            brk = 1;
            break;
          }
        }

        position_test += speed_test; // future_position --> posicao teste, i --> speed teste
      }

      if (!brk) // não deu errado? temos o proximo passo :) --> atualizar as variaveis
      {
        speed = new_speed;
        position += speed;
        solution_3_count++;
        solution_3_best.positions[move_number++] = position;
        solution_3_best.n_moves++;
        break;
      }
    }
  }
}

// save the values for the dinamic function
static int posD = 0;
static int spdD = 0;
static int movD = 0;
static solution_t solution_6_best;
static double solution_6_elapsed_time; // time it took to solve the problem
static unsigned long solution_6_count; // effort dispended solving the problem

static void solution_6_Dinamic(int move_number, int position, int speed, int final_position)
{
  // define some variables
  int speed_test, md_position, new_speed, position_test;
  // save de values from the save value of move_number
  solution_6_best.n_moves = move_number;
  // rd --> choose what option will the car choose (decrease rd=2, keep rd=1 or increase rd=0)
  int rd;
  // fp --> if the car is close to the final point fp different from 0
  int fp = 0;

  // repite this process until it gets to the final position
  while (position < final_position)
  {
    rd = 0;
    // increase
    if (rd == 0 || fp == 0)
    {
      new_speed = speed + 1;

      if (new_speed <= _max_road_speed_) // testa se não ultrapassa o final TODO: n(n+1)2
      {
        position_test = position;

        for (speed_test = new_speed; speed_test >= 1; speed_test--) // i percorre todos os valores desde speed+1 (movimento ideal) até 1... --> velocidades assumidas na travagem
        {
          if ((position_test + speed_test) > final_position)
          {
            fp = 1;
            rd = 1;
            break;
          }
          for (md_position = 0; md_position <= speed_test; md_position++) // testar posicao de travagem
          {
            if (speed_test > max_road_speed[position_test + md_position]) // passa a velocidade da casa?
            {
              rd = 1;
              break;
            }
          }

          position_test += speed_test; // future_position --> posicao teste, i --> speed teste
        }
      }
      else
      {
        rd = 1;
      }
    }
    // keep
    if (rd == 1 || fp == 1)
    {
      new_speed = speed;
      position_test = position;

      for (speed_test = new_speed; speed_test >= 1; speed_test--) // i percorre todos os valores desde speed+1 (movimento ideal) até 1... --> velocidades assumidas na travagem
      {
        if ((position_test + speed_test) > final_position)
        {
          fp = 2;
          rd = 2;
          break;
        }
        for (md_position = 0; md_position <= speed_test; md_position++) // testar posicao de travagem
        {
          if (speed_test > max_road_speed[position_test + md_position]) // passa a velocidade da casa?
          {
            rd = 2;
            break;
          }
        }

        position_test += speed_test; // future_position --> posicao teste, i --> speed teste
      }
    }
    // decrease (dont run any testes because its the last option)
    if (rd == 2 || fp == 2)
    {
      new_speed = speed - 1;
    }
    // save the values of this movement
    speed = new_speed;
    position += speed;
    solution_6_count++;
    solution_6_best.positions[move_number++] = position;
    solution_6_best.n_moves++;
    // if the car didnt get close to the final position, we save the current move to use in the next call for the bigger position 
    if (fp == 0)
    {
      movD = move_number;
      posD = position;
      spdD = speed;
    }
  }
}

static void solve_1(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_1: bad final_position\n");
    exit(1);
  }
  solution_1_elapsed_time = cpu_time();
  solution_1_count = 0ul;
  solution_1_best.n_moves = final_position + 100;
  solution_1_recursion(movD, posD, spdD, final_position);
  solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
}

// função 2v1
static void solve_2v1(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_1: bad final_position\n");
    exit(1);
  }
  solution_2v1_elapsed_time = cpu_time();
  solution_2v1_count = 0ul;
  solution_2v1_best.n_moves = final_position + 100;
  solution_2v1_recursionTeste(0, 0, 0, final_position);
  solution_2v1_elapsed_time = cpu_time() - solution_2v1_elapsed_time;
}

// função 3
static void solve_3(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_3: bad final_position\n");
    exit(1);
  }
  solution_3_elapsed_time = cpu_time();
  solution_3_count = 0ul;
  solution_3_best.n_moves = final_position + 100;
  solution_3_SmartWay(0, 0, 0, final_position);
  solution_3_elapsed_time = cpu_time() - solution_3_elapsed_time;
}

// função 3
static void solve_6(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_1: bad final_position\n");
    exit(1);
  }
  solution_6_elapsed_time = cpu_time();
  solution_6_count = 0ul;
  solution_6_best.n_moves = final_position + 100;
  // fazer os moves novos
  // em vez de começar com os valores 0,0,0,final_position começa com os valores guardados
  solution_6_Dinamic(movD, posD, spdD, final_position);
  // solution_6_Dinamic(0, 0, 0, final_position);
  solution_6_elapsed_time = cpu_time() - solution_6_elapsed_time;
}

//
// example of the slides
//

static void example(void)
{
  int i, final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_1(final_position);
  make_custom_pdf_file("example.pdf", final_position, &max_road_speed[0], solution_1_best.n_moves, &solution_1_best.positions[0], solution_1_elapsed_time, solution_1_count, "Plain recursion");
  printf("mad road speeds:");
  for (i = 0; i <= final_position; i++)
    printf(" %d", max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for (i = 0; i <= solution_1_best.n_moves; i++)
    printf(" %d", solution_1_best.positions[i]);
  printf("\n");
}

//
// main program
//

int main(int argc, char *argv[argc + 1])
{
#define _time_limit_ 3600.0
  int n_mec, final_position, print_this_one;
  char file_name[64];

  // generate the example data
  if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'e' && argv[1][2] == 'x')
  {
    example();
    return 0;
  }
  // initialization
  n_mec = (argc < 2) ? 0xAED2022 : atoi(argv[1]);
  srandom((unsigned int)n_mec);
  init_road_speeds();
  // run all solution methods for all interesting sizes of the problem
  final_position = 1;

  printf("    + --- ---------------- --------- +\n");
  printf("    |                plain recursion |\n");
  printf("--- + --- ---------------- --------- +\n");
  printf("  n | sol            count  cpu time |\n");
  printf("--- + --- ---------------- --------- +\n");
  while (final_position <= _max_road_size_ /* && final_position <= 20*/)
  {
    print_this_one = (final_position == 10 || final_position == 20 || final_position == 50 || final_position == 100 || final_position == 200 || final_position == 400 || final_position == 800) ? 1 : 0;
    printf("%3d |", final_position);

    if (strcmp(argv[argc - 1], "solution1") == 0)
    {
      solution_1_elapsed_time = 0.0;
      // first solution method (very bad)
      if (solution_1_elapsed_time < _time_limit_)
      {
        solve_1(final_position);
        if (print_this_one != 0)
        {
          sprintf(file_name, "%03d_1.pdf", final_position);
          make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_1_best.n_moves, &solution_1_best.positions[0], solution_1_elapsed_time, solution_1_count, "Plain recursion");
        }
        printf(" %3d %16lu %9.3e |", solution_1_best.n_moves, solution_1_count, solution_1_elapsed_time);
      }
      else
      {
        solution_1_best.n_moves = -1;
        printf("                                |");
      }
    }
    else if (strcmp(argv[argc - 1], "solution2v1") == 0)
    {
      solution_2v1_elapsed_time = 0.0;
      // first solution method (very bad)
      if (solution_2v1_elapsed_time < _time_limit_)
      {
        solve_2v1(final_position);
        if (print_this_one != 0)
        {
          sprintf(file_name, "%03d_1.pdf", final_position);
          make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_2v1_best.n_moves, &solution_2v1_best.positions[0], solution_2v1_elapsed_time, solution_2v1_count, "recursion but smart");
        }
        printf(" %3d %16lu %9.3e |", solution_2v1_best.n_moves, solution_2v1_count, solution_2v1_elapsed_time);
      }
      else
      {
        solution_2v1_best.n_moves = -1;
        printf("                                |");
      }
    }
    else if (strcmp(argv[argc - 1], "solution6") == 0)
    {
      // six solution  method (dinamic one)
      solution_6_elapsed_time = 0.0;
      if (solution_6_elapsed_time < _time_limit_)
      {
        solve_6(final_position);
        if (print_this_one != 0)
        {
          sprintf(file_name, "%03d_1.pdf", final_position);
          // TODO: solucao que dado uma position mostra as anteriores
          make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_6_best.n_moves, &solution_6_best.positions[0], solution_6_elapsed_time, solution_6_count, "Dinamic");
        }
        printf(" %3d %16lu %9.3e |", solution_6_best.n_moves, solution_6_count, solution_6_elapsed_time);
      }
      else
      {
        solution_6_best.n_moves = -1;
        printf("                                |");
      }
    }
    else
    {
      // third solution  method (better one)
      solution_3_elapsed_time = 0.0;
      if (solution_3_elapsed_time < _time_limit_)
      {
        solve_3(final_position);
        if (print_this_one != 0)
        {
          sprintf(file_name, "%03d_1.pdf", final_position);
          make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_3_best.n_moves, &solution_3_best.positions[0], solution_3_elapsed_time, solution_3_count, "Best Way");
        }
        printf(" %3d %16lu %9.3e |", solution_3_best.n_moves, solution_3_count, solution_3_elapsed_time);
      }
      else
      {
        solution_3_best.n_moves = -1;
        printf("                                |");
      }
    }
    // second solution method (less bad)
    //....

    // done
    printf("\n");
    fflush(stdout);
    // new final_position
    if (final_position < 50)
      final_position += 1;
    else if (final_position < 100)
      final_position += 5;
    else if (final_position < 200)
      final_position += 10;
    else
      final_position += 20;
  }
  printf("--- + --- ---------------- --------- +\n");
  return 0;
#undef _time_limit_
}
