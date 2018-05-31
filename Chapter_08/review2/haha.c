const char *sig_code_str(int si_code) {
    const char *str = NULL;
#ifdef CLD_EXITED
    if (si_code == CLD_EXITED) 
        str = SIG_CODE_STR(CLD_EXITED);
#endif

#ifdef CLD_KILLED
    if (si_code == CLD_KILLED) 
        str = SIG_CODE_STR(CLD_KILLED);
#endif

#ifdef CLD_DUMPED
    if (si_code == CLD_DUMPED) 
        str = SIG_CODE_STR(CLD_DUMPED);
#endif

#ifdef CLD_TRAPPED
    if (si_code == CLD_TRAPPED) 
        str = SIG_CODE_STR(CLD_TRAPPED);
#endif

#ifdef CLD_STOPPED
    if (si_code == CLD_STOPPED) 
        str = SIG_CODE_STR(CLD_STOPPED);
#endif

#ifdef CLD_CONTINUED
    if (si_code == CLD_CONTINUED) 
        str = SIG_CODE_STR(CLD_CONTINUED);
#endif
    return str;
}
