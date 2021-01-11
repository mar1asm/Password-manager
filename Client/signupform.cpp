#include "signupform.h"
#include "ui_signupform.h"

SignupForm::SignupForm ( QWidget *parent )
    : QWidget ( parent ), ui ( new Ui::SignupForm ) {
  ui->setupUi ( this );
}

SignupForm::~SignupForm ( ) { delete ui; }

void SignupForm::on_signupButton_clicked ( ) {
  QString username = this->ui->username->text ( );
  QString email = this->ui->email->text ( );
  QString pass = this->ui->password->text ( );
  QString cPass = this->ui->confirmPass->text ( );
  emit signup ( username, email, pass, cPass );
}

void SignupForm::on_loginButton_clicked ( ) { emit login ( ); }
