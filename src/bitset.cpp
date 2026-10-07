#include "bitset.hpp"
#include <algorithm> // для более общего union через std::min() и std::max()
#include <ostream>   // для вывода в std::cout
#include <sstream>   // для парсинга из строки
#include <stdexcept> // для обработки ошибок

namespace sets {
BitSet::BitSet(std::size_t capacity)
    // Список инициализации конструктора:
    // поля класса инициализируются до кода в {}
    : size_(capacity) {
  std::size_t blocks = (capacity + BLOCK_SIZE - 1) / BLOCK_SIZE;
  data_.assign(blocks, 0);
}

void BitSet::set(std::size_t bit_index, bool value) {
  if (bit_index >= size_) {
    throw std::out_of_range("Index out of BitSet capacity");
  }
  auto block_index = bit_index / BLOCK_SIZE;
  auto shift = bit_index % BLOCK_SIZE;
  auto mask = block_type{1} << shift;
  if (value == true) {
    data_[block_index] |= mask;
  } else {
    data_[block_index] &= ~mask;
  }
}

[[nodiscard]] bool BitSet::contains(std::size_t bit_index) const noexcept {
  if (bit_index >= size_) {
    return false;
  }
  auto block_index = bit_index / BLOCK_SIZE;
  auto shift = bit_index % BLOCK_SIZE;
  auto mask = block_type{1} << shift;

  return (data_[block_index] & mask) != 0;
}

void BitSet::intersect_with(const BitSet& other) {
  auto min_blocks = std::min(data_.size(), other.data_.size());

  // Пересечение на совпадающих участках
  for (std::size_t i = 0; i < min_blocks; ++i) {
    data_[i] &= other.data_[i];
  }

  // Если other был меньше - остальное нули
  for (std::size_t i = min_blocks; i < data_.size(); ++i) {
    data_[i] = 0;
  }
}

void BitSet::union_with(const BitSet& other) {
  // Если other был больше - ресайзим исходник и дописываем из other
  if (other.data_.size() > data_.size()) {
    data_.resize(other.data_.size(), 0);
    size_ = std::max(size_, other.size_);
  }
  // Объединение на совпадающих участках
  for (std::size_t i = 0; i < other.data_.size(); ++i) {
    data_[i] |= other.data_[i];
  }
}

void BitSet::invert() {
  if (data_.empty()) {
    return;
  }

  for (std::size_t i = 0; i < data_.size(); ++i) {
    data_[i] = ~data_[i];
  }

  auto extra_bits = size_ % BLOCK_SIZE;
  if (extra_bits != 0) {
    auto mask = (block_type{1} << extra_bits) - 1;
    data_.back() &= mask;
  }
}

BitSet& BitSet::operator&=(const BitSet& other) {
  intersect_with(other);
  return *this;
}

BitSet& BitSet::operator|=(const BitSet& other) {
  union_with(other);
  return *this;
}

[[nodiscard]] BitSet BitSet::operator&(const BitSet& other) const {
  BitSet result = *this;
  result &= other;
  return result;
}

[[nodiscard]] BitSet BitSet::operator|(const BitSet& other) const {
  BitSet result = *this;
  result |= other;
  return result;
}

[[nodiscard]] BitSet BitSet::operator~() const {
  BitSet result = *this;
  result.invert();
  return result;
}

std::ostream& operator<<(std::ostream& os, const BitSet& set) {
  os << "{";
  bool first = true;
  for (std::size_t i = 0; i < set.data_.size(); ++i) {
    auto block = set.data_[i];
    if (block == 0)
      continue; // Пропускаем пустые блоки

    for (std::size_t bit = 0; bit < BitSet::BLOCK_SIZE; ++bit) {
      std::size_t idx = i * BitSet::BLOCK_SIZE + bit;
      if (idx >= set.size_)
        break;

      if ((block & (BitSet::block_type{1} << bit)) != 0) {
        if (!first)
          os << ", ";
        os << idx;
        first = false;
      }
    }
  }
  os << "}";
  return os;
}

[[nodiscard]] BitSet BitSet::from_string(const std::string& input,
                                         std::size_t capacity) {
  BitSet result(capacity);

  // Заменяем запятые на пробелы для правильной токенизации
  std::string formatted_input = input;
  std::replace(formatted_input.begin(), formatted_input.end(), ',', ' ');

  // Передаем строку в поток
  std::stringstream ss(formatted_input);
  std::string token;

  while (ss >> token) {
    auto dash_pos = token.find('-');

    // Обработка диапазона "14-20"
    if (dash_pos != std::string::npos) {
      std::size_t start = 0;
      std::size_t end = 0;
      try {
        start = std::stoull(token.substr(0, dash_pos));
        end = std::stoull(token.substr(dash_pos + 1));
      } catch (const std::exception&) {
        throw std::invalid_argument("Некорректный элемент в строке: " + token);
      }

      if (start > end) {
        throw std::invalid_argument("Некорректный диапазон: " + token);
      }

      if (end >= capacity) {
        throw std::out_of_range("Индекс " + std::to_string(end) +
                                " выходит за пределы мощности U (" +
                                std::to_string(capacity) + ")");
      }

      // Итерируемся ДО end включительно
      for (std::size_t i = start; i <= end; ++i) {
        result.set(i, true);
      }
    }
    // Обработка одиночного числа
    else {
      std::size_t value = 0;
      try {
        value = std::stoull(token);
      } catch (const std::exception&) {
        throw std::invalid_argument("Некорректный элемент в строке: " + token);
      }

      if (value >= capacity) {
        throw std::out_of_range("Индекс " + std::to_string(value) +
                                " выходит за пределы мощности U (" +
                                std::to_string(capacity) + ")");
      }

      result.set(value, true);
    }
  }

  return result;
}

} // namespace sets
