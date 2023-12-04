#include <fmt/core.h>
#include <time.h>

int main ()
{
    srand(time(NULL));

    int a_res[32] = {0};

    for (int i = 0; i < 32; i++)
    {
        a_res[i] = (rand() % 4096) - 2048;
    }

    for (int j = 0; j < 32; j++)
    {
        fmt::print("addi x{0}, x0, {1}\n", j, a_res[j]);
    }

    
    for (int j = 0; j < 32; j++)
        fmt::print("\"x{}\": \"{}\",\n", j, j ? a_res[j] : 0);
}