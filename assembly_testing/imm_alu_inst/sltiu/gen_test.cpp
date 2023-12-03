#include <fmt/core.h>
#include <time.h>

int main ()
{
    srand(time(NULL));

    int a_res[32] = {0};
    int imm[32] = {0};
    int sltiu_results[32] = {0};

    for (int i = 1; i < 32; i++)
    {
        a_res[i] = (rand() % 4096) - 2048;
        imm[i] = (rand() % 4096) - 2048;
        sltiu_results[i] = (unsigned)a_res[i] < (unsigned)imm[i] ? 1 : 0;
    }

    for (int j = 0; j < 32; j++)
    {
        fmt::print("addi x{0}, x0, {1}\n", j, a_res[j]);
    }
    for (int j = 0; j < 32; j++)
    {
        fmt::print("sltiu x{0}, x{0}, {1}\n", j, imm[j]);
    }


    for (int j = 0; j < 32; j++)
        fmt::print("\"x{}\": \"{}\",\n", j, j ? sltiu_results[j] : 0);
}