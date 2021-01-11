#ifndef PASSWORDDETAILSDIALOG_H
#define PASSWORDDETAILSDIALOG_H

#include <QDialog>

namespace Ui {
class PasswordDetailsDialog;
}

class PasswordDetailsDialog : public QDialog {
  Q_OBJECT

public:
  explicit PasswordDetailsDialog ( QWidget *parent = 0,
                   QStringList *infos = 0 );
  ~PasswordDetailsDialog ( );

private:
  Ui::PasswordDetailsDialog *ui;
};

#endif // PASSWORDDETAILSDIALOG_H
