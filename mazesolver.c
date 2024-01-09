#define _CRT_SECURE_NO_WARNINGS

/* Preprocessor directives to include libraries */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mazesolver.h"


void process()
{
  /* Variables */
  int dimension = 0;
  FILE* maze_file = NULL;
  maze_cell** maze = NULL;
  char outputstring[BUFFER]; // holds output for shortest/cheapest path

  /* Declare variables for holding generated path information */
  char** paths = NULL;
  int paths_found = 0;

  /* Opens and parses the maze file.*/
  maze_file = fopen(MAZE1, "r");

    if (maze_file) {

    /* Calls the functions that:
      a) get the size of the maze and stores it in the dimension variable
      b) copies the maze into memory */
        dimension = get_maze_dimension(maze_file);
        maze = parse_maze(maze_file, dimension);

  }
  else {
    fprintf(stderr, "Unable to parse maze file: %s\n", MAZE1);
    system("pause");
  }

  /* Traverses maze and generates all solutions */
  generate_all_paths(&paths, &paths_found, maze, dimension, 0, 0, "");

  /* Calculates and displays required data */
    construct_shortest_path_info(paths, paths_found, outputstring);
    printf("%s\n", outputstring);
    construct_cheapest_path_info(paths, paths_found, outputstring);
    printf("%s\n", outputstring);
}

/*
 Acquires and returns the maze size.  Since the maze is always a square, all we
 need to do is find the length of the top row!
 */
int get_maze_dimension( FILE* maze_file )  {

  int  dimension = 0;
  char line_buffer[BUFFER];

	dimension = strlen( fgets ( line_buffer, BUFFER, maze_file ) );

	fseek( maze_file, 0, SEEK_SET );

  if ( strchr( line_buffer, '\r' ) != NULL ) {
    // INSERT CODE HERE (1 line)
    return dimension - 2;
  } else {
    // INSERT CODE HERE (1 line)
    return dimension - 1;
  }
}

/*
 Parses and stores maze as a 2D array of maze_cell.  This requires a few steps:
 1) Allocating memory for a 2D array of maze_cell, e.g., maze_cell[rows][columns]
    a) Dynamically allocates memory for 'dimension' pointers to maze_cell, and assign
	   the memory (case as a double pointer to maze_cell) to maze, which is a
	   double pointer to maze_cell (this makes the maze[rows] headers)
	b) For each row of the maze, dynamically allocate memory for 'dimension' maze_cells
	   and assign it (cast as a pointer to maze_cell) to maze[row]
 2) Copying the file to the allocated space
    a) For each row obtained from the file using fgets and temporarily stored in line_buffer
	   i) For each of the 'dimension' columns in that row
	   ii)Assign the character from the file to the struct, and set the struct to unvisited
 3) Returns the double pointer called maze.
 */
maze_cell** parse_maze( FILE* maze_file, int dimension )
{
	/* Variables */
  char        line_buffer[BUFFER];
  int         row = 0;
	int         column = 0;
	maze_cell** maze = NULL; 

  /* Allocates memory for correctly-sized maze */
  maze = (maze_cell**)calloc(dimension, sizeof(maze_cell*));

  for ( row = 0; row < dimension; ++row ) {
    maze[row] = ( maze_cell* ) calloc( dimension, sizeof( maze_cell));
  }

  /* Copies maze file to memory */
	row = 0;
  while ( fgets ( line_buffer, BUFFER, maze_file ) ) {
    for ( column = 0; column < dimension; ++column ) {
      maze[row][column].character = line_buffer[column];
      maze[row][column].visited = 0;
	  }
    row++;
  }
	return maze;
}

/**
 Generates all paths through a maze recursively.
 */
void generate_all_paths( char*** pathsetref, int* numpathsref, maze_cell** maze, int dimension, int row, int column, char* path )
{
	/* Variables */
	int path_length   = 0;
	char* new_point  = NULL;
	char* new_path   = NULL;

  /* Checks for base cases */
  if (row < 0 || row >= dimension || column < 0 || column >= dimension || maze[row][column].character == '*' || maze[row][column].visited == 1) {
    return;
	}

  /* Otherwise deals with the recursive case.  Pushes the current coordinate onto the path
	  and checks to see if the right boundary of the maze has been reached
	  IF   right boundary reached
	  THEN path is added to the list as a successful path and function returns
	  ELSE the current location is marked as used and the search continues
	      in each cardinal direction using a recursive call using these steps:
		1. get length of path
		2. allocate space for a larger new path
		3. allocate space for a new point to add to the path
		4. assign the value in the maze cell to the new point
		5. concatenate old path to new path
		6. concatenate new point to new path */	
  else {
	  path_length = strlen( path );
	  new_path = ( char * ) calloc( path_length + 2, sizeof( char ) );
	  new_point = ( char * ) calloc( 2, sizeof( char ) );
	  new_point[0] = maze[row][column].character;
	  if ( path_length ) {
		  new_path = strcat( new_path, path );
	  }
	  new_path = strcat( new_path, new_point );
    free(new_point);

    if ( column == ( dimension - 1 ) ) {
	    /* 1. Reallocate memory in global paths array to make room
			    for a new solution string
			 2. Copy the solution path to the location of new string
			 3. Increment paths counter */
	    *pathsetref = ( char** ) realloc ( *pathsetref, ( (*numpathsref) + 1 ) * sizeof( char* ) );
      (*pathsetref)[*numpathsref] = ( char* ) calloc( strlen( new_path ) + 1, sizeof( char ));
	    strcpy( (*pathsetref)[*numpathsref], new_path );
	    (*numpathsref)++;
      return;
    } 
    else {
		  /* 1. Mark point as visited
			   2. Recursively search in each direction using the new path, and the same pathsetref and numpathsref
			   3. Mark point as unvisited */
        maze[row][column].visited = 1;
        generate_all_paths( pathsetref, numpathsref, maze, dimension, row, column + 1, new_path );
        generate_all_paths( pathsetref, numpathsref, maze, dimension, row, column - 1, new_path );
        generate_all_paths( pathsetref, numpathsref, maze, dimension, row + 1, column, new_path );
        generate_all_paths( pathsetref, numpathsref, maze, dimension, row - 1, column, new_path );
        maze[row][column].visited = 0;
		return;
    }
  }
}

/*
 Calculates the cost for a given path.
 */
int path_cost ( char* path_string )
{
  int cost = 0;
  for (int i = 0; i < strlen(path_string); i++) {
    cost += path_string[i] - '0';
  }

  return cost;
}

/*
 Writes the shortest path in the paths global variable into the outputbuffer parameter,
 where the shortest path has the fewest cells.
 In the event of a tie, use the first 'shortest' path found.
 */
void construct_shortest_path_info ( char** pathset, int numpaths, char* outputbuffer )
{
    int index = 0;
    int shortest = strlen(pathset[0]);

    for (int i = 1; i < numpaths; i++) {
        if (strlen(pathset[i]) < shortest) {
            shortest = strlen(pathset[i]);
            index = i;
        }
    }

    sprintf(outputbuffer, "Shortest path: %s", pathset[index]);
}

/*
 Writes the cheapest path in the paths global variable into the outputbuffer parameter,
 where the cheapest path has the lowest sum value of its cells.
 In the event of a tie, use the first 'cheapest' path found.
 */
void construct_cheapest_path_info ( char** pathset, int numpaths, char* outputbuffer )
{
    int index = 0;
    int cheapest = path_cost(pathset[0]);

    for (int i = 1; i < numpaths; i++) {
        if (path_cost(pathset[i]) < cheapest) {
            cheapest = path_cost(pathset[i]);
            index = i;
        }
    }

    sprintf(outputbuffer, "Cheapest path: %s\nCheapest path cost: %d", pathset[index], cheapest);
}