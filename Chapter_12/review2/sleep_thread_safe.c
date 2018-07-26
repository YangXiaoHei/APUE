

unsigned int yh_sleep(unsigned int seconds)
{
    time_t start, end;
    time(&start);
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    int n = select(0, NULL, NULL, NULL, &tv);
    if (n == 0)
        return 0;
    time(&end);
    unsigned int slept = end - start;
    if (slept > seconds)
        return 0;
    return seconds - slept;
}

int main(int argc, char const *argv[])
{
    
    return 0;
}