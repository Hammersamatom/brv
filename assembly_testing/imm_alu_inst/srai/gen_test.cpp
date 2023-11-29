#include <fmt/core.h>
#include <time.h>

int main ()
{
    srand(time(NULL));

    int a_res[32] = {0};
    int shamt[32] = {0};
    int srli_results[32] = {0};

    for (int i = 1; i < 32; i++)
    {
        a_res[i] = (rand() % 4096) - 2048;
        shamt[i] = rand() % 32;
        srli_results[i] = (signed)a_res[i] >> shamt[i];
    }

    for (int j = 0; j < 32; j++)
    {
        fmt::print("addi x{0}, x0, {1}\n", j, a_res[j]);
    }
    for (int j = 0; j < 32; j++)
    {
        fmt::print("srai x{0}, x{0}, {1}\n", j, shamt[j]);
    }


    for (int j = 0; j < 32; j++)
        fmt::print("\"x{}\": \"{}\",\n", j, j ? srli_results[j] : 0);
}