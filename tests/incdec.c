int main()
{
int i = 20;
int j = ++i;
i++;
--j;
i = j * i++;
j = i * --j;
return 0;
}
