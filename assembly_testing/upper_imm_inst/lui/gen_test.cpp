#include <fmt/core.h>
#include <time.h>

int main ()
{
    srand(time(NULL));

    int a_res[32] = {0};

    for (int i = 0; i < 32; i++)
    {
        a_res[i] = rand() & 0xFFFFF000;
    }

    for (int j = 0; j < 32; j++)
    {
        fmt::print("lui x{0}, {1}\n", j, (unsigned)a_res[j] >> 12);
    }

    
    for (int j = 0; j < 32; j++)
        fmt::print("\"x{}\": \"{}\",\n", j, j ? a_res[j] & 0xFFFFF000 : 0);
}