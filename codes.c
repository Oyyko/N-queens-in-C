#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 123456789
// 最大能运行多少个皇后

int output_ok;

void swap_int(int *p, int *q)
{
    int temp;
    temp = *(p);
    *(p) = *(q);
    *(q) = temp;
}

int swap_ok, change_of_collisions, temp, n;
// swap_ok判断是否交换两个皇后会使得collisions减小
// change_of_collisions计算交换前后的collisions的变化量
// temp用作交换两个变量的临时变量
// n为皇后数目

void print_solve1(int *x, int n, FILE *fp) // 传入queen,n和fp
// 输出形象化的棋盘
// *代表皇后 -代表棋盘
{
    fprintf(fp, "\nBEGIN\n");
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (j == x[i])
            {
                fprintf(fp, "*");
            }
            else
            {
                fprintf(fp, "-");
            }
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "\nEND\n\n\n");
}

void print_solve(int *x, int n, FILE *fp) // 传入queen,n,fp
// 输出皇后数列到文件n-queens-out.txt
{
    fprintf(fp, "%d\n", n);
    for (int i = 1; i <= n; ++i)
    {
        fprintf(fp, "%d ", x[i]);
    }
    if (n <= 20)
    {
        print_solve1(x, n, fp);
    }
}

int main()
{
    FILE *fp = fopen("n-queens-out.txt", "w");

    int n;
    // n皇后
    printf("@Author: ZhangJia\n");
    printf("@university: SCGY of USTC\n");
    printf("@Github: Oyyko\n");
    printf("\n");
    printf("\n");
    printf("Please Input the number N where the board is N*N.\n");
    printf("which means we will have N queens\n");
    scanf("%d", &n);

    printf("Do you want to output to\"n-queens-out.txt\"\n");
    printf("input 1 as yes ,other character as no\n");
    scanf("%d", &output_ok);
    if (output_ok != 1)
    {
        output_ok = 0;
    }
    printf("\nProgram START\n");

    clock_t start, end;
    //用于记录时间

    int *pre_queen = (int *)malloc(sizeof(int) * N);
    int *pre_dn = (int *)malloc(sizeof(int) * (2 * N - 1));
    int *pre_dp = (int *)malloc(sizeof(int) * (2 * N - 1));
    int *pre_attack = (int *)malloc(sizeof(int) * N);

    int *queen = pre_queen - 1;
    int *dn = pre_dn - 2;
    int *dp = pre_dp - 1 + n;
    int *attack = pre_attack - 1;
    // 四个pre数组开辟空间
    // 之后由对应的指针指向它们中间的位置
    // 从而满足算法运行对数组下标的要求(即可能会出现负的下标)
    // 例如 queen[1] = pre_queen[0]
    // 例如 queen[m] = pre_queen[m-1]
    // 例如 dn[m]    = pre_dn[m-2]
    // 例如 dp[m]    = pre_dp[m+n-1]
    // 例如 dp[-2]   = pre_dp[-2+n-1]

    // 数列queen[i]=j 表示第i行的皇后在第j列
    // 数列dn[i]=k 表示斜率为负的对角线中编号为i的对角线上有k个皇后
    // 数列dp[i]=k 表示斜率为正的对角线中编号为i的对角线上有k个皇后
    // dn,dp的编号方法分别为(列号+行号),(列号-行号)
    // 数列attack[i]=j 表示第i个被攻击的皇后在queen中的序号为j

    memset(pre_dn, 0, sizeof(int) * (2 * n - 1));
    memset(pre_dp, 0, sizeof(int) * (2 * n - 1));

    int limit, collisions, number_of_attacks, loopcount;

    collisions = 1;
    start = clock();

    while (collisions)
    // 当评估函数为0时，不断执行，直到评估函数为0
    // 评估函数为0也就是找到一组解
    {
        memset(pre_dn, 0, sizeof(int) * (2 * n - 1));
        memset(pre_dp, 0, sizeof(int) * (2 * n - 1));
        collisions = 0;

        for (int i = 0; i <= n - 1; ++i)
        {
            pre_queen[i] = i + 1;
        }
        int Search_Max = (n << 2);
        size_t x = -1;
        x = (x >> 1);
        int i, j;
        int m;
        int rand_max = n;
        size_t rand_seed;
        rand_seed = (size_t)time(NULL);
        for (i = 1, j = 1; i <= Search_Max && j <= n; ++i)
        {
            // 我们的目标是在初始化阶段生成一个很好的初态
            // 这样我们就能很快的爬山找到最优解
            // 因为初态的collisions已经很小
            // 因此爬山的过程会大幅度缩短
            // 优化的方法就是在初始化的时候就尽可能的避免碰撞
            // j的作用是衡量已经有多少个皇后被优化处理了,如果优化了n个,那么说明该停止优化了
            // i的作用是限制优化阶段所采用的时间 每循环一次i就自加1
            // 当i大于Search_Max的时候,强制退出优化过程
            rand_seed *= 1103515245;
            rand_seed += 12345;
            rand_seed = rand_seed % (x + 1);
            // LCG随机数生成器
            // X(n+1) = (a * X(n) + c) % m
            // 此处我采用a等于1103515245 c等于12345 的参数取法
            // 该取法为gcc编译器的参数取值
            // 为了合理的取值范围我采用了size_t类型
            // 实际上就是生成一个 0~x的随机数

            m = rand_max * ((double)rand_seed / (double)x) + j;
            // m为 j~n之间的一个随机数
            if (!(dn[queen[m] + j]) && !(dp[queen[m] - j]))
            // 如果 皇后m 与 皇后j 交换可以减小评估函数的值
            // 那么就交换m与j
            {
                swap_int(&queen[m], &queen[j]);
                ++dn[queen[j] + j];
                ++dp[queen[j] - j];
                ++j;
                --rand_max;
            }
        }
        // 在优化完了j个皇后之后
        // 打乱剩下的皇后,以使得初态更加平均
        // 同时计算出所有dn与dp的值
        for (i = j; i <= n; ++i)
        {
            m = rand() % rand_max + i;
            swap_int(&queen[m], &queen[i]);
            ++dn[queen[i] + i];
            ++dp[queen[i] - i];
            --rand_max;
        }
        // 以上完成了对整个棋盘的初始化

        for (int i = 2; i <= 2 * n; ++i)
        {
            if (dn[i] > 1)
                collisions += (dn[i] - 1);
        }
        for (int i = 1 - n; i <= n - 1; ++i)
        {
            if (dp[i] > 1)
                collisions += (dp[i] - 1);
        }
        if (!collisions)
        // 若恰好生成了满足要求的皇后排列,则输出答案并且结束程序
        {
            end = clock();
            printf("It takes %f seconds to find a solotion\n", (double)(end - start) / CLOCKS_PER_SEC);
            print_solve(queen, n, fp);
            exit(0);
        }

        limit = collisions >> 1; // means limit=0.5*collisions
                                 // limit的作用是评估何时适合重新计算attack数组，从而达到更快的运行速度。
                                 // 如果每次交换都重新计算attack数组,那么开销过大.
                                 // 为此我们采用设置阈值的方法
                                 // 仅当collision<limit时，才重新计算attack。从而减小了不必要的损耗

        // compute attack START

        number_of_attacks = 0;
        int k = 1;
        for (int i = 1; i <= n; ++i)
        {
            if (dn[queen[i] + i] > 1 || dp[queen[i] - i] > 1)
            {
                attack[k++] = i;
                ++number_of_attacks;
            }
        }
        //compute attack END

        loopcount = 0;
        // loopcount用来判断何时随机重启
        // 每次爬山都会增加loopcount的值
        // 当loopcount比较大时,说明爬山法陷入了局部困境,需要进行随机重启

        // Initialization END
        // 初始化过程结束,下面开始爬山算法

        while (loopcount < (n << 5))
        {
            for (int k = 1; k <= number_of_attacks; ++k)
            {
                int i = attack[k];
                int j = ((rand() << 6) + rand()) % (n) + 1;
                // 取一个被攻击的皇后和一个随机取得皇后,观察是否可以交换

                swap_ok = 0;
                change_of_collisions = 0;

                change_of_collisions -= (dp[queen[i] - i] > 1);
                change_of_collisions -= (dp[queen[j] - j] > 1);
                change_of_collisions -= (dn[queen[i] + i] > 1);
                change_of_collisions -= (dn[queen[j] + j] > 1);

                change_of_collisions += (dn[queen[j] + i] >= 1);
                change_of_collisions += (dn[queen[i] + j] >= 1);
                change_of_collisions += (dp[queen[j] - i] >= 1);
                change_of_collisions += (dp[queen[i] - j] >= 1);
                // 计算评估函数的改变量
                if (change_of_collisions < 0)
                {
                    if (!(queen[i] + i - queen[j] - j) && !(dp[queen[i] - j]))
                    {
                        change_of_collisions += 2;
                    }
                    if (!(queen[i] - i - queen[j] + j) && !(dn[queen[i] + j]))
                    {
                        change_of_collisions += 2;
                    }
                    if (change_of_collisions < 0)
                    {
                        // perform swap
                        // 若改变量小于0,则执行交换
                        --dn[queen[i] + i];
                        --dp[queen[i] - i];
                        --dn[queen[j] + j];
                        --dp[queen[j] - j];
                        ++dn[queen[j] + i];
                        ++dn[queen[i] + j];
                        ++dp[queen[j] - i];
                        ++dp[queen[i] - j];

                        temp = queen[j];
                        queen[j] = queen[i];
                        queen[i] = temp;

                        collisions += change_of_collisions;

                        if (!collisions)
                        // 若找到了解,则输出答案并且结束程序
                        {
                            end = clock();
                            printf("It takes %f seconds to find a solotion\n", (double)(end - start) / CLOCKS_PER_SEC);
                            printf("Press any key to EXIT\n");
                            if (output_ok)
                                print_solve(queen, n, fp);
                            getchar();
                            getchar();
                            exit(0);
                        }
                        if (collisions < limit)
                        // 当棋盘变动较大时,重新计算attack数列
                        {
                            limit = collisions >> 1;
                            // compute attack

                            number_of_attacks = 0;
                            int k = 1;
                            for (int i = 1; i <= n; ++i)
                            {
                                if (dn[queen[i] + i] > 1 || dp[queen[i] - i] > 1)
                                {
                                    attack[k++] = i;
                                    ++number_of_attacks;
                                }
                            }
                        }
                    }
                }
            }

            loopcount = loopcount + number_of_attacks;
        }
    }

    return 0;
}
