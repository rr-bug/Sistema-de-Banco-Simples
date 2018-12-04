#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<locale.h>
#include<time.h>

int tCli=0,tCont=0;

typedef struct{
    char logradouro[100];
    int numero;
    char bairro[100];
    char cep[20];
    char cidade[100];
    char uf[3];
}endereco;

typedef struct{
    char codigo[100];
    char cpf[20];
    char rg[20];
    char nome[100];
    endereco end;
}cliente;

typedef struct{
    char numero[100];
    cliente cl;
    float saldo;
    float limite;
}conta;

void menu();
int buscaNumeroContaCliente(conta *);
int verificaSacaValor(conta *, int, float);
void cadastrar_clientes(cliente *, int);
void cadastrar_contas(conta *, int, cliente *);
int excluir_contas(conta *, cliente *);
void depositar(conta *, cliente *, FILE *);
void sacar(conta *, cliente *, FILE *);
int transferir(conta *, cliente *, FILE *);
void pagar_boleto(conta *, cliente *, FILE *);
void imprimir_saldo(conta *);
void imprimir_extrato(conta *, FILE *, FILE *, FILE *, FILE *);

int main(void)
{
    int opcao, cadastro, quantCli, simNao, quantCont, testeCli=0, testeCont=0,i;
    conta *cont;
    cliente *cli;
    FILE *deposito, *saque, *transferencia, *pagamento_boleto;

    deposito = fopen("deposito.txt", "w");
    saque = fopen("saque.txt", "w");
    transferencia = fopen("transferencia.txt", "w");
    pagamento_boleto = fopen("pagamento_boleto.txt", "w");

    do{
        menu();
        scanf("%d",&opcao);
        switch(opcao)
        {
            case 1:
                printf("\n1 - Cadastrar Cliente\n");
                printf("2 - Cadastrar Conta\n");
                scanf("%d",&cadastro);
                if(cadastro==1)
                {
                    printf("Digite a quantidade de clientes: \n");
                    scanf("%d",&quantCli);
                    tCli = tCli + quantCli;
                    if(testeCli>=1)
                    {
                        cli = (cliente *) realloc(cli,tCli * (sizeof(cliente)));
                        if(!cli)
                        {
                            printf("A alocação falhou!\n",setlocale(LC_ALL,""));
                            exit (-1);
                        }
                    }
                    else
                    {
                        cli = (cliente *) malloc(quantCli*sizeof(cliente));
                        if(!cli)
                        {
                            printf("A alocação falhou!\n",setlocale(LC_ALL,""));
                            exit (-1);
                        }
                        testeCli++;
                    }
                    cadastrar_clientes(cli,quantCli);
                }
                else if(cadastro==2)
                {
                    printf("Digite a quantidade de contas: \n");
                    scanf("%d",&quantCont);
                    tCont = tCont + quantCont;
                    if(testeCont>=1)
                    {
                        cont = (conta *) realloc(cont,tCont * (sizeof(conta)));
                        if(!cont)
                        {
                            printf("A alocação falhou!\n",setlocale(LC_ALL,""));
                            exit (-1);
                        }
                    }
                    else
                    {
                        cont = (conta *) malloc(quantCont*sizeof(conta));
                        if(!cont)
                        {
                            printf("A alocação falhou!\n",setlocale(LC_ALL,""));
                            exit (-1);
                        }
                        testeCont++;
                    }
                    cadastrar_contas(cont,quantCont,cli);
                }
                else
                {
                    printf("\n                                Digitou Errado \n");
                }
            break;
            case 2:
                simNao = excluir_contas(cont,cli);
                if(simNao == 1)
                {
                    tCont--;
                    printf("\n                                Exclusão Finalizada \n",setlocale(LC_ALL,""));
                }
                else
                {
                    printf("\n                                Exclusão Não Finalizada \n",setlocale(LC_ALL,""));
                }
            break;
            case 3:
                depositar(cont, cli, deposito);
            break;
            case 4:
                sacar(cont, cli, saque);
            break;
            case 5:
                simNao = transferir(cont, cli, transferencia);
                if(simNao == 1)
                {
                    printf("\n                                Transferência Finalizada \n",setlocale(LC_ALL,""));
                }
                else
                {
                    printf("\n                                Transferência Não Finalizada \n",setlocale(LC_ALL,""));
                }
            break;
            case 6:
                pagar_boleto(cont, cli, pagamento_boleto);
            break;
            case 7:
                imprimir_saldo(cont);
            break;
            case 8:
                imprimir_extrato(cont, deposito,saque,pagamento_boleto,transferencia);
            break;
            case 0:
                    printf("\n                                Programa Finalizado \n\n");
            break;
            default:
                    printf("\n                                Digitou Errado \n\n");
            break;
        }
    }while(opcao!=0);

    fclose(deposito);
    fclose(saque);
    fclose(transferencia);
    fclose(pagamento_boleto);

    free(cli);
    free(cont);
    system("pause");
    return(0);
}

void menu()
{
    printf("\n                      -------------- Sistema Bancário ------------\n\n",setlocale(LC_ALL,""));
    printf("                              1 - Cadastrar Conta ou Cliente\n");
    printf("                              2 - Excluir Conta \n");
    printf("                              3 - Depósito \n",setlocale(LC_ALL,""));
    printf("                              4 - Saque \n");
    printf("                              5 - Transferência \n",setlocale(LC_ALL,""));
    printf("                              6 - Pagamento de Boleto \n");
    printf("                              7 - Imprimir Saldo \n");
    printf("                              8 - Imprimir Extrato\n");
    printf("                              0 - Sair \n");
    printf("\n                      --------------------------------------------\n\n");
}

int buscaNumeroContaCliente(conta *co)
{
    char codig[100];
    int j,pos=-1;

    printf("Digite o número da conta do cliente: \n",setlocale(LC_ALL,""));
    scanf(" %[^\n]",codig);
        if(tCont > 0)
        {
            for(j=0;j<tCont;j++)
            {
                if (strcmp(codig,co[j].numero) == 0)
                {
                    pos = j;
                    return pos;
                }
            }
            if(pos<0)
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
}

int verificaSacaValor(conta *con,int p, float v)
{
    float a,d;
                if((con[p].saldo + con[p].limite) >= v)
                {
                    if(con[p].saldo==0)
                    {
                        if(con[p].limite>=v)
                        {
                            con[p].limite -= v;
                            return 1;
                        }
                        else
                        {
                            return 0;
                            printf("\n                                Sem saldo e sem limite\n");
                        }
                    }
                    else
                    {
                        a = con[p].saldo - v;
                        if(a>=0)
                        {
                            con[p].saldo -= v;
                            return 1;
                        }
                        else
                        {
                            a = a * (-1);
                            d = v - a;
                            if(con[p].limite>=a)
                            {
                                con[p].saldo -= d;
                                con[p].limite -= a;
                                return 1;
                            }
                            else
                            {
                                return 0;
                                printf("\n                                Sem limite para sacar o dinheiro completo\n");
                                printf("\n                                Seu saldo atual: %f e Limite atual: %f\n",con[p].saldo,con[p].limite);
                            }
                        }
                    }
                }
                else
                {
                    return 0;
                    printf("\n                                Saque Não Concluído\n",setlocale(LC_ALL,""));
                }
}

void cadastrar_clientes(cliente *cl, int n)
{
    system("cls");
    int i,k,a=0,qtdNome,qtdCPF,qtdC;
    for(i = tCli-n;i<tCli;i++)
    {
        printf("Digite o código do cliente:\n",setlocale(LC_ALL,""));
        scanf(" %[^\n]",cl[i].codigo);
        printf("Digite o CPF do cliente:\n");
        scanf(" %[^\n]",cl[i].cpf);
        printf("Digite o RG do cliente:\n");
        scanf(" %[^\n]",cl[i].rg);
        printf("Digite o nome do cliente:\n");
        scanf(" %[^\n]",cl[i].nome);
        printf("Digite o logradouro do cliente:\n");
        scanf(" %[^\n]",cl[i].end.logradouro);
        printf("Digite o número do cliente:\n",setlocale(LC_ALL,""));
        scanf("%d",&cl[i].end.numero);
        printf("Digite o bairro do cliente:\n");
        scanf(" %[^\n]",cl[i].end.bairro);
        printf("Digite o CEP do cliente:\n");
        scanf(" %[^\n]",cl[i].end.cep);
        printf("Digite o cidade do cliente:\n");
        scanf(" %[^\n]",cl[i].end.cidade);
        printf("Digite o UF do cliente:\n");
        scanf(" %[^\n]",cl[i].end.uf);

        /*
        qtdNome = (strlen(cl[i].nome))-1;
        qtdCPF = (strlen(cl[i].cpf));
        cl[i].codigo[0] = cl[i].rg[0];
        printf("\n%s\n",cl[i].codigo);
        cl[i].codigo[1] = cl[i].rg[2];
        printf("\n%s\n",cl[i].codigo);
        cl[i].codigo[2] = cl[i].rg[4];
        printf("\n\n%s\n\n",cl[i].codigo);
        cl[i].codigo[3] = cl[i].nome[0];
        printf("\n%s\n",cl[i].codigo);
        cl[i].codigo[4] = cl[i].nome[qtdNome];
        printf("\n%s\n",cl[i].codigo);
        strcat(cl[i].codigo, cl[i].cpf);
        qtdC = (strlen(cl[i].codigo));
        cl[i].codigo[qtdC] = '\0';
        printf("\n%s\n",cl[i].codigo);
        */
    }
    system("cls");
}

void cadastrar_contas(conta *co, int n, cliente *cl)
{
    system("cls");
    char codig[100], testeN[100]="";
    int i,j,pos=-1;

    for(i=tCont-n;i<tCont;i++)
    {
        printf("Digite o código do cliente: \n",setlocale(LC_ALL,""));
        fflush(stdin);
        scanf(" %[^\n]",codig);
        if(tCli > 0)
        {
            for(j=0;j<tCli;j++)
            {
                if (strcmp(codig,cl[j].codigo) == 0)
                {
                    pos = j;
                }
            }
        }
        else
        {
            printf("\n                                Sem clientes cadastrados\n");
        }
        if(pos >= 0)
        {
            snprintf(co[i].numero, 10, "%s%d", testeN, i+1);
            co[i].cl = cl[pos];
            printf("Seu saldo começa com R$ 0.0:\n",setlocale(LC_ALL,""));
            co[i].saldo = 0.0;
            printf("Seu limite começa com R$ 1000.0:\n",setlocale(LC_ALL,""));
            co[i].limite = 1000.0;
        }
        else
        {
            printf("\n            Sem código do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
        }
    }
}

int excluir_contas(conta * co, cliente *cl)
{
    system("cls");
    float valor,d,e,f;
    int pos,pos2,p,o,teste1,teste2;

    time_t dataAtual;
    dataAtual = time(NULL);
    struct tm tm = *localtime(&dataAtual);
    int dia,mes,ano;
    dia = tm.tm_mday;
    mes = tm.tm_mon+1;
    ano = tm.tm_year+1900;

    pos = buscaNumeroContaCliente(co);

    if(pos >= 0)
    {
        if(co[pos].limite==1000)
        {
            if(co[pos].saldo==0)
            {
                for(p = pos; p < tCont; p++)
                {
                    co[p] = co[p+1];
                }
                return 1;
            }
            else
            {
                printf("Deseja fazer? \n");
                printf("1 - Saque\n");
                printf("2 - Transferência\n",setlocale(LC_ALL,""));
                scanf("%d",&o);
                valor = co[pos].saldo;
                if(o==1)
                {
                    teste1 = verificaSacaValor(co,pos,co[pos].saldo);
                    if(teste1==1)
                    {
                        for(p = pos; p < tCont; p++)
                        {
                            co[p] = co[p+1];
                        }
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else if(o==2)
                {
                    pos2 = buscaNumeroContaCliente(co);
                    if(pos2>=0)
                    {
                        teste2 = verificaSacaValor(co,pos,co[pos].saldo);
                        if(co[pos2].limite<1000)
                        {
                            d = co[pos2].limite + valor;
                            e = d - 1000;
                            if(d>=1000)
                            {
                                f = 1000 - co[pos2].limite;
                            }
                            else
                            {
                                f = d - co[pos2].limite;
                            }
                            co[pos2].limite += f;
                            if(e>=0)
                            {
                                co[pos2].saldo += e;
                            }
                        }
                        else
                        {
                            co[pos2].saldo += valor;
                        }
                        if(teste2==1)
                        {
                            for(p = pos; p < tCont; p++)
                            {
                                co[p] = co[p+1];
                            }
                            return 1;
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    else
                    {
                        return 0;
                        printf("\n\nNão encontrou posição da conta no banco de dados!\n\n",setlocale(LC_ALL,""));
                    }
                }
                else
                {
                    return 0;
                    printf("\n                                Digitou Errado \n");
                }
            }
        }
        else
        {
                    return 0;
                    printf("\n                                Para excluir conta tem que resolver seu limite \n");
        }
    }
    else
    {
        return 0;
        printf("\n\nNão encontrou posição da conta no banco de dados!\n\n",setlocale(LC_ALL,""));
    }
}

void depositar(conta *co, cliente *cl, FILE *arq)
{
    system("cls");
    float valor,d,f,e;
    int opcao,pos;

    time_t dataAtual;
    dataAtual = time(NULL);
    struct tm tm = *localtime(&dataAtual);
    int dia,mes,ano;
    dia = tm.tm_mday;
    mes = tm.tm_mon+1;
    ano = tm.tm_year+1900;

    pos = buscaNumeroContaCliente(co);

        if(pos >= 0)
        {
            printf("Nome: %s    Código: %s    CPF: %s\n", co[pos].cl.nome,co[pos].cl.codigo, co[pos].cl.cpf,setlocale(LC_ALL,""));
            printf("Se são essas informações, deseja confirmar o depósito? \n",setlocale(LC_ALL,""));
            printf("1 - Sim\n");
            printf("0 - Sair\n");
            scanf("%d",&opcao);
            if(opcao==1)
            {
                printf("Digite o valor a ser depositado: \n");
                scanf("%f",&valor);
                if(co[pos].limite<1000)
                {
                    d = co[pos].limite + valor;
                    e = d - 1000;
                    if(d>=1000)
                    {
                        f = 1000 - co[pos].limite;
                    }
                    else
                    {
                        f = d - co[pos].limite;
                    }
                    co[pos].limite += f;
                    if(e>=0)
                    {
                        co[pos].saldo += e;
                    }
                }
                else
                {
                    co[pos].saldo += valor;
                }
                fprintf(arq,"%s;%d;%d;%d;%s;%s;%f;%f;%f\n", co[pos].numero, dia, mes, ano, co[pos].cl.nome,co[pos].cl.codigo,co[pos].saldo,valor, co[pos].limite);
                printf("\n                                Depósito Concluído\n",setlocale(LC_ALL,""));
            }
            else if(opcao==0)
            {
                printf("\n                                Depósito cancelado\n",setlocale(LC_ALL,""));
            }
            else
            {
                printf("\n                                Digitou errado\n");
            }
        }
        else
        {
            printf("\n            Sem número da conta do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
        }
}

void sacar(conta *co, cliente *cl, FILE *arq)
{
    system("cls");
    int pos,teste;
    float valor, a,d;

    time_t dataAtual;
    dataAtual = time(NULL);
    struct tm tm = *localtime(&dataAtual);
    int dia,mes,ano;
    dia = tm.tm_mday;
    mes = tm.tm_mon+1;
    ano = tm.tm_year+1900;

    pos = buscaNumeroContaCliente(co);

        if(pos >= 0)
        {
                printf("Digite o valor a sacar: \n");
                scanf("%f",&valor);

                teste = verificaSacaValor(co,pos,valor);

                if(teste==1)
                {
                    fprintf(arq,"%s;%d;%d;%d;%s;%s;%f;%f;%f\n", co[pos].numero, dia, mes, ano, cl[pos].nome,cl[pos].codigo,co[pos].saldo,valor, co[pos].limite);
                    printf("\n                                Saque Concluída\n",setlocale(LC_ALL,""));
                }
                else
                {
                    printf("\n                                Saque Não Concluída\n",setlocale(LC_ALL,""));
                }
        }
        else
        {
            printf("\n            Sem número da conta do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
        }
}

int transferir(conta *co, cliente *cl, FILE *arq)
{
    system("cls");
    float valor,d,e,f;
    int pos1, pos2,teste;

    time_t dataAtual;
    dataAtual = time(NULL);
    struct tm tm = *localtime(&dataAtual);
    int dia,mes,ano;
    dia = tm.tm_mday;
    mes = tm.tm_mon+1;
    ano = tm.tm_year+1900;

    pos1 = buscaNumeroContaCliente(co);
    pos2 = buscaNumeroContaCliente(co);

        if(pos1 >= 0 && pos2 >= 0)
        {
                printf("Digite o valor a transferir: \n");
                scanf("%f",&valor);

                teste = verificaSacaValor(co,pos1,valor);
                if(co[pos2].limite<1000)
                {
                    d = co[pos2].limite + valor;
                    e = d - 1000;
                    if(d>=1000)
                    {
                        f = 1000 - co[pos2].limite;
                    }
                    else
                    {
                        f = d - co[pos2].limite;
                    }
                    co[pos2].limite += f;
                    if(e>=0)
                    {
                        co[pos2].saldo += e;
                    }
                }
                else
                {
                    co[pos2].saldo += valor;
                }
                if(teste==1)
                {
                    fprintf(arq,"%s;%d;%d;%d;%s;%s;%f;%f;%f;%s;%s;%s;%f;%f;%f\n", co[pos1].numero, dia,mes, ano, co[pos1].cl.nome,co[pos1].cl.codigo,co[pos1].saldo,valor, co[pos1].limite,co[pos2].numero, co[pos2].cl.nome,co[pos2].cl.codigo,co[pos2].saldo,valor, co[pos2].limite);
                    return 1;
                }
                else
                {
                    return 0;
                }
        }
        else
        {
            return 0;
            printf("\n            Sem número da conta do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
        }
}

void pagar_boleto(conta *co, cliente *cl, FILE *arq)
{
    system("cls");
    char codFatura[100];
    int pos,teste;
    float valor,a,d;

    time_t dataAtual;
    dataAtual = time(NULL);
    struct tm tm = *localtime(&dataAtual);
    int dia,mes,ano;
    dia = tm.tm_mday;
    mes = tm.tm_mon+1;
    ano = tm.tm_year+1900;

    pos = buscaNumeroContaCliente(co);

        if(pos >= 0)
        {
                printf("Digite o código da fatura: \n",setlocale(LC_ALL,""));
                fflush(stdin);
                scanf(" %[^\n]",codFatura);
                printf("Digite o valor a pagar o boleto: \n");
                scanf("%f",&valor);

                teste = verificaSacaValor(co,pos,valor);

                if(teste==1)
                {
                    fprintf(arq,"%s;%d;%d;%d;%s;%s;%s;%f;%f;%f\n", co[pos].numero, dia, mes, ano, codFatura, co[pos].cl.nome, co[pos].cl.codigo,co[pos].saldo,valor, co[pos].limite);
                    printf("\n                                Pagamento Concluído\n",setlocale(LC_ALL,""));
                }
                else
                {
                    printf("\n                                Pagamento Não Concluída\n",setlocale(LC_ALL,""));
                }
        }
        else
        {
            printf("\n            Sem número da conta do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
        }
}

void imprimir_saldo(conta *co)
{
    system("cls");
    int pos;
    char num[100];
    float soma;

    pos = buscaNumeroContaCliente(co);

        if(pos>=0)
        {
            soma = co[pos].limite + co[pos].saldo;
            printf("Saldo: %.2f\nSaldo + Limite: %.2f\n",co[pos].saldo,soma);
        }
        else
        {
            printf("\n            Sem código da conta do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
        }
}

void imprimir_extrato(conta *co, FILE *arq, FILE *arq1, FILE *arq2, FILE *arq3)
{
    system("cls");
    //inicio_deposito
    int pos,dia,mes,ano;
    char a[100],b[100],c[100],g[100],c1[100],n2[100],c2[100];
    float d,e,f,s1,v1,l1;

    fclose(arq);
                arq = fopen("deposito.txt","r");

                pos = buscaNumeroContaCliente(co);

                    if(pos>=0)
                    {
                        printf("\n-------------------Início Depósitos-------------------\n",setlocale(LC_ALL,""));
                        //!feof(arq)
                        while((fscanf(arq," %[^;];%d;%d;%d; %[^;]; %[^;];%f;%f;%f\n",&a,&dia,&mes,&ano,&b,&c,&d,&e,&f)) != EOF)
                        {
                            if(strcmp(co[pos].numero,a)==0)
                            {
                                printf("%d/%d/%d\n",dia,mes,ano);
                                printf("Código: %s\n Nome: %s\n Código: %s\n Saldo: %.2f\n Valor: %.2f\n Limite: %.2f\n",a,b,c,d,e,f,setlocale(LC_ALL,""));
                            }
                        }
                        printf("\n-------------------Fim Depósitos---------------------\n",setlocale(LC_ALL,""));
                    }
                    else
                    {
                        printf("\n            Sem código da conta do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
                    }
                fclose(arq);
    //fim_deposito
    //inicio_saque
    fclose(arq1);
                arq1 = fopen("saque.txt","r");

                    if(pos>=0)
                    {
                        printf("\n-------------------Início Saques-------------------\n",setlocale(LC_ALL,""));
                        while((fscanf(arq1," %[^;];%d;%d;%d; %[^;]; %[^;];%f;%f;%f\n",&a,&dia,&mes,&ano,&b,&c,&d,&e,&f)) != EOF)
                        {
                            if(strcmp(co[pos].numero,a)==0)
                            {
                                printf("%d/%d/%d\n",dia,mes,ano);
                                printf("Código: %s\n Nome: %s\n Código: %s\n Saldo: %.2f\n Valor: %.2f\n Limite: %.2f\n",a,b,c,d,e,f,setlocale(LC_ALL,""));
                            }
                        }
                        printf("\n-------------------Fim Saques---------------------\n");
                    }
                    else
                    {
                        printf("\n            Sem código da conta do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
                    }
                fclose(arq1);
    //fim_saque
    //inicio_transferencia
    fclose(arq3);
                arq3 = fopen("transferencia.txt","r");

                    if(pos>=0)
                    {
                        printf("\n-------------------Início Transferências-------------------\n",setlocale(LC_ALL,""));
                        while((fscanf(arq3," %[^;];%d;%d;%d; %[^;]; %[^;];%f;%f;%f; %[^;]; %[^;]; %[^;];%f;%f;%f\n",&a,&dia,&mes,&ano,&b,&c,&d,&e,&f,&c1,&n2,&c2,&s1,&v1,&l1)) != EOF)
                        {
                            if(strcmp(co[pos].numero,a)==0)
                            {
                                printf("%d/%d/%d\n",dia,mes,ano);
                                printf("Código: %s\n Nome: %s\n Código: %s\n Saldo: %.2f\n Valor: %.2f\n Limite: %.2f\n",a,b,c,d,e,f,setlocale(LC_ALL,""));
                                printf("Código: %s\n Nome: %s\n Código: %s\n Saldo: %.2f\n Valor: %.2f\n Limite: %.2f\n",c1,n2,c2,s1,v1,l1,setlocale(LC_ALL,""));
                            }
                        }
                        printf("\n-------------------Fim Transferências---------------------\n",setlocale(LC_ALL,""));
                    }
                    else
                    {
                        printf("\n            Sem código da conta do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
                    }
                fclose(arq3);
    //fim_transferencia
    //inicio_pagamento_boleto
    fclose(arq2);
                arq2 = fopen("pagamento_boleto.txt","r");

                    if(pos>=0)
                    {
                        printf("\n-------------------Início Pagamentos Boletos-------------------\n",setlocale(LC_ALL,""));
                        while((fscanf(arq2," %[^;];%d;%d;%d; %[^;]; %[^;]; %[^;];%f;%f;%f\n",&a,&dia,&mes,&ano,&b,&c,&g,&d,&e,&f))!=EOF)
                        {
                            if(strcmp(co[pos].numero,a)==0)
                            {
                                printf("%d/%d/%d\n",dia,mes,ano);
                                printf("Código: %s\n Nome: %s\n Código: %s\nCodigo Fatura: %s\n Saldo: %.2f\n Valor: %.2f\n Limite: %.2f\n",a,b,g,c,d,e,f,setlocale(LC_ALL,""));
                            }
                        }
                        printf("\n-------------------Fim Pagamentos Boletos---------------------\n");
                    }
                    else
                    {
                        printf("\n            Sem código da conta do cliente cadastrado ou encontrado\n",setlocale(LC_ALL,""));
                    }
                fclose(arq2);
    //fim_pagamento_boleto
}


