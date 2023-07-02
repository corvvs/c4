#include "c4.h"

#define BUF_SIZE 1024

int	get_col(unsigned int* np) {
	char buf[BUF_SIZE];

	int rs = read(STDIN_FILENO, buf, BUF_SIZE);
	if (rs <= 0) {
		return -1;
	}
	char *nl = ft_memchr(buf, '\n', rs);
	if (nl == NULL)
		return -1;
	*nl = '\0';
	return ft_parseuint_base(buf, np, "0123456789");
}
