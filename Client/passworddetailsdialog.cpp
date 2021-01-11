#include "passworddetailsdialog.h"
#include "ui_passworddetailsdialog.h"

PasswordDetailsDialog::PasswordDetailsDialog ( QWidget *parent,
                           QStringList *info )
    : QDialog ( parent ), ui ( new Ui::PasswordDetailsDialog ) {
  ui->setupUi ( this );
  this->ui->title->setText ( info->at ( 0 ) );
  this->ui->username->setText ( info->at ( 1 ) );
  this->ui->password->setText ( info->at ( 2 ) );
  this->ui->url->setText ( info->at ( 3 ) );
  this->ui->notes->setText ( info->at ( 4 ) );
}

PasswordDetailsDialog::~PasswordDetailsDialog ( ) { delete ui; }
