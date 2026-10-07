#include "bitset.hpp"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>

void clear_screen() {
#if defined(_WIN32) || defined(_WIN64)
  std::system("cls");
#else
  std::system("clear");
#endif
}

void print_header() {
  std::cout << "=============================================\n"
            << "  SETS PROJECT (TUI)\n"
            << "=============================================\n";
}

void print_menu(std::optional<std::size_t> universe_size,
                const std::optional<sets::BitSet>& a,
                const std::optional<sets::BitSet>& b) {
  print_header();

  if (universe_size.has_value()) {
    std::cout << "Мощность U: " << *universe_size << std::endl;
  } else {
    std::cout << "Мощность U: " << "- не задано -" << std::endl;
  }

  if (a.has_value()) {
    std::cout << "Подмножество A: " << *a << std::endl;
  } else {
    std::cout << "Подмножество A: " << "- не задано -" << std::endl;
  }

  if (b.has_value()) {
    std::cout << "Подмножество B: " << *b << std::endl;
  } else {
    std::cout << "Подмножество B: " << "- не задано -" << std::endl;
  }

  std::cout << std::endl;

  if (a.has_value() && b.has_value()) {
    try {
      std::cout << "Пересечение A∩B: " << (*a & *b) << std::endl;
      std::cout << "Объединение A∪B:: " << (*a | *b) << std::endl;
      std::cout << "Отрицание ¬A: " << ~*a << std::endl;
      std::cout << "Отрицание ¬B: " << ~*b << std::endl;
      std::cout << std::endl;
    } catch (const std::exception& e) {
      std::cout << "[Ошибка вычисления операций: " << e.what() << "]\n\n";
    }
  }

  std::cout << "1) Изменить мощность U\n"
            << "2) Изменить A\n"
            << "3) Изменить B\n"
            << "0) Выход\n"
            << "---------------------------------------------\n"
            << "> ";
}

// Обрезает пробелы слева и справа строки
std::string_view trim(std::string_view s) {
  constexpr std::string_view whitespace = " \t\n\r\f\v";
  const auto start = s.find_first_not_of(whitespace);
  if (start == std::string_view::npos)
    return {};
  const auto end = s.find_last_not_of(whitespace);
  return s.substr(start, end - start + 1);
}

// Возвращает {string, bool}
std::pair<std::string, bool> read_line() {
  std::string line;
  if (std::getline(std::cin, line)) {
    return {std::string(trim(line)), true};
  }
  return {"", false};
}

enum class Command { ChangeU = 1, ChangeA = 2, ChangeB = 3, Exit = 0 };

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::optional<size_t> u_size;
  std::optional<sets::BitSet> setA;
  std::optional<sets::BitSet> setB;

  while (true) {
    clear_screen();
    print_menu(u_size, setA, setB);

    auto [input, ok] = read_line();
    if (!ok) {
      break;
    }
    if (input.empty()) {
      continue;
    }

    try {
      int choice = std::stoi(input);

      auto option = static_cast<Command>(choice);

      switch (option) {
      case Command::ChangeU: {
        std::cout << "Мощность U: ";
        auto [line, ok] = read_line();
        if (!ok || line.empty()) {
          break;
        }
        try {
          std::uint64_t n = std::stoull(line);
          u_size = n;
          setA = std::nullopt;
          setB = std::nullopt;
        } catch (const std::invalid_argument&) {
          std::cerr << "Ошибка: введено некорректное число!\n";
        } catch (const std::out_of_range&) {
          std::cerr
              << "Ошибка: число выходит за пределы допустимого диапазона!\n";
        }
        break;
      }
      case Command::ChangeA: {
        if (!u_size.has_value()) {
          std::cout << "Сначала задай U!\n";
          break;
        }
        std::cout
            << "Элементы через пробел (поддерживаются диапазоны вида 1-10): ";
        auto [line, ok] = read_line();
        if (!ok || line.empty()) {
          break;
        }
        try {
          setA = sets::BitSet::from_string(line, *u_size);
        } catch (const std::invalid_argument& e) {
          std::cerr << "Ошибка формата ввода: " << e.what() << "\n";
        } catch (const std::out_of_range& e) {
          std::cerr << "Ошибка диапазона (индекс выходит за пределы U): "
                    << e.what() << "\n";
        }
        break;
      }
      case Command::ChangeB: {
        if (!u_size.has_value()) {
          std::cout << "Сначала задай U!\n";
          break;
        }
        std::cout
            << "Элементы через пробел (поддерживаются диапазоны вида 1-10): ";
        auto [line, ok] = read_line();
        if (!ok || line.empty()) {
          break;
        }
        try {
          setB = sets::BitSet::from_string(line, *u_size);
        } catch (const std::invalid_argument& e) {
          std::cerr << "Ошибка формата ввода: " << e.what() << "\n";
        } catch (const std::out_of_range& e) {
          std::cerr << "Ошибка диапазона (индекс выходит за пределы U): "
                    << e.what() << "\n";
        }
        break;
      }
      case Command::Exit:
        break;
      default:
        std::cout << "Неизвестный пункт меню\n";
        break;
      }
    } catch (const std::invalid_argument&) {
      std::cerr << "Ошибка: введено не число!\n";
    } catch (const std::out_of_range&) {
      std::cerr << "Ошибка: число слишком большое!\n";
    }
  }
  return 0;
}
