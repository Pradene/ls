#include "libft.h"

int	ft_lstsize(List *lst) {
	int	i;

	i = 0;
	while (lst) {
		lst = lst->next;
		i++;
	}
	return (i);
}
