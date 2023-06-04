// Подключаем библиотеку для работы с дисплеем
#include <QuadDisplay2.h>
// создаём объект класса QuadDisplay и передаём номер пина CS
QuadDisplay qd(9);

void setup()
{
  qd.begin();
}

void loop()
{  // можно выводить целые числа
  qd.displayInt(123);
  delay(1000);
  // и отрицательные тоже
  qd.displayInt(-123);
  delay(1000);
  // можно показывать ведущие нули (0012)
  qd.displayInt(12, true);
  delay(1000);
  // и несколько точек одновременно
  qd.displayInt(12, true, 0b0000);
  delay(100);
  qd.displayInt(12, true, 0b0001);
  delay(100);
  qd.displayInt(12, true, 0b0011);
  delay(100);
  qd.displayInt(12, true, 0b0111);
  delay(100);
  qd.displayInt(12, true, 0b1111);
  delay(100);
  qd.displayInt(12, true, 0b1110);
  delay(100);
  qd.displayInt(12, true, 0b1100);
  delay(100);
  qd.displayInt(12, true, 0b1000);
  delay(100);
  qd.displayInt(12, true, 0b0000);
  delay(1000);

  // можно показывать вещественные числа
  // с заданной точностью, например 2 знака после запятой
  qd.displayFloat(-1.23, 2);
  delay(1000);
  // 1 знак после запятой
  qd.displayFloat(-1.23, 1);
  delay(1000);
  // 0 знаков после запятой (как признак округления)
  qd.displayFloat(-1.23, 0);
  delay(1000);

  // можно показывать нехитрый текст (on/off, например)...
  qd.displayDigits(QD_O, QD_n, QD_NONE, QD_NONE); // On
  delay(1000);
  qd.displayDigits(QD_O, QD_f, QD_f, QD_NONE); // Off
  delay(1000);

  // или произвольную графику
  // заполненим сегменты экрана по одному справа налево
  for (int i = 1; i < 33; i++) {
    qd.displaySegments(0xffffffff << i);
    delay(30);
  }
  delay(1000);

  // можно показывать температуру в °C
  qd.displayTemperatureC(-5);
  delay(1000);
  // можно влажность в %
  qd.displayHumidity(42);
  delay(1000);

  // счет...
  qd.displayScore(5, 0);
  delay(1000);

  // и, конечно, всё очищать
  qd.displayClear();
  delay(1000);
}
