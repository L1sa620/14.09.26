#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <vector>

using data_t = std::vector<unsigned long long>;
using value_t = data_t::value_type;

class Clicker
{
public:
  Clicker():
    start_(std::chrono::steady_clock::now())
  {}

  double millisec() const
  {
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start_).count();
  }

private:
  std::chrono::steady_clock::time_point start_;
};

int main()
{
  try
  {
    constexpr std::size_t size = 1'000'000'000;
    const data_t values(size, 1);
    value_t sum = 0;

    const Clicker timer;
    for (std::size_t i = 0; i < values.size(); ++i)
    {
      sum += values[i];
    }
    const double elapsed = timer.millisec();

    std::cout << "Sum: " << sum << "\n";
    std::cout << "Time: " << elapsed << " ms\n";
  }
  catch (const std::exception &error)
  {
    std::cerr << error.what() << "\n";
    return 1;
  }
}
