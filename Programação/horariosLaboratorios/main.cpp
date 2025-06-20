#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <algorithm>
using namespace std;

int main() {
    /* declaracao das variaveis de ambiente */
    //variaveis de apoio
    int i = 0, l = 1, lines = -1;
    string disciplina, identificacao, linha, turma, periodos[] = {"07:30", "08:20", "09:10", "10:20", "11:10", "13:10", "14:00", "14:50", "16:00", "16:50", "17:40"};
    
    //variaveis de stream
    fstream alocacao, horarios, output;
    
    //hashmap para os laboratorios (lab -> disciplina)
    map<string, vector<string>> lab;
    
    //hashmap dos conflitos (lab -> numero de conflitos)
    map<string, int> errors;

    //hashmap do csv final (lab -> tabela[11][5])
    map<string, array<array<string, 5>, 11>> matrix;

    //inicializacao e verificacao de integridade dos arquivos de input (horarios.csv e alocacao.csv) e do arquivo de output (labs.csv)
    alocacao.open("alocacao.csv", ios::in);
    if (!alocacao) {
        cerr << "alocacao.csv nao pode ser aberto" << endl;
        return -1;
    }
    horarios.open("horarios.csv", ios::in);
    if (!horarios) {
        cerr << "horarios.csv nao pode ser aberto" << endl;
        return -1;
    }
    output.open("labs.csv", ios::out);
    if (!output) {
        cerr << "labs.csv nao pode ser aberto" << endl;
        return -1;
    }

    //itera por todas as linhas do arquivo de alocacao
    while(getline(alocacao, linha)){
        if (l > 2) { //ignora as duas primeiras linhas de cabecalho
            int i;

            //enquanto nao encontrar uma virgula, adiciona os caracteres a string disciplina
            for(i = 0; linha[i] != ',' && i < linha.size(); i++) disciplina.push_back(linha[i]);
            
            //ignorando a sala, enquanto nao chegar ao final da linha, adiciona os caracteres a identificacao da sala
            for(i = i + 5; i < linha.size(); i++) identificacao.push_back(linha[i]);
            
            lab[identificacao].push_back(disciplina); //adiciona a disciplina ao seu lab utilizado
            
            //zera ambas as strings para que nao ocorram erros
            disciplina = "";
            identificacao = "";
        }
        l++;
    }
    alocacao.close();

    //itera por todas as linhas do arquivo de horarios
    while(getline(horarios, linha)){
        //se for a linha inicial da tabela, volta o indice da linha de "matrix" para 0
        if(linha.substr(0, 5) == "07:30") i = 0;
        
        //se for um cabecalho de turma, guarda a turma
        if(linha.substr(0, 5) == "Turma") {
            turma = linha.substr(6, 4);
            continue;
        }
        if(linha.substr(2, 1) == ":") { //se for uma coluna valida
            int k = 6, j = 0; //comeca a iterar depois dos horarios (a partir da disciplina), inicializa a coluna em zero
            for(; k < linha.size(); k++){ //itera pelos caracteres da disciplina
                if (linha[k] == ',') { //se for uma virgula... (chegou no final da palavra daquela coluna)
                    disciplina = "";   //zera a string de apoio que guarda as disciplinas para nao ocorrerem erros
                    j++;               //incrementa o valor das colunas
                } else {
                    disciplina.push_back(linha[k]); //se nao for uma virgula (nao chegou no final da palavra daquela coluna)
                    for (auto & l : lab) { //itera por todos os labs e suas materias
                        for (int m = 0; m < l.second.size(); m++) { //itera por todas as materias de cada lab
                            if (disciplina == l.second[m]) { //se a disciplina atual for igual a alguma de algum lab
                                
                                //verifica se ja existe alguma turma no horario atual
                                if (matrix[l.first][i][j] != "") {  //se ja hover turmas nesse horario
                                    regex pattern("-\\s*(\\d{4})"); //cria um padrao para procurar o numero da turma
                                    smatch match; //cria a variavel de resultado do regex
                                    
                                    //procura no horario atual pela turma que esta la
                                    if (regex_search(matrix[l.first][i][j], match, pattern)) { 
                                        string t = match.str(1); //guarda o valor da turma encontrada
                                        if (t != turma) { //se a turma atual for diferente da turma que ja ocupa o local
                                            /* 
                                                OBS: essa verificacao é necessaria para que nao haja uma duplicata
                                                    nos horarios do laboratorio de micros, visto que, ja que o comeco
                                                    dos nomes das materias é identico, caso nao haja essa verificacao
                                                    um conflito irreal aparecera nos registros. ao verificar se a turma
                                                    que ocupa o local é a mesma que a atual, esse erro é corrigido, visto
                                                    que nenhuma turma pode ter duas materias ao mesmo tempo
                                            */

                                            errors[l.first]++; //incrementa em um os erros do lab atual
                                            matrix[l.first][i][j] += " / " + disciplina + " - " + turma; //adicona o conflito de horarios na tabela final
                                        }
                                    }
                                } else { //caso nao existam turmas no horario atual
                                    matrix[l.first][i][j] = disciplina + " - " + turma; //adicona a turma e materia no horario atual
                                }
                            }
                        }
                    }
                }
            }
            disciplina = ""; //zera a string de apoio para evitar erros
            i++; //incrementa a linha do horario atual, pois a linha chegou ao fim
        }
    }
    horarios.close();

    //itera por todos os itens da matrix do csv final
    for (auto & it : matrix) {
        output << it.first << ",,,,,\n,,,,,\n,Segunda,Terca,Quarta,Quinta,Sexta\n"; //adiciona os cabecalhos iniciais
        
        for (int i = 0; i < 11; i++) { //itera por todas as linhas
            output << periodos[i] << ","; //adiciona os horarios (guardados previamente em uma string, para facilitar a manipulacao)
            for (int j = 0; j < 5; j++) { //itera por todas as colunas
                output << it.second[i][j]; //manda o valor do horario atual para o arquivo final
                if(j != 4) output << ",";  //se nao chegamos ao final das colunas, adiciona uma "," para seguir para a proxima coluna
                else output << "\n";       //se chegamos a coluna final, pula para a linha abaixo
            }
        }
        output << ",,,,,\n"; //se chegamos ao final da tabela do lab, adiciona uma linha em branco para melhor legibilidade
    }

    //adicona os cabecalhos do relatorio de conflitos
    output << "Relatorio de Conflitos," << endl;
    output << "Sala,Conflitos" << endl;
    output << "," << endl;
    
    //itera por todos os conflitos
    for (auto & it : errors) {
        output << it.first << "," << it.second << endl; //manda o valor dos conflitos de cada lab para o arquivo final
    }
    output.close();
}