creat 相当于 open(fd, O_CREAT | O_WRONLY | O_TRUNC, flags);

请看 creat.c 文件验证