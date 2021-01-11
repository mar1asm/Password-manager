#include "editpassworddialog.h"
#include "ui_editpassworddialog.h"

EditPasswordDialog::EditPasswordDialog ( QWidget *parent, int id,
                     QStringList *info )
    : QDialog ( parent ), ui ( new Ui::EditPasswordDialog ) {
  ui->setupUi ( this );
  this->id = id;
  this->ui->titleEdit->setText ( info->at ( 0 ) );
  this->ui->usernameEdit->setText ( info->at ( 1 ) );
  this->ui->passwordEdit->setText ( info->at ( 2 ) );
  this->ui->urlEdit->setText ( info->at ( 3 ) );
  this->ui->notesEdit->setText ( info->at ( 4 ) );
}

EditPasswordDialog::~EditPasswordDialog ( ) { delete ui; }

void EditPasswordDialog::on_cancelButton_clicked ( ) { emit cancelEdit ( ); }

void EditPasswordDialog::on_saveButton_clicked ( ) {
  emit saveEdit ( this->id, ui->titleEdit->text ( ), ui->usernameEdit->text ( ),
          ui->passwordEdit->text ( ), ui->urlEdit->text ( ),
          ui->notesEdit->text ( ) );
}
