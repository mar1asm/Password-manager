#ifndef EDITPASSWORDDIALOG_H
#define EDITPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class EditPasswordDialog;
}

class EditPasswordDialog : public QDialog {
  Q_OBJECT

public:
  explicit EditPasswordDialog ( QWidget *parent = 0, int id = 0,
                QStringList *infos = 0 );
  ~EditPasswordDialog ( );

private slots:

  void on_cancelButton_clicked ( );

  void on_saveButton_clicked ( );

private:
  Ui::EditPasswordDialog *ui;
  int id;
signals:
  void cancelEdit ( );
  void saveEdit ( int, QString, QString, QString, QString, QString );
};

#endif // EDITPASSWORDDIALOG_H
