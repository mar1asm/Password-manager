#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "editpassworddialog.h"
#include "loginform.h"
#include "passworddetailsdialog.h"
#include "signupform.h"
#include <QCryptographicHash>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLayoutItem>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <addpassworddialog.h>
#include <iostream>
//#include <openssl/sha.h>
#include <thread>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow ( QWidget *parent = 0 );
  ~MainWindow ( );

private:
  LoginForm *loginForm;
  SignupForm *signupForm;
  AddPasswordDialog *addPasswordDialog;
  PasswordDetailsDialog *passwordDetailsDialog;
  EditPasswordDialog *editPasswordDialog;
  Ui::MainWindow *ui;
  bool loginPageActive = false;
  bool signupPageActive = false;
  Client *client;
  bool loggedIn = false;
  QVector< QLabel * > passwords;
  QVector< QLabel * > titles;
  QVector< QStringList > passwordsInfo;//rretin toate info pe care mi le trimite serverul;
  QString username;
  QVector< QHBoxLayout * > entries;//pt interfata grafica
  QVBoxLayout *layout;//same
  QVector< QWidget * > listItems;//same
  QWidget *mainWidget;//same
  QScrollArea *scroll;//same
  QPushButton *addNew;//same
  QPushButton *logout;//same

//slots- in qt am doua chestii: semnale si sloturi; slloturile sunt doar functii
private slots:
  void showLoginPage ( );
  void showSignupPage ( );
  void showMainPage ( );
  void on_login ( QString username, QString password );
  void on_signup ( QString username, QString email, QString password,
           QString cPassword );

  // pentru parole
  void on_deletePassword ( );
  void on_editPassword ( );
  void on_showPassword ( );
  void on_showDetailsPassword ( );
  void on_addNewButton_clicked ( );

  void on_cancelButtonClicked ( );
  void on_addButtonClicked ( QString, QString, QString, QString, QString );

  void on_cancelEdit ( );
  void on_saveEdit ( int, QString, QString, QString, QString, QString );

  void on_logout ( );
  QString hash ( QString text );
};

#endif // MAINWINDOW_H
