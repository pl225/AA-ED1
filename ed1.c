#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define TAM 1000000
#define TAMANHO 1000

/**
* Defines
* Utilizados para minimizar o esforço de repetir o tamanho das variaveis em diversas partes do codigo, alem de facilitar a alteracao do tamanho das mesmas caso seja necessario, em que:
* TOTSIM representa o total de caracteres da tabela ASCII
* MAX representa o numero maximo de caracteres em cada linha do texto
* MIN representa o numero minimo de caracteres em uma string
*/

#define TOTSIM 128
#define MAX 10000
#define MIN 20


/**
* Struct No da Arvore de Huffman
* @brief Consiste em uma struct que estabele a base para a criacao de um no da Arvore de Huffman
*/
typedef struct No
{
    char letra; /**< Caractere que armazena uma letra da string a ser comprimida*/
    int frequencia; /**< Frequencia com que a letra da string a ser comprimida se repete*/
    char lado; /**< Lado que o no e filho, podendo ser o filho direito ou esquerdo do no pai*/
    struct No* proximo;/**< No que representa as letras que se repetem no texto. O conjunto destes forma uma lista encadeada ordenada pela frequencia de cada letra*/
    struct No* filhoesq;/**< No que indica qual e o filho esquerdo do no atual, apontando para aquele*/
    struct No* filhodir;/**< No que indica qual e o filho direito do no atual, apontando para aquele*/
} No;

/**
* Struct Arvore de Huffman
* @brief Consiste em um struct que estabelece a base para a criacao de uma arvore de huffman
*/
typedef struct Huffman
{
    No* cabeca; /**< No principal da arvore que indica o seu comeco*/
    char texto [MAX][MAX]; /** < Matriz de caracteres que contem o texto inteiro lido a partir do arquivo*/
    int caracteres; /** < Quantidade de caracteres lidos do texto*/
    int frequencia_letras [TOTSIM]; /** < Frequencia de cada caractere lido do texto*/
    int tamanho; /** < Soma da frequencia de todos os caracteres*/
    float tamanho_compressao_final; /** < Numero de bits gerados a partir da compressao do arquivo original*/
} Huffman;

/**
* Funcao Vazia
* @brief Funcao que verifica se a arvore de huffman esta vazia
* Funcao que atraves do no cabeca da arvore, avalia se o proximo daquela e nulo, retornando 1 caso verdadeiro e 0 caso falso
*/
int vazia (Huffman* h)
{
    return (h->cabeca->proximo == NULL);
}
/**
* Função Cria No
* @brief Funcao que desenvolve um no, criando-o a partir do zero sem conecta-lo a nada.
* Primeiramente a funcao tenta alocar o novo no na memoria. Se foi possivel aloca-lo, ela inicia a letra, o lado, o seu ponteiro
* para proximo elemento, seu filho esquerdo e seu filho direito com nulo e a frequencia com que a letra se repete com zero e
* retorna ao usuario o novo no, caso contrario, se nao for possivel alocar na memoria o no ela, retorna nulo ao usuario
*/
No* criar_no ()
{
    No* novo = (No*)malloc(sizeof(No));
    if (novo!=NULL)
    {
        novo->letra = 0;
        novo->frequencia = 0;
        novo->lado = 0;
        novo->proximo = NULL;
        novo->filhoesq = NULL;
        novo->filhodir = NULL;
        return novo;
    }
    return NULL;
}
/**
* Funcao Criar Arvore de Huffman
* @brief Funcao que gera uma arvore de huffman inicial totalmente nula e a retorna para o usuario
* Primeiramente a funcao tenta alocar a arvore na memoria, se foi possivel aloca-la ela
*/
Huffman* criar_arvore_huffman ()
{
    Huffman* h = (Huffman*) malloc(sizeof(Huffman));
    if (h!=NULL)
    {
        h->cabeca = criar_no();
        h->caracteres = 0;
        h->tamanho = 0;
        h->tamanho_compressao_final = 0;
        int i, j;
        for (i=0; i<MAX; i++)
        {
            for (j=0; j<MAX; j++)
            {
                h->texto[i][j] = 0;
            }
        }
        for (i=0; i<TOTSIM; i++)
        {
            h->frequencia_letras[i] = 0;
        }
        return h;
    }
    return NULL;
}
/**
* Funcao Frequencia de Texto na Arvore
* @brief
* A funcao que recebe a arvore @param h e o arquivo @param arq e com isso
*/
void frequencia_texto_arvore (Huffman* h, FILE* arq)
{
    int i = 0, j = 0, k = 0;

    while (!feof(arq))
    {
        k = fgetc(arq);
        h->texto[i][j] = k;
        h->frequencia_letras[k]++;
        j++;
        if (k == '\n')
        {
            i++;
            j = 0;
        }
    }
    h->texto[i][j-1] = 0;
}
/**
* Funcao Quantidade de Caracteres
* @brief
* Funcao que recebe a arvore @param h e com isso
*/
void qtd_caracteres (Huffman* h)
{
    int i;
    for (i=0; i<TOTSIM; i++)
    {
        if (h->frequencia_letras[i]>0)
        {
            h->caracteres+=h->frequencia_letras[i];
        }
    }
}

void imprimir_texto (Huffman* h)
{
    int i = 0, j = 0;
    for (i=0; h->texto[i][j]!=0; i++)
    {
        printf("%s", h->texto[i]);
    }
}
/**
* Funcao Frequencia de Caracteres
* @brief Funcao que calcula quantas vezes os caracteres da arvore se apresentam
* A funcao recebe como parametro a arvore @param h
*/
void frequencia_caractere (Huffman* h)
{
    int i = 0;
    puts("\nFrequencia das letras:");
    for (i=0; i<TOTSIM; i++)
    {
        if (h->frequencia_letras[i]>0)
            printf("%c: %d\n", i, h->frequencia_letras[i]);
    }
    qtd_caracteres(h);
    printf("\nCaracteres: %d\n", h->caracteres);
}
/**
* Funcao Inserir No
* @brief Funçao que insere ordenadamente, atraves da frequencia, um no
* A funcao recebe como parametro a arvore @param h e o no @param novo, a ser inseridos. Com isso, se o no não for nulo a funcao
* busca identificar qual e a frequencia anterior a do mesmo e o insere como uma lista encadeada, caso contrario se o no for nulo
* e exibida na tela uma mensagem de erro e o no não e inserido
*/
void inserir_no (Huffman* h, No* novo)
{

    if (novo!=NULL)
    {

        No* p = h->cabeca;
        while (p->proximo!=NULL && p->proximo->frequencia < novo->frequencia)
        {
            p = p->proximo;
        }

        novo->proximo = p->proximo;
        p->proximo = novo;
        h->tamanho++;
    }
    else
    {
        puts("Memoria insuficiente!");
        exit(1);
    }
}
/**
* Funcao Remover Item de Menor Frequencia
* @brief Funcao que elimina um no que possui a menor frequencia e o retorna para o usuario
* Funcao que recebe como parametro a arvore @param h e atraves dela verifica se a mesma nao e nula para assim criar um no p que
* aponta para o proximo elemento do no cabeca da arvore, apos isso o proximo elemento do no p torna-se uma referencia para o no
* cabeca da arvore, por fim o no p e removido e retornado ao usuario. Caso a arvore esteja vazia nao ha remocao de elementos e o
* retorno e nulo
*/
No* remover_item_menor_frequencia(Huffman* h)
{
    if (!vazia(h))
    {
        No* p = h->cabeca->proximo;
        h->cabeca->proximo = h->cabeca->proximo->proximo;
        p->proximo = NULL;
        h->tamanho--;
        return p;
    }
    return NULL;
}
/**
* Funcao Criar Nos Folhas
* @brief Funcao que gera os nos iniciais para a montagem da arvore de huffman
* Funcao que avalia se a frequencia das letras que estao na arvore for maior que zero, se isso ocorre nos sao criados para cada
* letra e sao inseridos utilizando a funcao de inserir os nos de maneira ordenada pela frequencia (@see inserir_no)
*/
void criar_nos_folhas (Huffman* h)
{
    int i; /**< indice do for*/
    No* novo;
    for (i=0; i<TOTSIM; i++)
    {
        if (h->frequencia_letras[i]>0)
        {
            novo = criar_no();
            novo->letra = i;
            novo->frequencia = h->frequencia_letras[i];
            inserir_no(h, novo);
        }
    }
}
void imprimir (Huffman* h)
{
    No* p = h->cabeca->proximo;
    while (p!=NULL)
    {
        printf("%c-%d ", p->letra, p->frequencia);
        p = p->proximo;
    }
    printf("\n");
}
/**
* Funcao Montar Arvore de Huffman
* @brief Funcao gera uma arvore de huffman produzindo todos os nos da mesma
* Funcao que possui como entrada uma arvore @param h que atraves da lista encadeada que a variavel h possui monta uma arvore,
* removendo os elementos de menor frequencia no processo e unindo-os em um novo no x que representa a soma das frequencias dos nos
* de menor frequencia, informando tambem os lados que os nos removidos e retornados ao programa irao se encaixar na arvore. Portanto,
* percebe-se que essa funcao destroi a antiga lista encadeada e a remonta como a verdadeira arvore de huffman
*/
void montar_arvore_huffman (Huffman* h)
{
    while (h->tamanho>1)
    {
        No* s0 = remover_item_menor_frequencia(h);
        No* s1 = remover_item_menor_frequencia(h);
        No* x = criar_no();
        s0->lado = '0';
        s1->lado = '1';
        x->filhoesq = s0;
        x->filhodir = s1;
        x->frequencia = s0->frequencia + s1->frequencia;
        inserir_no(h, x);
    }
}
/**
* Função Caminho
* @brief Função que gera a codificação de cada letra
* Funcao que recebe como parametros um no @param no, um vetor de caracteres @param str, um inteiro @param i e um caractere @param c, que sao utilizados para gerar
* a codificação dos bits relativos a cada caractere da string a ser comprimida, em que um @return int é retornado para o usuário dependendo de cada caso de verificacao
* no primeiro caso se o no for nulo retorna-se 0, o que significa que a função nao foi executada com sucesso, já se o a letra contida no nó for diferente de zero e
* a letra for igual ao caractere c o vetor de string ira receber o valor correspondente ao lado podendo ser zero para direita e um para esquerda, contudo se nenhuma
* condição foi atendida a função é chamada recursivamente até satisfazer a condição anterior, gerando assim a codificação
*/
int caminho (No* no, char* str, int i, char c)
{
    if (no == NULL)
    {
        return 0;
    }
    else if (no->letra!=0 && no->letra == c)
    {
        str[i] = no->lado;
        return 1;
    }
    else
    {
        int n = caminho(no->filhoesq, str, i+1, c);
        if (n==0)
        {
            n = caminho(no->filhodir, str, i+1, c);
            if (n==0)
            {
                return 0;
            }
            else
            {
                str[i] = no->lado;
                return 1;
            }
        }
        else
        {
            str[i] = no->lado;
            return 1;
        }
    }
}
/**
* Função Zerar Palavra
* @brief Funcao que preenche com zeros uma string
* Funcao que dado um char @param str de tamanho igual a MIN, preenche-o com multiplos zeros
*/
void zerar_palavra(char* str)
{
    int i;
    for (i=0; i<MIN; i++)
    {
        str[i] = 0;
    }
}
/**
* Função Zerar Carateres
* @brief Funcao que preenche com zeros a string que contem os bits do arquivo
* Como cada byte pode ser espresso por 8 bits, essa funcao preenche um vetor de 1 byte com zeros  
*/
void zerar_caractere(char str[])
{
    int i;
    for (i=0; i<9; i++)
    {
        str[i] = 0;
    }
}
/**
* Função Construir o Codigo da Tabela
* @brief Funcao que gera o codigo que ira ser inserido na tabela para posterior uso de codificacao e decodificacao
*	Dada a entrada de uma árvore de huffman @param h e uma matriz de caracteres @param tabela[][MIN] é posivel gerar uma codificacao para ser reaproveitada posteriormente
* em uma tabela, essa tabela é gerada percorrendo-se a arvore de huffman com a função caminho (@see caminho) e atraves dela anotar a letra e o codigo correspondente a essa letra
* na matriz de caracteres
*/
void construir_tabela_codigo (Huffman* h, char tabela[][MIN])
{
    int i = 0, j = 0;
    char str[MIN];
    zerar_palavra(str);
    for (i=1; i<TOTSIM; i++)
    {
        if (h->frequencia_letras[i]>0)
        {
            caminho(h->cabeca->proximo, str, 0, (char) i);
            for (j=1; str[j]!=0; j++)
            {
                tabela[i][j-1] = str[j];
            }
            zerar_palavra(str);
        }
    }
}
/**
* Função Gerar Codigo
* @brief Funcao que transforma 8 bits de dados em 1 byte 
* Atraves dessa funcao torna-se possivel comprimir os dados, pois a mesma transforma os 8 bits da string codificada em um unico byte que é obtido pela transformacao
* dos 8 bits em decimal e a conversao desse numero decimal em um char da tabela ASCII
*/
unsigned char gerar_codigo(char s_aux[])
{
    unsigned char s = 0;
    if(s_aux[0] == '1')
      s += 128;
    if(s_aux[1] == '1') 
      s += 64;
    if(s_aux[2] == '1') 
      s += 32;
    if(s_aux[3] == '1')
      s += 16;
    if(s_aux[4] == '1') 
      s += 8;
    if(s_aux[5] == '1')
      s += 4;
    if(s_aux[6] == '1') 
      s += 2;
    if(s_aux[7] == '1') 
      s += 1;

    return s;
}
/**
* Funcao Imprimir Codificado
* @brief Le cada caractere armazenado na matriz de texto, e busca uma codificacao correspondente ao caractere lido e a imprime no texto
* Antes de comecar a impressao no arquivo, um vetor e criado. Este contera a sequencia binaria final correspondente a todos os caracteres lidos do texto. 
* Logo apos, esta sequencia de 0's e 1's e dividida em grupos de 8 bits (1 byte). Cara grupo de 8 bits correspondera a um novo simbolo da tabela ASCII, os quais serao impressos no arquivo comprimido
* Desta forma, alem de diminuir os 8 bits fixos utilizados para representar cada letra do texto, o produto final de bits ainda e divido por oito para evitar que cada bit seja impresso como byte, prejudicando a compressao do algoritmo.
*/
void imprimir_codificado(Huffman* h, FILE* arq, char tabela[][MIN])
{
    int i = 0, j = 0, k, indice=0, total = 0;
    char cadeia_aux[9];
    unsigned char cadeia_aux2[TAM];
    cadeia_aux[8] = 0;
    unsigned char caractere;
    qtd_caracteres(h);
    for (i=0; h->texto[i][j]!=0; i++)
    {
        for (j=0; h->texto[i][j]!=0; j++)
        {
            h->tamanho_compressao_final += strlen(tabela[h->texto[i][j]]);
            for (k=0; tabela[h->texto[i][j]][k]!=0; k++)
            {
                cadeia_aux[indice] = tabela[h->texto[i][j]][k];
                if(indice == 7)
                {
                    caractere = gerar_codigo(cadeia_aux);
                    cadeia_aux2[total] = caractere;
                    total++;
                    zerar_caractere(cadeia_aux);
                    indice = 0;
                }
                else
                {
                    indice++;
                }
            }
        }
        j=0;
    }
    caractere = '\n';
    char outro[20];
    sprintf(outro, "%d", total);
    fwrite(&outro,sizeof(char),strlen(outro),arq);
    fwrite(&caractere,sizeof(char),1,arq);
    for(i=0; i<total; i++)
        fwrite(&cadeia_aux2[i],sizeof(unsigned char),1,arq);

    if(indice != 0)
    {
        i=0;
        for (i=0; cadeia_aux[i]!=0; i++)
            fwrite(&cadeia_aux[i],sizeof(char),1,arq);
    }

}
/**
* Função Imprimir o Codigo da Tabela
* @brief Funcao que imprime o codigo da tabela em um arquivo
* Visto que a tabela é necessária para recuperar os dados comprimidos, imprime-se a tabela no arquivo da string codificada para posterior uso, nesse processo escreve-se
* no arquivo o caracter seguido de seu codigo 
*/
void imprimir_tabela_codigo (FILE* arq, char tabela[][MIN])
{
    int i =0, tam;
    char c;
    for (i=0; i<TOTSIM; i++)
    {
        if (tabela[i][0]!=0)
        {
            c = (char) i;
            fwrite(&c, sizeof(char), 1, arq);
            tam = strlen(tabela[i]);
            fwrite (tabela[i] , sizeof(char), tam, arq);
            fwrite ("\n" , sizeof(char), 1, arq);
        }
    }
}
/**
* Funcao Zerar Tabela
* @brief Preenche cada posicao da tabela de codigo com caractere nulo
* Utilizada para inicializar todas as posicoes da tabela de codigo
*/
void zerar_tabela(char tabela[][MIN])
{
    int i = 0, j = 0;
    for(i=0; i < TOTSIM; i++)
    {
        for(j=0; j < MIN; j++)
        {
            tabela[i][j] = 0;
        }
    }
}
/**
* Funcao Imprimir Tabela
* @brief Imprime a tabela de codigo construida a partir do algoritmo de Huffman
* Utilizada apenas no desenvolvimento parcial do trabalho, com o intuito de concluir se a tabela de codigo gerada pelo programa era a correta
*/
void imprimir_tabela(char tabela[][MIN])
{
    int i = 0, j = 0;
    for(i=0; tabela[i][j] != 0; i++)
    {
        for(j=0; tabela[i][j] != 0; j++)
        {
            printf("%c", tabela[i][j]);
        }
        j = 0;
    }
}
/**
* Funcao Limpa Auxiliar
* @brief Preenche todas as posicoes de uma dada string com caractere nulo
* E utilizada na funcao @see decodificacao para eliminar o risco de que algum caractere de uma palavra anterior esteja presente na analise dos caracteres da palavra atual
*/
void limpa_auxiliar(char str[])
{
    int i = 0;
    for(i=0; i < MIN; i++)
    {
        str[i] = 0;
    }
}
/**
* Funcao Verifica String
* @brief Verifica se um determinado codigo esta associado a um caractere existente na tabela de codigo
* Esta funcao e utilizada na funcao @see decodificacao. Ela analisa o codigo atual contido na string str para concluir se ele esta relacionado a algum caractere da tabela de codigo. Caso esteja, o caractere correspondente e retornado. O caractere nulo e retornado caso o codigo nao esteja relacionado a nenhum caractere da tabela de codigo
*/
char verifica_string(char str[], char tabela[][MIN] )
{
    int i=0, j=0;
    for(i = 0; tabela[i][j]!=0; i++)
    {
        for(j = 1; tabela[i][j] != 0  && str[j-1]!=0; j++)
        {
            if (tabela[i][j] == str[j-1])
            {
                if (tabela[i][j+1] == '\n')
                {
                    return tabela[i][0];
                }
            }
            else
            {
                j = 0;
                break;
            }
        }
    }
    return 0;
}
/**
* Funcao Decodificacao
* @brief Converte o vetor codificacao binario para uma sequencia de caracteres correspondentes aos grupos de 8 bits 
* Apos a funcao @see gera_binario ter lido e convertido os caracteres do arquivo comprimido para a sequencia binaria, a funcao atual e chamada para converter a sequencia binaria em caracteres comuns a fim de restaurar o arquivo original
*/
void decodificacao(char tabela[][MIN], char codificacao[TAM], char nome_original[])
{
    FILE * arq3 = fopen(nome_original, "w");
    int i = 0, j=0;
    char string_auxiliar[MIN], caractere;
    limpa_auxiliar(string_auxiliar);
    for(i = 0; codificacao[i] != 0; i++)
    {
        string_auxiliar[j] = codificacao[i];
        caractere = verifica_string(string_auxiliar, tabela);
        if (caractere != 0)
        {
            //putchar(caractere);
            fprintf(arq3,"%c", caractere);
            limpa_auxiliar(string_auxiliar);
            j = 0;
        }
        else
        {
            j++;
        }
    }
    fclose(arq3);
}
/**
* Funcao Decimal Binario
* @brief Converte a codificacao decimal de um caractere da tabela ASCII para a codificacao binaria
* Esta funcao e utilizada na funcao @see gera_binario quando e necessario que converter a codificacao de um caractere lido do arquivo comprimido para a base binaria
*/
void dec_bin (int num, char vetor[])
{
    char binario[9];
    binario[8] = 0, vetor[8] = 0;
    int i = 0, x = 0, a;
    if (num == 0)
    {
        for (x=i; x<8; x++)
        {
            vetor[x]='0';
        }
    }
    else
    {
        while (num!=0)
        {
            a = num % 2;
            if (a==0)
            {
                binario[i]= '0';
            }
            else
            {
                binario[i]= '1';
            }
            num=num/2;
            i++;
            if (num==0)
            {
                if (i!=8)
                {
                    a = 8-i;
                    for (x=0; x<a; x++)
                    {
                        vetor[x]='0';
                    }
                }
            }
        }
        a = i-1;
        i--;
        for(a=x; a<8; a++)
        {
            vetor[a]=binario[i];
            i--;
        }
    }
}
/**
* Funcao Gerar Binario
* @brief Converte cada caractere impresso no arquivo comprimido para a codificacao binaria correspondente ao seu numero na tabela ASCII 
* Os numeros binarios correspondentes a cada caractere sao colocados na sequencia em que aparecem num vetor maior. Este sera usado na decodificacao e restauracao do arquivo original
*/
void gerar_binario (FILE* arq, char codificacao[], int tamanho)
{
    int i=0, j=0, total = 0;
    char vetor[9], c;
    unsigned char caractere;
    while (!feof(arq))
    {
        if(total >= tamanho)
        {
            fread(&c,sizeof(char),1,arq);
            codificacao[i] = c;
            i++;
        }
        else
        {
            fread(&caractere,sizeof(unsigned char),1,arq);
            dec_bin((int) caractere, vetor);
            for (j=0; vetor[j]!=0; j++)
            {
                codificacao[i] = vetor[j];
                i++;
            }
            if(j == 8)
            {
                for(j=0; j<9; j++)
                    vetor[j] = 0;
            }
            total++;
        }
    }
    codificacao[i]=0;
}
/**
* Funcao Principal do Codigo 
* Primeiramente, verifica-se o numero de argumentos de entrada para assim decidir se o codigo entrará na funcao de codificacao, caso hajam tres argumentos
* e decodificacao, caso hajam dois argumentos, apos isso o algoritmo comeca a executar diversas funcoes em que para a codificacao imprime em um arquivo a 
* tabela, o codigo codificado e o nome original e na decodificacao recupera esses dados e os aplica para gerar um novo arquivo com o nome do arquivo original
* e com o conteudo original
*/
int main (int argc, char* argv[])
{

    FILE* arq;
    FILE* arq_comprimido;
    Huffman* huffman;
    char tabela[TOTSIM][MIN];
    zerar_tabela(tabela);
	int inicio, fim;
    if (argc>1 && argc<4)
    {
        if (argc==3)
        {
            arq = fopen(argv[1], "r");
        }
        else if (argc==2)
        {
            arq = fopen(argv[1],"rb");
        }
        else
        {
            puts("Argumentos invalidos!");
            return 0;
        }
        if(argc == 3)
        {
            int i,j;
            zerar_tabela(tabela);

            if (arq!=NULL)
            {
				inicio = GetTickCount();
                huffman = criar_arvore_huffman();
                frequencia_texto_arvore(huffman, arq);
                criar_nos_folhas(huffman);
                montar_arvore_huffman(huffman);
                construir_tabela_codigo(huffman, tabela);
                arq_comprimido = fopen(argv[2], "wb");
                imprimir_tabela_codigo(arq_comprimido, tabela);
                fwrite("\n", sizeof(char),1,arq_comprimido);
                i = strlen(argv[1]);
                fwrite (argv[1] , sizeof(char), i, arq_comprimido);
                fwrite("\n", sizeof(char),1,arq_comprimido);
                imprimir_codificado(huffman, arq_comprimido, tabela);
				fim = GetTickCount();
                printf("Porcentagem de compactacao %.2f\n", (float) (1-(huffman->tamanho_compressao_final/(huffman->caracteres*8)))*100);
				printf("Tempo computacional: %d\n", fim-inicio);
            }
            else
            {
                puts("Arquivo nao encontrado!");
                return 0;
            }

            fclose(arq);
            fclose(arq_comprimido);
        }
        else if(argc == 2)
        {
            if (arq==NULL)
            {
                puts("Arquivo nao encontrado!");
                return 0;
            }
            int i = 0, j = 0;
            char caractere, caractere_antigo = 0;
            int okay = 1;
            while(!feof(arq))
            {
                fread(&caractere,sizeof(char),1,arq);
                if (caractere_antigo == '\n' && caractere == '\n')
                {
                    break;
                }
                if(caractere == '\n' && okay == 1)
                {
                    tabela[i][j] = caractere;
                    j++;
                    okay = 0;
                }
                else if(caractere == '\n')
                {
                    tabela[i][j] = caractere;
                    i++;
                    j = 0;
                }
                else
                {
                    tabela[i][j] = caractere;
                    j++;
                }
                caractere_antigo = caractere;
            }
            char nome_original[50];
            char codificacao[TAM];
            i=0;
            while(!feof(arq))
            {
                fread(&caractere,sizeof(char),1,arq);
                if (caractere!='\n')
                {
                    nome_original[i] = caractere;
                }
                else
                {
                    break;
                }
                i++;
            }
            nome_original[i+1] = 0;
            char total_char[MIN];
            i= 0;
            while(!feof(arq))
            {
                fread(&caractere,sizeof(char),1,arq);
                if (caractere!='\n')
                {
                    total_char[i] = caractere;
                }
                else
                {
                    break;
                }
                i++;
            }
            total_char[i+1] = 0;
            int total_int = atoi(total_char);
            gerar_binario(arq, codificacao, total_int);
            decodificacao(tabela, codificacao, nome_original);
            fclose(arq);
        }
    } else 
	{
		puts("Argumentos invalidos!");
	}
	
	
	
	return 0;
}
