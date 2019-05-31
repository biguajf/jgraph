#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void makePlot(char ***matrizint,int linhas, int colunas);
    void mostrarConteudo(char ***matrizint,int linhas, int colunas);

    void convertArq();
    void gerarArq();
    void selectionChanged();
    void on_pushButton_clicked();
    void GerarMatrizVariando();
    void gerarMatrizConcentracao();
    void on_actionSalvar_triggered();
    void on_checkBox_stateChanged(int arg1);
    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_9_stateChanged(int arg1);

    void on_checkBox_3_stateChanged(int arg1);

    void on_checkBox_4_stateChanged(int arg1);

    void on_checkBox_6_stateChanged(int arg1);

    void on_checkBox_5_stateChanged(int arg1);

    void on_checkBox_8_stateChanged(int arg1);

    void on_checkBox_7_stateChanged(int arg1);

    void on_actionSalvar_Arq_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
