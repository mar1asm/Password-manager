#include "addpassworddialog.h"
#include "ui_addpassworddialog.h"

AddPasswordDialog::AddPasswordDialog ( QWidget *parent )
    : QDialog ( parent ), ui ( new Ui::AddPasswordDialog ) {
  ui->setupUi ( this );
}

AddPasswordDialog::~AddPasswordDialog ( ) { delete ui; }

void AddPasswordDialog::on_cancelButton_clicked ( ) { emit cancel ( ); }

void AddPasswordDialog::on_addButton_clicked ( ) {
  QString username = this->ui->usernameInput->text ( );
  QString password = this->ui->passwordInput->text ( );
  QString url = this->ui->URLInput->text ( );
  QString title = this->ui->titleInput->text ( );
  QString notes = this->ui->notesInout->toPlainText ( );
  if ( password == "" || title == "" ) {
    QMessageBox::warning ( this, "Incomplet",
               "Trebuie sa completezi macar parola si titlul!" );
    return;
  }
  emit addPassword ( title, username, password, url, notes );
}
