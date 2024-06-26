/*
  Пример линейной аппроксимации методом наименьших квадратов
  Два массива: по оси Х и по оси У
  Наполнение массивов осуществляется динамически: сдвигом и записью в крайнюю ячейку,
  то есть аппроксимация по последним ARRAY_SIZE изменениям!!
*/
#define ARRAY_SIZE 10   // размер пространства для аппроксимации

// два массива с данными (одинаковой размероности и размера)
int x_array[ARRAY_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};  // ось x от 1 до 10, допустим СЕКУНД
int y_array[ARRAY_SIZE];    // значения по оси У будем брать с датчика

#include <GyverFilters.h>
GLinear<int> test;    // указываем тип данных в <>

void setup() {
  Serial.begin(9600);
}

void loop() {
  for (byte i = 0; i < ARRAY_SIZE - 1; i++) {    // счётчик от 0 до ARRAY_SIZE
    y_array[i] = y_array[i + 1];    // сдвинуть массив давлений КРОМЕ ПОСЛЕДНЕЙ ЯЧЕЙКИ на шаг назад
  }
  // последний элемент массива теперь - новое значение (просто с аналог. датчика)
  y_array[ARRAY_SIZE - 1] = analogRead(0);

  // передаём массивы и размер одного из них
  test.compute((int*)x_array, (int*)y_array, sizeof(x_array));

  // по нашим исходным данным это будет производная, т.е. "изменение единиц в секунду"
  Serial.println(test.getDelta());  // получить изменение (аппроксимированное)

  delay(1000);  // секундная задержка
}
