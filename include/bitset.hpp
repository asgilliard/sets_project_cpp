#pragma once
#include <cstddef> // для std::size_t (размерный тип)
#include <cstdint> // для std::uint32_t (тип фиксированной ширины)
#include <iosfwd> // для std::ostream (вывод)
#include <string> // для парсинга из std::string в BitSet
#include <vector> // для хранения data_

namespace sets {
class BitSet {
private:
  using block_type = std::uint32_t;
  // static - константа общая для всех экземпляров класса,
  // constexpr - константа вычисляется на этапе компиляции
  static constexpr std::size_t BLOCK_SIZE = sizeof(block_type) * 8;
  std::size_t size_;
  std::vector<block_type> data_;

public:
  // explicit - функция, ожидающая BitSet, не создаст его сама неявно из числа
  explicit BitSet(std::size_t capacity = 0);
  void set(std::size_t bit_index, bool value);
  [[nodiscard]] bool contains(std::size_t bit_index) const noexcept;
  void intersect_with(const BitSet& other); // AND (&=)
  void union_with(const BitSet& other);     // OR (|=)
  void invert();                            // NOT (~)

  // Унарные операторы-модификаторы (возвращают ссылку на себя *this)
  BitSet& operator&=(const BitSet& other);
  BitSet& operator|=(const BitSet& other);

  // Унарное отрицание (возвращает новый объект)
  [[nodiscard]] BitSet operator~() const;

  // Бинарные операторы (возвращают новый объект)
  [[nodiscard]] BitSet operator&(const BitSet& other) const;
  [[nodiscard]] BitSet operator|(const BitSet& other) const;

  // friend даёт доступ к приватным полям не-методу
  friend std::ostream& operator<<(std::ostream& os, const BitSet& set);

  // Парсинг в BitSet из строки
  // static - метод принадлежит всему классу, а не конкретному объекту
  static BitSet from_string(const std::string& input, std::size_t capacity);
};
} // namespace sets
