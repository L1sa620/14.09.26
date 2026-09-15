#include <chrono>
#include <cstddef>
#include <exception>
#include <iostream>
#include <vector>
#include <functional>
#include <future>
#include <stdexcept>
#include <string>

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

value_t calc(const data_t &values, std::size_t begin, std::size_t end)
{
  value_t sum = 0;
  for (std::size_t i = begin; i < end; ++i)
  {
    sum += values[i];
  }
  return sum;
}

value_t parallelSum(const data_t &values, std::size_t threads)
{
  std::vector<std::future<value_t>> results(threads);

  const std::size_t count = values.size() / threads;
  const std::size_t remainder = values.size() % threads;
  std::size_t begin = 0;

  for (std::size_t i = 0; i < threads; ++i)
  {
    const std::size_t end = begin + count + (i < remainder);

    results[i] = std::async(
      std::launch::async, calc, std::cref(values), begin, end
    );

    begin = end;
  }

  value_t sum = 0;
  for (auto &result: results)
  {
    sum += result.get();
  }
  return sum;
}

int main(int argc, char *argv[])
{
  try
  {
    if (argc != 2)
    {
      throw std::invalid_argument("Usage: program <threads>");
    }

    const std::string argument = argv[1];
    std::size_t position = 0;
    const int count = std::stoi(argument, &position);

    if (count <= 0 || position != argument.size())
    {
      throw std::invalid_argument("Invalid thread count");
    }

    const std::size_t threads = static_cast<std::size_t>(count);
    constexpr std::size_t size = 1'000'000'000;
    const data_t values(size, 1);

    const Clicker timer;
    const value_t sum = parallelSum(values, threads);
    const double elapsed = timer.millisec();

    std::cout << "Threads: " << threads << "\n";
    std::cout << "Sum: " << sum << "\n";
    std::cout << "Time: " << elapsed << " ms\n";
  }
  catch (const std::exception &error)
  {
    std::cerr << error.what() << "\n";
    return 1;
  }
}
