#include <fmt/core.h>
#include <time.h>

int main ()
{
    srand(time(NULL));

    int a_res[32] = {0};
    int shamt[32] = {0};
    int slli_results[32] = {0};

    for (int i = 1; i < 32; i++)
    {
        a_res[i] = (rand() % 4096) - 2048;
        shamt[i] = rand() % 32;
        slli_results[i] = (unsigned)a_res[i] << shamt[i];
    }

    for (int j = 0; j < 32; j++)
    {
        fmt::print("addi x{0}, x0, {1}\n", j, a_res[j]);
    }
    for (int j = 0; j < 32; j++)
    {
        fmt::print("slli x{0}, x{0}, {1}\n", j, shamt[j]);
    }


    for (int j = 0; j < 32; j++)
        fmt::print("\"x{}\": \"{}\",\n", j, j ? slli_results[j] : 0);
}