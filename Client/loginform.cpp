#include "loginform.h"
#include "ui_loginform.h"

LoginForm::LoginForm ( QWidget *parent )
    : QWidget ( parent ), ui ( new Ui::Form ) {
  ui->setupUi ( this );
}

LoginForm::~LoginForm ( ) { delete ui; }

void LoginForm::on_signupButton_clicked ( ) { emit signup ( ); }

void LoginForm::on_loginButton_clicked ( ) {
  QString username = this->ui->username->text ( );
  QString password = this->ui->password->text ( );
  emit login ( username, password );
}
