#include "bcrypt.h"
#include <iostream>
#include <string>

using namespace std;

int main() {
  string password;

  cout << "Уведіть Ваш пароль для створення хеш-паролю: ";
  getline(cin, password);

  cout << "Отриманий хеш-пароль:" << endl
       << endl
       << bcrypt::generateHash(password) << endl
       << endl;

  cout << "Копіюйте, та йдіть у базу даних" << endl;
  cin.get();

  return 0;
}
