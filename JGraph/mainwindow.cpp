#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>


int contarColuna(FILE *arqle);
int contarPalavra(FILE *arqle);
double temp(double temperatura, double pressao, double Ar ,double CH4, double H2, double flux);
void trans (char ***matriz, int linhas, int colunas);
int control = 0; // controle quando variar concentração para aparecer botão

QString arq="spec.out";
QCPLayoutGrid *subLayout;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Salvar Imagem
void MainWindow::on_actionSalvar_triggered()
{
    QString fileName=QFileDialog::getSaveFileName(
                this,
                tr("Salvar"),
                "C://"
                 ,"PNG(*.png);;JPG(*.jpg);;PDF(*.pdf);;BMP(*.bmp)"
                );

    if(!fileName.isEmpty())
        {
            if(fileName.endsWith(".png", Qt::CaseInsensitive))
            {
                ui->customPlot->savePng(fileName);
            }
            else if(fileName.endsWith(".jpg", Qt::CaseInsensitive))
            {
                ui->customPlot->saveJpg(fileName);
            }
            else if(fileName.endsWith(".pdf", Qt::CaseInsensitive))
            {
                ui->customPlot->savePdf(fileName);
            }
            else if(fileName.endsWith(".bmp", Qt::CaseInsensitive))
            {
                ui->customPlot->saveBmp(fileName);
            }
            else
            {
                fileName += ".png";
                ui->customPlot->savePng(fileName);
            }
        }

}

//Salvar arquivo
void MainWindow::on_actionSalvar_Arq_triggered()
{
    int i, j;
    FILE *arq;
    int linhas = ui->tableWidget->verticalHeader()->count();
    qDebug("\n %d", linhas);
    int colunas = ui->tableWidget->horizontalHeader()->count();
     qDebug("\n %d", colunas);
    QString fileName=QFileDialog::getSaveFileName(
                this,
                tr("Salvar"),
                "C://"
                 ,"DAT(*.dat)"
                );

    // Convert Qstring para const *char
    char* cstr;
    char* valor;
    std::string fname = fileName.toStdString();
    cstr = new char [fname.size()+1];
    strcpy( cstr, fname.c_str() );

    //Salva no arquivo
    if((arq = fopen(cstr,"w"))==NULL){
        QMessageBox::information(0,"Info","Não encontrei o arquivo");
    }
    for ( i=0; i<linhas; i++ )
    {
      for ( j=0; j<colunas; j++ )
          {
          QString val = ui->tableWidget->model()->data(ui->tableWidget->model()->index(i,j)).toString();

          std::string fname2 = val.toStdString();
          valor = new char [fname2.size()+1];
          strcpy( valor, fname2.c_str() );

          fprintf(arq,"%s  ",valor);
         }
      fprintf(arq,"\n");
     }
    fclose(arq);
}

// Quando gerar gráfico for pressionado
void MainWindow::on_pushButton_clicked()
{
    double soma;
    if(ui->checkBox->checkState() || ui->checkBox_2->checkState()){
        double inicio;
        double fim;
        if(ui->checkBox->checkState()){
            inicio = ui->doubleSpinBox_8->value();
            fim = ui->doubleSpinBox_9->value();
        }else if(ui->checkBox_2->checkState()){
            inicio = ui->doubleSpinBox_10->value();
            fim = ui->doubleSpinBox_11->value();
        }
        double  Ar = (ui->doubleSpinBox_2->value())/100;
        double  CH4 = (ui->doubleSpinBox_3->value())/100;
        double  H2 = (ui->doubleSpinBox_4->value())/100;
        soma = Ar + CH4 + H2;

        if(fim>inicio){
            if(soma==1){
              GerarMatrizVariando();
            }else{
              QMessageBox::information(0,"Info","A soma das concentrações deve ser 100%");
            }

        }else{
            QMessageBox::information(0,"Info","A variação têm que ser maior que zero");
        }

    }else if(ui->checkBox_9->checkState()){
        double inicio = ui->doubleSpinBox_6->value();
        double fim = ui->doubleSpinBox_7->value();
       if(fim>inicio){
           if(ui->checkBox_4->checkState()){
             double  Ar = (ui->doubleSpinBox_2->value())/100;
             soma = Ar + (fim/100);
             if(soma<1){
              gerarMatrizConcentracao();
             }else{
                 QMessageBox::information(0,"Info","A soma das concentrações deve ser 100%");
               }
           }
           if(ui->checkBox_5->checkState()){
             double  CH4 = (ui->doubleSpinBox_3->value())/100;
             soma = CH4 + (fim/100);
             if(soma<1){
              gerarMatrizConcentracao();
             }else{
                 QMessageBox::information(0,"Info","A soma das concentrações deve ser 100%");
               }
           }
           if(ui->checkBox_7->checkState()){
             double  H2 = (ui->doubleSpinBox_4->value())/100;
             soma = H2 + (fim/100);
             if(soma<1){
              gerarMatrizConcentracao();
             }else{
                 QMessageBox::information(0,"Info","A soma das concentrações deve ser 100%");
               }
           }

       }else{
           QMessageBox::information(0,"Info","A variação têm que ser maior que zero");
       }
    }else{
        double  Ar = (ui->doubleSpinBox_2->value())/100;
        double  CH4 = (ui->doubleSpinBox_3->value())/100;
        double  H2 = (ui->doubleSpinBox_4->value())/100;
        double soma = Ar + CH4 + H2;
        if(soma==1){
        gerarArq();
        }else{
          QMessageBox::information(0,"Info","A soma das concentrações deve ser 100%");
        }
    }

}

void MainWindow::gerarArq(){

    double temperatura = ui->doubleSpinBox->value();
    double  pressao = ui->doubleSpinBox_5->value()*0.00131579;
    double press = ui->doubleSpinBox_5->value();
    double  Ar = (ui->doubleSpinBox_2->value())/100;
    double  CH4 = (ui->doubleSpinBox_3->value())/100;
    double  H2 = (ui->doubleSpinBox_4->value())/100;
    double soma = Ar + CH4 + H2;

    // Calcula o tempo e o passo
    double flux = ui->doubleSpinBox_12->value();
    double  tempo = temp(temperatura, press, Ar, CH4, H2, flux);
   // double  passo = tempo/50;
    char car[10];
    FILE *arq3;
    int i;

    if ((arq3 = fopen("tempos.dat","r")) == NULL)
       { qDebug("\n 1 Nao encontrei o arquivo entrada\n");
         exit(1);
       }

    for ( i=0; i<9; i++ )
          fscanf(arq3,"%s",&car);
    double passo = atof(car);
    fclose(arq3);

    //Condição Soma das concentrações = 100%
    if(soma != 1){
        QMessageBox::information(0,"Info","A soma das concentrações deve ser 100%");

    }else{

        //Salva o arquivo
        FILE *arq;

        if((arq = fopen("mole.inp","w"))==NULL){
            QMessageBox::information(0,"Info","Não encontrei o arquivo mole.inp");
        }

        fprintf(arq, "10\n");
        fprintf(arq, "%.1f   %1.6E\n",temperatura, pressao);
        fprintf(arq, "0.0d-6 %.7f  %.7f\n",tempo, passo);
        fprintf(arq, "CH4\n%.7f   0\n",CH4);
        fprintf(arq, "H2\n%.7f   0\n",H2);
        fprintf(arq, "Ar\n%.7f   0\n",Ar);
        fprintf(arq, "end");

        fclose(arq);
        system("BigCkin.exe < a.txt");
        //lerArq();
        convertArq();
    }
}

// Coloca o conteudo do Arq em uma matriz
void MainWindow::convertArq(){

    FILE *arqle;
      int i, j , k , l , m = 0, colunas, linhas, palavras;
      char ***matriz;
      char car[10];
      size_t length;
      length = 50;

      //Abre o arquivo em modo leitura (r)
      if ((arqle = fopen("spec.out","r")) == NULL)
         { qDebug("\n Nao encontrei o arquivo entrada\n");
           //system("pause");
           exit(1);
         }

      // Quantidade de linha e de Colunas
      colunas = contarColuna(arqle);
      palavras = contarPalavra(arqle);
      linhas = (palavras /colunas)+1;
      fclose(arqle);

      // Alocação dinâmica da Linha
      matriz = (char ***)calloc (linhas,sizeof(char**));

      // Alocação dinâmica da Coluna
      for (i=0; i<linhas; i++)
      matriz[i] = (char**)calloc(colunas,sizeof(char*));

      // Alocação dinâmica to tamanho da string
      for ( i=0; i<linhas; i++ )
      for ( j=0; j<colunas; j++ )
      matriz[i][j] = (char*)calloc(length, sizeof(char));


      //Abre novamente o arquivo em modo leitura (r) para acertar o ponteiro
      if ((arqle = fopen("spec.out" ,"r")) == NULL)
         { qDebug("\n Nao encontrei o arquivo entrada\n");
           system("pause");
           exit(1);
         }

      //le o arquivo e salva na matriz
      k = 0;
      double value = 0;
      double incremento = 0;
      for ( i=0; i<linhas; i++ )
      {
        if (i==10 || i==20 || i==30 || i==40 || i==50){
              k++;
        }
        for ( j=0; j<colunas; j++ )
            {
            if( j == 0 && i == 2){
              fscanf(arqle,"%s",&car);
              incremento = atof(car);
              strcpy(matriz[i - k][j], car);
            }else if(j == 0 && i != 2){
               fscanf(arqle,"%s",&car); // lê só para acertar o ponteiro
               value = value + incremento;
               char* cstr;
               char fname[32];
               snprintf(fname, sizeof(fname), "%g", value);
               cstr = new char [sizeof(fname)+1];
               strcpy( cstr, fname );
               strcpy(matriz[i - k][j], cstr);
               if(j == 0 && i == 0){
                 strcpy(matriz[i][j], "t[usec]");
               }
            }else{
              fscanf(arqle,"%s",&car);
                if (i!=10 && i!=20 && i!=30 && i!=40 && i!=50){
                   strcpy(matriz[i - k][j], car);
                }
            }
         }
      }
        //Imprime a matriz
       //trans(matriz, linhas, colunas);

        //fecha o arquivo e chama a função para gerar o grafico
        fclose(arqle);
        mostrarConteudo(matriz, linhas, colunas);
        makePlot(matriz, linhas, colunas);
}

//função que cria matriz para o gráfico das variações de temperatura e pressão
void MainWindow::GerarMatrizVariando(){

    double var, variacao, fim, inicio, temperatura, pressao;
    FILE *arqle, *arq;
      int i, j , k , colunas, meio, linhas, linhaVar, palavras, flag;
      char ***matriz, ***matrizVar;
      char car[10];
      size_t length;
      length = 50;
      flag = 0;
      linhaVar = 12;

    if(ui->checkBox->checkState()){
        inicio = ui->doubleSpinBox_8->value();
        fim = ui->doubleSpinBox_9->value();
        variacao =  ( fim - inicio )/10;
    }else if(ui->checkBox_2->checkState()){
        inicio = ui->doubleSpinBox_10->value();
        fim = ui->doubleSpinBox_11->value();
        variacao =  ( fim - inicio )/10;
    }

        for(var=inicio; var<=fim; var=var+variacao){
            flag = flag+1;
            double press;
            if(ui->checkBox->checkState()){
                temperatura = var;
            }else{
                temperatura = ui->doubleSpinBox->value() ;
            }
            if(ui->checkBox_2->checkState()){
                pressao = var*0.00131579;
                press = var;
            }else{
                pressao = ui->doubleSpinBox_5->value()*0.00131579;
                press = ui->doubleSpinBox_5->value();
            }

            double  Ar = (ui->doubleSpinBox_2->value())/100;
            double  CH4 = (ui->doubleSpinBox_3->value())/100;
            double  H2 = (ui->doubleSpinBox_4->value())/100;
            double soma = Ar + CH4 + H2;

            // Calcula o tempo e o passo
            double flux = ui->doubleSpinBox_12->value();
            double  tempo = temp(temperatura, press, Ar, CH4, H2, flux);
            double  passo = tempo/50;

            //Condição Soma das concentrações = 100%
            if(soma != 1){
                QMessageBox::information(0,"Info","A soma das concentrações deve ser 100%");
                break;

            }else{

                //Salva o arquivo

                if((arq = fopen("mole.inp","w"))==NULL){
                    QMessageBox::information(0,"Info","Não encontrei o arquivo mole.inp");
                }

                fprintf(arq, "10\n");
                fprintf(arq, "%.1f   %1.6E\n",temperatura, pressao);
                fprintf(arq, "0.0d-6 %.7f  %.7f\n",tempo, passo);
                fprintf(arq, "CH4\n%.7f   0\n",CH4);
                fprintf(arq, "H2\n%.7f   0\n",H2);
                fprintf(arq, "Ar\n%.7f   0\n",Ar);
                fprintf(arq, "end");

                fclose(arq);
                system("BigCkin.exe < a.txt");

                //Abre o arquivo em modo leitura (r)
                if ((arqle = fopen("spec.out","r")) == NULL)
                   { qDebug("\n 1 Nao encontrei o arquivo entrada\n");
                     //system("pause");
                     exit(1);
                   }

                // Quantidade de linha e de Colunas
                colunas = contarColuna(arqle);
                palavras = contarPalavra(arqle);
                linhas = (palavras /colunas)+1;
                meio = linhas/2;
                fclose(arqle);

                // Alocação dinâmica da Linha
                matriz = (char ***)calloc (linhas,sizeof(char**));

                // Alocação dinâmica da Coluna
                for (i=0; i<linhas; i++)
                matriz[i] = (char**)calloc(colunas,sizeof(char*));

                // Alocação dinâmica to tamanho da string
                for ( i=0; i<linhas; i++ )
                for ( j=0; j<colunas; j++ )
                matriz[i][j] = (char*)calloc(length, sizeof(char));


                //Abre novamente o arquivo em modo leitura (r) para acertar o ponteiro
                if ((arqle = fopen("spec.out" ,"r")) == NULL)
                   { qDebug("\n 2 Nao encontrei o arquivo entrada\n");
                     system("pause");
                     exit(1);
                   }

                //le o arquivo e salva na matriz
                k = 0;
                double value = 0;
                double incremento = 0;
                for ( i=0; i<linhas; i++ )
                {
                  if (i==10 || i==20 || i==30 || i==40 || i==50){
                        k++;
                  }
                  for ( j=0; j<colunas; j++ )
                      {
                      if( j == 0 && i == 2){
                        fscanf(arqle,"%s",&car);
                        incremento = atof(car);
                        strcpy(matriz[i - k][j], car);
                      }else if(j == 0 && i != 2){
                         fscanf(arqle,"%s",&car); // lê só para acertar o ponteiro
                         value = value + incremento;
                         char* cstr;
                         char fname[32];
                         snprintf(fname, sizeof(fname), "%g", value);
                         cstr = new char [sizeof(fname)+1];
                         strcpy( cstr, fname );
                         strcpy(matriz[i - k][j], cstr);
                         if(j == 0 && i == 0){
                           strcpy(matriz[i][j], "t[usec]");
                         }
                      }else{
                        fscanf(arqle,"%s",&car);
                          if (i!=10 && i!=20 && i!=30 && i!=40 && i!=50){
                             strcpy(matriz[i - k][j], car);
                          }
                      }
                   }
                }

                  //fecha o arquivo
                  fclose(arqle);

                  //na primeira passagem faz alocação da matriz das varções depois só coloca os valores na matriz
                  if(flag<2){
                      // Alocação dinâmica da Linha
                      matrizVar = (char ***)calloc (linhaVar,sizeof(char**));

                      // Alocação dinâmica da Coluna
                      for (i=0; i<linhaVar; i++)
                      matrizVar[i] = (char**)calloc(colunas,sizeof(char*));


                      // Alocação dinâmica to tamanho da string
                      for ( i=0; i<linhaVar; i++ )
                      for ( j=0; j<colunas; j++ )
                      matrizVar[i][j] = (char*)calloc(length, sizeof(char));


                      // Salvando a primeira linha na matrizVar
                      for ( j=0; j<colunas; j++ ){
                        matrizVar[0][j] = matriz[0][j];
                        matrizVar[flag][j] = matriz[meio][j];

                      }
                  }else{
                      for ( j=0; j<colunas; j++ ){
                         matrizVar[flag][j] = matriz[meio][j];
                        // qDebug("temperatura = %f",  temperatura);
                      }
                  }

            }

        }
    //Imprime a matriz

 //trans(matrizVar, linhaVar, colunas);
 mostrarConteudo(matrizVar, linhaVar, colunas);
 makePlot(matrizVar, linhaVar, colunas);

}

 void MainWindow::gerarMatrizConcentracao(){

     double var, variacao, fim, inicio, temperatura, pressao, press, Ar, CH4, H2, soma;
     FILE *arqle, *arq;
     int i, j , k, colunas, meio, linhas, linhaVar, palavras, flag;
     char ***matriz, ***matrizVar;
     char car[10];
     size_t length;
     length = 50;
     flag = 0;
     linhaVar = 12;
     inicio = ui->doubleSpinBox_6->value();
     fim = ui->doubleSpinBox_7->value();
     variacao =  ( fim - inicio )/10;

// Variar concentração
        for(var=inicio; var<=fim; var=var+variacao){
            flag = flag+1;
            temperatura = ui->doubleSpinBox->value();
            pressao = ui->doubleSpinBox_5->value()*0.00131579;
            press = ui->doubleSpinBox_5->value();

            // Variando
            if(ui->checkBox_3->checkState() && ui->checkBox_5->checkState()){
              Ar = var/100;
              CH4 = ui->doubleSpinBox_3->value()/100;
              soma = Ar + CH4;
              H2 = 1 - soma;
            }

            if(ui->checkBox_3->checkState() && ui->checkBox_7->checkState()){
              Ar = var/100;
              H2 = ui->doubleSpinBox_4->value()/100;
              soma = Ar + H2;
              CH4 = 1 - soma;
            }

            if(ui->checkBox_6->checkState() && ui->checkBox_4->checkState()){
              CH4 = var/100;;
              Ar = ui->doubleSpinBox_2->value()/100;
              soma = CH4 + Ar;
              H2 = 1 - soma;
            }

            if(ui->checkBox_6->checkState() && ui->checkBox_7->checkState()){
              CH4 = var/100;;
              H2 = ui->doubleSpinBox_4->value()/100;
              soma = CH4 + H2;
              Ar = 1 - soma;
            }

            if(ui->checkBox_8->checkState() && ui->checkBox_4->checkState()){
              H2 = var/100;;
              Ar = ui->doubleSpinBox_2->value()/100;
              soma =  H2 + Ar;
              CH4 = 1 - soma;
            }

            if(ui->checkBox_8->checkState() && ui->checkBox_5->checkState()){
              H2 = var/100;
              CH4 = ui->doubleSpinBox_3->value()/100;
              soma = H2 + CH4;
              Ar = 1 - soma;
            }

            // Calcula o tempo e o passo
            double flux = ui->doubleSpinBox_12->value();
            double  tempo = temp(temperatura, press, Ar, CH4, H2, flux);
            double  passo = tempo/50;
            double soma2 = Ar + CH4 + H2;
            qDebug("%f",H2);
            //Condição Soma das concentrações = 100%
            if(soma2 != 1){
                QMessageBox::information(0,"Info","O soma das concentrações deve ser 100%");
                break;
            }else{

                //Salva o arquivo

                if((arq = fopen("mole.inp","w"))==NULL){
                    QMessageBox::information(0,"Info","Não encontrei o arquivo mole.inp");
                }

                fprintf(arq, "10\n");
                fprintf(arq, "%.1f   %1.6E\n",temperatura, pressao);
                fprintf(arq, "0.0d-6 %.7f  %.7f\n",tempo, passo);
                fprintf(arq, "CH4\n%.7f   0\n",CH4);
                fprintf(arq, "H2\n%.7f   0\n",H2);
                fprintf(arq, "Ar\n%.7f   0\n",Ar);
                fprintf(arq, "end");

                fclose(arq);
                system("BigCkin.exe < a.txt");

                //Abre o arquivo em modo leitura (r)
                if ((arqle = fopen("spec.out","r")) == NULL)
                   { qDebug("\n 1 Nao encontrei o arquivo entrada\n");
                     exit(1);
                   }

                // Quantidade de linha e de Colunas
                colunas = contarColuna(arqle);
                palavras = contarPalavra(arqle);
                linhas = (palavras /colunas)+1;
                meio = linhas/2;
                fclose(arqle);

                // Alocação dinâmica da Linha
                matriz = (char ***)calloc (linhas,sizeof(char**));

                // Alocação dinâmica da Coluna
                for (i=0; i<linhas; i++)
                matriz[i] = (char**)calloc(colunas,sizeof(char*));

                // Alocação dinâmica to tamanho da string
                for ( i=0; i<linhas; i++ )
                for ( j=0; j<colunas; j++ )
                matriz[i][j] = (char*)calloc(length, sizeof(char));


                //Abre novamente o arquivo em modo leitura (r) para acertar o ponteiro
                if ((arqle = fopen("spec.out" ,"r")) == NULL)
                   { qDebug("\n 2 Nao encontrei o arquivo entrada\n");
                     system("pause");
                     exit(1);
                   }

                //le o arquivo e salva na matriz
                k = 0;
                double value = 0;
                double incremento = 0;
                for ( i=0; i<linhas; i++ )
                {
                  if (i==10 || i==20 || i==30 || i==40 || i==50){
                        k++;
                  }
                  for ( j=0; j<colunas; j++ )
                      {
                      if( j == 0 && i == 2){
                        fscanf(arqle,"%s",&car);
                        incremento = atof(car);
                        strcpy(matriz[i - k][j], car);
                      }else if(j == 0 && i != 2){
                         fscanf(arqle,"%s",&car); // lê só para acertar o ponteiro
                         value = value + incremento;
                         char* cstr;
                         char fname[32];
                         snprintf(fname, sizeof(fname), "%g", value);
                         cstr = new char [sizeof(fname)+1];
                         strcpy( cstr, fname );
                         strcpy(matriz[i - k][j], cstr);
                         if(j == 0 && i == 0){
                           strcpy(matriz[i][j], "t[usec]");
                         }
                      }else{
                        fscanf(arqle,"%s",&car);
                          if (i!=10 && i!=20 && i!=30 && i!=40 && i!=50){
                             strcpy(matriz[i - k][j], car);
                          }
                      }
                   }
                }

                  //fecha o arquivo
                  fclose(arqle);

                  //na primeira passagem faz alocação da matriz das varções depois só coloca os valores na matriz
                  if(flag<2){
                      // Alocação dinâmica da Linha
                      matrizVar = (char ***)calloc (linhaVar,sizeof(char**));

                      // Alocação dinâmica da Coluna
                      for (i=0; i<linhaVar; i++)
                      matrizVar[i] = (char**)calloc(colunas,sizeof(char*));


                      // Alocação dinâmica to tamanho da string
                      for ( i=0; i<linhaVar; i++ )
                      for ( j=0; j<colunas; j++ )
                      matrizVar[i][j] = (char*)calloc(length, sizeof(char));


                      // Salvando a primeira linha na matrizVar
                      for ( j=0; j<colunas; j++ ){
                        matrizVar[0][j] = matriz[0][j];
                        matrizVar[flag][j] = matriz[meio][j];

                      }
                  }else{
                      for ( j=0; j<colunas; j++ ){
                         matrizVar[flag][j] = matriz[meio][j];
                      }
                  }

            }
        }
    mostrarConteudo(matrizVar, linhaVar, colunas);
    makePlot(matrizVar, linhaVar, colunas);
 }

//função que gera os gráficos
void MainWindow::makePlot(char ***matriz, int linhas, int colunas)
{
    int i, j, k=0;
    double incremento = 0, valorColuna;
    double variacao, fim, inicio;
    ui->customPlot->clearGraphs();
    ui->customPlot->clearPlottables();

    if(ui->checkBox->checkState() || ui->checkBox_2->checkState() || ui->checkBox_9->checkState()){
        k = 1;
    }
    QVector<double> eixoY(60), eixoX(60);
    for (int gi=3; gi<colunas; ++gi)
    {
      ui->customPlot->addGraph();
      QPen pen;
      pen.setColor(QColor(0, 0, 255, 200));
      ui->customPlot->graph()->setLineStyle(QCPGraph::lsLine);
      ui->customPlot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%14+1)));
      QPen graphPen;
       graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
       graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
       ui->customPlot->graph()->setPen(graphPen);


      if(k==1){

          incremento = ui->doubleSpinBox_8->value();
          if(ui->checkBox->checkState()){
             inicio = ui->doubleSpinBox_8->value();
              fim = ui->doubleSpinBox_9->value();
              variacao =  ( fim - inicio )/10;

          }else if(ui->checkBox_2->checkState()){
              inicio = ui->doubleSpinBox_10->value();
              fim = ui->doubleSpinBox_11->value();
              variacao =  ( fim - inicio )/10;

          }else if( ui->checkBox_9->checkState() ){
              inicio = ui->doubleSpinBox_6->value();
              fim = ui->doubleSpinBox_7->value();
              variacao =  ( fim - inicio )/10;
           }

         for (int i=0; i<linhas-1; ++i)
          {
                eixoY[i] = inicio;
                eixoX[i] = atof(matriz[i+1][gi]);
                inicio = inicio+variacao;
          }
      }else {

          for (int i=0; i<linhas-1; ++i)
          {
              eixoY[i] = atof(matriz[i+1][0]);
              eixoX[i] = atof(matriz[i+1][gi]);

          }

      }

      QString fname;
      fname = QString(matriz[0][gi]);
      ui->customPlot->graph()->setData(eixoY, eixoX);
      ui->customPlot->graph()->setName(fname);

    }

    // set a more compact font size for bottom and left axis tick labels:

    ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // apply manual tick and tick label for left axis:

    ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->customPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
    ui->customPlot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
    ui->customPlot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
    ui->customPlot->yAxis->setRange(1e-20, 1e5);
    ui->customPlot->yAxis2->setRange(1e-20, 1e5);

    // set axis labels:
    if(ui->checkBox->checkState() || ui->checkBox_2->checkState() || ui->checkBox_9->checkState()){
        if(ui->checkBox->checkState()){
            ui->customPlot->xAxis->setLabel("Temperature [K]");
        }else if(ui->checkBox_2->checkState()){
            ui->customPlot->xAxis->setLabel("Pressão");
        }else if(ui->checkBox_3->checkState()){
        ui->customPlot->xAxis->setLabel("[Ar]");
        }else if(ui->checkBox_6->checkState()){
            ui->customPlot->xAxis->setLabel("[CH4]");
        }else if(ui->checkBox_8->checkState()){
            ui->customPlot->xAxis->setLabel("[H2]");
        }

    }else{
        ui->customPlot->xAxis->setLabel("Residence Time");
    }

    ui->customPlot->yAxis->setLabel("Molar Fraction");

    // make top and right axes visible but without ticks and labels:
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTicks(false);
    ui->customPlot->yAxis2->setTicks(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setTickLabels(false);

    // set axis ranges to show all data:
    if(ui->checkBox->checkState() || ui->checkBox_2->checkState() || ui->checkBox_9->checkState()){

        if(ui->checkBox->checkState()){
            ui->customPlot->xAxis->setRange(ui->doubleSpinBox_8->value() , ui->doubleSpinBox_9->value());

        }else if(ui->checkBox_2->checkState()){
            ui->customPlot->xAxis->setRange(ui->doubleSpinBox_10->value() , ui->doubleSpinBox_11->value());

        }else if(ui->checkBox_3->checkState() || ui->checkBox_6->checkState() || ui->checkBox_8->checkState()){
            ui->customPlot->xAxis->setRange(ui->doubleSpinBox_6->value() , ui->doubleSpinBox_7->value());
        }

     }else{
        double ry = atof(matriz[linhas-3][0]);
        qDebug("%f",ry);
        ui->customPlot->xAxis->setRange(0 , ry);
    }

    // show legend:

    ui->customPlot->legend->setVisible(true);

    // mostra a legenda ao lado go gráfico

    free(subLayout);
    subLayout = new QCPLayoutGrid;
    ui->customPlot->plotLayout()->addElement(0, 1, subLayout);
    subLayout->addElement(0, 0, new QCPLayoutElement);
    subLayout->addElement(1, 0, ui->customPlot->legend);
    subLayout->addElement(2, 0, new QCPLayoutElement);
    ui->customPlot->plotLayout()->setColumnStretchFactor(1, 0.001);


    //Seleciona o grafico pela legenda e vice versa
    connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));

    //Interação
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iMultiSelect | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    //Mostra o gráfico
    ui->customPlot->replot();

    //libera a memória da alocação dinâmica
    for ( i=0; i<linhas; i++ )
    for ( j=0; j<colunas; j++ ){
        free (matriz[i][j]);
    }
    for (i=0; i<linhas; i++)
        free(matriz[i]);
    free(matriz);
}

//Funcao para jogar na tela o conteudo da Matriz em uma tabela

void MainWindow::mostrarConteudo(char ***matriz, int linhas, int colunas){

    int i, j;
    double incremento = atof(matriz[2][0]);
    double valorColuna = 0;
    QTableWidgetItem *newItem;

    //Quantidade de linhas e colunas da taela
    ui->tableWidget->setRowCount(linhas);
    ui->tableWidget->setColumnCount(colunas);

    for ( j=0; j<colunas; j++ ){
        ui->tableWidget->setHorizontalHeaderItem(j, new QTableWidgetItem(matriz[0][j]));
    }
    //Se variar
    if(ui->checkBox->checkState() || ui->checkBox_2->checkState()){
        if(ui->checkBox->checkState()){ // variando temperatura
            for ( i=0; i<linhas; i++ ){
                ui->tableWidget->setVerticalHeaderItem(i,new QTableWidgetItem(matriz[i][1]));
            }
        }else if(ui->checkBox_2->checkState()){ // variando pressão
            for ( i=0; i<linhas; i++ ){
                ui->tableWidget->setVerticalHeaderItem(i,new QTableWidgetItem(matriz[i][2]));
            }
        }
    }else{

        for ( i=0; i<linhas; i++ ){
            ui->tableWidget->setVerticalHeaderItem(i,new QTableWidgetItem(matriz[i][0]));
        }
    }
    //Jogando os valores da matriz na tabela
    for ( i=0; i<linhas; i++ )
        {
          for ( j=0; j<colunas; j++ )
              {
                  newItem = new QTableWidgetItem( matriz[i][j]);
                  ui->tableWidget->setItem(i, j, newItem);
         }
       valorColuna = valorColuna+incremento;
       //qDebug("%f", valorColuna );
    }


}

// Funcao para imprimir a Matriz
void trans (char ***matriz, int linhas, int colunas){
    int i, j;
    for ( i=0; i<linhas; i++ )
    for ( j=0; j<colunas; j++ ){

               qDebug ("%s  ", matriz[ i ][ j ]);

    }
    qDebug ("\n \n");
}

// Funcao para contar a quantidade de  colunas do arquivo
int contarColuna(FILE *arqle){
  char caracter;
  int flag = 0;
  int comecouPalavra = 0, numPalavras = 0;

        while (flag != 1) {
        caracter = getc(arqle);

        if  ((caracter!=' ') && (caracter!='\n') && (!comecouPalavra)) {
             comecouPalavra = 1;
          }
          if  (((caracter==' ') || (caracter == '\n')) && (comecouPalavra)) {
             comecouPalavra = 0;
             numPalavras++;
          }
          if (caracter=='.') {
             flag++;
             }
        }
        return(numPalavras);
}

// Funcao para contar a quantidade de palavras do arquivo
int contarPalavra(FILE *arqle){

    int comecouPalavra = 0, numPalavras = 0;
    char caracter;

    while (!feof(arqle)) {
          caracter = getc(arqle);
          if  ((caracter!=' ') && (caracter!='\n') && (!comecouPalavra)) {
             comecouPalavra = 1;
          }
          if  (((caracter==' ') || (caracter == '\n')) && (comecouPalavra)) {
             comecouPalavra = 0;
             numPalavras++;
          }

   }
   return(numPalavras);
}

//Função para calcular o tempo de deposição
double temp(double temperatura,double pressao,double Ar,double CH4,double H2, double flux){
    FILE *arq, *arq2, *arq3;

    //Fera o arquivo temppres.dat com temperatura e pressao
    if((arq = fopen("temppres.dat","w"))==NULL){
        QMessageBox::information(0,"Info","Não encontrei o arquivo");
    }

    fprintf(arq, "1\n");
    fprintf(arq, "%.1f \n",temperatura);
    fprintf(arq, "\n");
    fprintf(arq, "1\n");
    fprintf(arq, "%.1f \n",pressao);

    fclose(arq);

    //Gera o arquivo fluxos.dat com Alfa e Beta


    double alfa = H2/CH4;
    double base = H2 + CH4;
    double beta = Ar/ base;
    if((arq2 = fopen("fluxos.dat","w"))==NULL){
        QMessageBox::information(0,"Info","Não encontrei o arquivo");
    }

    fprintf(arq2, "1\n");
    fprintf(arq2, "%.5f  %.5f  %.5f\n", flux, alfa, beta);

    fclose(arq2);

    // Rodar tempo.exe
    system("tempo.exe");

    //Abre o arquivo em modo leitura (r)e salva retorna o tempo de deposição
    char car[10];

    int i;

    if ((arq3 = fopen("tempos.dat","r")) == NULL)
       { qDebug("\n 1 Nao encontrei o arquivo entrada\n");
         exit(1);
       }

    for ( i=0; i<8; i++ )
          fscanf(arq3,"%s",&car);

    fclose(arq3);
    double tempDep = atof(car);
    return(tempDep);
}

//Função para mostrar só os gráficos selecionados
void MainWindow::selectionChanged()
{
  int flag = 0; //Para mostrar se tem algum grafico selecionado
  for (int i=0; i<ui->customPlot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->customPlot->graph(i);
    QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setVisible(true);
      flag = 1; // Se selecionou gráfico o flag vira 1

    }else{
        graph->setVisible(false);
    }

   }

  //Se nao tem nenhum grafico selecionado mostra todos
  for (int i=0; i<ui->customPlot->graphCount(); ++i)
  {
      QCPGraph *graph = ui->customPlot->graph(i);
      if (flag == 0){
          graph->setVisible(true);
      }
  }
  ui->customPlot->replot();
}

//Função para aparecer os inputBox quando o checkBox for true
void MainWindow::on_checkBox_stateChanged(int arg1)
{

        if(ui->checkBox->checkState()){
            ui->doubleSpinBox_8->setEnabled(true);
            ui->label_8->setEnabled(true);
            ui->doubleSpinBox_9->setEnabled(true);
            ui->label_9->setEnabled(true);
            ui->checkBox_2->setEnabled(false);
            ui->checkBox_9->setEnabled(false);
            ui->doubleSpinBox->setEnabled(false);
        }else{
            ui->doubleSpinBox_8->setEnabled(false);
            ui->label_8->setEnabled(false);
            ui->doubleSpinBox_9->setEnabled(false);
            ui->label_9->setEnabled(false);
            ui->checkBox_2->setEnabled(true);
            ui->checkBox_9->setEnabled(true);
            ui->doubleSpinBox->setEnabled(true);
        }
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{

        if(ui->checkBox_2->checkState()){
            ui->doubleSpinBox_10->setEnabled(true);
            ui->label_10->setEnabled(true);
            ui->doubleSpinBox_11->setEnabled(true);
            ui->label_11->setEnabled(true);
            ui->doubleSpinBox_5->setEnabled(false);
            ui->checkBox->setEnabled(false);
            ui->checkBox_9->setEnabled(false);
        }else{
            ui->doubleSpinBox_10->setEnabled(false);
            ui->label_10->setEnabled(false);
            ui->doubleSpinBox_11->setEnabled(false);
            ui->label_11->setEnabled(false);
            ui->doubleSpinBox_5->setEnabled(true);
            ui->checkBox->setEnabled(true);
            ui->checkBox_9->setEnabled(true);
        }
}

void MainWindow::on_checkBox_9_stateChanged(int arg1)
{

        if(ui->checkBox_9->checkState()){
            ui->pushButton->setEnabled(false);
            ui->splitter_16->setEnabled(true);
            ui->splitter_18->setEnabled(true);
            ui->splitter_20->setEnabled(true);

            ui->doubleSpinBox_2->setEnabled(false);
            ui->doubleSpinBox_3->setEnabled(false);
            ui->doubleSpinBox_4->setEnabled(false);
            ui->checkBox->setEnabled(false);
            ui->checkBox_2->setEnabled(false);
        }else{
            ui->pushButton->setEnabled(true);
            ui->splitter_16->setEnabled(false);
            ui->splitter_18->setEnabled(false);
            ui->splitter_20->setEnabled(false);

            ui->doubleSpinBox_2->setEnabled(true);
            ui->doubleSpinBox_3->setEnabled(true);
            ui->doubleSpinBox_4->setEnabled(true);
            ui->checkBox->setEnabled(true);
            ui->checkBox_2->setEnabled(true);
            ui->splitter_11->setEnabled(false);
        }
}

void MainWindow::on_checkBox_3_stateChanged(int arg1) //variar
{
    if(ui->checkBox_3->checkState()){
        control++;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        ui->checkBox_6->setEnabled(false);
        ui->checkBox_8->setEnabled(false);
        ui->checkBox_4->setEnabled(false);
        ui->splitter_11->setEnabled(true);

    }else{
        control--;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        if(ui->checkBox_5->checkState()== 0){
        ui->checkBox_6->setEnabled(true);
        }
        if(ui->checkBox_7->checkState()== 0){
        ui->checkBox_8->setEnabled(true);
        }
        ui->splitter_11->setEnabled(false);
        if(ui->checkBox_7->checkState()== 0 && ui->checkBox_5->checkState()== 0){
           ui->checkBox_4->setEnabled(true);
        }

    }
}

void MainWindow::on_checkBox_4_stateChanged(int arg1) // fixar
{
    if(ui->checkBox_4->checkState()){
        control++;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        ui->checkBox_5->setEnabled(false);
        ui->checkBox_7->setEnabled(false);
        ui->checkBox_3->setEnabled(false);
        ui->doubleSpinBox_2->setEnabled(true);
    }else{
        control--;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        if(ui->checkBox_6->checkState()== 0){
        ui->checkBox_5->setEnabled(true);
        }
        if(ui->checkBox_8->checkState()== 0){
        ui->checkBox_7->setEnabled(true);
        }
        ui->doubleSpinBox_2->setEnabled(false);
        if(ui->checkBox_6->checkState()== 0 && ui->checkBox_8->checkState()== 0){
           ui->checkBox_3->setEnabled(true);
        }
    }
}

void MainWindow::on_checkBox_6_stateChanged(int arg1) //variar
{
    if(ui->checkBox_6->checkState()){
        control++;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        ui->checkBox_3->setEnabled(false);
        ui->checkBox_8->setEnabled(false);
        ui->checkBox_5->setEnabled(false);
        ui->splitter_11->setEnabled(true);

    }else{
        control--;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        if(ui->checkBox_4->checkState()== 0){
        ui->checkBox_3->setEnabled(true);
        }
        if(ui->checkBox_7->checkState()== 0){
        ui->checkBox_8->setEnabled(true);
        }
        ui->splitter_11->setEnabled(false);
        if(ui->checkBox_4->checkState()== 0 && ui->checkBox_7->checkState()== 0){
           ui->checkBox_5->setEnabled(true);
        }
    }
}

void MainWindow::on_checkBox_5_stateChanged(int arg1) // fixar
{
    if(ui->checkBox_5->checkState()){
        control++;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        ui->checkBox_4->setEnabled(false);
        ui->checkBox_7->setEnabled(false);
        ui->checkBox_6->setEnabled(false);
        ui->doubleSpinBox_3->setEnabled(true);
    }else{
        control--;
        if(control != 2){
            ui->pushButton->setEnabled(false);
        }else{
            ui->pushButton->setEnabled(true);
        }
        if(ui->checkBox_3->checkState()== 0){
        ui->checkBox_4->setEnabled(true);
        }
        if(ui->checkBox_8->checkState()== 0 ){
        ui->checkBox_7->setEnabled(true);
        }
        ui->doubleSpinBox_3->setEnabled(false);
        if(ui->checkBox_8->checkState()== 0 && ui->checkBox_3->checkState()== 0){
           ui->checkBox_6->setEnabled(true);
        }
    }
}

void MainWindow::on_checkBox_8_stateChanged(int arg1) //variar
{
    if(ui->checkBox_8->checkState()){
        control++;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        ui->checkBox_6->setEnabled(false);
        ui->checkBox_3->setEnabled(false);
        ui->checkBox_7->setEnabled(false);
        ui->splitter_11->setEnabled(true);
    }else{
        control--;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        if(ui->checkBox_5->checkState()== 0){
        ui->checkBox_6->setEnabled(true);
        }
        if(ui->checkBox_4->checkState()== 0){
        ui->checkBox_3->setEnabled(true);
        }
        ui->splitter_11->setEnabled(false);
        if(ui->checkBox_4->checkState()== 0 && ui->checkBox_5->checkState()== 0){
           ui->checkBox_7->setEnabled(true);
        }
    }
}

void MainWindow::on_checkBox_7_stateChanged(int arg1) //fixar
{
    if(ui->checkBox_7->checkState()){
        control++;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        ui->checkBox_5->setEnabled(false);
        ui->checkBox_4->setEnabled(false);
        ui->checkBox_8->setEnabled(false);
        ui->doubleSpinBox_4->setEnabled(true);
    }else{
        control--;
        if(control == 2){
            ui->pushButton->setEnabled(true);
        }else{
            ui->pushButton->setEnabled(false);
        }
        if(ui->checkBox_6->checkState()== 0){
            ui->checkBox_5->setEnabled(true);
        }
        if(ui->checkBox_3->checkState()== 0){
        ui->checkBox_4->setEnabled(true);
        }
        ui->doubleSpinBox_4->setEnabled(false);
        if(ui->checkBox_6->checkState()== 0 && ui->checkBox_3->checkState()== 0){
           ui->checkBox_8->setEnabled(true);
        }
    }
}

