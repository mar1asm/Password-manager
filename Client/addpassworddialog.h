#ifndef ADDPASSWORDDIALOG_H
#define ADDPASSWORDDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class AddPasswordDialog;
}

class AddPasswordDialog : public QDialog {
  Q_OBJECT

public:
  explicit AddPasswordDialog ( QWidget *parent = 0 );
  ~AddPasswordDialog ( );

private slots:
  void on_cancelButton_clicked ( );

  void on_addButton_clicked ( );

private:
  Ui::AddPasswordDialog *ui;

signals:
  void cancel ( );
  void addPassword ( QString, QString, QString, QString, QString );
};

#endif // ADDPASSWORDDIALOG_H
