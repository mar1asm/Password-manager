#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>

namespace Ui {
class Form;
}

class LoginForm : public QWidget {
  Q_OBJECT

public:
  explicit LoginForm ( QWidget *parent = 0 );
  ~LoginForm ( );

private slots:
  void on_signupButton_clicked ( );

  void on_loginButton_clicked ( );

private:
  Ui::Form *ui;

signals:
  void signup ( );
  void login ( QString username, QString password );
};

#endif // LOGINFORM_H
