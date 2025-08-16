#include "libft.h"

void	ft_lstadd_front(List **lst, List *new) {
	if (lst) {
		if (*lst) {
			new->next = *lst;
		}
		*lst = new;
	}
}
