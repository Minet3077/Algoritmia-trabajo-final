#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#define MAX_LINE 1024
#define NUM_ATRIB 8 //Atributos númericos que se usan, asi evitamos las constantes verificaciones con el if

typedef struct std{
    int flag; //anado "flag" para saber si borrarla o no
    float pre_GPA,post_GPA,gen_ai_hours,trad_study_hours,perc_ai_dep,tool_div,anx_lvl,skill_ret;
    char major[30],c_year[20],prim_use_case[40],prompt_eng_skill[20],inst_policy[30],burn_risk[20];
    bool paid_sub;
} student;

int obtener_indice_numerico(int col_idx) {
    switch (col_idx) {
        case 3:  return 0; // pre_GPA
        case 4:  return 1; // gen_ai_hours
        case 7:  return 2; // tool_div
        case 9:  return 3; // trad_study_hours
        case 10: return 4; // perc_ai_dep
        case 12: return 5; // anx_lvl
        case 13: return 6; // post_GPA
        case 14: return 7; // skill_ret
        default: return -1; // No es numérico
    }
}

void maximos_y_minimos(float atrib[NUM_ATRIB][2]){

    FILE* f = fopen("ai_student_impact_dataset_1_.csv","r");

    char str[MAX_LINE];
    fgets(str, sizeof(str), f); //ignorar la primera linea

    for(int i = 0; i < NUM_ATRIB; i++){
        atrib[i][0] = 0; //inicializo la tabla de maximos y minimos
        atrib[i][1] = 0;
    }

    bool inicializado[NUM_ATRIB] = { false };

    while(fgets(str,sizeof(str),f) != NULL){

        int col = 0;
        char* line = strtok(str,",");
        while (line != NULL){
            int num_indx = obtener_indice_numerico(col);
            if (num_indx != -1){
                float valor = atof(line);
                if(!inicializado[num_indx]){
                    atrib[num_indx][0] = valor;
                    atrib[num_indx][1] = valor;
                    inicializado[num_indx] = true;
                }
                else{
                    if (valor > atrib[num_indx][0]){
                        atrib[num_indx][0] = valor;
                    }
                    if (valor < atrib[num_indx][1]){
                        atrib[num_indx][1] = valor;
                    }
                }
            }
            line = strtok(NULL, ",");
            col ++;
        }
    }
    fclose(f);
}



//precondicion: f es un fichero que no ha sido normalizado
//y contiene estudiantes validos

void normalizar(float atrib[NUM_ATRIB][2]){ //atrib es la tabla de maximos y minimos por cada columna numerica


    FILE* f = fopen("ai_student_impact_dataset_1_.csv", "r");
    FILE* h = fopen("normalizado.txt","w");

    char str[MAX_LINE];
    fgets(str, sizeof(str), f); //ignorar la primera linea
    fputs(str,h); //copio la primera linea al fichero normalizado


    while(fgets(str,sizeof(str),f) != NULL){

        str[strcspn(str, "\r\n")] = 0; // ** <- Pondré estos simbolos para los comandos necesarios que no entiendo pero con los que el database no se rompe xd, strcspn cuenta el número de caracteres de un string que hay hasta antes de llegar al segundo string (en este caso, devolverá el numero de caracteres que hay antes de que el string (str) llegue a un salto de linea ("\r\n"))

        int col = 0;
        char* line = strtok(str,","); //obtengo el primer atributo (fuera del for, para poder usar strtok dentro del for)

        while (line != NULL){

            int num_indx = obtener_indice_numerico(col);
            if ( num_indx != -1){
                float valor = atof(line);
                float min_val = atrib[num_indx][1];
                float max_val = atrib[num_indx][0];
                float norm = max_val - min_val;

                if (norm == 0){
                    norm = 0;
                }
                else{
                    norm = (valor - min_val)/(max_val - min_val);
                }

                if (num_indx == 4){
                    fprintf(h, "%.2f", norm);
                }

                else{
                    fprintf(h, "%.3f", norm);
                }
            }
            else{
                fprintf(h, "%s", line);
            }

            line = strtok(NULL, ",");
            if (line != NULL){
                fprintf(h, ",");
            }
            col++;
        }
        fprintf(h,"\n"); // ** Junto con lo de arriba, agregamos manualmente los saltos de linea en el ultimo elemento de cada fila, de este modo evitamos doble saltos de linea y mantemos la estructura del database
    }
    fclose(f);
    fclose(h);
}



//precondicion: a y b son estudiantes validos, y han sido normalizados
// sus valores numericos.

float distancia(student a, student b){
    float d = 0;    //atributo numerico: valor_atributo_a - valor_atributo_b (en valor absoluto)
                    //atributo categorico: 0 si son iguales, 1 si son distintos
    //atributos numericos
    d += pow(a.pre_GPA - b.pre_GPA,2);
    d += pow(a.post_GPA - b.post_GPA,2);
    d += pow(a.gen_ai_hours - b.gen_ai_hours,2);
    d += pow(a.trad_study_hours - b.trad_study_hours,2);
    d += pow(a.perc_ai_dep - b.perc_ai_dep,2);
    d += pow(a.tool_div - b.tool_div,2);
    d += pow(a.anx_lvl - b.anx_lvl,2);
    d += pow(a.skill_ret - b.skill_ret,2);

    //atributos categoricos
    d += (strcmp(a.major, b.major)!= 0);
    d += (strcmp(a.c_year, b.c_year)!= 0);
    d += (strcmp(a.prim_use_case, b.prim_use_case)!= 0);
    d += (strcmp(a.prompt_eng_skill, b.prompt_eng_skill)!= 0);
    d += (strcmp(a.inst_policy,b.inst_policy)!= 0);
    d += (strcmp(a.burn_risk, b.burn_risk)!= 0);
    d += (a.paid_sub != b.paid_sub);
    return sqrt(d);


}

int main(int argc, char* args[]){
    float atrib[NUM_ATRIB][2]; //tabla que utilizare para normalizar los atributos numericos
    maximos_y_minimos(atrib);
    normalizar(atrib);
    return 0;
}
