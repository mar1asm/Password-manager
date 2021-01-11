#ifndef SIGNUPFORM_H
#define SIGNUPFORM_H

#include <QWidget>

namespace Ui {
class SignupForm;
}

class SignupForm : public QWidget {
  Q_OBJECT

public:
  explicit SignupForm ( QWidget *parent = 0 );
  ~SignupForm ( );

private slots:
  void on_signupButton_clicked ( );

  void on_loginButton_clicked ( );

private:
  Ui::SignupForm *ui;
signals:
  void signup ( QString username, QString email, QString password,
        QString cPassword );
  void login ( );
};

#endif // SIGNUPFORM_H
