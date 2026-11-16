#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <math.h>

typedef struct std{
    int flag; //anado "flag" para saber si borrarla o no
    float pre_GPA,post_GPA,gen_ai_hours,trad_study_hours,perc_ai_dep,tool_div,anx_lvl,skill_ret;
    char major,c_year,prim_use_case,prompt_eng_skill,inst_policy,burn_risk;
    bool paid_sub;
} student;

void maximos_y_minimos(FILE* f, float atrib[7][2]){
    char str[1000];
    f = fopen("ai_student_impact_dataset_1_.csv","r");
    fgets(str, sizeof(str), f); //ignorar la primera linea
    for(int i=0;i<7;i++){
        atrib[i][0] = 0; //inicializo la tabla de maximos y minimos
        atrib[i][1] = 0;
    }

    while(fgets(str,sizeof(str),f) != NULL){
        char* line = strtok(str,",");
        for(int i=0;i<3;i++){
            line = strtok(NULL,","); //ignorar los primeros 3 atributos
        }
        for(int i=0;i<10;i++){
            if(i == 0 || i == 1 || i == 4 || i == 6 || i == 7 || i == 9 || i == 10 || i == 11){
                float valor = atof(line);
                if(valor > atrib[i][0]){
                    atrib[i][0] = valor; //actualizo el maximo
                }
                if(valor < atrib[i][1] || atrib[i][1] == 0){
                    atrib[i][1] = valor; //actualizo el minimo
                }
            line = strtok(NULL,",");
            }
        }
    }
    fclose(f);
}



//precondicion: f es un fichero que no ha sido normalizado
//y contiene estudiantes validos

void normalizar(FILE* f,float atrib[7][2]){ //atrib es la tabla de maximos y minimos por cada columna numerica
    char str[1000];
    int j=0; //aplico esto para lelvar el maximo y minimo de cada columna numerica correspondiente
    f = fopen("ai_student_impact_dataset_1_.csv","r");
    FILE* h = fopen("normalizado.txt", O_WRONLY | O_CREAT | 0644);
    fgets(str, sizeof(str), f); //ignorar la primera linea
    fputcs(str,h); //copio la primera linea al fichero normalizado
    while(fgets(str,sizeof(str),f) != NULL){
        char* line = strtok(str,","); //obtengo el primer atributo (fuera del for, para poder usar strtok dentro del for)
        for(int i=0;i<15;i++){
            if(i==3 || i==4 || i==7 || i==9 || i==10 || i==12 || i==13 || i==14){ //funcionamiento de mi pensamiento:
                float valor = atof(line);       //si el atributo es numerico, lo normalizo, j lleva el indice de la columna de atributo en la que estoy.
                                                //una vez normalizado, lo copio al fichero normalizado. Si el atributo es categorico, lo copio tal cual al fichero normalizado.
                valor = (valor - atrib[j][1]) / (atrib[j][0] - atrib[j][1]); //formula de normalizacion
                j++; //actualizo el indice de la columna de atributo en la que estoy (para los atributos numericos)
                if(j==4)
                    fprintf(h, "%.2f,", valor); //copio el valor normalizado al fichero normalizado
                else
                    fprintf(h, "%.3f,", valor); //copio el valor normalizado al fichero normalizado (para la posicion 4, que es el porcentaje de dependencia de la IA, lo copio con 2 decimales, para los demas atributos numericos, lo copio con 3 decimales)
            }
            else
                fputc(line,h); //copio el valor al fichero normalizado
            line = strtok(NULL,","); //obtengo el siguiente atributo
        }
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
    d += fabs(a.pre_GPA - b.pre_GPA);
    d += fabs(a.post_GPA - b.post_GPA);
    d += fabs(a.gen_ai_hours - b.gen_ai_hours);
    d += fabs(a.trad_study_hours - b.trad_study_hours);
    d += fabs(a.perc_ai_dep - b.perc_ai_dep);
    d += fabs(a.tool_div - b.tool_div);
    d += fabs(a.anx_lvl - b.anx_lvl);
    d += fabs(a.skill_ret - b.skill_ret);

    //atributos categoricos
    d += (a.major != b.major);
    d += (a.c_year != b.c_year);
    d += (a.prim_use_case != b.prim_use_case);
    d += (a.prompt_eng_skill != b.prompt_eng_skill);
    d += (a.inst_policy != b.inst_policy);
    d += (a.burn_risk != b.burn_risk);
    d += (a.paid_sub != b.paid_sub); 
    return d;


}

int main(char* args[], int argc){
    FILE* f = fopen("ai_student_impact_dataset_1_.csv","r");
    float atrib[7][2]; //tabla que utilizare para normalizar los atributos numericos
    maximos_y_minimos(f, atrib);
    normalizar(f, atrib);

}