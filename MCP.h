//
// Created by noahmancino on 11/1/20.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef INC_415_PROJECT2_MCP_H
#define INC_415_PROJECT2_MCP_H
#define MAX_TOKENS 50
#define MAX_COMMANDS 500
#define MAX_TOKEN 500

/*
 * Given the name of a file holding a list of commands this function returns an array of argv arrays. The array is
 * null terminated, as are the inner argv arrays.
 */
char ***parse(const char *filename) {
    char ***parsed_lines = (char ***)malloc(sizeof(char **) * MAX_COMMANDS);
    FILE *command_file = fopen(filename, "r");
    char *line = NULL;
    size_t n = 0;
    int i;
    for (i = 0; getline(&line, &n, command_file) != -1; i++) {
        parsed_lines[i] = (char **)malloc(sizeof(char *) * MAX_TOKENS);
        char *token = strtok(line, " \n");
        int j;
         for (j = 0; token != NULL; j++) {
            parsed_lines[i][j] = (char *)malloc(sizeof(char) * MAX_TOKEN);
            strcpy(parsed_lines[i][j], token);
            token = strtok(NULL, " \n");
         }
         parsed_lines[i][j] = NULL;

    }
    free(line);
    fclose(command_file);
    parsed_lines[i] = NULL;
    return parsed_lines;
}

/*
 * Frees a null terminated array of null terminated argv arrays.
 */
void free_parsed(char ***parsed) {
    for (int i = 0; parsed[i] != NULL; i++) {
        for (int j = 0; parsed[i][j] != NULL; j++) {
            free(parsed[i][j]);
        }
        free(parsed[i]);
    }
    free(parsed);
}

#endif //INC_415_PROJECT2_MCP_H