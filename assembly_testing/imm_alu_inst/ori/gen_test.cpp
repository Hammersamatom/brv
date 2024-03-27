#include <fmt/core.h>
#include <time.h>

int main ()
{
    srand(time(NULL));

    int a_res[32] = {0};
    int b_res[32] = {0};
    int or_results[32] = {0};

    for (int i = 1; i < 32; i++)
    {
        a_res[i] = (rand() % 4096) - 2048;
        b_res[i] = (rand() % 4096) - 2048;
        or_results[i] = a_res[i] | b_res[i];
    }

    for (int j = 0; j < 32; j++)
    {
        fmt::print("addi x{0}, x0, {1}\n", j, a_res[j]);
    }
    for (int j = 0; j < 32; j++)
    {
        fmt::print("ori x{0}, x{0}, {1}\n", j, b_res[j]);
    }

    for (int j = 0; j < 32; j++)
        fmt::print("\"x{}\": \"{}\",\n", j, j ? or_results[j] : 0);
}